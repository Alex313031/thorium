#!/usr/bin/env python3

# Copyright (c) 2026 Alex313031 and gz83.

"""Copy Thorium overlays and apply its patch series to Chromium."""

import argparse
import dataclasses
import json
import os
from pathlib import Path, PurePosixPath
import platform
import shlex
import shutil
import stat
import subprocess
import sys
import tempfile
from typing import Sequence


EXIT_FAILURE = 111
OVERLAY_COMPONENTS = ("chrome", "components", "content", "third_party", "ui")
PAK_METADATA_FILES = ("README.chromium", "LICENSE", "OWNERS")
GRD_SYNC_CONFIG_OPTIONS = (
    ("--file-allowlist", "file_allowlist.csv"),
    ("--message-allowlist", "message_allowlist.csv"),
    ("--feature-message-ownership", "feature_patch_message_ownership.csv"),
)
SETUP_MANIFEST_NAME = ".thorium-setup-files.json"
SETUP_MANIFEST_VERSION = 1
SIMD_PROFILES = {
    "avx512": ("AVX512", "wrapper-avx512"),
    "avx2": ("AVX2", "wrapper-avx2"),
    "sse4": ("SSE4.1", "wrapper-sse4"),
    "sse3": ("SSE3", "wrapper-sse3"),
    "sse2": ("SSE2", "wrapper-sse2"),
}


@dataclasses.dataclass(frozen=True)
class CopyPlan:
    files: tuple[tuple[Path, Path], ...] = ()
    directories: tuple[tuple[Path, Path], ...] = ()


@dataclasses.dataclass(frozen=True)
class ProfilePlan:
    message: str | None = None
    copies: CopyPlan = CopyPlan()
    art: Path | None = None


class SetupError(RuntimeError):
    """An expected setup or copy failure."""


def environment_path(value: str) -> Path:
    return Path(os.path.expandvars(value)).expanduser()


def default_chromium_src() -> Path:
    configured = os.environ.get("CR_DIR")
    if configured:
        return environment_path(configured)
    if os.name == "nt":
        return Path("C:/src/chromium/src")
    return Path.home() / "chromium" / "src"


def default_thorium_root() -> Path:
    configured = os.environ.get("THOR_DIR")
    if configured:
        return environment_path(configured)
    return Path.home() / "thorium"


def thor_ver_source(thorium_root: Path, profile: str) -> Path:
    if profile == "woa":
        return thorium_root / "arm" / "thor_ver"
    if profile in SIMD_PROFILES:
        source_name, _ = SIMD_PROFILES[profile]
        return thorium_root / "other" / source_name / "thor_ver"
    return thorium_root / "infra" / "thor_ver"


def pak_source(thorium_root: Path, profile: str) -> Path:
    filename = "pak_arm64" if profile == "raspi" else "pak"
    return thorium_root / "pak_src" / "binaries" / filename


def parse_args(argv: Sequence[str]) -> argparse.Namespace:
    parser = argparse.ArgumentParser(
        description="Copy Thorium files and patches over the Chromium tree.",
        epilog=(
            "For optimized LLVM builds, run infra/build_llvm.py after this "
            "setup script and before building. Restore a clean Chromium tree "
            "with version.py before switching setup profiles."
        ),
    )
    parser.add_argument(
        "--chromium-src",
        type=environment_path,
        default=default_chromium_src(),
        metavar="PATH",
        help="Chromium src directory (default: CR_DIR or the platform default)",
    )
    parser.add_argument(
        "--thorium-root",
        type=environment_path,
        default=default_thorium_root(),
        metavar="PATH",
        help="Thorium repository root (default: THOR_DIR or ~/thorium)",
    )
    profiles = parser.add_mutually_exclusive_group()
    profiles.add_argument(
        "--mac",
        "--macos",
        action="store_const",
        const="mac",
        dest="profile",
        help="prepare a macOS build",
    )
    profiles.add_argument(
        "--raspi",
        "--arm64",
        action="store_const",
        const="raspi",
        dest="profile",
        help="prepare a Raspberry Pi ARM64 build",
    )
    profiles.add_argument(
        "--woa",
        action="store_const",
        const="woa",
        dest="profile",
        help="prepare a Windows on ARM64 build",
    )
    for option, profile, description in (
        ("--avx512", "avx512", "an AVX-512"),
        ("--avx2", "avx2", "an AVX2"),
        ("--sse4", "sse4", "an SSE4.1"),
        ("--sse3", "sse3", "an SSE3"),
        ("--sse2", "sse2", "a 32-bit SSE2"),
    ):
        profiles.add_argument(
            option,
            action="store_const",
            const=profile,
            dest="profile",
            help=f"prepare {description} build",
        )
    profiles.add_argument(
        "--android",
        action="store_const",
        const="android",
        dest="profile",
        help="prepare an Android build",
    )
    profiles.add_argument(
        "--cros",
        action="store_const",
        const="cros",
        dest="profile",
        help="prepare a ChromiumOS build",
    )
    parser.set_defaults(profile="default")
    return parser.parse_args(argv)


