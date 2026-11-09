# macOS native smoke tests

[`smoke_test.py`](smoke_test.py) is the native macOS test harness used by
[the macOS smoke-test workflow](../../.github/workflows/macos-smoke-test.yml).
It validates packaged `Thorium.app` bundles from x64 and ARM64 DMGs, including
applications cross-built on Linux. It does not compile Chromium or create DMGs;
see the [macOS build and packaging guide](../../docs/BUILDING_MAC.md) for those
steps.

The workflow compares both architectures at the same application version. Each
job uses a native runner and verifies:

- the optional DMG SHA-256 matches and the disk image passes verification;
- the host architecture and macOS major version match the selected target,
  without Rosetta translation;
- the application has the expected version, valid code signatures, and the
  correct architecture in every Mach-O payload;
- headless and normal-window Thorium can start with fresh profiles;
- JavaScript, typed arrays, BigInt, regular expressions, and WebAssembly execute
  on a local test page;
- the normal browser window and startup renderer respond through DevTools;
- a local HTTP form accepts input, submits with Enter, and loads the expected
  result while its renderer remains responsive;
- normal-window Thorium can submit a Baidu search and verify the result page,
  subject to the external-service handling described below;
- `chrome://version` identifies the expected running version, and
  `chrome://newtab` and a final local page remain responsive.

WebGL shader compilation, drawing and pixel readback, `OfflineAudioContext`
rendering, and navigation to Google are recorded as additional diagnostics.
Page and desktop screenshots are attempted independently.

The DMG is mounted read-only and `Thorium.app` is copied with `ditto` into a
temporary directory. The harness preserves the package's original signature
and quarantine attributes. The mount, copied application and temporary profiles
are removed during cleanup.

The baseline probes retain normal extension, GPU and sandbox settings. They
use a DevTools pipe, mock Keychain access, stderr logging, and suppress the
first-run and default-browser prompts. Python 3.11+ and Node.js 22+ are required;
there is no npm dependency, ChromeDriver requirement or debugging TCP port.

The local interaction check serves `navigation.html` on `127.0.0.1`, enters
`hello` through CDP, and submits the form with Enter. Success requires the
server to receive `/search?q=hello`, the page to display the expected result,
and JavaScript to remain responsive. The server is closed after the check.

The Baidu check opens the homepage, focuses a visible search field, enters
`hello` and submits with Enter. It requires a loaded `/s` result URL with
`wd=hello`, actual result content, and a responsive renderer after a short
observation period. Screenshots, full HTML snapshots, input inventories and
step results are retained when available. CAPTCHA is recorded without being
bypassed.

## Results and diagnostic runs

| Status | Meaning |
| --- | --- |
| `PASS` | The check completed and met its assertions. |
| `FAIL` | An assertion failed, a required command timed out, or a crash or unresponsive renderer was detected. |
| `INCOMPLETE` | The check was not reached or could not be verified, including a blocked external page or missing search input/results. |

An overall `PASS` requires both baselines, startup responsiveness and local
input/navigation to pass. The Baidu search must also pass unless it encounters
a verified CAPTCHA or network block while its renderer still responds. In
that case the search remains `INCOMPLETE`, and `external_blocked: true` records
the exception. Missing input fields or result content do not qualify for this
exception. An overall smoke pass therefore does not guarantee a successful
external search.

WebGL and audio assertion failures are diagnostic and do not determine the
overall result. Each is checked independently after navigation, so a WebGL
failure does not skip audio. Required CDP timeouts, renderer hangs and crashes
during these checks still fail the baseline. A Google network failure alone
also remains diagnostic when the renderer is responsive.

After a GUI baseline `FAIL`, `gui-repeat` first repeats the GUI probe with
the same browser arguments and a fresh profile. This checks whether a second
launch succeeds without changing browser settings, for example after system
services have warmed up. A passing repeat does not establish the cause of the
original failure.

After this repeat, if applicable, three separate diagnostic runs add only
`--disable-extensions`, `--disable-gpu`, or `--no-sandbox`, each with a fresh
profile. They repeat the failing baseline mode; headless is selected if both
baselines failed. When only headless fails, these flag diagnostics run without
a GUI repeat. No diagnostic pass overrides a baseline failure. An `INCOMPLETE`
result alone does not trigger diagnostic runs.

GUI testing still runs after a headless failure. Each browser probe has a
five-minute deadline and bounded cleanup. Detected renderer crashes, early
browser exits and crash files fail the baseline even if an earlier page
rendered successfully. Exit codes are `0` for `PASS`, `1` for `FAIL`, and `2`
for `INCOMPLETE`. Missing reports cannot produce a successful comparison.

## Reading the reports

Each `macos-smoke-<os>-<arch>-<run>-<attempt>` artifact contains:

- `summary.json` and `summary.md` with baseline and diagnostic results;
- `input.json`, the DMG hash, bundle identity, actual OS/CPU/display details,
  Mach-O architectures, UUIDs and load commands;
- code-signature, entitlement and extended-attribute inspection output;
- separate probe directories with browser logs, DevTools events, startup,
  navigation, search and capability results;
- available page/desktop screenshots, Baidu HTML snapshots and input inventories;
- `gui/gpu-info.json` with GPU devices, feature status and driver workarounds,
  when available;
- Crashpad dumps, fallback profile crash files and new Thorium system reports;
- process lists and attempted `sample` thread stacks collected before stopping
  a failing browser. Sampling errors are retained if macOS denies access.

