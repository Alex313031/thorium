# Cross-Compiling Thorium for Windows on Linux

As many Chromium developers are on Linux/Mac, cross-compiling Chromium for
Windows targets facilitates development for Windows targets on non-Windows
machines.

It's possible to build most parts of the codebase on a Linux or Mac host while
targeting Windows.  It's also possible to run the locally-built binaries on
swarming.  This document describes how to set that up, and current restrictions.

## Limitations

What does *not* work:

* `js2gtest` tests are omitted from the build ([bug](https://crbug.com/1010561))
* on Mac hosts, 32-bit builds don't work ([bug](https://crbug.com/794838) has
  more information, and this is unlikely to ever change)

All other targets build fine (including `chrome`, `thorium_shell`, etc...).

Uses of `.asm` files have been stubbed out.  As a result, Crashpad cannot
report crashes, and NaCl defaults to disabled and cannot be enabled in cross
builds ([.asm bug](https://crbug.com/762167)).

## Setup
First make sure you've followed the instructions for getting the Chromium and Thorium code from [HERE](https://github.com/Alex313031/Thorium/blob/main/docs/BUILDING.md#get-the-code).
Also make sure you have run `setup.sh` to copy the Thorium code over the Chromium tree.

## .gclient setup

1. Tell gclient that you need Windows build dependencies by adding
   `target_os = ['win']` to the end of your `.gclient` file present in *~/chromium/*.  (If you already
   have a `target_os` line in there, just add `'win'` to the list.) e.g.

       solutions = [
         {
           ...
         }
       ]
       target_os = ['linux', 'win']

2. Run a `trunk.sh`, follow instructions on screen.

### Installing the MSVS Artifacts Archive

Download the latest MSVS Artifacts Archive from [HERE](https://github.com/Alex313031/Snippets/releases/latest).
Then, make a subdir in *chromium* called win, i.e. `mkdir ~/chromium/win`, and then place the .zip file in there.

Then add these lines to your `.bashrc` or `.zshrc`.

    export DEPOT_TOOLS_WIN_TOOLCHAIN_BASE_URL=/home/USER/chromium/win
    export GYP_MSVS_HASH_<toolchain hash>=<hash value>

`<base url>` is the path of the directory containing the zip file (note that
specifying scheme `file://` is not required).

`<toolchain hash>` is hardcoded in `src/build/vs_toolchain.py` and can be found by
setting `DEPOT_TOOLS_WIN_TOOLCHAIN_BASE_URL` and running `gclient runhooks`:

    gclient runhooks
    ...
    Running hooks:  17% (11/64) win_toolchain
    ________ running '/usr/bin/python src/build/vs_toolchain.py update --force' in <chromium dir>
    Windows toolchain out of date or doesn't exist, updating (Pro)...
    current_hashes:
    desired_hash: <toolchain hash>

`<hash value>` is the name of the .zip, without .zip at the end, i.e. `80909eccbb`
## Building
Follow (Setting up the build)[https://github.com/Alex313031/Thorium/blob/main/docs/BUILDING.md#setting-up-the-build], except instead of using the `args.gn` from the
root of the Thorium repo, use the `[win_args.gn](https://github.com/Alex313031/Thorium/blob/main/infra/win_args.gn)`, from *~/Thorium/infra/*

Then, to build, run `build_win.sh`, instead of `build.sh`. See > [Here](https://github.com/Alex313031/Thorium/blob/main/docs/BUILDING.md#build-thorium-).
