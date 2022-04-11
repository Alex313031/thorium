This contains the source code for pak.

Rather than a nested git repo, it is simply copy/pasted from upstream, and contains a "bin" dir where there are binaries for Linux and Windows.
The Linux binaries were compiled myself, the Windows ones were from myfreeer's releases, as he only offered Windows builds.
These are copied when running ./setup during the Thorium build to the //chromium/src/out/thorium dir for the installer to include.

The original README.md from chrome-pak-customizer was renamed to README.orig, so that this would show on GitHub explaining what this dir is.

Credit goes to:
https://github.com/myfreeer/

And the original repo is:
https://github.com/myfreeer/chrome-pak-customizer