def run(command: Sequence[str], cwd: Path) -> None:
    printable = (
        subprocess.list2cmdline(command) if os.name == "nt" else shlex.join(command)
    )
    print(f"\n[{cwd}] {printable}", flush=True)
    try:
        subprocess.run(command, cwd=cwd, check=True)
    except OSError as error:
        raise SetupError(f"could not run {printable}: {error}") from error
    except subprocess.CalledProcessError as error:
        raise SetupError(
            f"command failed with exit code {error.returncode}: {printable}"
        ) from error


def require_directory(path: Path, description: str) -> None:
    if not path.is_dir():
        raise SetupError(f"{description} directory does not exist: {path}")


def require_checkout(path: Path, description: str) -> None:
    require_directory(path, description)
    if not (path / ".git").exists():
        raise SetupError(f"{description} is not a Git checkout: {path}")


def require_file(path: Path, description: str) -> None:
    if not path.is_file():
        raise SetupError(f"{description} does not exist: {path}")


def copy_file(source: Path, destination: Path) -> None:
    require_file(source, "source file")
    destination.parent.mkdir(parents=True, exist_ok=True)
    print(f"Copying {source} -> {destination}")
    try:
        # Keep executable mode bits, but refresh the destination timestamp so
        # Ninja observes an overlaid source file as changed.
        shutil.copy(source, destination)
    except OSError as error:
        raise SetupError(
            f"failed to copy {source} to {destination}: {error}"
        ) from error


def copy_tree(source: Path, destination: Path) -> None:
    require_directory(source, "source")
    print(f"Copying directory {source} -> {destination}")
    try:
        shutil.copytree(
            source,
            destination,
            copy_function=shutil.copy,
            dirs_exist_ok=True,
        )
    except OSError as error:
        raise SetupError(
            f"failed to copy {source} to {destination}: {error}"
        ) from error


def remove_file(path: Path) -> None:
    if not path.exists() and not path.is_symlink():
        return
    print(f"Removing {path}")
    try:
        path.unlink()
    except PermissionError:
        try:
            path.chmod(stat.S_IWRITE)
            path.unlink()
        except OSError as error:
            raise SetupError(f"failed to remove {path}: {error}") from error
    except OSError as error:
        raise SetupError(f"failed to remove {path}: {error}") from error


def read_art(path: Path) -> None:
    require_file(path, "ASCII art")
    try:
        print(f"\n{path.read_text(encoding='utf-8')}")
    except (OSError, UnicodeError) as error:
        raise SetupError(f"failed to read {path}: {error}") from error


def base_copy_plan(
    thorium_root: Path, chromium_src: Path, profile: str
) -> CopyPlan:
    output = chromium_src / "out" / "thorium"
    files = [
        (pak_source(thorium_root, profile), output / "pak"),
        (thor_ver_source(thorium_root, profile), output / "thor_ver"),
    ]
    pak_metadata = chromium_src / "third_party" / "pak"
    files.extend(
        (thorium_root / "pak_src" / filename, pak_metadata / filename)
        for filename in PAK_METADATA_FILES
    )
    directories = [
        (thorium_root / "src" / component, chromium_src / component)
        for component in OVERLAY_COMPONENTS
    ]
    directories.extend(
        (
            (thorium_root / "thorium_shell", output),
            (thorium_root / "pak_src" / "binaries" / "pak-win", output),
        )
    )
    return CopyPlan(files=tuple(files), directories=tuple(directories))


