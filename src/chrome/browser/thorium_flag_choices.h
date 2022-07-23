// Copyright (c) 2022 Alex313031. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef CHROME_BROWSER_THORIUM_FLAG_CHOICES_H_
#define CHROME_BROWSER_THORIUM_FLAG_CHOICES_H_
const FeatureEntry::Choice kShowAvatarButtonChoices[] = {
    {flags_ui::kGenericExperimentChoiceDefault, "", ""},
    {"Always",
     "show-avatar-button",
     "always"},
    {"Incognito and Guest",
     "show-avatar-button",
     "incognito-and-guest"},
    {"Never",
     "show-avatar-button",
     "never"}
};

const FeatureEntry::Choice kWebglMSAASampleCountChoices[] = {
    {flags_ui::kGenericExperimentChoiceDefault, "", ""},
    {"0",
     switches::kWebglMSAASampleCount, "0"},
    {"2",
     switches::kWebglMSAASampleCount, "2"},
    {"4",
     switches::kWebglMSAASampleCount, "4"},
    {"8",
     switches::kWebglMSAASampleCount, "8"},
    {"16",
     switches::kWebglMSAASampleCount, "16"},
};

const FeatureEntry::Choice kGpuRasterizationMSAASampleCountChoices[] = {
    {flags_ui::kGenericExperimentChoiceDefault, "", ""},
    {"0",
     blink::switches::kGpuRasterizationMSAASampleCount, "0"},
    {"2",
     blink::switches::kGpuRasterizationMSAASampleCount, "2"},
    {"4",
     blink::switches::kGpuRasterizationMSAASampleCount, "4"},
    {"8",
     blink::switches::kGpuRasterizationMSAASampleCount, "8"},
    {"16",
     blink::switches::kGpuRasterizationMSAASampleCount, "16"},
};

const FeatureEntry::Choice kNumRasterThreadsChoices[] = {
    {flags_ui::kGenericExperimentChoiceDefault, "", ""},
    {"1",
     blink::switches::kNumRasterThreads, "1"},
    {"2",
     blink::switches::kNumRasterThreads, "2"},
    {"3",
     blink::switches::kNumRasterThreads, "3"},
    {"4",
     blink::switches::kNumRasterThreads, "4"},
    {"6",
     blink::switches::kNumRasterThreads, "6"},
};
#endif  // CHROME_BROWSER_THORIUM_FLAG_CHOICES_H_
