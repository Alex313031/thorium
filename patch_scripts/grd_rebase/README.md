# GRD/XTB Rebase

This directory contains the reviewed Thorium GRD/GRDP and XTB rebase tooling
for moving Thorium string changes out of the overlay and into repeatable
scripts.

The runtime surface is intentionally small:

- `sync_grd_strings.py` updates reviewed Chromium GRD/GRDP messages, computes
  old and new GRIT translation IDs, and copies compatible upstream XTB
  translations to the new Thorium IDs.
- `merge_thorium_xtb.py` merges reviewed Thorium-owned translation additions
  from `config/xtb_additions.tsv` into Chromium XTB bundles.
- `update_config_from_patches.py` refreshes low-risk config rows that can be
  derived from the current patch series.

These scripts use only the Python standard library. They do not require
`vpython`, `depot_tools`, or a Chromium checkout's Python wrapper. Python 3.11
or newer is the supported runtime.

## Configuration

The files in `config/` are reviewed inputs, not generated setup output:

- `file_allowlist.csv`: reviewed GRD/GRDP file scope and file ownership role.
  `from_overlay` records the legacy source of the reviewed change; pure
  `overlay_text_sync` files do not need to remain under `src/` once their
  messages are covered by automatic branding discovery or
  `message_allowlist.csv`.
- `message_allowlist.csv`: reviewed message-level exceptions and special
  replacements. Plain branding replacements are auto-discovered from
  `file_allowlist.csv` text-sync files; this CSV only keeps special rows.
  The three current overrides cover desktop Dino instructions, the
  accessibility label, and the slower-speed toggle.
- `feature_patch_message_ownership.csv`: feature-patch and overlay-added
  message ownership; used to prevent feature-patch strings from being handled
  by the overlay replacement workflow. Pass a non-default ownership file to
  `sync_grd_strings.py` with `--feature-message-ownership`.
- `xtb_additions.tsv`: canonical reviewed translation additions; currently
  4374 translation rows across 405 XTB files. They form 54 owner/bundle/message
  groups, each covering all 81 supported translation targets. Chromium bundles
  may use different locale aliases for the same language, such as `he` or
  `iw` for Hebrew. Rows are grouped by the explicit `owner` column, then by
  bundle, translation ID, and locale. An owner may be a feature patch path or
  a stable GRD rebase workflow identifier, such as
  `grd_rebase:branding:IDS_VERSION_UI_LICENSE`; translations produced by
  branding synchronization do not need a synthetic or historical patch owner.
  `source_path` separately records where each translation was recovered or
  reviewed. It is audit provenance only and may use a stable reviewed source
  identifier instead of a historical Git commit; it does not restrict which
  Chromium checkout receives the translation. The `chromium-upstream:` prefix
  identifies translations recovered from Chromium-owned XTB data without
  binding them to a particular Chromium milestone. The following path and
  message or translation ID retain the concrete audit source. No runtime tool
  parses this prefix or uses it to select translations. Pass an alternate
  reviewed inventory to `merge_thorium_xtb.py` with `--inventory`.

`update_config_from_patches.py` may rewrite
`config/feature_patch_message_ownership.csv` and
`config/file_allowlist.csv` by default. It does not rewrite
`message_allowlist.csv` or `xtb_additions.tsv`; those remain reviewed
inputs because they contain special text behavior or translation-data
decisions.

## Run Order

Run the scripts after Chromium and non-string feature patches are in place:

1. Run `sync_grd_strings.py`.
2. Run `merge_thorium_xtb.py`.

This order keeps overlay-derived old/new ID syncing separate from reviewed
Thorium-owned additions.

## Python Runtime

Use any Python 3.11+ interpreter available on the host:

```shell
python3 patch_scripts/grd_rebase/sync_grd_strings.py --help
python3 patch_scripts/grd_rebase/merge_thorium_xtb.py --help
```

On Windows, either `py -3.11`, a normal `python.exe`, or
`C:\src\depot_tools\python3.bat` can be used. The depot_tools wrapper is only a
convenient Chromium-environment Python, not a requirement for these scripts.

All config paths stored in this directory use repository-relative POSIX-style
paths. Command-line paths may use native platform separators or `/`; the scripts
normalize them internally where needed.

## Dry Run

Dry-run low-risk config refresh:

```shell
python3 patch_scripts/grd_rebase/update_config_from_patches.py --dry-run
```

Dry-run the overlay string sync and write compact audit summaries:

