# Checking out and Building Thorium for Windows &nbsp;<img src="https://github.com/Alex313031/thorium/blob/main/logos/NEW/build_light.svg#gh-dark-mode-only" width="48"> <img src="https://github.com/Alex313031/thorium/blob/main/logos/NEW/build_dark.svg#gh-light-mode-only" width="48">

There are instructions for other platforms here in the Thorium Docs directory.

## System Requirements

* A 64-bit machine with at least 8GB of RAM. More than 16GB is highly
  recommended.
* At least 100GB of free disk space on an NTFS-formatted drive. FAT32
  will not work, as some of the Git packfiles are larger than 4GB.
* An appropriate version of Visual Studio, as described below.
* An appropriate version of Bash (such as Git Bash), as described below.
* Windows 10 1809 or newer.

## Setting up Windows

### Visual Studio

Chromium requires [Visual Studio 2022](https://visualstudio.microsoft.com/vs/) (>=17.0.0)
to build. Visual Studio can also be used to debug Chromium.
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

 - You must have the version 10.1.22621.2428 [Windows 11 SDK](https://developer.microsoft.com/en-us/windows/downloads/sdk-archive/)
installed. This can be installed separately or by checking the appropriate box
in the Visual Studio Installer. __(Note that MSVS 2022 will try to install the 22000 version by default, uncheck this and check the 22621 version)__.

The 10.1.22621.2428 SDK Debugging Tools must also be installed. This
version of the Debugging tools is needed in order to support reading the
large-page PDBs that Chrome uses to allow greater-than 4 GiB PDBs.
This can be installed after the matching Windows SDK version is installed, from: 
Control Panel -> Programs and Features -> Windows Software Development Kit [version] -> Change -> Debugging Tools for Windows.
Or, you can download the standalone SDK installer and use it to install the Debugging Tools. 
If building on ARM64 Windows then you will need to manually copy the
Debuggers\x64 directory from another machine because it does not get installed
on ARM64 and is needed, whether you are building Chromium for x64 or ARM64 on
ARM64.

## Install `depot_tools`

Download the [depot_tools bundle](https://storage.googleapis.com/chrome-infra/depot_tools.zip)
and extract it to *C:\src\depot_tools*.

***
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

You should also set the variable `vs2022_install` to your installation path of Visual Studio 22, like 
`vs2022_install` = __C:\Program Files\Microsoft Visual Studio\2022\Community__

Once all of this is done, we will download some infra archives using `gclient`.  
From a __cmd.exe__ shell, run:
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

## Downloading the Chromium code <a name="get-the-code"></a>

First, configure Git (you may want to backup your current .gitconfig file if it exists):

```shell
git config --global user.name "My Name"
git config --global user.email "my-name@chromium.org"
git config --global core.autocrlf false
git config --global core.filemode false
git config --global branch.autosetuprebase always
git config --global core.longpaths true
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

*Optional:* You can also [build with API keys](https://www.chromium.org/developers/how-tos/api-keys) if you want your
build to talk to some Google services like Google Sync, Translate, and GeoLocation.&nbsp;<img src="https://github.com/Alex313031/thorium/blob/main/logos/NEW/Key_Light.svg#gh-dark-mode-only" width="26"> <img src="https://github.com/Alex313031/thorium/blob/main/logos/NEW/Key_Dark.svg#gh-light-mode-only" width="26">&nbsp;Thorium has its own keys in a private repository, if you are a builder or would like access to them, contact me. Otherwise, for personal or development builds, 
you can create your own keys and add yourself to [google-browser-signin-testaccounts](https://groups.google.com/u/1/a/chromium.org/g/google-browser-signin-testaccounts)
to enable Sync.

##  Downloading the Thorium code

Using Git:
It should be placed in $HOME, i.e. C:\Users\$USERNAME.  

```shell
git clone --recursive https://github.com/Alex313031/thorium.git
```

## Setting up the build

To use the build scripts, we need bash on Windows. It is recommended to use Git Bash from [Git for Windows](https://gitforwindows.org/).
All the following script instructions assume that Chromium is in *C:\src\chromium\src* and that you have changed to the thorium repo in bash.

First, we need to make sure we have all the tags/branches and are on Tip of Tree.
For this, run (from within the Thorium repo):

```shell
./trunk.sh
```

Secondly, we need to check out the revision that Chromium is currently using. 
For this, run:

```shell
./version.sh
```

At the end it will download the [PGO profiles](https://chromium.googlesource.com/chromium/src.git/+/refs/heads/main/docs/pgo.md) for Chromium for all platforms. 
The file for Windows will be downloaded to *C:\src\chromium\src\chrome\build\pgo_profiles\&#42;.profdata* with the actual file name looking something like 
'chrome-win64-6167-1706032279-97e63d82a0938b7701d8cdf028299c39d523a3c6.profdata', which should be added to the end of args.gn as per below.
Take note of this, as we will be using it in the `args.gn` below.

Lastly, we need to copy the Thorium source files over the Chromium tree.
For this, run:

```shell
./setup.sh
```
This will copy all the files and patches to the needed locations.  
- NOTE: To build for AVX2, use `./setup.sh --avx2`. To build SSE3, use `./setup.sh --sse3`. Use `./setup.sh --help` to see all options/platforms.

### Creating the build directory
Chromium & Thorium use [Ninja](https://ninja-build.org) as its main build tool along with
a tool called [GN](https://gn.googlesource.com/gn/+/main/docs/quick_start.md) to generate `.ninja` files.

NOTE: Instructions from here on out assume you are back in __cmd.exe__ (not Bash), and have changed to the *C:\src\chromium\src* directory.

Create the build output directory by running:

```shell
gn args out\thorium
```

This will open up notepad.exe, and this is where we will specify build arguments ("args") which direct Ninja on how to lay out the build directory tree.
We will be copy/pasting the contents of the [win_args.gn](https://github.com/Alex313031/thorium/blob/main/win_args.gn) file (from *C:\src\Thorium\win_args.gn*) into notepad.
Notice the three lines at the top, related to API Keys. It is fine to leave them blank, or add the ones you have made.  
__At the bottom__, though, notice the line that says *pgo_data_path = ""*. This is where we will put the full path to the PGO profile data file we downloaded earlier.

That line should look something like:

`pgo_data_path = "C:\src\chromium\src\chrome\build\pgo_profiles\chrome-win64-6167-1706032279-97e63d82a0938b7701d8cdf028299c39d523a3c6.profdata"`

* For other build arguments, and what the ones that Thorium uses do, see [ABOUT_GN_ARGS.md](https://github.com/Alex313031/thorium/blob/main/docs/ABOUT_GN_ARGS.md) & [win_args.list](https://github.com/Alex313031/thorium/blob/main/infra/win_args.list)
* For more info on GN, run `gn help` on the command line or read the [quick start guide](https://gn.googlesource.com/gn/+/main/docs/quick_start.md).

## Build Thorium <a name="build"></a>

Build Thorium (the "chrome" target), and the other things like [chromedriver](https://chromedriver.chromium.org/home) and [thorium_shell](https://github.com/Alex313031/thorium/tree/main/thorium_shell#readme) with Ninja using the command:

```shell
autoninja -C out\thorium thorium chromedriver thorium_shell setup mini_installer -j8
```
(Where -j# can be any number, and should generally be set to the number of cores on your CPU).

`autoninja` is a wrapper that automatically provides optimal values for the
arguments passed to `ninja`.

You can get a list of all of the other build targets from GN by running
`gn ls out\thorium` from the command line. To compile one, pass to Ninja
the GN label with no preceding "//" (so for `//chrome/test:unit_tests`
use autoninja -C out\thorium chrome/test:unit_tests).

## Install/Run Thorium

Once it is built, you can simply run the browser:

```shell
out\thorium\thorium.exe
```

Or install it using the mini_installer:

```shell
out\thorium\mini_installer.exe
```

## Update your checkout

To update an existing Chromium checkout, you should run the `trunk.sh` script again:

```shell
./trunk.sh
```

This syncs the subrepositories to the appropriate versions,
deleting those that are no longer needed, and re-runs the hooks as needed.

To update an existing Thorium checkout, just download the latest .zip, or do a git pull:

```shell
git pull --recurse-submodules origin main
```

### Debugging <a name="debugging"></a>
*   See the [Thorium DEBUG](https://github.com/Alex313031/thorium/tree/main/infra/DEBUG#readme) dir, including the [More Info](https://github.com/Alex313031/thorium/blob/main/infra/DEBUG/README.md#more-info-) section, and [DEBUGGING.md](https://github.com/Alex313031/thorium/blob/main/infra/DEBUG/DEBUGGING.md).

If you have problems building, join us in the Thorium IRC Channel at 
`#thorium` on `irc.libera.chat` and ask there.

If you suspect that Windows Defender is slowing your build then you can try Microsoft's
[Performance analyzer for Microsoft Defender Antivirus](https://learn.microsoft.com/en-us/microsoft-365/security/defender-endpoint/tune-performance-defender-antivirus?view=o365-worldwide)
to investigate in detail.

*Happy Thorium Building!*

<img src="https://github.com/Alex313031/thorium/blob/main/logos/STAGING/Thorium90_504.jpg" width="200">
