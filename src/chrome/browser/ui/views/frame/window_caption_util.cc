// Copyright 2024 The Chromium Authors and Alex313031
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "chrome/browser/ui/views/frame/window_caption_util.h"

#include "build/build_config.h"
#include "chrome/browser/ui/browser.h"
#include "chrome/browser/ui/thorium_2024.h"

// static
bool WindowCaptionUtil::IsWindowsTabSearchCaptionButtonEnabled(
    const Browser* browser) {
#if BUILDFLAG(IS_WIN)
  static const bool remove_tabsearch_button =
      base::CommandLine::ForCurrentProcess()->HasSwitch("remove-tabsearch-button");
  static const bool disable_caption_button =
      base::CommandLine::ForCurrentProcess()->HasSwitch("disable-caption-button");
  return features::IsThorium2024() && browser->is_type_normal() &&
      !remove_tabsearch_button && !disable_caption_button;
#else
  return false;
#endif  // BUILDFLAG(IS_WIN)
}
