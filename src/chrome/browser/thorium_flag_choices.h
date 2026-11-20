// Copyright (c) 2026 Alex313031.
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
    {"User Gesture Required (Per Media Element)",
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

const FeatureEntry::Choice kEncryptedClientHelloChoices[] = {
    {flags_ui::kGenericExperimentChoiceDefault, "", ""},
    {"Enabled",
     "encrypted-client-hello", "enabled"},
    {"Disabled",
     "encrypted-client-hello", "disabled"},
};

const FeatureEntry::Choice kWebRtcIPHandlingPolicyChoices[] = {
    {flags_ui::kGenericExperimentChoiceDefault, "", ""},
    {"All interfaces (Chromium default)",
     switches::kWebRtcIPHandlingPolicy, "default"},
    {"Default public and private interfaces",
     switches::kWebRtcIPHandlingPolicy,
     "default_public_and_private_interfaces"},
    {"Default public interface only",
     switches::kWebRtcIPHandlingPolicy, "default_public_interface_only"},
    {"Disable non-proxied UDP",
     switches::kWebRtcIPHandlingPolicy, "disable_non_proxied_udp"},
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
    {"Show confirmation with any window",
     "close-confirmation",
     "any"},
};

const FeatureEntry::Choice kCloseWindowWithLastTab[] = {
    {flags_ui::kGenericExperimentChoiceDefault, "", ""},
    {"Never",
     "close-window-with-last-tab",
     "never"},
};

constexpr flags_ui::FeatureEntry::FeatureParam kHoverActivateTab250ms[] = {
    {"hover_delay_ms", "250"}};
constexpr flags_ui::FeatureEntry::FeatureParam kHoverActivateTab500ms[] = {
    {"hover_delay_ms", "500"}};
constexpr flags_ui::FeatureEntry::FeatureParam kHoverActivateTab750ms[] = {
    {"hover_delay_ms", "750"}};
constexpr flags_ui::FeatureEntry::FeatureParam kHoverActivateTab1000ms[] = {
    {"hover_delay_ms", "1000"}};

constexpr flags_ui::FeatureEntry::FeatureVariation
    kHoverActivateTabVariations[] = {
    {"250ms delay", kHoverActivateTab250ms, nullptr},
    {"500ms delay (default)", kHoverActivateTab500ms, nullptr},
    {"750ms delay", kHoverActivateTab750ms, nullptr},
    {"1000ms delay", kHoverActivateTab1000ms, nullptr}};

constexpr flags_ui::FeatureEntry::FeatureParam
    kOpenBookmarksInNewTabForeground[] = {
    {"mode", "foreground"}};
constexpr flags_ui::FeatureEntry::FeatureParam
    kOpenBookmarksInNewTabBackground[] = {
    {"mode", "background"}};

constexpr flags_ui::FeatureEntry::FeatureVariation
    kOpenBookmarksInNewTabVariations[] = {
    {"Open in foreground tab (default)", kOpenBookmarksInNewTabForeground,
     nullptr},
    {"Open in background tab", kOpenBookmarksInNewTabBackground,
     nullptr}};

constexpr flags_ui::FeatureEntry::FeatureParam
    kOpenOmniboxUrlInNewTabForeground[] = {
    {"mode", "foreground"}};
constexpr flags_ui::FeatureEntry::FeatureParam
    kOpenOmniboxUrlInNewTabBackground[] = {
    {"mode", "background"}};

constexpr flags_ui::FeatureEntry::FeatureVariation
    kOpenOmniboxUrlInNewTabVariations[] = {
    {"Open in foreground tab (default)", kOpenOmniboxUrlInNewTabForeground,
     nullptr},
    {"Open in background tab", kOpenOmniboxUrlInNewTabBackground,
     nullptr}};

#if BUILDFLAG(IS_LINUX)
const FeatureEntry::Choice kPasswordStoreChoices[] = {
    {flags_ui::kGenericExperimentChoiceDefault, "", ""},
    {"Basic",
     password_manager::kPasswordStore, "basic"},
    {"Kwallet",
     password_manager::kPasswordStore, "kwallet"},
    {"Kwallet5",
     password_manager::kPasswordStore, "kwallet5"},
    {"Kwallet6",
     password_manager::kPasswordStore, "kwallet6"},
    {"Gnome-LibSecret",
     password_manager::kPasswordStore, "gnome-libsecret"},
};
#endif // BUILDFLAG(IS_LINUX)

#endif  // CHROME_BROWSER_THORIUM_FLAG_CHOICES_H_
