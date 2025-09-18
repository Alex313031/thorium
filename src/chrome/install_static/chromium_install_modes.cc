// Copyright 2025 The Chromium Authors and Alex313031
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Brand-specific constants and install modes for Thorium.

#include "chrome/install_static/chromium_install_modes.h"

#include <stdlib.h>

#include "chrome/install_static/install_modes.h"

namespace install_static {

const wchar_t kCompanyPathName[] = L"";

const wchar_t kProductPathName[] = L"Thorium";

const size_t kProductPathNameLength = _countof(kProductPathName) - 1;

const char kSafeBrowsingName[] = "thorium";

}  // namespace install_static
