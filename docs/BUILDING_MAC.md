# Checking out and building Thorium for Mac

There are instructions for other platforms here in the Thorium Docs directory.

## System requirements

*   A Mac, Intel or Arm.
    ([More details about Arm Macs](https://chromium.googlesource.com/chromium/src.git/+/main/docs/mac_arm64.md).)
*   MacOS 10.15 or higher.
*   [Xcode](https://developer.apple.com/xcode/). Xcode comes with...
*   The macOS SDK. Run

    ```shell
    $ ls `xcode-select -p`/Platforms/MacOSX.platform/Developer/SDKs
    ```

    to check whether you have it, and what version you have.
    `mac_sdk_official_version` in [mac_sdk.gni](../build/config/mac/mac_sdk.gni)
    is the SDK version used on all the bots and for
    [official builds](https://source.chromium.org/search?q=MAC_BINARIES_LABEL&ss=chromium),
    so that version is guaranteed to work. Building with a newer SDK usually
    works too (please fix or file a bug if it doesn't).

    Building with an older SDK might also work, but if it doesn't then we won't
    accept changes for making it work.

    The easiest way to get the newest SDK is to use the newest version of Xcode,
    which often requires using the newest version of macOS. We don't use Xcode
    itself much, so if you're know what you're doing, you can likely get the
    build working with an older version of macOS as long as you get a new
    version of the macOS SDK on it.
*   An APFS-formatted volume (this is the default format for macOS volumes).

##  Downloading the Thorium code <a name="get-the-code"></a>
Using Git:

```shell
git clone --recursive https://github.com/Alex313031/thorium.git
```

## Install `depot_tools`

Clone the `depot_tools` repository:

```shell
$ git clone https://chromium.googlesource.com/chromium/tools/depot_tools.git
```

Add `depot_tools` to the end of your PATH (you will probably want to put this in
your `~/.bash_profile` or `~/.zshrc`). Assuming you cloned `depot_tools` to
`/path/to/depot_tools` (note: you **must** use the absolute path or Python will
not be able to find infra tools):

```shell
$ export PATH="$PATH:/path/to/depot_tools"
```

## Get the Chromium code

Create a `chromium` directory for the checkout and change to it (you can call
this whatever you like and put it wherever you like, as long as the full path
has no spaces):

```shell
$ mkdir chromium && cd chromium
```

Run the `fetch` tool from `depot_tools` to check out the code and its
dependencies.

```shell
$ caffeinate fetch chromium
```

Running the `fetch` with `caffeinate` is optional, but it will prevent the
system from sleeping for the duration of the `fetch` command, which may run for
a considerable amount of time.

If you don't need the full repo history, you can save time by using
`fetch --no-history chromium`. You can call `git fetch --unshallow` to retrieve
the full history later.

Expect the command to take 30 minutes on even a fast connection, and many
hours on slower ones.

When `fetch` completes, it will have created a hidden `.gclient` file and a
directory called `src` in the working directory. The remaining instructions
assume you have switched to the `src` directory:

```shell
$ cd src
```

*Optional:* You can also [build with API keys](https://www.chromium.org/developers/how-tos/api-keys) if you want your
build to talk to some Google services like Google Sync, Translate, and GeoLocation.&nbsp;<img src="https://github.com/Alex313031/thorium/blob/main/logos/NEW/Key_Light.svg#gh-dark-mode-only" width="26"> <img src="https://github.com/Alex313031/thorium/blob/main/logos/NEW/Key_Dark.svg#gh-light-mode-only" width="26">&nbsp;Thorium has its own keys in a private repository, if you are a builder or would like access to them, contact me. Otherwise, for personal or development builds, 
you can create your own keys and add yourself to [google-browser-signin-testaccounts](https://groups.google.com/u/1/a/chromium.org/g/google-browser-signin-testaccounts)
to enable Sync.

## Setting up the build

First, we need to make sure we have all the tags/branches and are on Tip of Tree.
For this, run (from within the Thorium repo):

```shell
./trunk.sh
```

Secondly, we need to check out the revision that Thorium is currently using. 
For this, run:

```shell
./version.sh
```

At the end it will download the [PGO profiles](https://chromium.googlesource.com/chromium/src.git/+/refs/heads/main/docs/pgo.md) for Chromium for all platforms. 
The file for MacOS will be downloaded to */Users/$USERNAME/chromium/src/build/pgo_profiles/&#42;.profdata* with the actual file name looking something like 
'chrome-mac-6167-1706032279-25144dc1c87be275c5981becbafed7785e2f39f2.profdata', which should be added to the end of args.gn as per below.
Take note of this, as we will be using it in the `args.gn` below.

Lastly, we need to copy the Thorium source files over the Chromium tree.
For this, run:

```shell
./setup.sh --mac
```
This will copy all the files and patches to the needed locations.  
- NOTE: To build for arm64, use `./setup.sh --mac-arm`. Use `./setup.sh --help` to see all options/platforms.

Chromium uses [Ninja](https://ninja-build.org) as its main build tool along with
a tool called [GN](https://gn.googlesource.com/gn/+/main/docs/quick_start.md)
to generate `.ninja` files. You can create any number of *build directories*
with different configurations. Create the build output directory by running:

```shell
$ gn args out/thorium
```
The contents of '[mac_args.gn](https://github.com/Alex313031/thorium/blob/other/Mac/mac_args.gn)' in the root of this repo should be copy/pasted into the editor.
*and edit the last line to point to the actual path and file name of the '&#42;.profdata' it*  
Use the '[mac_ARM_args.gn](https://github.com/Alex313031/thorium/blob/other/Mac/mac_ARM_args.gn)' for arm64 builds.

* For other build arguments, including release settings, see [GN build
  configuration](https://www.chromium.org/developers/gn-build-configuration).
  The default will be a debug component build matching the current host
  operating system and CPU.
* For more info on GN, run `gn help` on the command line or read the
  [quick start guide](https://gn.googlesource.com/gn/+/main/docs/quick_start.md).
* Building Thorium for arm64 Macs requires [additional setup](mac_arm64.md).


## Build Thorium <a name="build"></a>

Build Thorium (the "chrome" target) with Ninja using the command:

```shell
$ autoninja -C out/thorium thorium chromedriver
```

(`autoninja` is a wrapper that automatically provides optimal values for the
arguments passed to `ninja`.)

You can get a list of all of the other build targets from GN by running `gn ls
out/thorium` from the command line. To compile one, pass the GN label to Ninja
with no preceding "//" (so, for `//chrome/test:unit_tests` use `autoninja -C
out/thorium chrome/test:unit_tests`).

## Run/Install Thorium

Once it is built, you can simply run the browser:

```shell
$ out/thorium/Thorium.app/Contents/MacOS/Thorium
```

To generate a *.dmg* installation package, run (from within the Thorium repo):

```shell
$ ./build_dmg.sh
```

### Avoiding repetitive system permissions dialogs after each build

Every time you start a new developer build, you may get two system dialogs:
`Thorium wants to use your confidential information stored in "Thorium Safe
Storage" in your keychain.`, and `Do you want the application "Thorium.app" to
accept incoming network connections?`.

To avoid them, you can run Thorium with these command-line flags (but of
course beware that they will change the behavior of certain subsystems, like password storage):

```shell
--use-mock-keychain --disable-features=DialMediaRouteProvider
```

## Debugging

Good debugging tips can be found [here](https://chromium.googlesource.com/chromium/src/+/refs/heads/main/docs/mac/debugging.md).

If you have problems building, join us in the Thorium IRC Channel at 
`#thorium` on `irc.libera.chat` and ask there.

## Build and run test targets

Tests are split into multiple test targets based on their type and where they
exist in the directory structure. To see what target a given unit test or
browser test file corresponds to, the following command can be used:

```shell
$ gn refs out/thorium --testonly=true --type=executable --all chrome/browser/ui/browser_list_unittest.cc
//chrome/test:unit_tests
```

In the example above, the target is unit_tests. The unit_tests binary can be
built by running the following command:

```shell
$ autoninja -C out/thorium unit_tests
```

You can run the tests by running the unit_tests binary. You can also limit which
tests are run using the `--gtest_filter` arg, e.g.:

```shell
$ out/thorium/unit_tests --gtest_filter="BrowserListUnitTest.*"
```

You can find out more about GoogleTest at its
[GitHub page](https://github.com/google/googletest).

## Update your checkout

To update an existing checkout, you can run (from within the Thorium repo):

```shell
$ ./trunk.sh
```

## Tips, tricks, and troubleshooting

### Improving performance of `git status`

#### Increase the vnode cache size

`git status` is used frequently to determine the status of your checkout.  Due
to the large number of files in Chromium's checkout, `git status` performance
can be quite variable.  Increasing the system's vnode cache appears to help. By
default, this command:

```shell
$ sysctl -a | egrep 'kern\..*vnodes'
```

Outputs `kern.maxvnodes: 263168` (263168 is 257 * 1024).  To increase this
setting:

```shell
$ sudo sysctl kern.maxvnodes=$((512*1024))
```

Higher values may be appropriate if you routinely move between different
Chromium checkouts.  This setting will reset on reboot.  To apply it at startup:

```shell
$ sudo tee /Library/LaunchDaemons/kern.maxvnodes.plist > /dev/null <<EOF
<?xml version="1.0" encoding="UTF-8"?>
<!DOCTYPE plist PUBLIC "-//Apple//DTD PLIST 1.0//EN" "http://www.apple.com/DTDs/PropertyList-1.0.dtd">
<plist version="1.0">
  <dict>
    <key>Label</key>
      <string>kern.maxvnodes</string>
    <key>ProgramArguments</key>
      <array>
        <string>sysctl</string>
        <string>kern.maxvnodes=524288</string>
      </array>
    <key>RunAtLoad</key>
      <true/>
  </dict>
</plist>
EOF
```

Or edit the file directly.

#### Configure git to use an untracked cache

Try running

```shell
$ git update-index --test-untracked-cache
```

If the output ends with `OK`, then the following may also improve performance of
`git status`:

```shell
$ git config core.untrackedCache true
```

#### Configure git to use fsmonitor

You can significantly speed up git by using [fsmonitor.](https://github.blog/2022-06-29-improve-git-monorepo-performance-with-a-file-system-monitor/)
You should enable fsmonitor in large repos, such as Chromium and v8. Enabling
it globally will launch many processes and probably isn't worthwhile. The
command to enable fsmonitor in the current repo is:

```shell
$ git config core.fsmonitor true
```

### Xcode license agreement

If you're getting the error

> Agreeing to the Xcode/iOS license requires admin privileges, please re-run as
> root via sudo.

the Xcode license hasn't been accepted yet which (contrary to the message) any
user can do by running:

```shell
$ xcodebuild -license
```

Only accepting for all users of the machine requires root:

```shell
$ sudo xcodebuild -license
```

### Exclude checkout from Spotlight indexing

Chromium's checkout contains a lot of files, and building generates many more.
Spotlight will try to index all of those files, and uses a lot of CPU time
doing so, especially during a build, which can slow things down.

To prevent the Chromium checkout from being indexed by Spotlight, open System
Preferences, go to "Spotlight" -> "Privacy" and add your Chromium checkout
directory to the list of excluded locations.
