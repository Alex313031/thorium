#!/usr/bin/env python3

# Copyright (c) 2026 Alex313031 and gz83.

"""Download the repositories and prerequisites needed to build Thorium."""

import argparse
import ast
import os
from pathlib import Path
import platform
import shlex
import shutil
import stat
import subprocess
import sys
import time
from typing import Sequence

from trunk import CHROMIUM_NESTED_CHECKOUTS


DEPOT_TOOLS_URL = (
    "https://chromium.googlesource.com/chromium/tools/depot_tools.git"
)
THORIUM_URL = "https://github.com/Alex313031/thorium.git"
EXIT_FAILURE = 111
FETCH_INCOMPLETE_MARKER = ".thorium-fetch-incomplete"
FETCH_PHASE = "fetch --nohooks chromium"
SYNC_PHASE = "gclient sync --nohooks"
HOOKS_PHASE = "gclient runhooks pending"


class BootstrapError(RuntimeError):
    """An expected repository bootstrap failure."""


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


def default_depot_tools() -> Path:
    configured = os.environ.get("DEPOT_TOOLS_DIR")
    if configured:
        return environment_path(configured)
    gclient = shutil.which("gclient")
    if gclient:
        return Path(gclient).resolve().parent
    if os.name == "nt":
        return Path("C:/src/depot_tools")
    return Path.home() / "depot_tools"


