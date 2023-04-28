""" Contains the global configuration object.
"""

import os
from . import shell
from . import packages
from . import errors

class RoboConfiguration:
  __slots__ = ('_sushi_branch_prefix', '_gn_commit_title',
    '_patches_commit_title', '_readme_chromium_commit_title',
    '_origin_merge_base', '_llvm_path', '_autorename_git_file',
    '_chrome_src', '_host_operating_system', '_host_architecture',
    '_ffmpeg_home', '_relative_asan_directory', '_branch_name',
    '_sushi_branch_name', '_readme_chromium_commit_title', '_nasm_path',
    '_prompt_on_call', '_os_flavor', '_force_gn_rebuild', '_skip_allowed')
  def __init__(self):
    """Ensure that our config has basic fields fill in, and passes some sanity
    checks too.

    Important: We might be doing --setup, so these sanity checks should only be
    for things that we don't plan for fix as part of that.
    """
    self.set_prompt_on_call(False)
    # Pull this from parsed args
    self._force_gn_rebuild = False
    # Allowed to skip steps, default to yes, needs --no-skip flag to disable.
    self._skip_allowed = True
    # This is the prefix that our branches start with.
    self._sushi_branch_prefix = "sushi-"
    # This is the title that we use for the commit with GN configs.
    self._gn_commit_title = "GN Configuration"
    # Title of the commit with chromium/patches/README.
    self._patches_commit_title = "Chromium patches file"
    # Title of the commit with README.chromium
    self._readme_chromium_commit_title = "README.chromium file"
    self.EnsureHostInfo()
    self.EnsureChromeSrc()

    # Origin side of the merge.  Only needs to change if you're trying to
    # modify and test robosushi itself.  See robosushi.py for details.
    self._origin_merge_base = "origin/master"

    # Directory where llvm lives.
    self._llvm_path = os.path.join(self.chrome_src(), "third_party",
            "llvm-build", "Release+Asserts", "bin")

    self.EnsurePathContainsLLVM()
    self.EnsureNoMakeInfo()
    shell.log("Using chrome src: %s" % self.chrome_src())
    self.EnsureFFmpegHome()
    shell.log("Using ffmpeg home: %s" % self.ffmpeg_home())
    self.EnsureASANConfig()
    self.ComputeBranchName()
    shell.log("On branch: %s" % self.branch_name())
    if self.sushi_branch_name():
      shell.log("On sushi branch: %s" % self.sushi_branch_name())

    # Filename that we'll ask generate_gn.py to write git commands to.
    self._autorename_git_file = os.path.join(
      self.ffmpeg_home(), "chromium", "scripts", ".git_commands.sh")

  def chrome_src(self):
    """Return /path/to/chromium/src"""
    return self._chrome_src

  def host_operating_system(self):
    """Return host type, e.g. "linux" """
    return self._host_operating_system

  def host_architecture(self):
    """Return host architecture, e.g. "x64" """
    return self._host_architecture

  def ffmpeg_home(self):
    """Return /path/to/third_party/ffmpeg"""
    return self._ffmpeg_home

  def relative_asan_directory(self):
    return self._relative_asan_directory

  def absolute_asan_directory(self):
    return os.path.join(self.chrome_src(), self.relative_asan_directory())

  def chdir_to_chrome_src(self):
    os.chdir(self.chrome_src())

  def chdir_to_ffmpeg_home(self):
    os.chdir(self.ffmpeg_home())

  def branch_name(self):
    """Return the current workspace's branch name, or None.  This might be any
    branch (e.g., "master"), not just one that we've created."""
    return self._branch_name

  def sushi_branch_name(self):
    """If the workspace is currently on a branch that we created (a "sushi
    branch"), return it.  Else return None."""
    return self._sushi_branch_name

  def sushi_branch_prefix(self):
    """Return the branch name that indicates that this is a "sushi branch"."""
    return self._sushi_branch_prefix

  def gn_commit_title(self):
    return self._gn_commit_title

  def patches_commit_title(self):
    return self._patches_commit_title

  def readme_chromium_commit_title(self):
    return self._readme_chromium_commit_title

  def nasm_path(self):
    return self._nasm_path

  def origin_merge_base(self):
    return self._origin_merge_base

  def override_origin_merge_base(self, new_base):
    self._origin_merge_base = new_base

  def os_flavor(self):
    return self._os_flavor

  def force_gn_rebuild(self):
    return self._force_gn_rebuild

  def set_force_gn_rebuild(self):
    self._force_gn_rebuild = True

  def skip_allowed(self):
    return self._skip_allowed

  def set_skip_allowed(self, to):
    self._skip_allowed = to

  def EnsureHostInfo(self):
    """Ensure that the host architecture and platform are set."""
    kernel, host, os, *rest = shell.output_or_error(["uname", "-a"]).split()
    assert kernel in ("Linux", "linux")
    assert "x86_64" in rest
    self._host_operating_system = "linux"
    self._host_architecture = "x64"

    if "rodete" in os:
      self._os_flavor = packages.OsFlavor.Debian
    elif "arch" in os:
      self._os_flavor = packages.OsFlavor.Arch
    else:
      raise Exception("Couldn't determine OS flavor (needed to install packages)")

  def EnsureChromeSrc(self):
    """Find the /absolute/path/to/my_chrome_dir/src"""
    wd = os.getcwd()
    # Walk up the tree until we find src/AUTHORS
    while wd != "/":
      if os.path.isfile(os.path.join(wd, "src", "AUTHORS")):
        self._chrome_src = os.path.join(wd, "src")
        return
      wd = os.path.dirname(wd)
    raise Exception("could not find src/AUTHORS in any parent of the wd")

  def EnsureFFmpegHome(self):
    """Ensure that |self| has "ffmpeg_home" set."""
    self._ffmpeg_home = os.path.join(self.chrome_src(), "third_party", "ffmpeg")

  def EnsureASANConfig(self):
    """Find the asan directories.  Note that we don't create them."""
    # Compute gn ASAN out dirnames.
    self.chdir_to_chrome_src();
    local_directory = os.path.join("out", "sushi_asan")
    # ASAN dir, suitable for 'ninja -C'
    self._relative_asan_directory = local_directory

  def EnsurePathContainsLLVM(self):
    """Make sure that we have chromium's LLVM in $PATH.

    We don't want folks to accidentally use the wrong clang.
    """

    llvm_path = os.path.join(self.chrome_src(), "third_party",
            "llvm-build", "Release+Asserts", "bin")
    if self.llvm_path() not in os.environ["PATH"]:
      raise errors.UserInstructions(
          "Please add:\n%s\nto the beginning of $PATH\nExample: export PATH=%s:$PATH" %
          (self.llvm_path(), self.llvm_path()))

  def EnsureNoMakeInfo(self):
    """Ensure that makeinfo is not available."""
    if os.system("makeinfo --version > /dev/null 2>&1") == 0:
      raise errors.UserInstructions(
          "makeinfo is available and we don't need it, so please remove it\nExample: sudo apt-get remove texinfo"
      )

  def llvm_path(self):
    return self._llvm_path

  def ComputeBranchName(self):
    """Get the current branch name and set it."""
    self.chdir_to_ffmpeg_home()
    branch_name = shell.output_or_error(
      ["git", "rev-parse", "--abbrev-ref", "HEAD"])
    self.SetBranchName(str(branch_name))

  def SetBranchName(self, name):
    """Set our branch name, which may be a sushi branch or not."""
    self._branch_name = name
    # If this is one of our branches, then record that too.
    if name and not name.startswith(self.sushi_branch_prefix()):
      name = None
    self._sushi_branch_name = name

  def autorename_git_file(self):
    return self._autorename_git_file

  def prompt_on_call(self):
    """ Return True if and only if we're supposed to ask the user before running
    any command that might have a side-effect."""
    return self._prompt_on_call

  def set_prompt_on_call(self, value):
    self._prompt_on_call = value

  def Call(self, args, **kwargs):
    """Run the command specified by |args| (see subprocess.call), optionally
    prompting the user."""
    if self.prompt_on_call():
      print(f"[{os.getcwd()}] About to run: `{' '.join(args)}` ")
      input("Press ENTER to continue, or interrupt the script: ")
    return shell.check_run(args, **kwargs)