```shell
mkdir -p out/grd_rebase

python3 patch_scripts/grd_rebase/sync_grd_strings.py \
  /path/to/chromium/src \
  --file-allowlist patch_scripts/grd_rebase/config/file_allowlist.csv \
  --message-allowlist patch_scripts/grd_rebase/config/message_allowlist.csv \
  --dry-run \
  --xtb-conflict-report out/grd_rebase/xtb_conflicts_summary.tsv \
  --xtb-missing-report out/grd_rebase/xtb_missing_summary.tsv \
  > out/grd_rebase/grd_sync_dry_run.tsv
```

Dry-run the reviewed additions merge:

```shell
python3 patch_scripts/grd_rebase/merge_thorium_xtb.py \
  /path/to/chromium/src \
  --dry-run
```

The summary always reports the validated inventory size and then classifies
each row according to the supplied Chromium worktree, for example:

```text
validated 4374 Thorium translations across 405 XTB files: <inserted> inserted, <refreshed> refreshed, <already-present> already present, <changed-files> files changed
```

Only the first two counts are inventory invariants. The inserted, refreshed,
already-present, and changed-file counts vary with the Chromium revision and
the current state of its worktree.

Equivalent PowerShell form:

```powershell
New-Item -ItemType Directory -Force out/grd_rebase | Out-Null

py -3.11 patch_scripts/grd_rebase/sync_grd_strings.py `
  C:\src\chromium\src `
  --file-allowlist patch_scripts/grd_rebase/config/file_allowlist.csv `
  --message-allowlist patch_scripts/grd_rebase/config/message_allowlist.csv `
  --dry-run `
  --xtb-conflict-report out/grd_rebase/xtb_conflicts_summary.tsv `
  --xtb-missing-report out/grd_rebase/xtb_missing_summary.tsv `
  > out/grd_rebase/grd_sync_dry_run.tsv

py -3.11 patch_scripts/grd_rebase/merge_thorium_xtb.py `
  C:\src\chromium\src `
  --dry-run
```

## Apply

Refresh low-risk config from the current patch series:

```shell
python3 patch_scripts/grd_rebase/update_config_from_patches.py
```

Apply overlay GRD/GRDP replacements and copied XTB translations:

```shell
python3 patch_scripts/grd_rebase/sync_grd_strings.py \
  /path/to/chromium/src \
  --file-allowlist patch_scripts/grd_rebase/config/file_allowlist.csv \
  --message-allowlist patch_scripts/grd_rebase/config/message_allowlist.csv
```

Apply reviewed XTB additions:

```shell
python3 patch_scripts/grd_rebase/merge_thorium_xtb.py \
  /path/to/chromium/src
```

All apply operations are designed to be idempotent.

## GRIT ID Notes

`sync_grd_strings.py` contains a lightweight GRIT message ID replica for
auto-discovered branding messages and reviewed special messages. It matches
Chromium's `GenerateMessageId()` fingerprint and meaning-combination behavior:

- MD5 first 64 bits interpreted as signed.
- Optional `meaning` fingerprint combined with the message fingerprint.
- The high bit is stripped to produce a positive decimal ID.
- `use_name_for_id="true"` returns the message name.
- `<ph name="...">` uses the placeholder presentation/name in presentable
  content.

The replica is intentionally scoped to reviewed text-sync files plus explicit
special messages. If future special entries include conditional message bodies
that need platform-specific active-branch resolution, compare against Chromium
GRIT parser output before enabling them.

## Reports

`sync_grd_strings.py` can write compact audit reports:

- `--xtb-conflict-report`: summarized converged new-ID conflicts where multiple
  old translations map to the same new ID. Reviewed global or locale-specific
  preferences select the appropriate old translation where required. For
  remaining conflicts, the script deterministically keeps the first candidate
  and reports grouped review buckets instead of every locale row.
- `--xtb-missing-report`: summarized mapped XTB lookups where the old Chromium
  translation ID was not found. Missing translations are reported but do not
  block the run.

Current dry-runs may report converged XTB conflicts. Review the grouped conflict
report before accepting the deterministically selected candidates. A reviewed
locale-specific preference affects only that locale; other locales in the same
converged-ID group retain their normal selection and remain visible in the
report when they still conflict. The missing report is expected to contain only
its header. Any data row means that a new ID did not receive an upstream
translation and must be resolved in `xtb_additions.tsv` before release.

## Validation

Basic syntax checks:

```shell
python3 -m py_compile \
  patch_scripts/grd_rebase/update_config_from_patches.py \
  patch_scripts/grd_rebase/sync_grd_strings.py \
  patch_scripts/grd_rebase/merge_thorium_xtb.py
```

Behavior validation should run the dry-run command and inspect the compact
reports:

- conflict summary should be grouped into a small number of review buckets;
- missing summary should contain only its header; if it contains data rows,
  each row should group all missing locales for one message and must be
  resolved before release.

The full dry-run TSV is useful for script refactors and spot checks, but it is
not intended to be reviewed line by line.