def parse_args(argv: Sequence[str]) -> argparse.Namespace:
    parser = argparse.ArgumentParser(
        description="Download depot_tools, Thorium, Chromium, and prerequisites.",
        epilog=(
            "On Debian-based Linux systems, build dependency installation uses "
            "sudo. macOS and Windows prerequisites must be installed separately."
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
        help="Thorium checkout (default: THOR_DIR or ~/thorium)",
    )
    parser.add_argument(
        "--depot-tools",
        type=environment_path,
        default=default_depot_tools(),
        metavar="PATH",
        help=(
            "depot_tools checkout (default: DEPOT_TOOLS_DIR, the gclient "
            "location, or the platform default)"
        ),
    )
    parser.add_argument(
        "--no-history",
        action="store_true",
        help="fetch Chromium without full Git history",
    )
    parser.add_argument(
        "--skip-build-deps",
        action="store_true",
        help="do not install Debian build dependencies",
    )
    parser.add_argument(
        "--sync-existing",
        action="store_true",
        help=(
            "allow trunk.py to reset and synchronize an existing Chromium "
            "checkout, discarding uncommitted and untracked files"
        ),
    )
    parser.add_argument(
        "--recover-incomplete",
        action="store_true",
        help=(
            "allow gclient to reset and recover an incomplete Chromium "
            "checkout not created by an interrupted get_repo.py fetch"
        ),
    )
    parser.add_argument(
        "--yes",
        action="store_true",
        help="run without the interactive confirmation",
    )
    return parser.parse_args(argv)


def find_command(name: str) -> str:
    executable = shutil.which(name)
    if executable is None:
        raise BootstrapError(f"required command was not found in PATH: {name}")
    return executable


def run(command: Sequence[str], cwd: Path) -> None:
    printable = (
        subprocess.list2cmdline(command)
        if os.name == "nt"
        else shlex.join(command)
    )
    print(f"\n[{cwd}] {printable}", flush=True)
    try:
        subprocess.run(command, cwd=cwd, check=True)
    except OSError as error:
        raise BootstrapError(f"could not run {printable}: {error}") from error
    except subprocess.CalledProcessError as error:
        raise BootstrapError(
            f"command failed with exit code {error.returncode}: {printable}"
        ) from error


def require_checkout(path: Path, description: str, marker: str | None = None) -> None:
    if not path.is_dir() or not (path / ".git").exists():
        raise BootstrapError(f"{description} is not a Git checkout: {path}")
    if marker and not (path / marker).is_file():
        raise BootstrapError(f"{description} is missing {marker}: {path}")


def clone_repository(
    git: str,
    url: str,
    destination: Path,
    *,
    description: str,
    marker: str,
    recursive: bool = False,
) -> None:
    staging: Path | None = None
    try:
        destination.parent.mkdir(parents=True, exist_ok=True)
        timestamp = time.strftime("%Y%m%d-%H%M%S")
        staging = destination.with_name(f"{destination.name}.new-{timestamp}")
        suffix = 1
        while os.path.lexists(staging):
            staging = destination.with_name(
                f"{destination.name}.new-{timestamp}-{suffix}"
            )
            suffix += 1
        command = [git, "clone"]
        if recursive:
            command.append("--recursive")
        command.extend((url, str(staging)))
        run(command, destination.parent)
        require_checkout(staging, description, marker)
        staging.rename(destination)
    except (BootstrapError, KeyboardInterrupt, OSError) as error:
        cleanup_error = (
            remove_incomplete_checkout(staging) if staging is not None else None
        )
        if isinstance(error, KeyboardInterrupt):
            if cleanup_error:
                raise BootstrapError(
                    f"clone was cancelled: {url}; {cleanup_error}"
                ) from error
            raise
        message = f"failed to clone {url}: {error}"
        if cleanup_error:
            message += f"; {cleanup_error}"
        raise BootstrapError(message) from error


def remove_readonly(function, path: str, error_info) -> None:
    """Retry removal after making a Windows read-only path writable."""
    del error_info
    os.chmod(path, stat.S_IWRITE)
    function(path)


def remove_incomplete_checkout(path: Path) -> str | None:
    if not os.path.lexists(path):
        return None
    try:
        if path.is_symlink() or path.is_file():
            path.unlink()
        else:
            shutil.rmtree(path, onerror=remove_readonly)
    except OSError as error:
        return f"could not remove incomplete checkout {path}: {error}"
    return None


def prepare_depot_tools(git: str, depot_tools: Path) -> None:
    if depot_tools.exists():
        require_checkout(depot_tools, "depot_tools", "gclient.py")
        print(f"\nUsing existing depot_tools checkout: {depot_tools}")
        return
    clone_repository(
        git,
        DEPOT_TOOLS_URL,
        depot_tools,
        description="depot_tools",
        marker="gclient.py",
    )


def prepare_thorium(git: str, thorium_root: Path) -> None:
    if thorium_root.exists():
        require_checkout(thorium_root, "Thorium", "trunk.py")
        print(f"\nUsing existing Thorium checkout: {thorium_root}")
        run(
            [git, "submodule", "update", "--init", "--recursive"],
            thorium_root,
        )
        return
    clone_repository(
        git,
        THORIUM_URL,
        thorium_root,
        description="Thorium",
        marker="trunk.py",
        recursive=True,
    )


def depot_command(depot_tools: Path, name: str) -> Path:
    suffix = ".bat" if os.name == "nt" else ""
    command = depot_tools / f"{name}{suffix}"
    if not command.is_file():
        raise BootstrapError(f"depot_tools command does not exist: {command}")
    return command


def chromium_required_checkouts_exist(chromium_src: Path) -> bool:
    """Return whether the checkouts required by the Thorium workflow exist."""
    if not (chromium_src.parent / ".gclient").is_file():
        return False
    if not (chromium_src / ".git").exists():
        return False
    return all(
        (chromium_src / relative_path / ".git").exists()
        for relative_path, _ in CHROMIUM_NESTED_CHECKOUTS
    )


def remove_fetch_marker(fetch_marker: Path) -> None:
    try:
        fetch_marker.unlink(missing_ok=True)
    except OSError as error:
        raise BootstrapError(f"failed to remove {fetch_marker}: {error}") from error


def write_fetch_marker(fetch_marker: Path, operation: str) -> None:
    try:
        if fetch_marker.is_symlink():
            raise BootstrapError(
                f"refusing to replace symbolic-link fetch marker: {fetch_marker}"
            )
        fetch_marker.write_text(f"{operation}\n", encoding="utf-8")
    except OSError as error:
        raise BootstrapError(f"failed to create {fetch_marker}: {error}") from error


def read_fetch_marker(fetch_marker: Path) -> str | None:
    if not fetch_marker.is_file():
        return None
    try:
        return fetch_marker.read_text(encoding="utf-8").strip()
    except (OSError, UnicodeError) as error:
        raise BootstrapError(f"failed to read {fetch_marker}: {error}") from error


def ensure_checkout_pgo_profiles(gclient_file: Path) -> None:
    """Enable Chromium-managed optimization profile downloads in .gclient."""
    if gclient_file.is_symlink() or not gclient_file.is_file():
        raise BootstrapError(
            f"Chromium .gclient must be a regular file: {gclient_file}"
        )
    try:
        contents = gclient_file.read_bytes()
        tree = ast.parse(contents.decode("utf-8"), filename=str(gclient_file))
    except (OSError, UnicodeError, SyntaxError) as error:
        raise BootstrapError(f"could not parse {gclient_file}: {error}") from error

    solutions: ast.List | None = None
    for statement in tree.body:
        if not isinstance(statement, ast.Assign):
            continue
        if any(
            isinstance(target, ast.Name) and target.id == "solutions"
            for target in statement.targets
        ):
            if not isinstance(statement.value, ast.List):
                raise BootstrapError(
                    f"{gclient_file} has a non-list solutions assignment"
                )
            solutions = statement.value
            break
    if solutions is None:
        raise BootstrapError(f"{gclient_file} has no solutions assignment")

    custom_vars: ast.Dict | None = None
    for solution in solutions.elts:
        if not isinstance(solution, ast.Dict):
            continue
        fields = {
            key.value: value
            for key, value in zip(solution.keys, solution.values, strict=True)
            if isinstance(key, ast.Constant) and isinstance(key.value, str)
        }
        name = fields.get("name")
        if not (
            isinstance(name, ast.Constant)
            and name.value == "src"
            and "custom_vars" in fields
        ):
            continue
        candidate = fields["custom_vars"]
        if not isinstance(candidate, ast.Dict):
            raise BootstrapError(
                f"the src solution in {gclient_file} has non-dict custom_vars"
            )
        custom_vars = candidate
        break
    if custom_vars is None:
        raise BootstrapError(
            f"the src solution in {gclient_file} has no custom_vars dictionary"
        )

    profile_values: list[ast.expr] = []
    for key, value in zip(custom_vars.keys, custom_vars.values, strict=True):
        if (
            isinstance(key, ast.Constant)
            and key.value == "checkout_pgo_profiles"
        ):
            profile_values.append(value)
    if len(profile_values) > 1:
        raise BootstrapError(
            f"checkout_pgo_profiles is defined more than once in {gclient_file}"
        )
    profile_value = profile_values[0] if profile_values else None

    # AST column offsets count UTF-8 bytes, not decoded characters.
    newline = b"\r\n" if b"\r\n" in contents else b"\n"
    lines = contents.splitlines(keepends=True)
    if profile_value is not None:
        if not (
            isinstance(profile_value, ast.Constant)
            and isinstance(profile_value.value, bool)
        ):
            raise BootstrapError(
                f"checkout_pgo_profiles in {gclient_file} must be a Boolean"
            )
        if profile_value.value:
            print(
                "\nChromium optimization profile hooks are enabled in "
                f"{gclient_file}"
            )
            return
        line_index = profile_value.lineno - 1
        line = lines[line_index]
        lines[line_index] = (
            line[: profile_value.col_offset]
            + b"True"
            + line[profile_value.end_col_offset :]
        )
    elif custom_vars.lineno == custom_vars.end_lineno:
        line_index = custom_vars.lineno - 1
        line = lines[line_index]
        insertion = b'"checkout_pgo_profiles": True'
        if custom_vars.keys:
            insertion += b", "
        lines[line_index] = (
            line[: custom_vars.col_offset + 1]
            + insertion
            + line[custom_vars.col_offset + 1 :]
        )
    else:
        if custom_vars.values:
            last_value = custom_vars.values[-1]
            value_line_index = last_value.end_lineno - 1
            value_line = lines[value_line_index]
            suffix = value_line[last_value.end_col_offset :]
            if not suffix.lstrip().startswith(b","):
                lines[value_line_index] = (
                    value_line[: last_value.end_col_offset]
                    + b","
                    + value_line[last_value.end_col_offset :]
                )
        closing_index = custom_vars.end_lineno - 1
        closing_line = lines[closing_index]
        closing_column = custom_vars.end_col_offset - 1
        indentation = closing_line[:closing_column]
        lines.insert(
            closing_index,
            indentation + b'  "checkout_pgo_profiles": True,' + newline,
        )

    replacement = gclient_file.with_name(f"{gclient_file.name}.thorium-new")
    if os.path.lexists(replacement):
        raise BootstrapError(
            f"refusing to overwrite stale temporary file: {replacement}"
        )
    try:
        replacement.write_bytes(b"".join(lines))
        replacement.chmod(stat.S_IMODE(gclient_file.stat().st_mode))
        replacement.replace(gclient_file)
    except OSError as error:
        try:
            replacement.unlink(missing_ok=True)
        except OSError:
            pass
        raise BootstrapError(f"failed to update {gclient_file}: {error}") from error
    print(f"\nEnabled checkout_pgo_profiles in {gclient_file}")


def prepare_chromium(
    chromium_src: Path,
    depot_tools: Path,
    *,
    no_history: bool,
) -> str:
    state = chromium_checkout_state(chromium_src)
    fetch_marker = chromium_src.parent / FETCH_INCOMPLETE_MARKER
    if state == "existing":
        print(f"\nUsing existing Chromium checkout: {chromium_src}")
        return state
    if state == "hooks-pending":
        print(f"\nCompleting interrupted Chromium fetch: {chromium_src}")
        return "recovered"
    if state == "incomplete":
        print(f"\nRecovering incomplete Chromium checkout: {chromium_src}")
        if not (chromium_src.parent / ".gclient").is_file():
            cleanup_error = remove_incomplete_checkout(chromium_src)
            if cleanup_error:
                raise BootstrapError(cleanup_error)
        else:
            gclient = str(depot_command(depot_tools, "gclient"))
            write_fetch_marker(fetch_marker, SYNC_PHASE)
            run(
                [
                    gclient,
                    "sync",
                    "--with_branch_heads",
                    "--with_tags",
                    "--force",
                    "--reset",
                    "--nohooks",
                    "--delete_unversioned_trees",
                ],
                chromium_src.parent,
            )
            require_checkout(chromium_src, "Chromium")
            if not chromium_required_checkouts_exist(chromium_src):
                raise BootstrapError(
                    f"Chromium checkout is still incomplete after gclient sync: "
                    f"{chromium_src}"
                )
            write_fetch_marker(fetch_marker, HOOKS_PHASE)
            return "recovered"

    if chromium_src.name != "src":
        raise BootstrapError(
            "a new Chromium checkout path must end in 'src': "
            f"{chromium_src}"
        )

    checkout_root = chromium_src.parent
    try:
        checkout_root.mkdir(parents=True, exist_ok=True)
    except OSError as error:
        raise BootstrapError(
            f"failed to create Chromium checkout root {checkout_root}: {error}"
        ) from error
    write_fetch_marker(fetch_marker, FETCH_PHASE)
    command = [str(depot_command(depot_tools, "fetch")), "--nohooks"]
    if no_history:
        command.append("--no-history")
    command.append("chromium")
    print("\nDownloading Chromium source; this may take a long time.")
    run(command, checkout_root)
    require_checkout(chromium_src, "Chromium")
    if not chromium_required_checkouts_exist(chromium_src):
        raise BootstrapError(
            f"Chromium checkout is incomplete after fetch: {chromium_src}"
        )
    write_fetch_marker(fetch_marker, HOOKS_PHASE)
    return "new"


def install_linux_dependencies(chromium_src: Path, *, skip: bool) -> None:
    if skip:
        print("\nSkipping Linux build dependency installation.")
        return
    if platform.system() != "Linux":
        print("\nSkipping Debian build dependencies on non-Linux host.")
        return

    find_command("sudo")
    installer = chromium_src / "build" / "install-build-deps.sh"
    if not installer.is_file():
        raise BootstrapError(f"Chromium dependency installer is missing: {installer}")

    print("\nInstalling Debian build prerequisites.")
    run([str(installer), "--arm", "--chromeos-fonts"], chromium_src)


def confirm(args: argparse.Namespace) -> None:
    if args.yes:
        return
    if not sys.stdin.isatty():
        raise BootstrapError("interactive confirmation is unavailable; pass --yes")
    print(
        "\nThis operation may clone several large repositories and use sudo "
        "on Linux."
    )
    print(f"  depot_tools: {args.depot_tools}")
    print(f"  Thorium:     {args.thorium_root}")
    print(f"  Chromium:    {args.chromium_src}")
    if args.chromium_state == "existing":
        print(
            "  WARNING: the existing Chromium checkout and its nested "
            "repositories will be reset and cleaned."
        )
    elif args.chromium_state == "incomplete":
        print("  WARNING: the incomplete Chromium checkout will be recovered.")
    elif args.chromium_state == "hooks-pending":
        print("  The completed Chromium fetch will resume by running hooks.")
    try:
        input("Press Enter to continue, or Ctrl+C to cancel: ")
    except EOFError as error:
        raise BootstrapError("confirmation input was closed") from error


def validate_paths(
    chromium_src: Path,
    thorium_root: Path,
    depot_tools: Path,
) -> None:
    named_paths = (
        ("Chromium root", chromium_src.parent),
        ("Thorium", thorium_root),
        ("depot_tools", depot_tools),
    )
    for index, (left_name, left_path) in enumerate(named_paths):
        for right_name, right_path in named_paths[index + 1 :]:
            if left_path.is_relative_to(right_path) or right_path.is_relative_to(
                left_path
            ):
                raise BootstrapError(
                    f"checkout paths must not overlap: {left_name}={left_path}, "
                    f"{right_name}={right_path}"
                )


def resolve_and_validate_paths(args: argparse.Namespace) -> None:
    args.chromium_src = args.chromium_src.expanduser().resolve()
    args.thorium_root = args.thorium_root.expanduser().resolve()
    args.depot_tools = args.depot_tools.expanduser().resolve()
    validate_paths(args.chromium_src, args.thorium_root, args.depot_tools)
    args.chromium_state = chromium_checkout_state(args.chromium_src)
    if args.chromium_state == "existing":
        if not args.sync_existing:
            raise BootstrapError(
                f"Chromium checkout already exists: {args.chromium_src}; "
                "pass --sync-existing to explicitly allow trunk.py to reset "
                "and clean it"
            )
    elif args.chromium_state == "incomplete":
        fetch_marker = args.chromium_src.parent / FETCH_INCOMPLETE_MARKER
        if not fetch_marker.is_file() and not args.recover_incomplete:
            raise BootstrapError(
                f"Chromium checkout is incomplete: {args.chromium_src}; pass "
                "--recover-incomplete to explicitly allow gclient to reset "
                "and recover it"
            )


def chromium_checkout_state(chromium_src: Path) -> str:
    gclient_file = chromium_src.parent / ".gclient"
    fetch_marker = chromium_src.parent / FETCH_INCOMPLETE_MARKER
    source_present = os.path.lexists(chromium_src)
    gclient_present = os.path.lexists(gclient_file)
    marker_present = os.path.lexists(fetch_marker)
    if source_present and not chromium_src.is_dir():
        raise BootstrapError(
            f"Chromium source path is not a directory: {chromium_src}"
        )
    if gclient_present and not gclient_file.is_file():
        raise BootstrapError(f"Chromium .gclient is not a file: {gclient_file}")
    if marker_present and (
        fetch_marker.is_symlink() or not fetch_marker.is_file()
    ):
        raise BootstrapError(
            f"Chromium fetch marker is not a regular file: {fetch_marker}"
        )
    if not source_present and not gclient_present:
        return "absent"
    if source_present and chromium_required_checkouts_exist(chromium_src):
        marker_phase = read_fetch_marker(fetch_marker)
        if marker_phase == HOOKS_PHASE:
            return "hooks-pending"
        if marker_phase is not None:
            return "incomplete"
        return "existing"
    return "incomplete"


def bootstrap(args: argparse.Namespace) -> None:
    chromium_src = args.chromium_src
    thorium_root = args.thorium_root
    depot_tools = args.depot_tools

    if os.name != "nt":
        os.umask(0o022)

    git = find_command("git")
    prepare_depot_tools(git, depot_tools)
    prepare_thorium(git, thorium_root)

    os.environ["DEPOT_TOOLS_DIR"] = str(depot_tools)
    os.environ["PATH"] = str(depot_tools) + os.pathsep + os.environ.get("PATH", "")

    chromium_state = prepare_chromium(
        chromium_src,
        depot_tools,
        no_history=args.no_history,
    )
    ensure_checkout_pgo_profiles(chromium_src.parent / ".gclient")
    install_linux_dependencies(chromium_src, skip=args.skip_build_deps)

    if chromium_state in ("new", "recovered"):
        print("\nRunning Chromium hooks.")
        run([str(depot_command(depot_tools, "gclient")), "runhooks"], chromium_src)
        remove_fetch_marker(chromium_src.parent / FETCH_INCOMPLETE_MARKER)
    else:
        trunk = thorium_root / "trunk.py"
        print("\nResetting and synchronizing the existing Chromium checkout.")
        run(
            [
                sys.executable,
                str(trunk),
                f"--chromium-src={chromium_src}",
                f"--thorium-root={thorium_root}",
                f"--depot-tools={depot_tools}",
            ],
            thorium_root,
        )

    print("\nBootstrap completed.")
    print(f"Chromium source: {chromium_src}")
    print(f"Thorium source:  {thorium_root}")
    print(f"depot_tools:     {depot_tools}")
    if os.name == "nt":
        print(f"Add {depot_tools} to your user PATH if it is not already present.")
    else:
        print(f'Add this to your shell profile: export PATH="$PATH:{depot_tools}"')
    print(
        f'Next step: "{sys.executable}" "{thorium_root / "version.py"}" '
        f'--depot-tools="{depot_tools}"'
    )


def main(argv: Sequence[str] | None = None) -> int:
    if sys.version_info < (3, 11):
        print("error: Python 3.11 or newer is required", file=sys.stderr)
        return 2
    if platform.system() not in ("Linux", "Darwin", "Windows"):
        print("error: only Linux, macOS, and Windows are supported", file=sys.stderr)
        return 2

    args = parse_args(sys.argv[1:] if argv is None else argv)
    try:
        resolve_and_validate_paths(args)
        confirm(args)
        bootstrap(args)
    except BootstrapError as error:
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
    raise SystemExit(main())