The final comparison shows the actual OS and application version for each
target, alongside core, interaction and diagnostic results, including the GUI
repeat. It compares reports from the current run attempt. Start a new manual
run or re-run all jobs to collect both architectures together; re-running only
failed jobs may leave the other architecture's report in an earlier attempt.
Use the package hash and Framework UUID to distinguish rebuilds that share a
version number. Matching dSYMs from the exact binary build are required to
symbolize crashes.

Hosted images do not reproduce every physical Mac or macOS patch release.
Runner provisioning failures occur before browser testing and provide no
evidence of a browser failure. Screenshot availability depends on desktop
capture permissions; a captured image is not automatically judged nonblank.
CDP responsiveness does not verify native toolbar/menu input, and a WebGL pass
does not establish physical GPU acceleration. Media hardware paths, Widevine
playback and human-visible UI quality require separate testing.

## Select the test environment

Set `macos_version` explicitly when starting a run. The hosted workflow
supports these choices, with `15` as the default:

| `macos_version` | Native x64 runner | Native ARM64 runner |
| --- | --- | --- |
| `15` | `macos-15-intel` | `macos-15` |
| `26` | `macos-26-intel` | `macos-26` |

Both jobs test the selected OS major version and continue independently when
the other target fails. Hosted choices follow [GitHub's runner availability](https://github.com/actions/runner-images#available-images).
They do not restrict direct harness runs on a compatible local or self-hosted
Mac.

Set `expected_version` to the four-part version inside both application bundles,
without a release-tag prefix. A release tag may use a different name. Review
the form's version and source inputs for each build instead of relying on saved
or default values.

## Test artifacts from another Actions run

The producing workflow must upload each DMG at the root of a separate artifact:

| Artifact | File |
| --- | --- |
| `thorium-macos-x64` | `Thorium_MacOS_x64.dmg` |
| `thorium-macos-arm64` | `Thorium_MacOS_ARM64.dmg` |

Run **Compare macOS x64 and ARM64 artifacts** manually and provide:

```text
source_run_id: numeric ID of the producing run
artifact_prefix: thorium-macos
current_run_artifact: false
release_tag: leave empty
x64_asset: Thorium_MacOS_x64.dmg
arm64_asset: Thorium_MacOS_ARM64.dmg
expected_version: four-part application version
macos_version: supported OS choice from the workflow
x64_sha256: optional expected DMG digest
arm64_sha256: optional expected DMG digest
```

The run must belong to the same repository. `artifact_prefix` changes the
common artifact prefix; `x64_asset` and `arm64_asset` must match the exact DMG
filenames. Do not include source trees or build directories in these artifacts.

## Call it after a build

A producing workflow can upload both DMGs and call the reusable comparison.
In this example, the build job exposes the application version as an output:

```yaml
jobs:
  build:
    # Build and upload thorium-macos-x64 and thorium-macos-arm64.
    # Expose the four-part application version as outputs.version.
    # ...

  smoke-test:
    needs: build
    permissions:
      actions: read
      contents: read
    uses: ./.github/workflows/macos-smoke-test.yml
    with:
      current_run_artifact: true
      expected_version: ${{ needs.build.outputs.version }}
      # Set macos_version to a supported choice for this build.
```

## Test DMGs from a release

Upload both DMGs to a release in the workflow's repository, then select its
exact tag in `release_tag`. Leave `source_run_id` empty and
`current_run_artifact` false. Asset inputs are exact filenames, not URLs or
wildcards. The workflow accepts exactly one input source per run.

For a local build, create a temporary draft release. Replace the repository,
paths, application version and OS choice below before running the commands on
Linux:

```shell
repo=OWNER/REPOSITORY
x64_dmg=/path/to/Thorium_MacOS_x64.dmg
arm64_dmg=/path/to/Thorium_MacOS_ARM64.dmg
version=MAJOR.MINOR.BUILD.PATCH
macos_version=SUPPORTED_OS_MAJOR
tag=macos-smoke-$(date +%Y%m%d-%H%M%S)
x64_sha256=$(sha256sum "$x64_dmg" | cut -d' ' -f1)
arm64_sha256=$(sha256sum "$arm64_dmg" | cut -d' ' -f1)

gh release create "$tag" --repo "$repo" --draft \
  --title "$tag" --notes "Temporary macOS smoke input"
gh release upload "$tag" "$x64_dmg" "$arm64_dmg" --repo "$repo"
gh workflow run macos-smoke-test.yml --repo "$repo" \
  -f current_run_artifact=false \
  -f source_run_id= \
  -f release_tag="$tag" \
  -f x64_asset="$(basename "$x64_dmg")" \
  -f arm64_asset="$(basename "$arm64_dmg")" \
  -f expected_version="$version" \
  -f macos_version="$macos_version" \
  -f x64_sha256="$x64_sha256" \
  -f arm64_sha256="$arm64_sha256"
```

Hash verification occurs before mounting or launching the package. Supply the
digest of the newly packaged DMG when replacing a release asset at the same
version. This prevents a stale or unintended package from passing unnoticed.

## Run directly on a Mac

The harness can also test a local DMG on a Mac with the matching native
architecture. Set `version` to the application's four-part version:

```shell
version=MAJOR.MINOR.BUILD.PATCH
python3 infra/macos/smoke_test.py \
  --dmg /path/to/Thorium_MacOS_ARM64.dmg \
  --arch arm64 \
  --expected-version "$version" \
  --results /tmp/thorium-macos-report
```

For an Intel Mac, use the x64 DMG and `--arch x64`. Add `--expected-sha256`
to verify a known package digest. Only test packages you built or otherwise
trust: the harness executes their binaries on its runner.
