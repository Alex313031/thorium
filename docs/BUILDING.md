# Checking out and building Thorium on Linux &nbsp;<img src="https://github.com/Alex313031/thorium/blob/main/logos/NEW/build_light.svg#gh-dark-mode-only" width="48"> <img src="https://github.com/Alex313031/thorium/blob/main/logos/NEW/build_dark.svg#gh-light-mode-only" width="48">

There are instructions for other platforms here in the Thorium Docs directory.
You can also read the [old building instructions](https://github.com/Alex313031/thorium/blob/main/infra/BUILDING.md).

#### Windows
For Windows and Windows [AVX2](https://en.wikipedia.org/wiki/Advanced_Vector_Extensions#Advanced_Vector_Extensions_2), I made new dedicated instructions. If you are building on Windows use [BUILDING_WIN.md](https://github.com/Alex313031/thorium/blob/main/docs/BUILDING_WIN.md) and if you are building for Windows on Linux, use [WIN_CROSS_BUILD_INSTRUCTIONS](https://github.com/Alex313031/thorium/blob/main/docs/WIN_CROSS_BUILD_INSTRUCTIONS.txt)

## System Requirements

*   A x64 machine with at least 8GB of RAM. 16GB or more is highly
    recommended.
*   At least 75GB of free disk space.
*   You must have Git and Python v3.8+ installed already (and `python3` must point
    to a Python v3.8+ binary, i.e. in your path or as default python install). 
    Depot_tools bundles an appropriate version of Python in `$depot_tools/python-bin`, 
    if you don't have an appropriate version already on your system.

Most development is done on Ubuntu 22.04, Jammy Jellyfish (This is what Chromium's build infrastructure currently runs). 
Ubuntu 16.04/18.04 no longer works. 20.04 and Debian 10/11/12 will work.
There are some instructions for other distros below, but they are mostly unsupported.

__The scripts to build Thorium assume that depot_tools, thorium and chromium are both in $HOME!__

## Install `depot_tools` <a name="depot-tools"></a>

Clone the `depot_tools` repository:

```shell
$ git clone https://chromium.googlesource.com/chromium/tools/depot_tools.git
```

Add *depot_tools* to the end of your *$PATH* (you will probably want to put this
in your `~/.bashrc` or `~/.zshrc`). When setting the path after cloning *depot_tools* to your home directory 
**do not** use `~` in the PATH, otherwise `gclient runhooks` will fail to run. Rather, you should use either
`$HOME` or the absolute path. So, assuming you cloned *depot_tools* to *$HOME*:

```shell
$ export PATH="${HOME}/depot_tools:$PATH" or $ export PATH="/home/alex/depot_tools:$PATH"
```

## Get the code <a name="get-the-code"></a>

### Thorium Code

Clone the Thorium repo into *$HOME*

```shell
$ git clone --recursive https://github.com/Alex313031/thorium.git
```

### Chromium Code

Create a *chromium* directory for the checkout and change to it.

```shell
$ mkdir ~/chromium && cd ~/chromium
```

Run the *fetch* tool from depot_tools to check out the code and its
dependencies.

```shell
$ fetch --nohooks chromium
```

The `--nohooks` flag is omitted on other platforms, we just use it on linux to explicitly run the hooks
later, after installing the prerequisites.
`fetch` and `repo` are used to download, rebase, and sync all Google repositories, including Chromium, ChromiumOS, 
Android, Fuchsia, Infra, Monorail, GN, etc.

If you don't want the full repo history, you can save a lot of time by
adding the `--no-history` flag to `fetch`. This is equivalent to a shallow git clone with a depth of 1.

Expect the command to take 20 minutes on a fast (150mbps+) connection, and many
hours on slower ones.

If you've already installed the build dependencies on the machine (from another
checkout, for example), you can omit the `--nohooks` flag and *fetch*
will automatically execute `gclient runhooks` at the end.

When *fetch* completes, it will have created a hidden `.gclient` file and a
directory called `src` in the *chromium* directory. The remaining instructions
assume you have switched to the `src` directory, so:

```shell
$ cd src
```

### Install additional build dependencies

Once you have checked out the code, and assuming you're using Ubuntu, run the
[*`install-build-deps.sh`*](https://chromium.googlesource.com/chromium/src/+/main/build/install-build-deps.sh) script.

```shell
$ ./build/install-build-deps.sh --no-nacl
```

You can run it with the flag `--help` to see arguments. For example, you would want `--lib32` if building for 32 bit Linux, `--arm` for building
a Raspberry Pi release, `--chromeos-fonts` for building Thorium for ThoriumOS, and `--quick-check` just to verify needed libraries are installed.

You may need to adjust the build dependencies for other distros. There are
some [notes](#notes) at the end of this document, but we make no guarantees
for their accuracy, as distros get updated over time.

### Run the hooks

Once you've run `install-build-deps` at least once, you can now run the
Chromium-specific hooks, which will download additional binaries and other
things like LLVM and a Debian Sysroot.:

```shell
$ gclient runhooks
```

*Optional:* You can also [build with API
keys](https://www.chromium.org/developers/how-tos/api-keys) if you want your
build to talk to some Google services like Google Sync, Translate, and GeoLocation.&nbsp;<img src="https://github.com/Alex313031/thorium/blob/main/logos/NEW/Key_Light.svg#gh-dark-mode-only" width="26"> <img src="https://github.com/Alex313031/thorium/blob/main/logos/NEW/Key_Dark.svg#gh-light-mode-only" width="26">&nbsp;Thorium has its own keys in a private repository, if you are a builder or would like access to them, contact me. Otherwise, for personal or development builds, 
you can create your own keys and add yourself to [google-browser-signin-testaccounts](https://groups.google.com/u/1/a/chromium.org/g/google-browser-signin-testaccounts)
to enable Sync.

## Setting up the build <a name="setup"></a>

First, we need to run `./trunk.sh` (in the root of the Thorium repo.) This will Rebase/Sync the Chromium repo, and revert it to stock Chromium.  
It will also fetch all the tags/branches, which is needed for the version.sh script.
It should be used before every separate build. See the [Updating](#updating) section.

__IMPORTANT__
This will update and sync the sources to the latest revision (tip of tree) and ensure you have all the version tags.

- Then, to check out the current Chromium revision that Thorium is using, run `./version.sh`. At the end it will download the [PGO profiles](https://chromium.googlesource.com/chromium/src.git/+/refs/heads/main/docs/pgo.md) for Chromium for all platforms. The file will be downloaded to *//chromium/src/chrome/build/pgo_profiles/&#42;.profdata* with the actual file name looking something like 'chrome-linux-6167-1706004111-41f78c57fb3a1fe49a5c549b16f0221465339af9.profdata', which should be added to the end of args.gn as per below.
Take note of this, as we will be using it in the `args.gn` below.
- Then, (from where you cloned this repo) run `./setup.sh`. This will copy all the files and patches to the needed locations.
- NOTE: To build for MacOS, use `./setup.sh --mac`. To build for Raspberry Pi, use `./setup.sh --raspi`. Use `./setup.sh --help` to see all options/platforms.

Chromium and Thorium use [Ninja](https://ninja-build.org) as their main build tool, along with
a tool called [GN](https://gn.googlesource.com/gn/+/refs/heads/main/README.md)
to generate `.ninja` files in the build output directory. You can create any number of *build directories*
with different configurations. Create the build output directory by running:
- Run `gn args out/thorium` and the contents of '[args.gn](https://github.com/Alex313031/thorium/blob/main/args.gn)' in the root of this repo should be copy/pasted into the editor. Note that for Windows, Mac, ChromiumOS, or Android there are separate &#42;_args.gn files for those platforms. *--Include your api keys here at the top or leave blank, and edit the last line to point to the actual path and file name of '&#42;.profdata'* 
- For more info about args.gn, read the [ABOUT_GN_ARGS.md](https://github.com/Alex313031/thorium/blob/main/infra/DEBUG/ABOUT_GN_ARGS.md) file.
- '[infra/args.list](https://github.com/Alex313031/thorium/blob/main/infra/args.list)' contains an alphabetical list with descriptions of all possible build arguments; [gn_args.list](https://github.com/Alex313031/thorium/blob/main/infra/gn_args.list) gives a similar list but with the flags in args.gn added.

You can list all the possible build arguments and pipe it to a text file by running:

```shell
$ gn args out/thorium --list >> /path/to/ARGS.list
```

* You only have to run this once for each new build directory, Ninja will
  update the build files as needed.
* You can replace *thorium* with another name, but
  it should be a subdirectory of *out*. Note that if you choose another name, the `trunk.sh` and `build.sh` scripts will not work.
* For information on the args.gn that Thorium uses, see [ABOUT_GN_ARGS.md](https://github.com/Alex313031/thorium/blob/main/docs/ABOUT_GN_ARGS.md).  
* For other build arguments, including release settings, see [GN build
  configuration](https://www.chromium.org/developers/gn-build-configuration).
  The default will be a vanilla Chromium debug component build matching the current host
  operating system and CPU.
* For more info on GN, run `gn help` on the command line or read the
  [quick start guide](https://gn.googlesource.com/gn/+/main/docs/quick_start.md).

#### ccache

You can use [ccache](https://ccache.dev) to speed up local builds.

Increase your ccache hit rate by setting `CCACHE_BASEDIR` to a parent directory
that the working directories all have in common (e.g.,
`/home/yourusername/development`). Consider using
`CCACHE_SLOPPINESS=include_file_mtime` (since if you are using multiple working
directories, header times in svn sync'ed portions of your trees will be
different - see
[the ccache troubleshooting section](https://ccache.dev/manual/latest.html#_troubleshooting)
for additional information). If you use symbolic links from your home directory
to get to the local physical disk directory where you keep those working
development directories, consider putting

    alias cd="cd -P"

in your `.bashrc` so that `$PWD` or `cwd` always refers to a physical, not
logical directory (and make sure `CCACHE_BASEDIR` also refers to a physical
parent).

If you tune ccache correctly, a second working directory that uses a branch
tracking trunk and is up to date with trunk and was gclient sync'ed at about the
same time should build chrome in about 1/3 the time, and the cache misses as
reported by `ccache -s` should barely increase.

This is especially useful if you use
[git-worktree](http://git-scm.com/docs/git-worktree) and keep multiple local
working directories going at once.

## Build Thorium <a name="build"></a>

Build Thorium (the "chrome" target), as well as [chrome_sandbox](https://chromium.googlesource.com/chromium/src/+/HEAD/docs/linux/sandboxing.md), [chromedriver](https://chromedriver.chromium.org/home), and [thorium_shell](https://github.com/Alex313031/thorium/tree/main/thorium_shell#readme) (based on [content_shell](https://chromium.googlesource.com/chromium/src/+/HEAD/docs/testing/web_tests_in_content_shell.md#as-a-simple-browser) ), using the `build.sh` script in the root of the Thorium repo (where the # is the number of jobs). Assuming you have an 8 Core system:

```shell
$ ./build.sh 8
```

You could also manually issue the command (where -j is the number of jobs):

```shell
$ autoninja -C ~/chromium/src/out/thorium thorium chrome_sandbox chromedriver thorium_shell -j8
```

`autoninja` is a wrapper that automatically provides optimal values for the
arguments passed to `ninja`. `build.sh` uses a [custom autoninja](https://github.com/Alex313031/thorium/blob/main/depot_tools/autoninja) in the *depot_tools* directory in Thorium.

You can get a list of all of the other build targets from GN by running `gn ls
out/thorium` from the command line. To compile one, pass the GN label to Ninja
with no preceding "//" (so, for `//chrome/test:unit_tests` use `autoninja -C
out/thorium chrome/test:unit_tests`).

## Run Thorium

Once it is built, you can simply run the browser:

```shell
$ out/thorium/thorium
```
*Note - Copy and run [clean.sh](https://github.com/Alex313031/thorium/blob/main/clean.sh) within this dir to clean up build artifacts.*

## Installing Thorium

Of course, you will probably want to make installation packages. To make .deb and .rpm packages run `package.sh` (where the # is the number of jobs) in the root of the repo:

```shell
$ ./package.sh 8
```
To make an appimage, copy the .deb to `//thorium/infra/APPIMAGE/`
and follow the [Instructions](https://github.com/Alex313031/thorium/blob/main/infra/APPIMAGE/README.md#instructions) therein.

### Tests

See the [Debugging](#debugging) section below, as well as
[Thorium UI Debug Shell](https://github.com/Alex313031/thorium/blob/main/infra/DEBUG/DEBUG_SHELL_README.md).

Learn about [how to use Chromedriver](https://chromedriver.chromium.org/getting-started) and Google Test at its
[GitHub page](https://github.com/google/googletest).

## Update your checkout and revert to latest vanilla tip-o-tree Chromium. <a name="updating"></a>

Simply run `trunk.sh` in the root of the Thorium repo or execute the commands inside.

```shell
$ ./trunk.sh
```

## Running test targets

Tests are split into multiple test targets based on their type and where they
exist in the directory structure. To see what target a given unit test or
browser test file corresponds to, the following command can be used:

```shell
$ gn refs out/Default --testonly=true --type=executable --all chrome/browser/ui/browser_list_unittest.cc
//chrome/test:unit_tests
```

In the example above, the target is unit_tests. The unit_tests binary can be
built by running the following command:

```shell
$ autoninja -C out/Default unit_tests
```

## Tips, tricks, and troubleshooting

### More links

*   Information about [building with Clang](https://chromium.googlesource.com/chromium/src.git/+/refs/heads/main/docs/clang.md).
*   You may want to [use a chroot](https://chromium.googlesource.com/chromium/src.git/+/refs/heads/main/docs/linux/using_a_chroot.md) to
    isolate yourself from versioning or packaging conflicts.
*   Cross-compiling for ARM? (Raspberry Pi) See the [Thorium ARM](https://github.com/Alex313031/thorium/tree/main/arm#readme) dir and [chromium_arm.md](https://chromium.googlesource.com/chromium/src.git/+/refs/heads/main/docs/linux/chromium_arm.md).
*   [Atom](https://thorium.rocks/atom-ng/) and [Geany](https://www.geany.org/) are recommended IDEs for working on Thorium.

### Debugging <a name="debugging"></a>
*   See the [Thorium DEBUG](https://github.com/Alex313031/thorium/tree/main/infra/DEBUG#readme) dir, including the [More Info](https://github.com/Alex313031/thorium/blob/main/infra/DEBUG/README.md#more-info-) section, and [DEBUGGING.md](https://github.com/Alex313031/thorium/blob/main/infra/DEBUG/DEBUGGING.md).

If you have problems building, join us in the Thorium IRC Channel at 
`#thorium` on `irc.libera.chat` and ask there.

## Notes for other distros <a name="notes"></a>

### Arch Linux

Instead of running `install-build-deps.sh` to install build dependencies, run:

```shell
$ sudo pacman -S --needed automake autoconf base-devel curl xz squashfs-tools p7zip \
git tk python python-pkgconfig python-virtualenv python-oauth2client python-oauthlib \
perl gcc gcc-libs bison flex gperf pkgconfig dbus icoutils \
nss alsa-lib glib2 gtk3 nspr freetype2 cairo \
xorg-server-xvfb xorg-xdpyinfo
```

For the optional packages on Arch Linux:

*   `php-cgi` is provided with `pacman`
*   `wdiff` is not in the main repository but `dwdiff` is. You can get `wdiff`
    in AUR/`yaourt`

### Crostini on ChromiumOS/ThoriumOS (Debian based)

First install the `file` and `lsb-release` commands for the script to run properly:

```shell
$ sudo apt-get install file lsb-release
```

Then invoke install-build-deps.sh with the `--no-arm` argument,
because the ARM toolchain doesn't exist for this configuration:

```shell
$ sudo build/install-build-deps.sh --no-arm
```

### Fedora

Instead of running `build/install-build-deps.sh`, run:

```shell
su -c 'yum install git python bzip2 tar pkgconfig atk-devel alsa-lib-devel \
bison binutils brlapi-devel bluez-libs-devel bzip2-devel cairo-devel \
cups-devel dbus-devel dbus-glib-devel expat-devel fontconfig-devel \
freetype-devel gcc-c++ glib2-devel glibc.i686 gperf glib2-devel \
gtk3-devel java-1.*.0-openjdk-devel libatomic libcap-devel libffi-devel \
libgcc.i686 libjpeg-devel libstdc++.i686 libX11-devel \
libXScrnSaver-devel libXtst-devel libxkbcommon-x11-devel ncurses-compat-libs \
nspr-devel nss-devel pam-devel pango-devel pciutils-devel \
pulseaudio-libs-devel zlib.i686 httpd mod_ssl php php-cli python-psutil wdiff \
xorg-x11-server-Xvfb'
```

The fonts needed by Blink's web tests can be obtained by following [these
instructions](https://gist.github.com/pwnall/32a3b11c2b10f6ae5c6a6de66c1e12ae).
For the optional packages:

* `php-cgi` is provided by the `php-cli` package.
* `sun-java6-fonts` is covered by the instructions linked above.

### Gentoo

You can install the deps by doing a dry run of `emerge www-client/chromium`.

---------------------------------
*Happy Thorium Building!*

<img src="https://github.com/Alex313031/thorium/blob/main/logos/STAGING/Thorium90_504.jpg" width="200">
