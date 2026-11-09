#!/usr/bin/env python3
# Copyright (c) 2026 Alex313031 and gz83.
"""Check or apply the Thorium patch series."""

import argparse
import dataclasses
import os
import subprocess
import sys
import tempfile
from pathlib import Path


@dataclasses.dataclass(frozen=True)
class SeriesEntry:
    line_no: int
    conditions: tuple[str, ...]
    apply_root: str
    patch_path: str


def repo_root_from_script() -> Path:
    return Path(__file__).resolve().parents[2]


def env_path(value: str) -> Path:
    return Path(os.path.expandvars(value)).expanduser()


def default_thorium_root() -> Path:
    if os.environ.get("THOR_DIR"):
        return env_path(os.environ["THOR_DIR"])
    return repo_root_from_script()


def default_source_tree() -> Path:
    for name in ("CR_DIR", "CR_SRC_DIR", "CHROMIUM_SRC", "CHROMIUM_SRC_DIR"):
        value = os.environ.get(name)
        if value:
            return env_path(value)
    if os.name == "nt":
        return Path(r"C:\src\chromium\src")
    return Path.home() / "chromium" / "src"


def parse_series(path: Path) -> list[SeriesEntry]:
    entries: list[SeriesEntry] = []
    for line_no, raw in enumerate(path.read_text(encoding="utf-8").splitlines(), 1):
        line = raw.split("#", 1)[0].strip()
        if not line:
            continue

        conditions: list[str] = []
        while line.startswith("["):
            end = line.find("]")
            if end == -1:
                raise ValueError(f"{path}:{line_no}: unterminated condition")
            conditions.extend(
                item.strip().lower()
                for item in line[1:end].split(",")
                if item.strip()
            )
            line = line[end + 1 :].strip()

        apply_root = "."
        patch_path = line
        if ":" in line and not line.startswith("other/"):
            apply_root, patch_path = (item.strip() for item in line.split(":", 1))
            if not apply_root or not patch_path:
                raise ValueError(f"{path}:{line_no}: invalid apply root syntax")

        entries.append(
            SeriesEntry(
                line_no=line_no,
                conditions=tuple(conditions),
                apply_root=apply_root,
                patch_path=patch_path.replace("\\", "/"),
            )
        )
    return entries


def selected(entry: SeriesEntry, enabled_conditions: set[str]) -> bool:
    if not entry.conditions:
        return True
    return set(entry.conditions).issubset(enabled_conditions)


def run_git(apply_dir: Path, args: list[str], env: dict[str, str] | None = None) -> int:
    cmd = ["git", "-C", str(apply_dir), *args]
    return subprocess.run(cmd, env=env).returncode


def git_output(apply_dir: Path, args: list[str]) -> str:
    cmd = ["git", "-C", str(apply_dir), *args]
    return subprocess.check_output(cmd, text=True, stderr=subprocess.DEVNULL).strip()


def git_apply_command(apply_dir: Path, patch: Path, args: list[str]) -> list[str]:
    repo_root = Path(git_output(apply_dir, ["rev-parse", "--show-toplevel"]))
    cmd = ["git", "-C", str(repo_root), "apply", *args]
    relative_apply_dir = apply_dir.resolve().relative_to(repo_root.resolve())
    if str(relative_apply_dir) != ".":
        cmd.append(f"--directory={relative_apply_dir.as_posix()}")
    cmd.append(str(patch))
    return cmd


def git_apply_check(apply_dir: Path, patch: Path, reverse: bool = False) -> bool:
    args = ["--check"]
    if reverse:
        args.append("--reverse")
    cmd = git_apply_command(apply_dir, patch, args)
    return subprocess.run(cmd, stdout=subprocess.DEVNULL, stderr=subprocess.DEVNULL).returncode == 0


def prepare_temp_index(
    apply_dir: Path,
    temp_dir: Path,
    indexes: dict[str, Path],
    envs: dict[Path, dict[str, str]],
) -> dict[str, str]:
    cached_env = envs.get(apply_dir)
    if cached_env is not None:
        return cached_env

    repo_root = git_output(apply_dir, ["rev-parse", "--show-toplevel"])
    index = indexes.get(repo_root)
    if index is None:
        index = temp_dir / f"index-{len(indexes)}"
        indexes[repo_root] = index
        env = os.environ.copy()
        env["GIT_INDEX_FILE"] = str(index)
        if run_git(apply_dir, ["read-tree", "HEAD"], env=env) != 0:
            raise RuntimeError(f"failed to initialize temporary index for {repo_root}")

    env = os.environ.copy()
    env["GIT_INDEX_FILE"] = str(index)
    envs[apply_dir] = env
    return env