def profile_copy_plan(
    profile: str, thorium_root: Path, chromium_src: Path
) -> ProfilePlan:
    text_resources = chromium_src / "ui" / "webui" / "resources" / "text"
    version_destination = text_resources / "thorium_version.txt"
    linux_wrapper_destination = (
        chromium_src / "chrome" / "installer" / "linux" / "common" / "wrapper"
    )

    if profile in ("default", "android"):
        return ProfilePlan()
    if profile == "mac":
        return ProfilePlan(
            message="Copying files for macOS",
            copies=CopyPlan(
                files=(
                    (
                        thorium_root / "other" / "Mac" / "thorium_version.txt",
                        version_destination,
                    ),
                )
            ),
        )
    if profile == "raspi":
        return ProfilePlan(
            message="Copying Raspberry Pi ARM64 files",
            copies=CopyPlan(
                files=(
                    (thorium_root / "arm" / "thorium_version.txt", version_destination),
                    (
                        thorium_root / "other" / "thor_ver_linux" / "wrapper-raspi",
                        linux_wrapper_destination,
                    ),
                ),
                directories=(
                    (
                        thorium_root / "arm" / "third_party" / "widevine",
                        chromium_src / "third_party" / "widevine",
                    ),
                ),
            ),
            art=thorium_root / "logos" / "raspi_ascii_art.txt",
        )
    if profile == "woa":
        return ProfilePlan(
            message="Copying Windows on ARM64 files",
            copies=CopyPlan(
                files=(
                    (thorium_root / "arm" / "thorium_version.txt", version_destination),
                )
            ),
        )
    if profile in SIMD_PROFILES:
        source_name, wrapper_name = SIMD_PROFILES[profile]
        files = [
            (
                thorium_root / "other" / source_name / "thorium_version.txt",
                version_destination,
            )
        ]
        if sys.platform.startswith("linux"):
            files.append(
                (
                    thorium_root / "other" / "thor_ver_linux" / wrapper_name,
                    linux_wrapper_destination,
                )
            )
        return ProfilePlan(
            message=f"Copying {source_name} build files",
            copies=CopyPlan(files=tuple(files)),
        )
    if profile == "cros":
        return ProfilePlan(
            message="Copying ChromiumOS build files",
            copies=CopyPlan(
                files=(
                    (
                        thorium_root / "other" / "CrOS" / "thorium_version.txt",
                        version_destination,
                    ),
                )
            ),
        )
    raise SetupError(f"unsupported setup profile: {profile}")


def validate_copy_plan(plan: CopyPlan, description: str) -> None:
    for source, _ in plan.files:
        require_file(source, f"{description} file")
    for source, _ in plan.directories:
        require_directory(source, f"{description} directory")


def copy_plan_destinations(plan: CopyPlan) -> set[Path]:
    destinations = {destination for _, destination in plan.files}
    for source, destination in plan.directories:
        for source_file in source.rglob("*"):
            if source_file.is_file():
                destinations.add(destination / source_file.relative_to(source))
    return destinations


def is_managed_relative_path(parts: tuple[str, ...]) -> bool:
    return bool(parts) and (
        parts[0] in OVERLAY_COMPONENTS or parts[:2] == ("out", "thorium")
    )


def managed_relative_path(chromium_src: Path, destination: Path) -> str:
    try:
        relative = destination.relative_to(chromium_src)
    except ValueError as error:
        raise SetupError(
            f"setup destination is outside the Chromium tree: {destination}"
        ) from error
    if not is_managed_relative_path(relative.parts):
        raise SetupError(f"setup destination is outside managed roots: {destination}")
    return relative.as_posix()


def validate_manifest_entry(value: object) -> str:
    if not isinstance(value, str) or not value or "\\" in value:
        raise SetupError(f"invalid setup manifest path: {value!r}")
    relative = PurePosixPath(value)
    if relative.is_absolute() or any(
        part in ("", ".", "..") for part in relative.parts
    ):
        raise SetupError(f"invalid setup manifest path: {value!r}")
    if not is_managed_relative_path(relative.parts):
        raise SetupError(f"setup manifest path is outside managed roots: {value}")
    return relative.as_posix()


def read_setup_manifest(path: Path) -> set[str]:
    if not path.exists():
        return set()
    try:
        data = json.loads(path.read_text(encoding="utf-8"))
    except (OSError, UnicodeError, json.JSONDecodeError) as error:
        raise SetupError(f"failed to read setup manifest {path}: {error}") from error
    if not isinstance(data, dict) or data.get("version") != SETUP_MANIFEST_VERSION:
        raise SetupError(f"unsupported setup manifest format: {path}")
    files = data.get("files")
    if not isinstance(files, list):
        raise SetupError(f"invalid setup manifest file list: {path}")
    validated = {validate_manifest_entry(value) for value in files}
    if len(validated) != len(files):
        raise SetupError(f"duplicate paths in setup manifest: {path}")
    return validated


