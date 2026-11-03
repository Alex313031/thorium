#!/usr/bin/env python3

# Copyright (c) 2026 Alex313031 and gz83.

"""Check out and prepare Chromium for the current Thorium version."""

import argparse
import os
import platform
from pathlib import Path
import shlex
import shutil
import subprocess
import sys
from typing import Sequence

from chromium_sync import CheckoutError, prepare_dependencies, prepare_git_checkout


EXIT_FAILURE = 111
THORIUM_VERSION = "156.0.8078.80"


class VersionError(RuntimeError):
    """An expected Chromium checkout or synchronization failure."""


def environment_path(value: str) -> Path:
    return Path(os.path.expandvars(value)).expanduser()


def default_chromium_src() -> Path:
    configured = os.environ.get("CR_DIR")
    if configured:
        return environment_path(configured)
    if os.name == "nt":
        return Path("C:/src/chromium/src")
    return Path.home() / "chromium" / "src"


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


def sysroot_architectures() -> tuple[str, ...]:
    return ("amd64", "arm64") if sys.platform.startswith("linux") else ()


def parse_args(argv: Sequence[str]) -> argparse.Namespace:
    parser = argparse.ArgumentParser(
        description="Check out the Chromium tag used by the current Thorium version."
    )
    parser.add_argument(
        "--chromium-src",
        type=environment_path,
        default=default_chromium_src(),
        metavar="PATH",
        help="Chromium src directory (default: CR_DIR or the platform default)",
    )
    parser.add_argument(
        "--depot-tools",
        type=environment_path,
        default=default_depot_tools(),
        metavar="PATH",
        help=(
            "depot_tools directory (default: DEPOT_TOOLS_DIR, the gclient "
            "location, or the platform default)"
        ),
    )
    return parser.parse_args(argv)


def find_command(name: str) -> str:
    executable = shutil.which(name)
    if executable is None:
        raise VersionError(
            f"required command '{name}' was not found in PATH; "
            "ensure depot_tools and Git are installed"
        )
    return executable


def depot_command(depot_tools: Path, name: str) -> str:
    suffix = ".bat" if os.name == "nt" else ""
    command = depot_tools / f"{name}{suffix}"
    if not command.is_file():
        raise VersionError(f"depot_tools command does not exist: {command}")
    return str(command)


def run(command: Sequence[str], cwd: Path) -> None:
    printable = (
        subprocess.list2cmdline(command) if os.name == "nt" else shlex.join(command)
    )
    print(f"\n[{cwd}] {printable}", flush=True)
    try:
        subprocess.run(command, cwd=cwd, check=True)
    except OSError as error:
        raise VersionError(f"could not run {printable}: {error}") from error
    except subprocess.CalledProcessError as error:
        raise VersionError(
            f"command failed with exit code {error.returncode}: {printable}"
        ) from error


def require_directory(path: Path, description: str) -> None:
    if not path.is_dir():
        raise VersionError(f"{description} directory does not exist: {path}")


def require_checkout(path: Path, description: str) -> None:
    require_directory(path, description)
    if not (path / ".git").exists():
        raise VersionError(f"{description} is not a Git checkout: {path}")


def require_file(path: Path, description: str) -> None:
    if not path.is_file():
        raise VersionError(f"{description} does not exist: {path}")


def prepare_checkout(
    chromium_src: Path,
    depot_tools: Path,
) -> None:
    chromium_src = chromium_src.expanduser().resolve()
    depot_tools = depot_tools.expanduser().resolve()

    os.environ["DEPOT_TOOLS_DIR"] = str(depot_tools)
    os.environ["PATH"] = str(depot_tools) + os.pathsep + os.environ.get("PATH", "")

    require_checkout(chromium_src, "Chromium")
    git = find_command("git")
    gclient = depot_command(depot_tools, "gclient")

    print(f"\nCurrent Thorium version is: {THORIUM_VERSION}\n")
    print(f"Checking out tags/{THORIUM_VERSION} in {chromium_src}")

    prepare_git_checkout(git, chromium_src)
    run([git, "checkout", "-f", f"tags/{THORIUM_VERSION}"], chromium_src)
    run([git, "clean", "-ffd"], chromium_src)

    prepare_dependencies(gclient, git, chromium_src, run)

    print("\ngclient sync", flush=True)
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
        chromium_src,
    )
    run([git, "clean", "-ffd"], chromium_src)

    print("\ngclient runhooks", flush=True)
    run([gclient, "runhooks"], chromium_src)

    sysroot_arches = sysroot_architectures()
    if sysroot_arches:
        print("\nInstalling Linux sysroots for AMD64 and ARM64", flush=True)
        sysroot_installer = (
            chromium_src / "build" / "linux" / "sysroot_scripts" / "install-sysroot.py"
        )
        require_file(sysroot_installer, "Linux sysroot installer")
        for architecture in sysroot_arches:
            run(
                [sys.executable, str(sysroot_installer), f"--arch={architecture}"],
                chromium_src,
            )
    else:
        print("\nSkipping Linux sysroots on non-Linux host")

    print(f"\nChromium tree is checked out at tag: {THORIUM_VERSION}")
    print("\nDone! You can now run 'setup.py'.")


def main(argv: Sequence[str] | None = None) -> int:
    if sys.version_info < (3, 11):
        print("error: Python 3.11 or newer is required", file=sys.stderr)
        return 2
    if platform.system() not in ("Linux", "Darwin", "Windows"):
        print("error: only Linux, macOS, and Windows are supported", file=sys.stderr)
        return 2

    args = parse_args(sys.argv[1:] if argv is None else argv)
    try:
        prepare_checkout(args.chromium_src, args.depot_tools)
    except (VersionError, CheckoutError) as error:
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
