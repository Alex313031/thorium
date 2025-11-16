// Copyright (c) 2024 Alex313031.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef CHROME_BROWSER_THORIUM_FLAG_CHOICES_H_
#define CHROME_BROWSER_THORIUM_FLAG_CHOICES_H_

const FeatureEntry::Choice kCustomTabWidthChoices[] = {
    {flags_ui::kGenericExperimentChoiceDefault, "", ""},
    {"60",
     "custom-tab-width",
     "60"},
    {"120",
     "custom-tab-width",
     "120"},
    {"180",
     "custom-tab-width",
     "180"},
    {"240",
     "custom-tab-width",
     "240"},
    {"300",
     "custom-tab-width",
     "300"},
    {"400",
     "custom-tab-width",
     "400"}
};

const FeatureEntry::Choice kScrollEventChangesTab[] = {
    {flags_ui::kGenericExperimentChoiceDefault, "", ""},
    {"Always",
     "scroll-tabs",
     "always"},
    {"Never",
     "scroll-tabs",
     "never"}
};

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
     switches::kNumRasterThreads, "1"},
    {"2",
     switches::kNumRasterThreads, "2"},
    {"3",
     switches::kNumRasterThreads, "3"},
    {"4",
     switches::kNumRasterThreads, "4"},
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

#if BUILDFLAG(IS_LINUX)
const FeatureEntry::Choice kGtkVersionChoices[] = {
    {flags_ui::kGenericExperimentChoiceDefault, "", ""},
    {"3",
     "gtk-version", "3"},
    {"4",
     "gtk-version", "4"},
};
#endif // BUILDFLAG(IS_LINUX)

const FeatureEntry::Choice kTabHoverCardChoices[] = {
    {flags_ui::kGenericExperimentChoiceDefault, "", ""},
    {"None",
     "tab-hover-cards",
     "none"},
    {"Tooltip",
     "tab-hover-cards",
     "tooltip"},
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

const FeatureEntry::Choice kCloseWindowWithLastTab[] = {
    {flags_ui::kGenericExperimentChoiceDefault, "", ""},
    {"Never",
     "close-window-with-last-tab",
     "never"},
};

#if BUILDFLAG(IS_LINUX)
const FeatureEntry::Choice kPasswordStoreChoices[] = {
    {flags_ui::kGenericExperimentChoiceDefault, "", ""},
    {"Basic",
     password_manager::kPasswordStore, "basic"},
    {"Kwallet",
     password_manager::kPasswordStore, "kwallet"},
    {"Kwallet5",
     password_manager::kPasswordStore, "kwallet5"},
    {"Gnome",
     password_manager::kPasswordStore, "gnome"},
    {"Gnome-Keyring",
     password_manager::kPasswordStore, "gnome-keyring"},
    {"Gnome-LibSecret",
     password_manager::kPasswordStore, "gnome-libsecret"},
};
#endif // BUILDFLAG(IS_LINUX)

#endif  // CHROME_BROWSER_THORIUM_FLAG_CHOICES_H_