def write_setup_manifest(path: Path, files: set[str]) -> None:
    payload = {
        "version": SETUP_MANIFEST_VERSION,
        "files": sorted(files),
    }
    temporary: Path | None = None
    try:
        with tempfile.NamedTemporaryFile(
            mode="w",
            dir=path.parent,
            prefix=f".{path.name}.",
            suffix=".tmp",
            encoding="utf-8",
            newline="\n",
            delete=False,
        ) as output:
            temporary = Path(output.name)
            json.dump(payload, output, indent=2)
            output.write("\n")
        os.replace(temporary, path)
        temporary = None
    except OSError as error:
        raise SetupError(f"failed to write setup manifest {path}: {error}") from error
    finally:
        if temporary is not None:
            try:
                temporary.unlink(missing_ok=True)
            except OSError:
                pass


def existing_parent(path: Path) -> Path:
    current = path
    while not current.exists() and current != current.parent:
        current = current.parent
    return current if current.is_dir() else current.parent


def git_root_for_path(path: Path) -> Path:
    anchor = existing_parent(path.parent).resolve()
    for candidate in (anchor, *anchor.parents):
        if (candidate / ".git").exists():
            return candidate
    raise SetupError(f"could not locate the Git checkout for {path}")


def path_batches(paths: Sequence[str], size_limit: int = 16_000) -> list[list[str]]:
    batches: list[list[str]] = []
    current: list[str] = []
    current_size = 0
    for path in paths:
        path_size = len(os.fsencode(path)) + 1
        if current and current_size + path_size > size_limit:
            batches.append(current)
            current = []
            current_size = 0
        current.append(path)
        current_size += path_size
    if current:
        batches.append(current)
    return batches


def clean_repository_files(
    git: str, repository: Path, files: dict[str, Path]
) -> None:
    for batch in path_batches(sorted(files)):
        pathspecs = [f":(literal){path}" for path in batch]
        try:
            result = subprocess.run(
                [git, "-C", str(repository), "ls-files", "-z", "--", *pathspecs],
                check=True,
                stdout=subprocess.PIPE,
                stderr=subprocess.PIPE,
            )
        except (OSError, subprocess.CalledProcessError) as error:
            raise SetupError(
                f"could not inspect stale files in {repository}: {error}"
            ) from error
        tracked = {
            os.fsdecode(path)
            for path in result.stdout.split(b"\0")
            if path
        }
        tracked_pathspecs = [
            f":(literal){path}" for path in batch if path in tracked
        ]
        if tracked_pathspecs:
            run(
                [
                    git,
                    "restore",
                    "--source=HEAD",
                    "--worktree",
                    "--",
                    *tracked_pathspecs,
                ],
                repository,
            )
        for path in batch:
            if path not in tracked:
                remove_file(files[path])


def clean_stale_managed_files(
    chromium_src: Path, previous: set[str], current: set[str]
) -> None:
    stale = sorted(previous - current)
    if not stale:
        return
    git = shutil.which("git")
    if git is None:
        raise SetupError("required command 'git' was not found in PATH")
    print("\nCleaning files no longer published by setup.py")

    repositories: dict[Path, dict[str, Path]] = {}
    for relative in stale:
        parts = PurePosixPath(relative).parts
        destination = chromium_src.joinpath(*parts)
        if parts[:2] == ("out", "thorium"):
            remove_file(destination)
            continue
        repository = git_root_for_path(destination)
        try:
            repository_relative = destination.relative_to(repository).as_posix()
        except ValueError as error:
            raise SetupError(
                f"managed path is outside its Git checkout: {destination}"
            ) from error
        repositories.setdefault(repository, {})[repository_relative] = destination

    for repository, files in repositories.items():
        clean_repository_files(git, repository, files)


def execute_copy_plan(plan: CopyPlan) -> None:
    for source, destination in plan.directories:
        copy_tree(source, destination)
    for source, destination in plan.files:
        copy_file(source, destination)


def apply_patch_series(
    thorium_root: Path, chromium_src: Path, profile: str
) -> None:
    script = thorium_root / "patch_scripts" / "series" / "apply_series.py"
    condition = {
        "woa": "woa",
        "raspi": "raspi",
        "sse2": "sse2",
    }.get(profile)
    command = [
        sys.executable,
        str(script),
        "--thorium-root",
        str(thorium_root),
        "--source-tree",
        str(chromium_src),
        "--apply",
    ]
    if condition:
        command.extend(("--condition", condition))
    print("\nApplying Thorium patch series")
    run(command, thorium_root)


def apply_grd_rebase(thorium_root: Path, chromium_src: Path) -> None:
    grd_rebase = thorium_root / "patch_scripts" / "grd_rebase"
    config = grd_rebase / "config"
    sync_script = grd_rebase / "sync_grd_strings.py"
    merge_script = grd_rebase / "merge_thorium_xtb.py"

    command = [sys.executable, str(sync_script), str(chromium_src)]
    for option, filename in GRD_SYNC_CONFIG_OPTIONS:
        command.extend((option, str(config / filename)))

    print("\nApplying Thorium GRD/XTB rebase")
    run(command, thorium_root)
    run([sys.executable, str(merge_script), str(chromium_src)], thorium_root)


