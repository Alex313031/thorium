// Copyright 2024 The Chromium Authors and Alex313031
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "components/dom_distiller/core/dom_distiller_features.h"

#include <string>

#include "base/command_line.h"
#include "base/metrics/field_trial_params.h"
#include "components/dom_distiller/core/dom_distiller_switches.h"
#include "components/dom_distiller/core/pref_names.h"
#include "components/pref_registry/pref_registry_syncable.h"
#include "components/prefs/pref_service.h"

namespace dom_distiller {

BASE_FEATURE(kReaderMode, "ReaderMode", base::FEATURE_ENABLED_BY_DEFAULT);

void RegisterProfilePrefs(user_prefs::PrefRegistrySyncable* registry) {
  // Whether the reader mode option should be shown on distillable pages.
  registry->RegisterBooleanPref(
      dom_distiller::prefs::kOfferReaderMode, true,
      user_prefs::PrefRegistrySyncable::SYNCABLE_PREF);
}

bool IsDomDistillerEnabled() {
  return true;
}

bool OfferReaderModeInSettings() {
    return true;
}

bool ShowReaderModeOption(PrefService* pref_service) {
  if (OfferReaderModeInSettings())
    return pref_service->GetBoolean(prefs::kOfferReaderMode);
  return IsDomDistillerEnabled();
}

bool ShouldStartDistillabilityService() {
  return base::CommandLine::ForCurrentProcess()->HasSwitch(
             switches::kEnableDistillabilityService) ||
         base::FeatureList::IsEnabled(kReaderMode);
}

}  // namespace dom_distiller
