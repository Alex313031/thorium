#!/usr/bin/env bash
# Only used on the workflow's disposable GitHub-hosted Ubuntu runner.
set -euo pipefail
[[ "${GITHUB_ACTIONS:-}" == true && "${RUNNER_ENVIRONMENT:-}" == github-hosted ]]
[[ "$(uname -sm)" == 'Linux x86_64' ]]
# Remove unrelated preinstalled SDKs, never Chromium source or build outputs.
sudo rm -rf /usr/local/lib/android /usr/share/dotnet /usr/share/swift /opt/ghc /usr/local/.ghcup /opt/hostedtoolcache/CodeQL
sudo apt-get update
# Bootstrap/archive/packaging tools only. Chromium's own versioned installer
# supplies its host libraries after source extraction, and on every restore.
sudo apt-get install -y --no-install-recommends \
  build-essential binutils git lsb-release ca-certificates pkg-config cpio rpm dpkg-dev fakeroot \
  advancecomp icoutils optipng pngcrush \
  python3 python3-setuptools python3-jinja2 python3-six python3-protobuf python3-yaml \
  xz-utils zstd unzip zip curl wget file desktop-file-utils imagemagick squashfs-tools
sudo apt-get clean
df -h "$RUNNER_TEMP"