def execute_profile_plan(plan: ProfilePlan) -> None:
    if plan.message:
        print(f"\n{plan.message}")
    execute_copy_plan(plan.copies)
    if plan.art is not None:
        read_art(plan.art)


def validate_inputs(
    thorium_root: Path,
    chromium_src: Path,
    base_plan: CopyPlan,
    profile_plan: ProfilePlan,
) -> None:
    require_checkout(chromium_src, "Chromium")
    require_file(chromium_src / "BUILD.gn", "Chromium root BUILD.gn")
    validate_copy_plan(base_plan, "Thorium setup source")
    validate_copy_plan(profile_plan.copies, "profile source")
    if profile_plan.art is not None:
        require_file(profile_plan.art, "profile ASCII art")

    for path in (
        thorium_root / "logos" / "thorium_ascii_art.txt",
        thorium_root / "patch_scripts" / "series" / "apply_series.py",
        thorium_root / "patch_scripts" / "grd_rebase" / "sync_grd_strings.py",
        thorium_root / "patch_scripts" / "grd_rebase" / "merge_thorium_xtb.py",
    ):
        require_file(path, "Thorium setup input")
    config = thorium_root / "patch_scripts" / "grd_rebase" / "config"
    for _, filename in GRD_SYNC_CONFIG_OPTIONS:
        require_file(config / filename, "GRD rebase configuration")


def setup(thorium_root: Path, chromium_src: Path, profile: str) -> None:
    thorium_root = thorium_root.expanduser().resolve()
    chromium_src = chromium_src.expanduser().resolve()

    require_directory(thorium_root, "Thorium")
    output = chromium_src / "out" / "thorium"
    base_plan = base_copy_plan(thorium_root, chromium_src, profile)
    selected_profile_plan = profile_copy_plan(profile, thorium_root, chromium_src)
    validate_inputs(
        thorium_root,
        chromium_src,
        base_plan,
        selected_profile_plan,
    )

    try:
        output.mkdir(parents=True, exist_ok=True)
    except OSError as error:
        raise SetupError(f"failed to create {output}: {error}") from error

    destinations = copy_plan_destinations(base_plan)
    destinations.update(copy_plan_destinations(selected_profile_plan.copies))
    current_files = {
        managed_relative_path(chromium_src, destination)
        for destination in destinations
    }
    manifest = output / SETUP_MANIFEST_NAME
    previous_files = read_setup_manifest(manifest)

    # Publish the ownership union before mutating either tree. If setup fails,
    # the next run can still clean every file this run may have copied.
    pending_files = previous_files | current_files
    if pending_files != previous_files:
        write_setup_manifest(manifest, pending_files)
    clean_stale_managed_files(chromium_src, previous_files, current_files)

    print("\nCopying Thorium source overlays over the Chromium tree")
    execute_copy_plan(base_plan)

    apply_patch_series(thorium_root, chromium_src, profile)
    apply_grd_rebase(thorium_root, chromium_src)

    execute_profile_plan(selected_profile_plan)

    read_art(thorium_root / "logos" / "thorium_ascii_art.txt")
    if pending_files != current_files:
        write_setup_manifest(manifest, current_files)
    print("\nDone!")
    print("\nEnjoy Thorium!\n")


def main(argv: Sequence[str] | None = None) -> int:
    if sys.version_info < (3, 11):
        print("error: Python 3.11 or newer is required", file=sys.stderr)
        return 2
    if platform.system() not in ("Linux", "Darwin", "Windows"):
        print("error: only Linux, macOS, and Windows are supported", file=sys.stderr)
        return 2

    args = parse_args(sys.argv[1:] if argv is None else argv)
    try:
        setup(args.thorium_root, args.chromium_src, args.profile)
    except SetupError as error:
        print(f"{Path(sys.argv[0]).name}: {error}", file=sys.stderr)
        return EXIT_FAILURE
    except OSError as error:
        print(
            f"{Path(sys.argv[0]).name}: filesystem operation failed: {error}",
            file=sys.stderr,
        )
        return EXIT_FAILURE
    except KeyboardInterrupt:
        print(f"\n{Path(sys.argv[0]).name}: interrupted", file=sys.stderr)
        return 130
    return 0


if __name__ == "__main__":
    sys.exit(main())
