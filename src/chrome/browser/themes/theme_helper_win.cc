// Copyright 2024 The Chromium Authors and Alex313031
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "chrome/browser/themes/theme_helper_win.h"

#include "base/command_line.h"
#include "chrome/browser/themes/custom_theme_supplier.h"
#include "chrome/browser/themes/theme_properties.h"
#include "chrome/browser/win/mica_titlebar.h"
#include "chrome/browser/win/titlebar_config.h"
#include "chrome/grit/theme_resources.h"

int ThemeHelperWin::GetDefaultDisplayProperty(int id) const {
  if (id == ThemeProperties::SHOULD_FILL_BACKGROUND_TAB_COLOR) {
    return !ShouldDefaultThemeUseMicaTitlebar();
  }

  return ThemeHelper::GetDefaultDisplayProperty(id);
}

bool ThemeHelperWin::ShouldUseNativeFrame(
    const CustomThemeSupplier* theme_supplier) const {
  return !base::CommandLine::ForCurrentProcess()->HasSwitch("disable-aero");
}
