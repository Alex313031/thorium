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

bool IsDomDistillerEnabled() {
  static const bool kReaderModeDesktop =
      base::CommandLine::ForCurrentProcess()->HasSwitch("reader-mode");
  return base::CommandLine::ForCurrentProcess()->HasSwitch(
      switches::kEnableDomDistiller) || kReaderModeDesktop;
}

bool ShouldStartDistillabilityService() {
  static const bool kReaderModeDesktop =
      base::CommandLine::ForCurrentProcess()->HasSwitch("reader-mode");
  return base::CommandLine::ForCurrentProcess()->HasSwitch(
      switches::kEnableDistillabilityService) || kReaderModeDesktop;
}

}  // namespace dom_distiller
