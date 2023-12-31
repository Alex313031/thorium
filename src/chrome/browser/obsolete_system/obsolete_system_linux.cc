// Copyright 2023 The Chromium Authors and Alex313031
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "chrome/browser/obsolete_system/obsolete_system.h"

#include "base/linux_util.h"
#include "base/strings/string_util.h"
#include "chrome/grit/branded_strings.h"
#include "ui/base/l10n/l10n_util.h"

const char kLinuxObsoleteUrl[] =
    "https://support.google.com/chrome/?p=unsupported_linux";

// This list contains the 2 most recently obsoleted distros according to
// `kLinuxObsoleteUrl`.
const char* const kObsoleteDistros[] = {
    "Ubuntu 12.04", "Ubuntu 14.04", "Debian 7",           "Debian 8",
    "Fedora 30",    "Fedora 31",    "openSUSE Leap 15.0", "openSUSE Leap 15.1",
};

namespace ObsoleteSystem {

bool IsObsoleteNowOrSoon() {
  auto distro = base::GetLinuxDistro();
  for (const char* obsolete : kObsoleteDistros) {
    if (base::StartsWith(distro, obsolete)) {
      return true;
    }
  }
  return false;
}

std::u16string LocalizedObsoleteString() {
  return l10n_util::GetStringUTF16(IDS_LINUX_OBSOLETE);
}

bool IsEndOfTheLine() {
  return false;
}

const char* GetLinkURL() {
  return kLinuxObsoleteUrl;
}

}  // namespace ObsoleteSystem
