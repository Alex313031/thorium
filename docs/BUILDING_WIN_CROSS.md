# Cross-Compiling Thorium for Windows on Linux &nbsp;<img src="https://github.com/Alex313031/thorium/blob/main/logos/NEW/build_light.svg#gh-dark-mode-only" width="48"> <img src="https://github.com/Alex313031/thorium/blob/main/logos/NEW/build_dark.svg#gh-light-mode-only" width="48">

As many Thorium developers are on Linux/Mac, cross-compiling Thorium for
Windows targets facilitates development for Windows targets on non-Windows
machines.

It's possible to build most parts of the codebase on a Linux or Mac host while
targeting Windows. It's also possible to run the locally-built binaries on
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
First make sure you've followed the instructions for getting the Chromium and Thorium code from [HERE](https://github.com/Alex313031/thorium/blob/main/docs/BUILDING.md#get-the-code).

__IMPORTANT__
Also make sure you have run `./trunk.sh` , `./VERSION.sh` ,and `./setup.sh` to setup and copy the Thorium code over the Chromium tree as per [HERE](https://github.com/Alex313031/thorium/blob/main/docs/BUILDING.md#setting-up-the-build).

## *.gclient* setup

1. Tell gclient that you need Windows build dependencies by adding
   `target_os = ['win']` to the end of your `.gclient` file present in *~/chromium/*.  (If you already
   have a `target_os` line in there, just add `'win'` to the list.) e.g.

       solutions = [
         {
           ...
         }
       ]
       target_os = ['linux', 'win']

2. Run `./trunk.sh`, and follow instructions on screen.

### Installing the MSVS Artifacts Archive

Download the latest MSVS Artifacts Archive from [HERE](https://github.com/Alex313031/Snippets/releases/latest). \
Then, make a subdir in *chromium* called win, i.e. `mkdir ~/chromium/win`, and then place the .zip file in there.

Then, to use the
generated file on a Linux or Mac host, the following environment variables
need to be set, so add these lines to your `.bashrc` or `.zshrc`.

    export DEPOT_TOOLS_WIN_TOOLCHAIN_BASE_URL=<base url>
    export GYP_MSVS_HASH_<toolchain hash>=<hash value>

`<base url>` is the full path of the directory containing the .zip file, i.e. */home/alex/chromium/win/80909eccbb.zip*

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

### Generating a MSVS Artifacts Archive yourself

After installing [Microsoft's development tools](https://github.com/Alex313031/thorium/blob/main/docs/BUILDING_WIN.md#visual-studio),
you can package your Windows SDK installation into a zip file by running the following on a Windows machine in cmd.exe:

```shell
    cd path/to/depot_tools/win_toolchain
    python3 package_from_installed.py <vs version> -w <win version>
```

Where "path/to/depot_tools/win_toolchain" would usually be *C:\src\depot_tools\win_toolchain* and
where `<vs version>` and `<win version>` correspond respectively to the
versions of Visual Studio (e.g. 2019) and of the Windows SDK (e.g.
10.0.20348.0) installed on the Windows machine. Note that if you didn't
install the ARM64 components of the SDK as noted in the link above, you
should add `--noarm` to the parameter list.

__NOTE__: The -w flag takes the raw internal version of the SDK, not the actual patch version. This caused me and I'm sure
other people some confusion. So for example, the latest version of the Win 10 SDK is 10.1.20348.1, but its internal version doesn't change from
when it was released, and this is what you would use, e.g. 10.0.20348.0

These commands create a zip file named `<hash value>.zip`, which can be used with the instructions above ^.

## Building
Follow [Setting up the build](https://github.com/Alex313031/thorium/blob/main/docs/BUILDING.md#setting-up-the-build), except instead of using the `args.gn` from the
root of the Thorium repo, use the [`win_args.gn`](https://github.com/Alex313031/thorium/blob/main/infra/win_args.gn), from *~/Thorium/infra/*

Then, to build, run `./build_win.sh`, instead of `./build.sh`. See > [Here](https://github.com/Alex313031/thorium/blob/main/docs/BUILDING.md#build-thorium-).

*Happy Thorium Building!*

<img src="https://github.com/Alex313031/thorium/blob/main/logos/STAGING/Thorium90_504.jpg" width="200">
