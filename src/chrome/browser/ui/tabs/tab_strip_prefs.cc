// Copyright 2024 The Chromium Authors and Alex313031
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "chrome/browser/ui/tabs/tab_strip_prefs.h"

#include "base/command_line.h"
#include "chrome/browser/profiles/profile.h"
#include "chrome/browser/ui/tabs/features.h"
#include "chrome/browser/ui/ui_features.h"
#include "chrome/common/pref_names.h"
#include "components/pref_registry/pref_registry_syncable.h"
#include "components/prefs/pref_service.h"

namespace {

std::optional<bool> g_tab_search_trailing_tabstrip_at_startup = std::nullopt;
}

namespace tabs {

bool GetDefaultTabSearchRightAligned() {
  // These platforms are all left aligned, the others should be right.
#if BUILDFLAG(IS_CHROMEOS) || BUILDFLAG(IS_LINUX) || BUILDFLAG(IS_WIN)
  // Revert Google's stupid UI design decision
  static const bool left_aligned_tab_search_button =
      base::CommandLine::ForCurrentProcess()->HasSwitch("left-aligned-tab-search-button");
  if (left_aligned_tab_search_button) {
    return false;
  } else {
    return true;
  }
#else
  return true;
#endif
}

void RegisterProfilePrefs(user_prefs::PrefRegistrySyncable* registry) {
  registry->RegisterBooleanPref(prefs::kTabSearchRightAligned,
                                GetDefaultTabSearchRightAligned());
}

bool GetTabSearchTrailingTabstrip(const Profile* profile) {
  // If this pref has already been read, we need to return the same value.
  if (!g_tab_search_trailing_tabstrip_at_startup.has_value()) {
    g_tab_search_trailing_tabstrip_at_startup =
        profile && CanShowTabSearchPositionSetting()
            ? profile->GetPrefs()->GetBoolean(prefs::kTabSearchRightAligned)
            : GetDefaultTabSearchRightAligned();
  }

  return g_tab_search_trailing_tabstrip_at_startup.value();
}

void SetTabSearchRightAlignedForTesting(bool is_right_aligned) {
  g_tab_search_trailing_tabstrip_at_startup = is_right_aligned;
}

}  // namespace tabs
