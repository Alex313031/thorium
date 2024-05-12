// Copyright 2024 The Chromium Authors and Alex313031
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "components/dom_distiller/core/dom_distiller_features.h"

#include <string>

#include "base/command_line.h"
#include "base/metrics/field_trial_params.h"
#include "components/dom_distiller/core/dom_distiller_switches.h"
#include "components/dom_distiller/core/pref_names.h"

namespace dom_distiller {

BASE_FEATURE(kReaderMode, "ReaderMode", base::FEATURE_ENABLED_BY_DEFAULT);

bool IsDomDistillerEnabled() {
  return true;
}

bool OfferReaderModeInSettings() {
    return true;
}

bool ShowReaderModeOption() {
  if (OfferReaderModeInSettings())
    return true;
  return IsDomDistillerEnabled();
}

bool ShouldStartDistillabilityService() {
  return base::CommandLine::ForCurrentProcess()->HasSwitch(
             switches::kEnableDistillabilityService) ||
         base::FeatureList::IsEnabled(kReaderMode);
}

}  // namespace dom_distiller
