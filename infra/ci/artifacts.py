#!/usr/bin/env python3
"""Validate manual resume provenance and retire only superseded checkpoints."""

import argparse
import json
import os
import re
import urllib.request

from common import configuration, output, profile_name, summary


def artifact_prefix(kind):
    configuration()
    return f"thorium-{kind}-{profile_name()}-"


def api(path, method="GET"):
    repository = os.environ["GITHUB_REPOSITORY"]
    url = f"{os.environ.get('GITHUB_API_URL', 'https://api.github.com')}/repos/{repository}/{path}"
    request = urllib.request.Request(url, method=method, headers={
        "Authorization": "Bearer " + os.environ["GH_TOKEN"],
        "Accept": "application/vnd.github+json", "X-GitHub-Api-Version": "2022-11-28"})
    with urllib.request.urlopen(request, timeout=60) as response:
        data = response.read()
    return json.loads(data) if data else None


def validate(run_id, artifact_id):
    if not re.fullmatch(r"[1-9][0-9]*", run_id) or not re.fullmatch(r"[1-9][0-9]*", artifact_id):
        raise ValueError("resume requires positive numeric run and artifact IDs")
    run = api(f"actions/runs/{run_id}")
    _, profile = configuration()
    platform = {"win": "windows", "mac": "macos"}.get(profile["target_os"], profile["target_os"])
    workflow = f".github/workflows/build-{platform}.yml"
    if (run["head_sha"] != os.environ["GITHUB_SHA"] or
            run["event"] != "workflow_dispatch" or
            run["path"] != workflow or
            run["head_repository"]["full_name"] != os.environ["GITHUB_REPOSITORY"]):
        raise ValueError("resume must come from this workflow, repository and exact commit")
    artifact = api(f"actions/artifacts/{artifact_id}")
    if artifact["expired"] or artifact["workflow_run"]["id"] != int(run_id):
        raise ValueError("artifact expired or belongs to another run")
    name = artifact["name"]
    if re.fullmatch(re.escape(artifact_prefix("checkpoint")) + run_id + r"-[1-9][0-9]*-[0-9]+", name):
        status = "checkpointed"
    elif re.fullmatch(re.escape(artifact_prefix("build-inputs")) + run_id + r"-[1-9][0-9]*-[0-9]+", name):
        status = "complete"
    else:
        raise ValueError("artifact is not a checkpoint or completed input for this profile and run")
    return artifact, status


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("command", choices=("resume", "retire"))
    parser.add_argument("--run-id", required=True)
    parser.add_argument("--artifact-id", required=True)
    args = parser.parse_args()
    artifact, status = validate(args.run_id, args.artifact_id)
    if args.command == "resume":
        output("status", status)
        output("artifact-id", args.artifact_id)
        output("run-id", args.run_id)
        summary(f"Using artifact `{artifact['name']}` ({args.artifact_id}) with verified provenance; content digests will be checked during restore.")
    else:
        # Never remove the manually selected checkpoint of an earlier run.
        if args.run_id != os.environ["GITHUB_RUN_ID"] or status != "checkpointed":
            raise ValueError("only a superseded checkpoint in the current run may be retired")
        api(f"actions/artifacts/{args.artifact_id}", method="DELETE")
        print(f"Retired checkpoint {args.artifact_id}")


if __name__ == "__main__":
    main()
