// Copyright 2024 The Chromium Authors and Alex313031
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "chrome/browser/ui/ui_features.h"

#include "base/feature_list.h"
#include "base/metrics/field_trial_params.h"
#include "build/build_config.h"
#include "build/chromeos_buildflags.h"
#include "components/flags_ui/feature_entry.h"
#include "ui/base/ui_base_features.h"

namespace features {

// Enables the tab dragging fallback when full window dragging is not supported
// by the platform (e.g. Wayland). See https://crbug.com/896640
// TODO: Alex313031 possibly re-enable? Causes issues on newer Wayland
BASE_FEATURE(kAllowWindowDragUsingSystemDragDrop,
             "AllowWindowDragUsingSystemDragDrop",
             base::FEATURE_DISABLED_BY_DEFAULT);

// Enables the use of WGC for the Eye Dropper screen capture.
BASE_FEATURE(kAllowEyeDropperWGCScreenCapture,
             "AllowEyeDropperWGCScreenCapture",
#if BUILDFLAG(IS_WIN)
             base::FEATURE_ENABLED_BY_DEFAULT
#else
             base::FEATURE_DISABLED_BY_DEFAULT
#endif  // BUILDFLAG(IS_WIN)
);

// Enables Chrome Labs menu in the toolbar. See https://crbug.com/1145666
BASE_FEATURE(kChromeLabs, "ChromeLabs", base::FEATURE_ENABLED_BY_DEFAULT);
const char kChromeLabsActivationParameterName[] =
    "chrome_labs_activation_percentage";
const base::FeatureParam<int> kChromeLabsActivationPercentage{
    &kChromeLabs, kChromeLabsActivationParameterName, 99};

// When enabled, clicks outside the omnibox and its popup will close an open
// omnibox popup.
BASE_FEATURE(kCloseOmniboxPopupOnInactiveAreaClick,
             "CloseOmniboxPopupOnInactiveAreaClick",
             base::FEATURE_ENABLED_BY_DEFAULT);

// Create new Extensions app menu option (removing "More Tools -> Extensions")
// with submenu to manage extensions and visit chrome web store.
BASE_FEATURE(kExtensionsMenuInAppMenu,
             "ExtensionsMenuInAppMenu",
             base::FEATURE_ENABLED_BY_DEFAULT);

bool IsExtensionMenuInRootAppMenu() {
  return base::FeatureList::IsEnabled(kExtensionsMenuInAppMenu) ||
         features::IsChromeRefresh2023();
}

#if !defined(ANDROID)
// Enables "Access Code Cast" UI.
BASE_FEATURE(kAccessCodeCastUI,
             "AccessCodeCastUI",
             base::FEATURE_ENABLED_BY_DEFAULT);
#endif

#if !BUILDFLAG(IS_ANDROID) && !BUILDFLAG(IS_CHROMEOS) && !BUILDFLAG(IS_FUCHSIA)
// Enables camera preview in permission bubble and site settings.
BASE_FEATURE(kCameraMicPreview,
             "CameraMicPreview",
             base::FEATURE_DISABLED_BY_DEFAULT);
#endif

// Enables displaying the submenu to open a link with a different profile if
// there is at least one other active profile. Fully rolled out on Desktop.
BASE_FEATURE(kDisplayOpenLinkAsProfile,
             "DisplayOpenLinkAsProfile",
             base::FEATURE_ENABLED_BY_DEFAULT);

// Enables showing the EV certificate details in the Page Info bubble.
BASE_FEATURE(kEvDetailsInPageInfo,
             "EvDetailsInPageInfo",
             base::FEATURE_ENABLED_BY_DEFAULT);

#if !BUILDFLAG(IS_ANDROID) && BUILDFLAG(GOOGLE_CHROME_BRANDING)
// Enables showing the "Get the most out of Chrome" section in settings.
BASE_FEATURE(kGetTheMostOutOfChrome,
             "GetTheMostOutOfChrome",
             base::FEATURE_DISABLED_BY_DEFAULT);
#endif

#if !BUILDFLAG(IS_ANDROID)
// Enables or disables the Happiness Tracking Surveys being delivered via chrome
// webui, rather than a separate static website.
BASE_FEATURE(kHaTSWebUI, "HaTSWebUI", base::FEATURE_DISABLED_BY_DEFAULT);
#endif  // !BUILDFLAG(IS_ANDROID)

#if BUILDFLAG(ENABLE_EXTENSIONS)
// Controls whether we use a different UX for simple extensions overriding
// settings.
BASE_FEATURE(kLightweightExtensionOverrideConfirmations,
             "LightweightExtensionOverrideConfirmations",
             base::FEATURE_ENABLED_BY_DEFAULT);
#endif

// Enables the QuickCommands UI surface. See https://crbug.com/1014639
BASE_FEATURE(kQuickCommands,
             "QuickCommands",
             base::FEATURE_ENABLED_BY_DEFAULT);

// Enable responsive toolbar. Toolbar buttons overflow to a chevron button when
// the browser width is resized smaller than normal.
BASE_FEATURE(kResponsiveToolbar,
             "ResponsiveToolbar",
             base::FEATURE_ENABLED_BY_DEFAULT);

// Enables the side search feature for Google Search. Presents recent Google
// search results in a browser side panel.
BASE_FEATURE(kSideSearch, "SideSearch", base::FEATURE_DISABLED_BY_DEFAULT);

BASE_FEATURE(kSideSearchFeedback,
             "SideSearchFeedback",
             base::FEATURE_DISABLED_BY_DEFAULT);

// Displays right-click search results of a highlighted text in side panel,
// So users are not forced to switch to a new tab to view the search results
BASE_FEATURE(kSearchWebInSidePanel,
             "SearchWebInSidePanel",
             base::FEATURE_DISABLED_BY_DEFAULT);

// Feature that controls whether or not feature engagement configurations can be
// used to control automatic triggering for side search.
BASE_FEATURE(kSideSearchAutoTriggering,
             "SideSearchAutoTriggering",
             base::FEATURE_ENABLED_BY_DEFAULT);

// Feature param that determines how many times a user has to return to a given
// SRP before we automatically trigger the side search side panel for that SRP
// on a subsequent navigation.
const base::FeatureParam<int> kSideSearchAutoTriggeringReturnCount{
    &kSideSearchAutoTriggering, "SideSearchAutoTriggeringReturnCount", 2};

BASE_FEATURE(kSidePanelWebView,
             "SidePanelWebView",
             base::FEATURE_ENABLED_BY_DEFAULT);

BASE_FEATURE(kSidePanelJourneysQueryless,
             "SidePanelJourneysQueryless",
             base::FEATURE_DISABLED_BY_DEFAULT);

#if !defined(ANDROID)
BASE_FEATURE(kSidePanelCompanionDefaultPinned,
             "SidePanelCompanionDefaultPinned",
             base::FEATURE_DISABLED_BY_DEFAULT);

BASE_FEATURE(kSidePanelPinning,
             "SidePanelPinning",
             base::FEATURE_DISABLED_BY_DEFAULT);

bool IsSidePanelPinningEnabled() {
  return (IsChromeRefresh2023() &&
          base::FeatureList::IsEnabled(kSidePanelPinning));
}

BASE_FEATURE(kSidePanelMinimumWidth,
             "SidePanelMinimumWidth",
             base::FEATURE_DISABLED_BY_DEFAULT);
const base::FeatureParam<int> kSidePanelMinimumWidthParameter{
    &kSidePanelMinimumWidth, "minPanelWidth", 360};
int GetSidePanelMinimumWidth() {
  if (base::FeatureList::IsEnabled(kSidePanelMinimumWidth)) {
    return kSidePanelMinimumWidthParameter.Get();
  }

  // This is the default value used without this feature.
  return 320;
}

#endif

// Enables tabs to scroll in the tabstrip. https://crbug.com/951078
BASE_FEATURE(kScrollableTabStrip,
             "ScrollableTabStrip",
             base::FEATURE_DISABLED_BY_DEFAULT);
const char kMinimumTabWidthFeatureParameterName[] = "minTabWidth";

// Enables buttons when scrolling the tabstrip https://crbug.com/951078
BASE_FEATURE(kTabScrollingButtonPosition,
             "TabScrollingButtonPosition",
             base::FEATURE_ENABLED_BY_DEFAULT);
const char kTabScrollingButtonPositionParameterName[] = "buttonPosition";

// Enables tab scrolling while dragging tabs in tabstrip
// https://crbug.com/1145747
BASE_FEATURE(kScrollableTabStripWithDragging,
             "kScrollableTabStripWithDragging",
             base::FEATURE_ENABLED_BY_DEFAULT);
const char kTabScrollingWithDraggingModeName[] = "tabScrollWithDragMode";

// Enables different methods of overflow when scrolling tabs in tabstrip
// https://crbug.com/951078
BASE_FEATURE(kScrollableTabStripOverflow,
             "kScrollableTabStripOverflow",
             base::FEATURE_DISABLED_BY_DEFAULT);
const char kScrollableTabStripOverflowModeName[] = "tabScrollOverflow";

// Splits pinned and unpinned tabs into separate TabStrips.
// https://crbug.com/1346019
// TODO: Alex313031 Possibly Re-Enable after feedback
BASE_FEATURE(kSplitTabStrip,
             "SplitTabStrip",
             base::FEATURE_DISABLED_BY_DEFAULT);

// Enables tabs to be frozen when collapsed.
// https://crbug.com/1110108
BASE_FEATURE(kTabGroupsCollapseFreezing,
             "TabGroupsCollapseFreezing",
             base::FEATURE_ENABLED_BY_DEFAULT);

// Enables users to explicitly save and recall tab groups.
// https://crbug.com/1223929
BASE_FEATURE(kTabGroupsSave,
             "TabGroupsSave",
             base::FEATURE_ENABLED_BY_DEFAULT);

// Enables configuring tab hover card image previews in the settings.
BASE_FEATURE(kTabHoverCardImageSettings,
             "TabHoverCardImageSettings",
#if BUILDFLAG(IS_MAC)
             base::FEATURE_DISABLED_BY_DEFAULT
#else
             base::FEATURE_ENABLED_BY_DEFAULT
#endif
);

// Enables preview images in tab-hover cards.
// https://crbug.com/928954
BASE_FEATURE(kTabHoverCardImages,
             "TabHoverCardImages",
#if BUILDFLAG(IS_MAC)
             base::FEATURE_DISABLED_BY_DEFAULT
#else
             base::FEATURE_ENABLED_BY_DEFAULT
#endif
);

const char kTabHoverCardImagesNotReadyDelayParameterName[] =
    "page_not_ready_delay";
const char kTabHoverCardImagesLoadingDelayParameterName[] =
    "page_loading_delay";
const char kTabHoverCardImagesLoadedDelayParameterName[] = "page_loaded_delay";
const char kTabHoverCardImagesCrossfadePreviewAtParameterName[] =
    "crossfade_preview_at";
const char kTabHoverCardAdditionalMaxWidthDelay[] =
    "additional_max_width_delay";

BASE_FEATURE(kTabOrganization,
             "TabOrganization",
             base::FEATURE_DISABLED_BY_DEFAULT);

bool IsTabOrganization() {
  return IsChromeRefresh2023() &&
         base::FeatureList::IsEnabled(features::kTabOrganization);
}

const base::FeatureParam<base::TimeDelta> kTabOrganizationTriggerPeriod{
    &kTabOrganization, "trigger_period", base::Hours(6)};

const base::FeatureParam<double> kTabOrganizationTriggerBackoffBase{
    &kTabOrganization, "backoff_base", 2.0};

const base::FeatureParam<double> kTabOrganizationTriggerThreshold{
    &kTabOrganization, "trigger_threshold", 7.0};

const base::FeatureParam<double> kTabOrganizationTriggerSensitivityThreshold{
    &kTabOrganization, "trigger_sensitivity_threshold", 0.5};

const base::FeatureParam<bool> KTabOrganizationTriggerDemoMode{
    &kTabOrganization, "trigger_demo_mode", false};

BASE_FEATURE(kTabOrganizationRefreshButton,
             "TabOrganizationRefreshButton",
             base::FEATURE_DISABLED_BY_DEFAULT);

BASE_FEATURE(kTabSearchChevronIcon,
             "TabSearchChevronIcon",
             base::FEATURE_ENABLED_BY_DEFAULT);

// Enables the tab search submit feedback button.
BASE_FEATURE(kTabSearchFeedback,
             "TabSearchFeedback",
             base::FEATURE_DISABLED_BY_DEFAULT);

// Controls whether or not to use fuzzy search for tab search.
BASE_FEATURE(kTabSearchFuzzySearch,
             "TabSearchFuzzySearch",
             base::FEATURE_DISABLED_BY_DEFAULT);

const char kTabSearchSearchThresholdName[] = "TabSearchSearchThreshold";

const base::FeatureParam<bool> kTabSearchSearchIgnoreLocation{
    &kTabSearchFuzzySearch, "TabSearchSearchIgnoreLocation", false};

// If this feature parameter is enabled, show media tabs in both "Audio & Video"
// section and "Open Tabs" section.
const char kTabSearchAlsoShowMediaTabsinOpenTabsSectionParameterName[] =
    "Also show Media Tabs in Open Tabs Section";

const base::FeatureParam<int> kTabSearchSearchDistance{
    &kTabSearchFuzzySearch, "TabSearchSearchDistance", 200};

const base::FeatureParam<double> kTabSearchSearchThreshold{
    &kTabSearchFuzzySearch, kTabSearchSearchThresholdName, 0.6};

const base::FeatureParam<double> kTabSearchTitleWeight{
    &kTabSearchFuzzySearch, "TabSearchTitleWeight", 2.0};

const base::FeatureParam<double> kTabSearchHostnameWeight{
    &kTabSearchFuzzySearch, "TabSearchHostnameWeight", 1.0};

const base::FeatureParam<double> kTabSearchGroupTitleWeight{
    &kTabSearchFuzzySearch, "TabSearchGroupTitleWeight", 1.5};

const base::FeatureParam<bool> kTabSearchMoveActiveTabToBottom{
    &kTabSearchFuzzySearch, "TabSearchMoveActiveTabToBottom", true};

// Controls feature parameters for Tab Search's `Recently Closed` entries.
BASE_FEATURE(kTabSearchRecentlyClosed,
             "TabSearchRecentlyClosed",
             base::FEATURE_ENABLED_BY_DEFAULT);

const base::FeatureParam<int> kTabSearchRecentlyClosedDefaultItemDisplayCount{
    &kTabSearchRecentlyClosed, "TabSearchRecentlyClosedDefaultItemDisplayCount",
    8};

const base::FeatureParam<int> kTabSearchRecentlyClosedTabCountThreshold{
    &kTabSearchRecentlyClosed, "TabSearchRecentlyClosedTabCountThreshold", 100};

BASE_FEATURE(kTabSearchUseMetricsReporter,
             "TabSearchUseMetricsReporter",
             base::FEATURE_DISABLED_BY_DEFAULT);

BASE_FEATURE(kToolbarUseHardwareBitmapDraw,
             "ToolbarUseHardwareBitmapDraw",
             base::FEATURE_DISABLED_BY_DEFAULT);

// Controls whether top chrome pages will use the spare renderer if no top
// chrome renderers are present.
BASE_FEATURE(kTopChromeWebUIUsesSpareRenderer,
             "TopChromeWebUIUsesSpareRenderer",
             base::FEATURE_ENABLED_BY_DEFAULT);

#if BUILDFLAG(IS_WIN) || BUILDFLAG(IS_MAC) || BUILDFLAG(IS_LINUX)
// Enables alternate update-related text to be displayed in browser app menu
// button, menu item and confirmation dialog.
BASE_FEATURE(kUpdateTextOptions,
             "UpdateTextOptions",
             base::FEATURE_ENABLED_BY_DEFAULT);
// Used to present different flavors of update strings in browser app menu
// button.
const base::FeatureParam<int> kUpdateTextOptionNumber{
    &kUpdateTextOptions, "UpdateTextOptionNumber", 2};
#endif

// This enables enables persistence of a WebContents in a 1-to-1 association
// with the current Profile for WebUI bubbles. See https://crbug.com/1177048.
BASE_FEATURE(kWebUIBubblePerProfilePersistence,
             "WebUIBubblePerProfilePersistence",
             base::FEATURE_DISABLED_BY_DEFAULT);

// Enables a web-based tab strip. See https://crbug.com/989131. Note this
// feature only works when the ENABLE_WEBUI_TAB_STRIP buildflag is enabled.
BASE_FEATURE(kWebUITabStrip,
             "WebUITabStrip",
#if BUILDFLAG(IS_CHROMEOS)
             base::FEATURE_ENABLED_BY_DEFAULT
#else
             base::FEATURE_ENABLED_BY_DEFAULT
#endif
);

// The default value of this flag is aligned with platform behavior to handle
// context menu with touch.
// TODO(crbug.com/1257626): Enable this flag for all platforms after launch.
BASE_FEATURE(kWebUITabStripContextMenuAfterTap,
             "WebUITabStripContextMenuAfterTap",
#if BUILDFLAG(IS_CHROMEOS_ASH)
             base::FEATURE_DISABLED_BY_DEFAULT
#else
             base::FEATURE_ENABLED_BY_DEFAULT
#endif
);

#if BUILDFLAG(IS_CHROMEOS)
BASE_FEATURE(kChromeOSTabSearchCaptionButton,
             "ChromeOSTabSearchCaptionButton",
             base::FEATURE_ENABLED_BY_DEFAULT);
#endif

#if BUILDFLAG(IS_MAC)
// Enabled an experiment which increases the prominence to grant MacOS system
// location permission to Chrome when location permissions have already been
// approved. https://crbug.com/1211052
BASE_FEATURE(kLocationPermissionsExperiment,
             "LocationPermissionsExperiment",
             base::FEATURE_DISABLED_BY_DEFAULT);
constexpr base::FeatureParam<int>
    kLocationPermissionsExperimentBubblePromptLimit{
        &kLocationPermissionsExperiment, "bubble_prompt_count", 3};
constexpr base::FeatureParam<int>
    kLocationPermissionsExperimentLabelPromptLimit{
        &kLocationPermissionsExperiment, "label_prompt_count", 5};

BASE_FEATURE(kViewsFirstRunDialog,
             "ViewsFirstRunDialog",
             base::FEATURE_DISABLED_BY_DEFAULT);

BASE_FEATURE(kViewsTaskManager,
             "ViewsTaskManager",
             base::FEATURE_DISABLED_BY_DEFAULT);

BASE_FEATURE(kViewsJSAppModalDialog,
             "ViewsJSAppModalDialog",
             base::FEATURE_DISABLED_BY_DEFAULT);

int GetLocationPermissionsExperimentBubblePromptLimit() {
  return kLocationPermissionsExperimentBubblePromptLimit.Get();
}
int GetLocationPermissionsExperimentLabelPromptLimit() {
  return kLocationPermissionsExperimentLabelPromptLimit.Get();
}
#endif

// Reduce resource usage when view is hidden by not rendering loading animation.
// TODO(crbug.com/1322081): Clean up the feature in M117.
BASE_FEATURE(kStopLoadingAnimationForHiddenWindow,
             "StopLoadingAnimationForHiddenWindow",
             base::FEATURE_ENABLED_BY_DEFAULT);

}  // namespace features
