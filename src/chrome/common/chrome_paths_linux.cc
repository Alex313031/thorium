// Copyright 2023 The Chromium Authors and Alex313031
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "chrome/common/chrome_paths.h"

#include <memory>

#include "base/base_paths.h"
#include "base/environment.h"
#include "base/files/file_util.h"
#include "base/nix/xdg_util.h"
#include "base/path_service.h"
#include "base/strings/string_util.h"
#include "build/branding_buildflags.h"
#include "build/build_config.h"
#include "build/chromeos_buildflags.h"
#include "chrome/common/channel_info.h"
#include "chrome/common/chrome_paths_internal.h"

namespace chrome {

using base::nix::GetXDGDirectory;
using base::nix::GetXDGUserDirectory;
using base::nix::kDotConfigDir;
using base::nix::kXdgConfigHomeEnvVar;

namespace {

const char kDownloadsDir[] = "Downloads";
const char kMusicDir[] = "Music";
const char kPicturesDir[] = "Pictures";
const char kVideosDir[] = "Videos";

// Generic function for GetUser{Music,Pictures,Video}Directory.
bool GetUserMediaDirectory(const std::string& xdg_name,
                           const std::string& fallback_name,
                           base::FilePath* result) {
#if BUILDFLAG(IS_CHROMEOS_ASH)
  // No local media directories on CrOS.
  return false;
#else
  *result = GetXDGUserDirectory(xdg_name.c_str(), fallback_name.c_str());

  base::FilePath home;
  base::PathService::Get(base::DIR_HOME, &home);
  if (*result != home) {
    base::FilePath desktop;
    if (!base::PathService::Get(base::DIR_USER_DESKTOP, &desktop))
      return false;
    if (*result != desktop) {
      return true;
    }
  }

  *result = home.Append(fallback_name);
  return true;
#endif
}

}  // namespace

// This returns <config-home>/<product>, where
//   <config-home> is:
//     $CHROME_CONFIG_HOME if set
//     otherwise $XDG_CONFIG_HOME if set
//     otherwise ~/.config
//   and <product> is:
//     "chromium" for Chromium
//     "google-chrome" for stable channel official build
//     "google-chrome-beta" for beta channel official build
//     "google-chrome-unstable" for dev channel official build
//
// (Note that ChromeMainDelegate will override the value returned by this
// function if $CHROME_USER_DATA_DIR or --user-data-dir is set.)
//
// See http://standards.freedesktop.org/basedir-spec/basedir-spec-latest.html
// for a spec on where config files go.  Using ~/.config also helps us sidestep
// issues with other apps grabbing ~/.chromium .
bool GetDefaultUserDataDirectory(base::FilePath* result) {
  std::unique_ptr<base::Environment> env(base::Environment::Create());
  base::FilePath config_dir;
  std::string chrome_config_home_str;
  if (env->GetVar("CHROME_CONFIG_HOME", &chrome_config_home_str) &&
      base::IsStringUTF8(chrome_config_home_str)) {
    config_dir = base::FilePath::FromUTF8Unsafe(chrome_config_home_str);
  } else {
    config_dir =
        GetXDGDirectory(env.get(), kXdgConfigHomeEnvVar, kDotConfigDir);
  }

#if BUILDFLAG(GOOGLE_CHROME_BRANDING)
  std::string data_dir_basename = "google-chrome";
#else
  std::string data_dir_basename = "thorium";
#endif
  *result = config_dir.Append(data_dir_basename + GetChannelSuffixForDataDir());
  return true;
}

void GetUserCacheDirectory(const base::FilePath& profile_dir,
                           base::FilePath* result) {
  // See http://standards.freedesktop.org/basedir-spec/basedir-spec-latest.html
  // for a spec on where cache files go.  Our rule is:
  // - if the user-data-dir in the standard place,
  //     use same subdirectory of the cache directory.
  //     (this maps ~/.config/google-chrome to ~/.cache/google-chrome as well
  //      as the same thing for ~/.config/chromium)
  // - otherwise, use the profile dir directly.

  // Default value in cases where any of the following fails.
  *result = profile_dir;

  std::unique_ptr<base::Environment> env(base::Environment::Create());

  base::FilePath cache_dir;
  if (!base::PathService::Get(base::DIR_CACHE, &cache_dir))
    return;
  base::FilePath config_dir(GetXDGDirectory(env.get(),
                                            kXdgConfigHomeEnvVar,
                                            kDotConfigDir));

  if (!config_dir.AppendRelativePath(profile_dir, &cache_dir))
    return;

  *result = cache_dir;
}

bool GetUserDocumentsDirectory(base::FilePath* result) {
  *result = GetXDGUserDirectory("DOCUMENTS", "Documents");
  return true;
}

bool GetUserDownloadsDirectorySafe(base::FilePath* result) {
  base::FilePath home;
  base::PathService::Get(base::DIR_HOME, &home);
  *result = home.Append(kDownloadsDir);
  return true;
}

bool GetUserDownloadsDirectory(base::FilePath* result) {
  *result = GetXDGUserDirectory("DOWNLOAD", kDownloadsDir);
  return true;
}

// We respect the user's preferred pictures location, unless it is
// ~ or their desktop directory, in which case we default to ~/Music.
bool GetUserMusicDirectory(base::FilePath* result) {
  return GetUserMediaDirectory("MUSIC", kMusicDir, result);
}

// We respect the user's preferred pictures location, unless it is
// ~ or their desktop directory, in which case we default to ~/Pictures.
bool GetUserPicturesDirectory(base::FilePath* result) {
  return GetUserMediaDirectory("PICTURES", kPicturesDir, result);
}

// We respect the user's preferred pictures location, unless it is
// ~ or their desktop directory, in which case we default to ~/Videos.
bool GetUserVideosDirectory(base::FilePath* result) {
  return GetUserMediaDirectory("VIDEOS", kVideosDir, result);
}

bool ProcessNeedsProfileDir(const std::string& process_type) {
  // For now we have no reason to forbid this on Linux as we don't
  // have the roaming profile troubles there. Moreover the Linux breakpad needs
  // profile dir access in all process if enabled on Linux.
  return true;
}

}  // namespace chrome
