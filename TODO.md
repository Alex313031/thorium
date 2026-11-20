# TODO

Actionable bugs and feature requests are tracked in
[GitHub Issues](https://github.com/Alex313031/thorium/issues). This file records
cross-cutting product decisions, implementation work, and verification tasks,
whether or not they already have a dedicated issue.

## Product decisions

- [ ] Re-evaluate Session Restore disk writes
  despite [Thorium issue #61](https://github.com/Alex313031/thorium/issues/61)
  being closed as not planned.
  Measure the current Chromium baseline's Session Restore write frequency
  before deciding whether to adjust the save interval or offer an option that
  disables persistent session writes.

- [ ] Decide whether UA Client Hints should advertise an additional Chrome
  brand. The traditional User-Agent already uses `Chrome/<version>`; consider
  both website compatibility and accurate product identification.

- [ ] Decide whether Thorium should enable the upstream autoplay settings UI.
  First attempt to reproduce its historical Profile Picker crash on the
  current Chromium baseline and document the result.

- [ ] Decide whether desktop Web Audio contexts should honor `--no-autoplay`.
  The current `allow-webaudio-autoplay.patch` bypasses their autoplay
  user-gesture checks regardless of this switch or `--autoplay-policy`.
  Treat any change to that behavior as a separate product decision.

## Implementation

- [ ] Redesign Thorium keyboard shortcuts before restoring the temporarily
  removed `keyboard_shortcuts.patch`. Preserve the original customization
  goals: Ctrl/Cmd+Shift+D/P/K for duplicating the current tab, moving selected
  tabs to a new window, and opening a tab after the current tab. The D binding
  replaces "Bookmark all tabs"; retain that command in menus. Preserve the
  intended ChromeOS Search+Left/Right snapping, Search+Down minimize,
  Search+Up maximize/restore, and Alt+Search+- floating toggle. Reassess
  conflicting bindings and platform scope, including the original non-macOS
  Ctrl+Shift+Q exit binding; keep ChromeOS session sign-out and confirmation
  owned by Ash. The original Windows/Linux system-print remap to Ctrl+Alt+P
  freed Ctrl+Shift+P for tab movement. Keep system printing accessible while
  reassessing that combination for keyboard-layout conflicts, without
  disabling upstream modifier checks. Use event modifier flags such as
  `ui::EF_COMMAND_DOWN`, rather than key codes such as `ui::VKEY_LWIN`, in
  accelerator modifier fields. Integrate with Ash's active shortcut
  configuration, navigation rewrites, and user remappings. Keep browser and
  print-preview bindings and labels consistent. Cover macOS menu registration
  and Android's separate Java shortcut handling, and synchronize ChromeOS
  Shortcuts/settings metadata. Record platform runtime verification before
  restoring the patch. Until then, keep Chromium's upstream shortcut behavior.

- [ ] Make local `create_dmg.py` output names match the architecture-specific
  DMG filenames already produced by
  [the macOS packaging workflow](.github/workflows/package-macos-dmg.yml):
  `Thorium_MacOS_x64.dmg`, `Thorium_MacOS_ARM64.dmg`, and
  `Thorium_MacOS_Universal.dmg`. The script currently writes
  `Thorium_MacOS.dmg`; keep the workflow's input path and rename step in sync
  when changing the script's output.

- [ ] Add user-configurable disk-cache directory support and a Settings UI
  ([Thorium issue #860](https://github.com/Alex313031/thorium/issues/860)).
  M158's `chrome/browser/net/disk_cache_dir_util.cc` rejects user-controlled
  `prefs::kDiskCacheDir` values, so writing that preference from a new UI alone
  is insufficient. Extend the backend to support a user-selected path while
  preserving managed-policy and `--disk-cache-dir` precedence over that choice.
  Validate the selected directory and explain that the new location takes
  effect after restart. Store the setting in Local State for all profiles, do
  not automatically migrate or delete the old cache, and make the UI read-only
  or clearly indicate when managed policy or `--disk-cache-dir` controls the
  effective path.

- [ ] Make `DownloadShelfView` propagate item preferred-height changes to
  `BrowserView` and size the shelf from the tallest visible item. Verify that
  the top and item separators remain continuous during window resizing and
  transitions between normal, warning, and deep-scanning modes.

- [ ] Add Thorium-specific desktop app-menu commands:
  - Restart Thorium using `chrome::AttemptRestart()` while retaining Chromium's
    normal session-restoration semantics;
  - Report issue, opening Thorium's GitHub issue template;
  - What's new, opening Thorium release notes rather than Google Chrome-branded
    content.

## Verification and benchmarking

- [ ] Visually verify the Omnibox and bookmark bar with Thorium 2024 enabled and
  disabled. Cover the Omnibox outline, input icon and text, Views suggestion
  popup alignment, and bookmark bar underline with custom themes, keyword mode,
  permission chips, `classic-omnibox`, and 100%/125%/150% display scaling. If
  an issue exists, record the tested Thorium version, platform, theme, display
  scale, expected appearance, and result before changing the current color,
  border, or alignment handling; open a dedicated issue with screenshots for
  any failure.

- [ ] Establish a reproducible benchmark comparing non-optimized Thorium,
  optimized Thorium, Chromium, and Google Chrome based on the same full
  Chromium version where available. Define non-optimized Thorium from the same
  Thorium source and feature patches by disabling only the compiler
  optimizations under measurement. Keep the hardware, workload, profile, and
  number of runs consistent. Archive the complete `args.gn` for locally built
  binaries and record unavoidable differences in revision, toolchain, PGO
  configuration, branding, and proprietary components.

- [ ] Verify restored-window top drag and resize hit targets with Thorium 2024
  UI on Windows, Linux/X11, Linux/Wayland, and macOS. Confirm that tabs and
  caption buttons remain vertically aligned and that the top edge remains
  resizable.

- [ ] Run browser-level verification of Thorium's `--no-autoplay` behavior on
  every platform where Thorium distributes the switch, using
  [Supermium issue #1412](https://github.com/win32ss/supermium/issues/1412) as
  background context. Source-level behavior has been reviewed and flag
  descriptions updated; browser runtime results remain unverified:

  - confirm that initially locked audible HTML media without user activation
    are blocked in test cases without applicable policy or permission
    exemptions; test exemptions separately rather than expecting all autoplay
    to be blocked;
  - confirm that muted video continues to follow Chromium's muted-autoplay
    rules;
  - confirm that an explicit `--autoplay-policy` takes precedence over
    `--no-autoplay`;
  - confirm the current desktop `AudioContext` user-gesture exemption without
    treating it as a guarantee of playback during prerendering or context
    interruption, or as an exemption for connected HTML media elements.
