# Checking out and Building Thorium for Windows &nbsp;<img src="https://github.com/Alex313031/thorium/blob/main/logos/NEW/build_light.svg#gh-dark-mode-only" width="48"> <img src="https://github.com/Alex313031/thorium/blob/main/logos/NEW/build_dark.svg#gh-light-mode-only" width="48">

## System Requirements

* A 64-bit machine with at least 8GB of RAM. More than 16GB is highly
  recommended.
* At least 75GB of free disk space on an NTFS-formatted hard drive. FAT32
  will not work, as some of the Git packfiles are larger than 4GB.
* An appropriate version of Visual Studio, as described below.
* Windows 10 1809 or newer.

## Setting up Windows

### Visual Studio

Chromium requires [Visual Studio 2019](https://visualstudio.microsoft.com/vs/older-downloads/) (>=16.0.0)
to build, but [Visual Studio 2022](https://visualstudio.microsoft.com/vs/) (>=17.0.0)
is preferred. Visual Studio can also be used to debug Chromium, and version 2022 is
preferred for this as it handles Chromium's large debug information much better.
The *clang-cl* compiler is used, but Visual Studio's header files, libraries, and
some tools are required. Visual Studio Community Edition will work. You must install the "Desktop development with
C++" component and the "MFC/ATL support" sub-components. This can be done from
the command line by passing these arguments to the Visual Studio installer (see
below for ARM64 instructions):
```shell
VisualStudioSetup.exe --add Microsoft.VisualStudio.Workload.NativeDesktop --add Microsoft.VisualStudio.Component.VC.ATLMFC --includeRecommended
```

If you want to build for Windows on ARM64 then some extra arguments are needed. The
full set for that case is:
```shell
VisualStudioSetup.exe --add Microsoft.VisualStudio.Workload.NativeDesktop --add Microsoft.VisualStudio.Component.VC.ATLMFC --add Microsoft.VisualStudio.Component.VC.Tools.ARM64 --add Microsoft.VisualStudio.Component.VC.MFC.ARM64 --includeRecommended
```

 - You must have the version 10.1.22621.755 [Windows 11 SDK](https://developer.microsoft.com/en-us/windows/downloads/sdk-archive/)
installed. This can be installed separately or by checking the appropriate box
in the Visual Studio Installer __(Note that MSVS 2022 will try to install the 22000 version by default, uncheck this and check the 22621 version)__.
There is also experimental support for the Windows 11 10.1.22000.755 version.

The SDK Debugging Tools must also be installed. If the Windows 10 SDK was
installed via the Visual Studio installer, then they can be installed by going
to: Control Panel → Programs → Programs and Features → Select the "Windows
Software Development Kit" → Change → Change → Check "Debugging Tools For
Windows" → Change. Or, you can download the standalone SDK installer and use it
to install the Debugging Tools.

## Install `depot_tools`

Download the [depot_tools bundle](https://storage.googleapis.com/chrome-infra/depot_tools.zip)
and extract it to *C:\src\depot_tools*.

***note
__Warning:__ __*DO NOT*__ use drag-n-drop or copy-n-paste extract from Explorer,
this will not extract the hidden “.git” folder which is necessary for
depot_tools to autoupdate itself. You can use “Extract all…” from the
context menu, or 7-Zip though.
***

Add depot_tools to the start of your PATH (must be ahead of any installs of
Python). Note that environment variable names are case insensitive.

Assuming you unzipped the bundle to *C:\src\depot_tools*, open:

Control Panel → System and Security → System → Advanced system settings

If you have Administrator access, Modify the PATH system variable and
put `C:\src\depot_tools` at the front, by clicking "Move Up". (Or at least in front of any directory
that might already have a copy of Python or Git).

If you don't have Administrator access, you can add a user-level PATH
environment variable by opening:

Control Panel → System and Security → System → Search for "Edit environment variables for your account"

Add `C:\src\depot_tools` at the front. Note: If your system PATH has a Python in it, you will be out of luck.

Also, add a `DEPOT_TOOLS_WIN_TOOLCHAIN` environment variable in the same way, and set
it to __0__. This tells depot_tools to use your locally installed version of Visual
Studio (by default, depot_tools will try to use a google-internal version).

You should also set the variable `vs2019_install` or
`vs2022_install` to your installation path of Visual Studio 19 or 22, like
`vs2019_install` = __C:\Program Files (x86)\Microsoft Visual Studio\2019\Community__
for Visual Studio 2019, or
`vs2022_install` = __C:\Program Files\Microsoft Visual Studio\2022\Community__
for Visual Studio 2022.

Once all of this is done, we will download some infra archives using `gclient`. \
From a cmd.exe shell, run:

```shell
gclient
```

On first run, gclient will install all the Windows-specific bits needed to work
with the code, including msysgit and Python.

* If you run gclient from a non-cmd shell (e.g., cygwin, PowerShell),
  it may appear to run properly, but msysgit, python, and other tools
  may not get installed correctly.
* If you see strange errors with the file system on the first run of gclient,
  you may want to [disable Windows Indexing](https://tortoisesvn.net/faq.html#cantmove2).

## Check Python install

After running gclient open a command prompt and type `where python` and
confirm that the depot_tools `python.bat` comes ahead of any copies of
python.exe. Failing to ensure this can lead to overbuilding when
using gn - see [crbug.com/611087](https://crbug.com/611087).

[App Execution Aliases](https://docs.microsoft.com/en-us/windows/apps/desktop/modernize/desktop-to-uwp-extensions#alias) in Windows 10/11
can conflict with other installations of python on the system so disable
these for 'python.exe' and 'python3.exe' by opening 'App execution aliases'
section of Control Panel and unticking the boxes next to both of these
that point to 'App Installer'.

## Downloading the Chromium code

First, configure Git:

```shell
git config --global user.name "My Name"
git config --global user.email "my-name@chromium.org"
git config --global core.autocrlf false
git config --global core.filemode false
git config --global branch.autosetuprebase always
```

Create a `chromium` directory in *C:\src* for the checkout and change to it.

```shell
cd / && cd src && mkdir chromium && cd chromium
```

Run the `fetch` tool from `depot_tools` to check out the code and its
dependencies.

```shell
fetch chromium
```

If you don't want the full repo history, you can save a lot of time by
adding the `--no-history` flag to `fetch`.

Expect the command to take 30 minutes on even a fast connection, and many
hours on slower ones.

When `fetch` completes, it will have created a hidden `.gclient` file and a
directory called `src` in the working directory. The remaining instructions
assume you have switched to this directory (i.e. *C:\src\chromium\src*):

```shell
cd src
```

*Optional*: You can also [build with API
keys](https://www.chromium.org/developers/how-tos/api-keys) if you want your
build to talk to some Google services like Google Sync, Translate, and GeoLocation.&nbsp;<img src="https://github.com/Alex313031/thorium/blob/main/logos/NEW/Key_Light.svg#gh-dark-mode-only" width="26"> <img src="https://github.com/Alex313031/thorium/blob/main/logos/NEW/Key_Dark.svg#gh-light-mode-only" width="26">&nbsp;Thorium has its own keys in a private repository, if you are a builder or would like access to them, contact me. Otherwise, for personal or development builds, 
you can create your own keys and add yourself to [google-browser-signin-testaccounts](https://groups.google.com/u/1/a/chromium.org/g/google-browser-signin-testaccounts)
to enable Sync.

##  Downloading the Thorium code

You can either use git clone, or download a .zip from the repo. It should be placed side by side with the Chromium directory in *C:\src*. \
Using Git:

```shell
git clone https://github.com/Alex313031/thorium.git
```
Or download the .zip (Make sure to rename the extracted dir to just thorium, not thorium-main).
[https://github.com/Alex313031/thorium/archive/refs/heads/main.zip](https://github.com/Alex313031/thorium/archive/refs/heads/main.zip)

## Setting up the build

First, we need to check out the revision that Chromium is currently using. For this, run `version.bat`. \
Secondly, we need to copy the Thorium source files over the Chromium tree. \
Run the `setup.bat` script in *Thorium\win_scripts* to automate this.

```shell
cd C:\src\Thorium\win_scripts && setup.bat
```
It will drop you back to *C:\src\chromium\src*, which is where the rest of the commands will be carried out.

Next, we will download the PGO profile for Thorium, which changes with every revision. Run:

```shell
python3 tools\update_pgo_profiles.py --target=win64 update --gs-url-base=chromium-optimization-profiles/pgo_profiles
```
This will download a *.profdata file, looking something like `chrome-win64-main-1659409120-058034bd778fed227d12a29fd0edd0942810dbf8.profdata`. \
Take note of this, as we will be using it in the `args.gn` below.

### Creating the build directory
Chromium & Thorium use [Ninja](https://ninja-build.org) as its main build tool along with
a tool called [GN](https://gn.googlesource.com/gn/+/main/docs/quick_start.md) to generate `.ninja` files. Create the build directory by running:

```shell
gn args out\thorium
```

This will open up notepad.exe, and this is where we will specify build arguments ("args") which direct Ninja on how to lay out the build directory tree.
We will be copy/pasting the contents of the [win_args.gn](https://github.com/Alex313031/thorium/blob/main/infra/win_args.gn) file (from *C:\src\Thorium\infra\win_args.gn*) into notepad.
Notice the three lines at the top, related to API Keys. It is fine to leave them blank, or add the ones you have made. \
__At the bottom__, though, notice the line that says *pgo_data_path = ""*. This is where we will put the full path to the PGO profile data file we downloaded earlier.

That line should look something like:

`pgo_data_path = "C:\src\chromium\src\chrome\build\pgo_profiles\chrome-win64-main-1659409120-058034bd778fed227d12a29fd0edd0942810dbf8.profdata"`

* For other build arguments, and what the ones that Thorium uses do, see [ABOUT_GN_ARGS.md](https://github.com/Alex313031/thorium/blob/main/infra/DEBUG/ABOUT_GN_ARGS.md) & [win_args.list](https://github.com/Alex313031/thorium/blob/main/infra/win_args.list)
* For more info on GN, run `gn help` on the command line or read the [quick
  start guide](https://gn.googlesource.com/gn/+/main/docs/quick_start.md).

## Build Thorium

Build Thorium, and the other things like [chromedriver](https://chromedriver.chromium.org/home) and [thorium_shell](https://github.com/Alex313031/thorium/tree/main/thorium_shell#readme) with Ninja using the command:

```shell
autoninja -C out\thorium chrome chromedriver thorium_shell setup mini_installer -j8
```
(Where -j# can be any number, and should generally be set to the number of cores on your CPU).

`autoninja` is a wrapper that automatically provides optimal values for the
arguments passed to `ninja`.

You can get a list of all of the other build targets from GN by running
`gn ls out\thorium` from the command line. To compile one, pass to Ninja
the GN label with no preceding "//" (so for `//chrome/test:unit_tests`
use autoninja -C out/Default chrome/test:unit_tests).

## Install/Run Thorium

Once it is built, you can simply install the browser.

```shell
out\thorium\mini_installer.exe
```

## Update your checkout

To update an existing Chromium checkout, you should run the `trunk.bat` script in win_scripts:

```shell
cd C:\src\Thorium\win_scripts && trunk.bat
```
(This script will also download the latest PGO profile data file at the end; useful when making fresh builds.)

This syncs the subrepositories to the appropriate versions,
deleting those that are no longer needed, and re-runs the hooks as needed.

To update an existing Thorium checkout, just download the latest .zip, or do a git pull:

```shell
git pull origin main
```

*Happy Thorium Building!*

<img src="https://github.com/Alex313031/thorium/blob/main/logos/STAGING/Thorium90_504.jpg" width="200">
