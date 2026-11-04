#!/usr/bin/env python3
"""Prepare the locked full Chromium tarball for the selected build profile."""

import argparse
import bz2
import json
import os
from pathlib import Path
import platform
import re
import shutil
import subprocess
import sys
import time

from common import (ROOT, build_environment, checked_path, configuration, download,
                    input_identity, profile_name, run, sha256, summary, unpack, write_json)
from install_host_deps import install as install_host_dependencies, save_installer


def install_dependencies(work, cache, sources):
    records = []
    for item in sources["dependencies"]:
        path = checked_path(work, item["path"])
        probes = item.get("bundled_files", {})
        if probes and all((path / name if name != "." else path).is_file() and
                          sha256(path / name if name != "." else path) == expected
                          for name, expected in probes.items()):
            print(f"Using verified tools from full tarball: {item['path']}", flush=True)
            records.append({**item, "provided_by": "source_archive"})
            continue
        archive = download(item, cache)
        if item["kind"] == "bz2":
            path.parent.mkdir(parents=True, exist_ok=True)
            with bz2.open(archive, "rb") as src, path.open("wb") as out:
                shutil.copyfileobj(src, out)
        elif item["kind"] == "file":
            path.parent.mkdir(parents=True, exist_ok=True)
            shutil.copyfile(archive, path)
            if item.get("executable"):
                path.chmod(0o755)
        else:
            unpack(archive, path, item["kind"])
        records.append(item)
        # Archives are reproducible inputs, not required for resuming compilation.
        archive.unlink()
    return records