def git_apply_cached(apply_dir: Path, patch: Path, env: dict[str, str]) -> int:
    return subprocess.run(
        git_apply_command(apply_dir, patch, ["--cached"]),
        env=env,
    ).returncode


def git_apply(apply_dir: Path, patch: Path) -> int:
    cmd = git_apply_command(apply_dir, patch, ["--reject"])
    return subprocess.run(cmd).returncode


def validate_entries(entries: list[SeriesEntry], thorium_root: Path, source_tree: Path) -> list[str]:
    errors: list[str] = []
    seen: set[tuple[str, str, tuple[str, ...]]] = set()
    for entry in entries:
        key = (entry.apply_root, entry.patch_path, entry.conditions)
        if key in seen:
            errors.append(f"duplicate entry at line {entry.line_no}: {entry.patch_path}")
        seen.add(key)

        patch = thorium_root / entry.patch_path
        if not patch.is_file():
            errors.append(f"missing patch at line {entry.line_no}: {entry.patch_path}")

        apply_dir = source_tree / entry.apply_root
        if not apply_dir.is_dir():
            errors.append(f"missing apply root at line {entry.line_no}: {entry.apply_root}")
    return errors


def main(argv: list[str]) -> int:
    default_root = default_thorium_root()
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--thorium-root", type=Path, default=default_root, help="Thorium repository root. Defaults to THOR_DIR or the repository containing this script.")
    parser.add_argument(
        "--source-tree",
        type=Path,
        default=None,
        help="Chromium source tree. Defaults to CR_DIR, CR_SRC_DIR, CHROMIUM_SRC, or CHROMIUM_SRC_DIR; then falls back to C:\\src\\chromium\\src on Windows or ~/chromium/src elsewhere.",
    )
    parser.add_argument("--series", type=Path, default=default_root / "patch_scripts" / "series" / "series")
    parser.add_argument("--condition", action="append", default=[], help="Enable one conditional entry, e.g. raspi or sse2.")
    parser.add_argument("--apply", action="store_true", help="Apply patches with git apply --reject.")
    args = parser.parse_args(argv)
    if len(args.condition) > 1:
        parser.error("only one --condition value is supported per run")

    thorium_root = args.thorium_root.resolve()
    source_tree = (args.source_tree or default_source_tree()).resolve()
    series_path = args.series if args.series.is_absolute() else thorium_root / args.series

    entries = parse_series(series_path)
    enabled_conditions = {item.lower() for item in args.condition}
    entries = [entry for entry in entries if selected(entry, enabled_conditions)]

    errors = validate_entries(entries, thorium_root, source_tree)
    if errors:
        for error in errors:
            print(f"error: {error}", file=sys.stderr)
        return 2

    if not args.apply:
        with tempfile.TemporaryDirectory(prefix="thorium-series-index-") as temp:
            temp_dir = Path(temp)
            indexes: dict[str, Path] = {}
            envs: dict[Path, dict[str, str]] = {}
            for entry in entries:
                patch = (thorium_root / entry.patch_path).resolve()
                apply_dir = (source_tree / entry.apply_root).resolve()
                label = f"{entry.apply_root}: {entry.patch_path}" if entry.apply_root != "." else entry.patch_path
                env = prepare_temp_index(apply_dir, temp_dir, indexes, envs)

                if git_apply_cached(apply_dir, patch, env) == 0:
                    print(f"check: {label}")
                    continue

                print(f"failed: {label}", file=sys.stderr)
                return 1
        return 0

    for entry in entries:
        patch = (thorium_root / entry.patch_path).resolve()
        apply_dir = (source_tree / entry.apply_root).resolve()
        label = f"{entry.apply_root}: {entry.patch_path}" if entry.apply_root != "." else entry.patch_path

        if git_apply_check(apply_dir, patch):
            print(f"apply: {label}")
            if git_apply(apply_dir, patch) != 0:
                return 1
            continue

        if git_apply_check(apply_dir, patch, reverse=True):
            print(f"already applied: {label}")
            continue

        print(f"failed: {label}", file=sys.stderr)
        return 1

    return 0


if __name__ == "__main__":
    raise SystemExit(main(sys.argv[1:]))
