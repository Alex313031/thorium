// Copyright (c) 2023 Alex313031. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef CHROME_BROWSER_THORIUM_FLAG_CHOICES_H_
#define CHROME_BROWSER_THORIUM_FLAG_CHOICES_H_

const FeatureEntry::Choice kAutoplayPolicyChoices[] = {
    {flags_ui::kGenericExperimentChoiceDefault, "", ""},
    {"No User Gesture Required",
     switches::kAutoplayPolicy, "no-user-gesture-required"},
    {"User Gesture Required",
     switches::kAutoplayPolicy, "user-gesture-required"},
    {"Document User Activation Required",
     switches::kAutoplayPolicy, "document-user-activation-required"},
};

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

const FeatureEntry::Choice kWebglAntialiasingModeChoices[] = {
    {flags_ui::kGenericExperimentChoiceDefault, "", ""},
    {"None",
     switches::kWebglAntialiasingMode, "none"},
    {"Explicit",
     switches::kWebglAntialiasingMode, "explicit"},
    {"Implicit",
     switches::kWebglAntialiasingMode, "implicit"},
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
};

const FeatureEntry::Choice kForceGpuMemAvailableMbChoices[] = {
    {flags_ui::kGenericExperimentChoiceDefault, "", ""},
    {"128",
     "force-gpu-mem-available-mb", "128"},
    {"256",
     "force-gpu-mem-available-mb", "256"},
    {"512",
     "force-gpu-mem-available-mb", "512"},
    {"1024",
     "force-gpu-mem-available-mb", "1024"},
};

const FeatureEntry::Choice kCloseConfirmation[] = {
    {flags_ui::kGenericExperimentChoiceDefault, "", ""},
    {"Show confirmation with last window",
     "close-confirmation",
     "last"},
    {"Show confirmation with multiple windows",
     "close-confirmation",
     "multiple"},
};

#if BUILDFLAG(IS_LINUX)
const FeatureEntry::Choice kPasswordStoreChoices[] = {
    {flags_ui::kGenericExperimentChoiceDefault, "", ""},
    {"Basic",
     switches::kPasswordStore, "basic"},
    {"Kwallet",
     switches::kPasswordStore, "kwallet"},
    {"Kwallet5",
     switches::kPasswordStore, "kwallet5"},
    {"Gnome",
     switches::kPasswordStore, "gnome"},
    {"Gnome-Keyring",
     switches::kPasswordStore, "gnome-keyring"},
    {"Gnome-LibSecret",
     switches::kPasswordStore, "gnome-libsecret"},
};
#endif // BUILDFLAG(IS_LINUX)

#endif  // CHROME_BROWSER_THORIUM_FLAG_CHOICES_H_