def configure_tree(work, sources, profile):
    src = work / "src"
    if sha256(src / "DEPS") != sources["deps_sha256"]:
        raise ValueError("source DEPS does not match the locked .80 dependency set")
    # The official full export contains the correct compiler but an empty
    # cr_build_revision. Check the expected revision against the locked source
    # script before generating the stamp normally produced by the clang hook.
    update_script = (src / "tools/clang/scripts/update.py").read_text()
    revision = re.search(r"^CLANG_REVISION = '([^']+)'$", update_script, re.M)[1]
    subrevision = re.search(r"^CLANG_SUB_REVISION = (\d+)$", update_script, re.M)[1]
    if f"{revision}-{subrevision}" != sources["clang_package_version"]:
        raise ValueError("Clang source and dependency lock disagree")
    (src / "third_party/llvm-build/Release+Asserts/cr_build_revision").write_text(
        sources["clang_package_version"] + "\n")
    lastchange = src / "build/util/LASTCHANGE"
    if sources["chromium_revision"] not in lastchange.read_text():
        raise ValueError("tarball LASTCHANGE does not contain the locked Chromium revision")
    for name in ("build/util/LASTCHANGE.committime",):
        if not (src / name).is_file():
            raise ValueError(f"archive is missing required version metadata: {name}")
    position = re.search(r"-refs/(heads/main|branch-heads/\d+)@\{#(\d+)\}", lastchange.read_text())
    if not position:
        raise ValueError("tarball LASTCHANGE has no usable commit position")
    (src / "build/util/LASTCHANGE_commit_position.h").write_text(
        "/* Generated from the verified tarball LASTCHANGE. */\n"
        "#ifndef BUILD_UTIL_LASTCHANGE_COMMIT_POSITION_H_\n"
        "#define BUILD_UTIL_LASTCHANGE_COMMIT_POSITION_H_\n"
        f"#define CHROMIUM_COMMIT_POSITION_IS_MAIN {int(position[1] == 'heads/main')}\n"
        f'#define CHROMIUM_COMMIT_POSITION_NUMBER "{position[2]}"\n'
        "#endif\n")

    # gclient is never executed. Its wrappers need only the source-root marker.
    (work / ".gclient").write_text("solutions = [{'name': 'src', 'url': None, 'managed': False}]\n")
    (work / ".gclient_entries").write_text("entries = {'src': None}\n")
    depot = work / "depot_tools"
    (depot / "python-bin").mkdir(exist_ok=True)
    python = depot / "python-bin/python3"
    if not python.exists():
        python.symlink_to(sys.executable)
    (depot / ".disable_auto_update").touch()
    env = build_environment(work)

    # Parse the locked DEPS with depot_tools' own restricted parser. Only emit
    # gclient's GN variables; do not run sync, hooks, or dependency cloning.
    sys.path.insert(0, str(depot))
    import gclient_eval
    parsed = gclient_eval.Parse((src / "DEPS").read_text(), str(src / "DEPS"))
    variables = dict(parsed["vars"])
    variables.update({"checkout_linux": True, "checkout_win": False,
                      "checkout_mac": False, "checkout_android": profile["target_os"] == "android",
                      "host_os": "linux", "host_cpu": "x64"})
    gn_values = []
    for key in parsed["gclient_gn_args"]:
        value = variables[key]
        if isinstance(value, gclient_eval.ConstantString):
            value = value.value
        elif isinstance(value, str):
            value = gclient_eval.EvaluateCondition(value, variables)
        gn_values.append(f"{key} = {json.dumps(value)}")
    (src / "build/config/gclient_args.gni").write_text(
        "# Generated from the locked DEPS for archive builds on a Linux x64 host.\n" +
        "\n".join(gn_values) + "\n")

    # Siso still imports a backend module in offline mode. Empty properties
    # explicitly avoid selecting a remote backend or requiring credentials.
    siso = src / "build/config/siso"
    (siso / ".sisoenv").write_text("SISO_REAPI_INSTANCE=\n")
    (siso / "backend_config/backend.star").write_text(
        'load("@builtin//struct.star", "module")\n'
        'def properties(ctx):\n    return {"default": {}, "large": {}}\n'
        'backend = module("backend", platform_properties = properties)\n')
    setup_args = [] if profile["setup_profile"] == "default" else ["--" + profile["setup_profile"]]
    run([sys.executable, ROOT / "setup.py", "--chromium-src", src,
         "--thorium-root", ROOT, *setup_args, "--archive",
         "--expected-version", sources["chromium_version"]], env=env)
    out = src / profile["out_dir"]
    # Preserve the versioned args exactly. The locked Chromium defaults select
    # local Siso for a fresh output tree with .sisoenv and build_with_chromium.
    # Check the effective values below rather than silently overriding them.
    shutil.copyfile(ROOT / profile["args_file"], out / "args.gn")
    gn = src / "buildtools/linux64/gn"
    run([gn, "gen", out, "--fail-on-unused-args"], cwd=src, env=env)
    expected = {**profile["expected_gn_args"], "target_os": profile["target_os"],
                "target_cpu": profile["target_cpu"], "use_siso": True,
                "use_remoteexec": False, "use_reclient": False}
    for name, value in expected.items():
        value = json.dumps(value)
        actual = subprocess.check_output([str(gn), "args", str(out),
                                          f"--list={name}", "--short"],
                                         cwd=src, env=env, text=True).strip()
        if actual != f"{name} = {value}":
            raise ValueError(f"unexpected GN argument: {actual}; expected {name} = {value}")
    if profile.get("required_build_tools"):
        # Check the actual dependency graph, not the declared host_toolchain
        # argument (whose empty default is resolved later by BUILDCONFIG).
        dependencies = subprocess.check_output(
            [str(gn), "desc", str(out), "//:thorium_all", "deps", "--all"],
            cwd=src, env=env, text=True)
        labels = {line.strip() for line in dependencies.splitlines()}
        missing = set(profile["required_build_tools"]) - labels
        if missing:
            raise ValueError(f"required host build tools are missing from the GN graph: {sorted(missing)}")
    run([sys.executable, ROOT / "build.py", "--chromium-src", src,
         "--expect-os", profile["target_os"], "--expect-cpu", profile["target_cpu"],
         "--dry-run"], env=env)
    # Avoid `siso version`, which may query CIPD over the network for metadata.
    backend_version = next(item["revision"] for item in sources["dependencies"]
                           if item["path"] == "src/third_party/siso/cipd")
    write_json(work / "manifest.json", {
        "identity": input_identity(), "work_root": str(work), "status": "checkpointed",
        "phase_index": 0, "targets": profile["targets"], "stage": 0,
        "args_sha256": sha256(out / "args.gn"), "backend_version": backend_version,
        "runner_image": os.environ.get("ImageVersion", "local"),
        "dependencies": sources["dependencies"], "runtime_tests": "not_run",
        "created_at": int(time.time()), "resumed_stages": 0,
    })


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--work-root", required=True, type=Path)
    parser.add_argument("--cache", required=True, type=Path)
    parser.add_argument("--source-archive", type=Path, help="Use an already downloaded, locked tarball")
    parser.add_argument("--install-host-deps", action="store_true",
                        help="Install Chromium's host packages on a GitHub-hosted Linux x64 runner")
    args = parser.parse_args()
    if platform.system() != "Linux" or platform.machine() not in ("x86_64", "AMD64"):
        raise ValueError("this profile requires a Linux x64 build host")
    sources, profile = configuration()
    work, cache = args.work_root.resolve(), args.cache.resolve()
    if work.exists():
        raise ValueError("work root already exists; use a fresh directory or restore a checkpoint")
    if cache.is_relative_to(work):
        raise ValueError("download cache must be outside the checkpoint tree")
    work.mkdir(parents=True)
    source = args.source_archive or download(sources["chromium"], cache)
    if source.stat().st_size != sources["chromium"]["size_bytes"] or sha256(source) != sources["chromium"]["sha256"]:
        raise ValueError("source archive size or SHA-256 mismatch")
    unpack(source, work)
    (work / sources["chromium"]["root"]).rename(work / "src")
    if not args.source_archive:
        source.unlink()
    # Thorium patches this file during setup. Preserve the original so every
    # runner uses the same verified upstream package policy after restoration.
    save_installer(work, sources)
    if args.install_host_deps:
        install_host_dependencies(work, sources, profile, "prepare")
    write_json(work / "dependencies.json", install_dependencies(work, cache, sources))
    configure_tree(work, sources, profile)
    summary(f"Source preparation complete: {profile_name()}, full tarball, no additional pruning, local Siso execution.")


if __name__ == "__main__":
    main()
