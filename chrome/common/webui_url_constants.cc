// Copyright 2022 The Chromium Authors and Alex313031. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "chrome/common/webui_url_constants.h"

#include "base/strings/string_piece.h"
#include "build/build_config.h"
#include "build/chromeos_buildflags.h"
#include "components/history_clusters/history_clusters_internals/webui/url_constants.h"
#include "components/nacl/common/buildflags.h"
#include "components/optimization_guide/optimization_guide_internals/webui/url_constants.h"
#include "components/safe_browsing/core/common/web_ui_constants.h"
#include "extensions/buildflags/buildflags.h"
#include "third_party/blink/public/common/chrome_debug_urls.h"

namespace chrome {

// Please keep this file in the same order as the header.

// Note: Add hosts to |kChromeHostURLs| at the bottom of this file to be listed
// by chrome://chrome-urls (about:about) and the built-in AutocompleteProvider.

const char kChromeUIAboutHost[] = "about";
const char kChromeUIAboutURL[] = "thorium://about/";
const char kChromeUIActivateSafetyCheckSettingsURL[] =
    "thorium://settings/safetyCheck?activateSafetyCheck";
const char kChromeUIAccessibilityHost[] = "accessibility";
const char kChromeUIAllSitesPath[] = "/content/all";
const char kChromeUIAPCInternalsHost[] = "apc-internals";
const char kChromeUIAppIconHost[] = "app-icon";
const char kChromeUIAppIconURL[] = "thorium://app-icon/";
const char kChromeUIAppLauncherPageHost[] = "apps";
const char kChromeUIAppsURL[] = "thorium://apps/";
const char kChromeUIAppsWithDeprecationDialogURL[] =
    "thorium://apps?showDeletionDialog";
const char kChromeUIAppsWithForceInstalledDeprecationDialogURL[] =
    "thorium://apps?showForceInstallDialog=";
const char kChromeUIAutofillInternalsHost[] = "autofill-internals";
const char kChromeUIBluetoothInternalsHost[] = "bluetooth-internals";
const char kChromeUIBookmarksHost[] = "bookmarks";
const char kChromeUIBookmarksURL[] = "thorium://bookmarks/";
const char kChromeUIBrowsingTopicsInternalsHost[] = "topics-internals";
const char kChromeUICertificateViewerHost[] = "view-cert";
const char kChromeUICertificateViewerURL[] = "thorium://view-cert/";
const char kChromeUIChromeSigninHost[] = "chrome-signin";
const char kChromeUIChromeSigninURL[] = "thorium://chrome-signin/";
const char kChromeUIChromeURLsHost[] = "chrome-urls";
const char kChromeUIChromeURLsURL[] = "thorium://chrome-urls/";
const char kChromeUIComponentsHost[] = "components";
const char kChromeUIComponentsUrl[] = "thorium://components";
const char kChromeUIConflictsHost[] = "conflicts";
const char kChromeUIConstrainedHTMLTestURL[] = "thorium://constrained-test/";
const char kChromeUIContentSettingsURL[] = "thorium://settings/content";
const char kChromeUICookieSettingsURL[] = "thorium://settings/cookies";
const char kChromeUICrashHost[] = "crash";
const char kChromeUICrashesHost[] = "crashes";
const char kChromeUICrashesUrl[] = "thorium://crashes";
const char kChromeUICreditsHost[] = "credits";
const char kChromeUICreditsURL[] = "thorium://credits/";
const char kChromeUIDefaultHost[] = "version";
const char kChromeUIDelayedHangUIHost[] = "delayeduithreadhang";
const char kChromeUIDevToolsBlankPath[] = "blank";
const char kChromeUIDevToolsBundledPath[] = "bundled";
const char kChromeUIDevToolsCustomPath[] = "custom";
const char kChromeUIDevToolsHost[] = "devtools";
const char kChromeUIDevToolsRemotePath[] = "remote";
const char kChromeUIDevToolsURL[] =
    "devtools://devtools/bundled/inspector.html";
const char kChromeUIDeviceLogHost[] = "device-log";
const char kChromeUIDeviceLogUrl[] = "thorium://device-log";
const char kChromeUIDevUiLoaderURL[] = "thorium://dev-ui-loader/";
const char kChromeUIDiceWebSigninInterceptHost[] = "signin-dice-web-intercept";
const char kChromeUIDiceWebSigninInterceptURL[] =
    "thorium://signin-dice-web-intercept/";
const char kChromeUIDomainReliabilityInternalsHost[] =
    "domain-reliability-internals";
const char kChromeUIDownloadInternalsHost[] = "download-internals";
const char kChromeUIDownloadsHost[] = "downloads";
const char kChromeUIDownloadsURL[] = "thorium://downloads/";
const char kChromeUIDriveInternalsHost[] = "drive-internals";
const char kChromeUIDriveInternalsUrl[] = "thorium://drive-internals";
const char kChromeUIEDUCoexistenceLoginURLV2[] =
    "thorium://chrome-signin/edu-coexistence";
const char kChromeUIAccessCodeCastHost[] = "access-code-cast";
const char kChromeUIAccessCodeCastURL[] = "thorium://access-code-cast/";
const char kChromeUIExtensionIconHost[] = "extension-icon";
const char kChromeUIExtensionIconURL[] = "thorium://extension-icon/";
const char kChromeUIExtensionsHost[] = "extensions";
const char kChromeUIExtensionsInternalsHost[] = "extensions-internals";
const char kChromeUIExtensionsURL[] = "thorium://extensions/";
#if BUILDFLAG(ENABLE_SUPERVISED_USERS)
const char kChromeUIFamilyLinkUserInternalsHost[] =
    "family-link-user-internals";
#endif  // BUILDFLAG(ENABLE_SUPERVISED_USERS)
const char kChromeUIFaviconHost[] = "favicon";
const char kChromeUIFaviconURL[] = "thorium://favicon/";
const char kChromeUIFavicon2Host[] = "favicon2";
const char kChromeUIFeedbackHost[] = "feedback";
const char kChromeUIFeedbackURL[] = "thorium://feedback/";
const char kChromeUIFileiconURL[] = "thorium://fileicon/";
const char kChromeUIFlagsHost[] = "flags";
const char kChromeUIFlagsURL[] = "thorium://flags/";
const char kChromeUIGCMInternalsHost[] = "gcm-internals";
const char kChromeUIHangUIHost[] = "uithreadhang";
const char kChromeUIHelpHost[] = "help";
const char kChromeUIHelpURL[] = "thorium://help/";
const char kChromeUIHistoryClustersURL[] = "thorium://history/journeys";
const char kChromeUIHistoryHost[] = "history";
const char kChromeUIHistorySyncedTabs[] = "/syncedTabs";
const char kChromeUIHistoryURL[] = "thorium://history/";
const char kChromeUIHumanPresenceInternalsHost[] = "hps-internals";
const char kChromeUIIdentityInternalsHost[] = "identity-internals";
const char kChromeUIImageEditorHost[] = "image-editor";
const char kChromeUIImageEditorURL[] = "thorium://image-editor/";
const char kChromeUIImageHost[] = "image";
const char kChromeUIImageURL[] = "thorium://image/";
const char kChromeUIInspectHost[] = "inspect";
const char kChromeUIInspectURL[] = "thorium://inspect/";
const char kChromeUIInternalsHost[] = "internals";
const char kChromeUIInternalsQueryTilesPath[] = "query-tiles";
const char kChromeUIInterstitialHost[] = "interstitials";
const char kChromeUIInterstitialURL[] = "thorium://interstitials/";
const char kChromeUIInvalidationsHost[] = "invalidations";
const char kChromeUIInvalidationsUrl[] = "thorium://invalidations";
const char kChromeUIKillHost[] = "kill";
const char kChromeUILauncherInternalsHost[] = "launcher-internals";
const char kChromeUILocalStateHost[] = "local-state";
const char kChromeUIManagementHost[] = "management";
const char kChromeUIManagementURL[] = "thorium://management";
const char kChromeUIMediaEngagementHost[] = "media-engagement";
const char kChromeUIMediaHistoryHost[] = "media-history";
const char kChromeUIMediaRouterInternalsHost[] = "media-router-internals";
const char kChromeUIMemoryInternalsHost[] = "memory-internals";
const char kChromeUINTPTilesInternalsHost[] = "ntp-tiles-internals";
const char kChromeUINaClHost[] = "nacl";
const char kChromeUINetExportHost[] = "net-export";
const char kChromeUINetInternalsHost[] = "net-internals";
const char kChromeUINetInternalsURL[] = "thorium://net-internals/";
const char kChromeUINewTabHost[] = "newtab";
const char kChromeUINewTabIconHost[] = "ntpicon";
const char kChromeUINewTabPageHost[] = "new-tab-page";
const char kChromeUINewTabPageURL[] = "thorium://new-tab-page/";
const char kChromeUINewTabPageThirdPartyHost[] = "new-tab-page-third-party";
const char kChromeUINewTabPageThirdPartyURL[] =
    "thorium://new-tab-page-third-party/";
const char kChromeUINewTabURL[] = "thorium://newtab/";
const char kChromeUIProfileInternalsHost[] = "profile-internals";
const char kChromeUIOmniboxHost[] = "omnibox";
const char kChromeUIOmniboxURL[] = "thorium://omnibox/";
#if BUILDFLAG(IS_CHROMEOS)
const char kChromeUIAppDisabledURL[] = "thorium://app-disabled";
const char kChromeUIOsFlagsAppURL[] = "thorium://flags/";
const char kChromeUIOsUrlAppURL[] = "thorium://internal/";
#endif
const char kChromeUIPasswordManagerInternalsHost[] =
    "password-manager-internals";
const char kChromeUIPolicyHost[] = "policy";
const char kChromeUIPolicyURL[] = "thorium://policy/";
const char kChromeUIPredictorsHost[] = "predictors";
const char kChromeUIPrefsInternalsHost[] = "prefs-internals";
const char kChromeUIPrintURL[] = "thorium://print/";
const char kChromeUIPrivacySandboxDialogHost[] = "privacy-sandbox-dialog";
const char kChromeUIPrivacySandboxDialogURL[] =
    "thorium://privacy-sandbox-dialog";
const char kChromeUIQuitHost[] = "quit";
const char kChromeUIQuitURL[] = "thorium://quit/";
const char kChromeUIQuotaInternalsHost[] = "quota-internals";
const char kChromeUIResetPasswordHost[] = "reset-password";
const char kChromeUIResetPasswordURL[] = "thorium://reset-password/";
const char kChromeUIRestartHost[] = "restart";
const char kChromeUIRestartURL[] = "thorium://restart/";
const char kChromeUISafetyPixelbookURL[] = "https://g.co/Pixelbook/legal";
const char kChromeUISafetyPixelSlateURL[] = "https://g.co/PixelSlate/legal";
const char kChromeUISegmentationInternalsHost[] = "segmentation-internals";
#if BUILDFLAG(ENABLE_SESSION_SERVICE)
const char kChromeUISessionServiceInternalsPath[] = "session-service";
#endif
const char kChromeUISettingsHost[] = "settings";
const char kChromeUISettingsURL[] = "thorium://settings/";
const char kChromeUISignInInternalsHost[] = "signin-internals";
const char kChromeUISignInInternalsUrl[] = "thorium://signin-internals";
const char kChromeUISigninEmailConfirmationHost[] = "signin-email-confirmation";
const char kChromeUISigninEmailConfirmationURL[] =
    "thorium://signin-email-confirmation";
const char kChromeUISigninErrorHost[] = "signin-error";
const char kChromeUISigninErrorURL[] = "thorium://signin-error/";
const char kChromeUISigninReauthHost[] = "signin-reauth";
const char kChromeUISigninReauthURL[] = "thorium://signin-reauth/";
const char kChromeUISiteDataDeprecatedPath[] = "/siteData";
const char kChromeUISiteDetailsPrefixURL[] =
    "thorium://settings/content/siteDetails?site=";
const char kChromeUISiteEngagementHost[] = "site-engagement";
const char kChromeUISupervisedUserPassphrasePageHost[] =
    "managed-user-passphrase";
const char kChromeUISupportToolHost[] = "support-tool";
const char kChromeUISyncConfirmationHost[] = "sync-confirmation";
const char kChromeUISyncConfirmationLoadingPath[] = "loading";
const char kChromeUISyncConfirmationURL[] = "thorium://sync-confirmation/";
const char kChromeUISyncFileSystemInternalsHost[] = "syncfs-internals";
const char kChromeUISyncHost[] = "sync";
const char kChromeUISyncInternalsHost[] = "sync-internals";
const char kChromeUISyncInternalsUrl[] = "thorium://sync-internals";
const char kChromeUISystemInfoHost[] = "system";
const char kChromeUITermsHost[] = "terms";
const char kChromeUITermsURL[] = "thorium://terms/";
const char kChromeUIThemeHost[] = "theme";
const char kChromeUIThemeURL[] = "thorium://theme/";
const char kChromeUITranslateInternalsHost[] = "translate-internals";
const char kChromeUITopChromeDomain[] = "top-chrome";
const char kChromeUIUntrustedImageEditorURL[] =
    "chrome-untrusted://image-editor/";
const char kChromeUIUntrustedPrintURL[] = "chrome-untrusted://print/";
const char kChromeUIUntrustedThemeURL[] = "chrome-untrusted://theme/";
const char kChromeUIUsbInternalsHost[] = "usb-internals";
const char kChromeUIUserActionsHost[] = "user-actions";
const char kChromeUIVersionHost[] = "version";
const char kChromeUIVersionURL[] = "thorium://version/";
const char kChromeUIWelcomeHost[] = "welcome";
const char kChromeUIWelcomeURL[] = "thorium://welcome/";
const char kChromeUIWhatsNewHost[] = "whats-new";
const char kChromeUIWhatsNewURL[] = "thorium://whats-new/";
const char kChromeUIWebuiGalleryHost[] = "webui-gallery";

#if BUILDFLAG(IS_WIN)
// TODO(crbug.com/1003960): Remove when issue is resolved.
const char kChromeUIWelcomeWin10Host[] = "welcome-win10";
#endif  // BUILDFLAG(IS_WIN)

#if BUILDFLAG(IS_ANDROID)
const char kChromeUIExploreSitesInternalsHost[] = "explore-sites-internals";
const char kChromeUIJavaCrashURL[] = "thorium://java-crash/";
const char kChromeUINativeBookmarksURL[] = "chrome-native://bookmarks/";
const char kChromeUINativeExploreURL[] = "chrome-native://explore";
const char kChromeUINativeHistoryURL[] = "chrome-native://history/";
const char kChromeUINativeNewTabURL[] = "chrome-native://newtab/";
const char kChromeUIOfflineInternalsHost[] = "offline-internals";
const char kChromeUISnippetsInternalsHost[] = "snippets-internals";
const char kChromeUIUntrustedVideoTutorialsHost[] = "video-tutorials";
const char kChromeUIUntrustedVideoPlayerUrl[] =
    "chrome-untrusted://video-tutorials/";
const char kChromeUIWebApksHost[] = "webapks";
#else
const char kChromeUIAppServiceInternalsHost[] = "app-service-internals";
const char kChromeUINearbyInternalsHost[] = "nearby-internals";
const char kChromeUIBookmarksSidePanelHost[] =
    "bookmarks-side-panel.top-chrome";
const char kChromeUIBookmarksSidePanelURL[] =
    "thorium://bookmarks-side-panel.top-chrome/";
const char kChromeUIHistoryClustersSidePanelHost[] =
    "history-clusters-side-panel.top-chrome";
const char kChromeUIHistoryClustersSidePanelURL[] =
    "thorium://history-clusters-side-panel.top-chrome/";
const char kChromeUIReadAnythingSidePanelHost[] =
    "read-anything-side-panel.top-chrome";
const char kChromeUIReadAnythingSidePanelURL[] =
    "thorium://read-anything-side-panel.top-chrome/";
const char kChromeUIReadLaterHost[] = "read-later.top-chrome";
const char kChromeUIReadLaterURL[] = "thorium://read-later.top-chrome/";
const char kChromeUIUntrustedFeedURL[] = "chrome-untrusted://feed/";
const char kChromeUIWebAppInternalsHost[] = "web-app-internals";
const char kChromeUIWebUITestHost[] = "webui-test";
#endif

#if BUILDFLAG(PLATFORM_CFM)
const char kCfmNetworkSettingsHost[] = "cfm-network-settings";
const char kCfmNetworkSettingsURL[] = "thorium://cfm-network-settings";
#endif  // BUILDFLAG(PLATFORM_CFM)

#if BUILDFLAG(IS_CHROMEOS)
const char kChromeUIGpuURL[] = "thorium://gpu";
const char kChromeUIHistogramsURL[] = "thorium://histograms";
#endif

#if BUILDFLAG(IS_CHROMEOS_ASH)
// Keep alphabetized.
const char kChromeUIAccountManagerErrorHost[] = "account-manager-error";
const char kChromeUIAccountManagerErrorURL[] = "thorium://account-manager-error";
const char kChromeUIAccountMigrationWelcomeHost[] = "account-migration-welcome";
const char kChromeUIAccountMigrationWelcomeURL[] =
    "thorium://account-migration-welcome";
const char kChromeUIActivationMessageHost[] = "activationmessage";
const char kChromeUIAddSupervisionHost[] = "add-supervision";
const char kChromeUIAddSupervisionURL[] = "thorium://add-supervision/";
const char kChromeUIArcGraphicsTracingHost[] = "arc-graphics-tracing";
const char kChromeUIArcGraphicsTracingURL[] = "thorium://arc-graphics-tracing/";
const char kChromeUIArcOverviewTracingHost[] = "arc-overview-tracing";
const char kChromeUIArcOverviewTracingURL[] = "thorium://arc-overview-tracing/";
const char kChromeUIArcPowerControlHost[] = "arc-power-control";
const char kChromeUIArcPowerControlURL[] = "thorium://arc-power-control/";
const char kChromeUIAssistantOptInHost[] = "assistant-optin";
const char kChromeUIAssistantOptInURL[] = "thorium://assistant-optin/";
const char kChromeUIAudioHost[] = "audio";
const char kChromeUIAudioURL[] = "thorium://audio/";
const char kChromeUIBluetoothPairingHost[] = "bluetooth-pairing";
const char kChromeUIBluetoothPairingURL[] = "thorium://bluetooth-pairing/";
const char kChromeUICertificateManagerDialogURL[] =
    "thorium://certificate-manager/";
const char kChromeUICertificateManagerHost[] = "certificate-manager";
const char kChromeUIConfirmPasswordChangeHost[] = "confirm-password-change";
const char kChromeUIConfirmPasswordChangeUrl[] =
    "thorium://confirm-password-change";
const char kChromeUICrostiniInstallerHost[] = "crostini-installer";
const char kChromeUICrostiniInstallerUrl[] = "thorium://crostini-installer";
const char kChromeUICrostiniUpgraderHost[] = "crostini-upgrader";
const char kChromeUICrostiniUpgraderUrl[] = "thorium://crostini-upgrader";
const char kChromeUICryptohomeHost[] = "cryptohome";
const char kChromeUICryptohomeURL[] = "thorium://cryptohome";
const char kChromeUIDeviceEmulatorHost[] = "device-emulator";
const char kChromeUIDiagnosticsAppURL[] = "thorium://diagnostics";
const char kChromeUIFirmwareUpdatesAppURL[] = "thorium://accessory-update";
const char kChromeUIIntenetConfigDialogURL[] =
    "thorium://internet-config-dialog/";
const char kChromeUIIntenetDetailDialogURL[] =
    "thorium://internet-detail-dialog/";
const char kChromeUIInternetConfigDialogHost[] = "internet-config-dialog";
const char kChromeUIInternetDetailDialogHost[] = "internet-detail-dialog";
const char kChromeUIBorealisCreditsHost[] = "borealis-credits";
const char kChromeUIBorealisCreditsURL[] = "thorium://borealis-credits/";
const char kChromeUICrostiniCreditsHost[] = "crostini-credits";
const char kChromeUICrostiniCreditsURL[] = "thorium://crostini-credits/";
const char kChromeUILockScreenNetworkHost[] = "lock-network";
const char kChromeUILockScreenNetworkURL[] = "thorium://lock-network";
const char kChromeUILockScreenStartReauthHost[] = "lock-reauth";
const char kChromeUILockScreenStartReauthURL[] = "thorium://lock-reauth";
const char kChromeUIMobileSetupHost[] = "mobilesetup";
const char kChromeUIMobileSetupURL[] = "thorium://mobilesetup/";
const char kChromeUIMultiDeviceInternalsHost[] = "multidevice-internals";
const char kChromeUIMultiDeviceSetupHost[] = "multidevice-setup";
const char kChromeUIMultiDeviceSetupUrl[] = "thorium://multidevice-setup";
const char kChromeUINetworkHost[] = "network";
const char kChromeUINetworkUrl[] = "thorium://network";
const char kChromeUINotificationTesterHost[] = "notification-tester";
const char kChromeUINotificationTesterURL[] = "thorium://notification-tester";
const char kChromeUIOSCreditsHost[] = "os-credits";
const char kChromeUIOSCreditsURL[] = "thorium://os-credits/";
const char kChromeUIOobeHost[] = "oobe";
const char kChromeUIOobeURL[] = "thorium://oobe/";
const char kChromeUIParentAccessHost[] = "parent-access";
const char kChromeUIParentAccessURL[] = "thorium://parent-access/";
const char kChromeUIPasswordChangeHost[] = "password-change";
const char kChromeUIPasswordChangeUrl[] = "thorium://password-change";
const char kChromeUIPrintManagementUrl[] = "thorium://print-management";
const char kChromeUIPowerHost[] = "power";
const char kChromeUIPowerUrl[] = "thorium://power";
const char kChromeUIScanningAppURL[] = "thorium://scanning";
const char kChromeUIScreenlockIconHost[] = "screenlock-icon";
const char kChromeUIScreenlockIconURL[] = "thorium://screenlock-icon/";
const char kChromeUISetTimeHost[] = "set-time";
const char kChromeUISetTimeURL[] = "thorium://set-time/";
const char kChromeUISlowHost[] = "slow";
const char kChromeUISlowTraceHost[] = "slow_trace";
const char kChromeUISlowURL[] = "thorium://slow/";
const char kChromeUISmbShareHost[] = "smb-share-dialog";
const char kChromeUISmbShareURL[] = "thorium://smb-share-dialog/";
const char kChromeUISmbCredentialsHost[] = "smb-credentials-dialog";
const char kChromeUISmbCredentialsURL[] = "thorium://smb-credentials-dialog/";
const char kChromeUISysInternalsHost[] = "sys-internals";
const char kChromeUISysInternalsUrl[] = "thorium://sys-internals";
const char kChromeUIUntrustedCroshHost[] = "crosh";
const char kChromeUIUntrustedCroshURL[] = "chrome-untrusted://crosh/";
const char kChromeUIUntrustedTerminalHost[] = "terminal";
const char kChromeUIUntrustedTerminalURL[] = "chrome-untrusted://terminal/";
const char kChromeUIUserImageHost[] = "userimage";
const char kChromeUIUserImageURL[] = "thorium://userimage/";
const char kChromeUIVmHost[] = "vm";
const char kChromeUIVmUrl[] = "thorium://vm";
const char kChromeUIEmojiPickerURL[] = "thorium://emoji-picker/";
const char kChromeUIEmojiPickerHost[] = "emoji-picker";

const char kChromeUIUrgentPasswordExpiryNotificationHost[] =
    "urgent-password-expiry-notification";
const char kChromeUIUrgentPasswordExpiryNotificationUrl[] =
    "thorium://urgent-password-expiry-notification/";

const char kOsUIAccountManagerErrorURL[] = "os://account-manager-error";
const char kOsUIAccountMigrationWelcomeURL[] = "os://account-migration-welcome";
const char kOsUIAddSupervisionURL[] = "os://add-supervision";
const char kOsUIAppDisabledURL[] = "os://app-disabled";
const char kOsUICrashesURL[] = "os://crashes";
const char kOsUICreditsURL[] = "os://credits";
const char kOsUIDeviceLogURL[] = "os://device-log";
const char kOsUIDriveInternalsURL[] = "os://drive-internals";
const char kOsUIEmojiPickerURL[] = "os://emoji-picker";
const char kOsUIGpuURL[] = "os://gpu";
const char kOsUIHistogramsURL[] = "os://histograms";
const char kOsUIInvalidationsURL[] = "os://invalidations";
const char kOsUILockScreenNetworkURL[] = "os://lock-network";
const char kOsUINetworkURL[] = "os://network";
const char kOsUIRestartURL[] = "os://restart";
const char kOsUIScanningAppURL[] = "os://scanning";
const char kOsUISettingsURL[] = "os://settings";
const char kOsUISignInInternalsURL[] = "os://signin-internals";
const char kOsUISyncInternalsURL[] = "os://sync-internals";
const char kOsUISysInternalsUrl[] = "os://sys-internals";
const char kOsUISystemURL[] = "os://system";
const char kOsUITermsURL[] = "os://terms";

// Keep alphabetized.

bool IsSystemWebUIHost(base::StringPiece host) {
  // Compares host instead of full URL for performance (the strings are
  // shorter).
  static const char* const kHosts[] = {
    kChromeUIAccountManagerErrorHost,
    kChromeUIAccountMigrationWelcomeHost,
    kChromeUIActivationMessageHost,
    kChromeUIAddSupervisionHost,
    kChromeUIAssistantOptInHost,
    kChromeUIBluetoothPairingHost,
    kChromeUIBorealisCreditsHost,
    kChromeUICertificateManagerHost,
    kChromeUICrostiniCreditsHost,
    kChromeUICrostiniInstallerHost,
    kChromeUICryptohomeHost,
    kChromeUIDeviceEmulatorHost,
    kChromeUIInternetConfigDialogHost,
    kChromeUIInternetDetailDialogHost,
    kChromeUILockScreenNetworkHost,
    kChromeUILockScreenStartReauthHost,
    kChromeUIMobileSetupHost,
    kChromeUIMultiDeviceSetupHost,
    kChromeUINetworkHost,
    kChromeUINotificationTesterHost,
    kChromeUIOobeHost,
    kChromeUIOSCreditsHost,
    kChromeUIOSSettingsHost,
    kChromeUIPasswordChangeHost,
    kChromeUIPowerHost,
    kChromeUISetTimeHost,
    kChromeUISmbCredentialsHost,
    kChromeUISmbShareHost,
    kChromeUIEmojiPickerHost,
#if BUILDFLAG(PLATFORM_CFM)
    kCfmNetworkSettingsHost,
#endif  // BUILDFLAG(PLATFORM_CFM)
  };
  for (const char* h : kHosts) {
    if (host == h)
      return true;
  }
  return false;
}
#endif  // BUILDFLAG(IS_CHROMEOS_ASH)

#if BUILDFLAG(IS_CHROMEOS)
const char kChromeUIAppDisabledHost[] = "app-disabled";
const char kChromeUIOSSettingsHost[] = "os-settings";
const char kChromeUIOSSettingsURL[] = "thorium://os-settings/";
const char kOsUIAboutURL[] = "os://about";
const char kOsUIComponentsURL[] = "os://components";
const char kOsUIFlagsURL[] = "os://flags";
const char kOsUIHelpAppURL[] = "os://help-app";
const char kOsUIVersionURL[] = "os://version";
#endif

#if BUILDFLAG(IS_LINUX) || BUILDFLAG(IS_CHROMEOS)
const char kChromeUIWebUIJsErrorHost[] = "webuijserror";
const char kChromeUIWebUIJsErrorURL[] = "thorium://webuijserror/";
#endif

#if BUILDFLAG(IS_WIN) || BUILDFLAG(IS_MAC) || BUILDFLAG(IS_LINUX) || \
    BUILDFLAG(IS_CHROMEOS_ASH)
const char kChromeUIConnectorsInternalsHost[] = "connectors-internals";
#endif

#if BUILDFLAG(IS_WIN) || BUILDFLAG(IS_MAC) || BUILDFLAG(IS_LINUX) || \
    BUILDFLAG(IS_CHROMEOS)
const char kChromeUIDiscardsHost[] = "discards";
const char kChromeUIDiscardsURL[] = "thorium://discards/";
#endif

#if !BUILDFLAG(IS_ANDROID)
const char kChromeUINearbyShareHost[] = "nearby";
const char kChromeUINearbyShareURL[] = "thorium://nearby/";
#endif  // !BUILDFLAG(IS_ANDROID)

#if BUILDFLAG(IS_POSIX) && !BUILDFLAG(IS_MAC) && !BUILDFLAG(IS_ANDROID)
const char kChromeUILinuxProxyConfigHost[] = "linux-proxy-config";
#endif

#if BUILDFLAG(IS_WIN) || BUILDFLAG(IS_LINUX) || BUILDFLAG(IS_CHROMEOS) || \
    BUILDFLAG(IS_ANDROID)
const char kChromeUISandboxHost[] = "sandbox";
#endif

// TODO(crbug.com/1052397): Revisit the macro expression once build flag switch
// of lacros-chrome is complete.
#if BUILDFLAG(IS_WIN) || BUILDFLAG(IS_MAC) || BUILDFLAG(IS_FUCHSIA) || \
    (BUILDFLAG(IS_LINUX) || BUILDFLAG(IS_CHROMEOS_LACROS))
const char kChromeUIBrowserSwitchHost[] = "browser-switch";
const char kChromeUIBrowserSwitchURL[] = "thorium://browser-switch/";
const char kChromeUIEnterpriseProfileWelcomeHost[] =
    "enterprise-profile-welcome";
const char kChromeUIEnterpriseProfileWelcomeURL[] =
    "thorium://enterprise-profile-welcome/";
const char kChromeUIProfileCustomizationHost[] = "profile-customization";
const char kChromeUIProfileCustomizationURL[] =
    "thorium://profile-customization";
const char kChromeUIProfilePickerHost[] = "profile-picker";
const char kChromeUIProfilePickerUrl[] = "thorium://profile-picker/";
const char kChromeUIProfilePickerStartupQuery[] = "startup";
#endif

#if ((BUILDFLAG(IS_LINUX) || BUILDFLAG(IS_CHROMEOS)) && \
     defined(TOOLKIT_VIEWS)) ||                         \
    defined(USE_AURA)
const char kChromeUITabModalConfirmDialogHost[] = "tab-modal-confirm-dialog";
#endif

#if BUILDFLAG(ENABLE_PRINT_PREVIEW)
const char kChromeUIPrintHost[] = "print";
#endif

#if BUILDFLAG(ENABLE_WEBUI_TAB_STRIP)
const char kChromeUITabStripHost[] = "tab-strip.top-chrome";
const char kChromeUITabStripURL[] = "thorium://tab-strip.top-chrome";
#endif

#if !BUILDFLAG(IS_ANDROID)
const char kChromeUICommanderHost[] = "commander";
const char kChromeUICommanderURL[] = "thorium://commander";
const char kChromeUIDownloadShelfHost[] = "download-shelf.top-chrome";
const char kChromeUIDownloadShelfURL[] = "thorium://download-shelf.top-chrome/";
const char kChromeUITabSearchHost[] = "tab-search.top-chrome";
const char kChromeUITabSearchURL[] = "thorium://tab-search.top-chrome/";
#endif

const char kChromeUIWebRtcLogsHost[] = "webrtc-logs";

// Settings sub pages.

// NOTE: Add sub page paths to |kChromeSettingsSubPages| in
// chrome_autocomplete_provider_client.cc to be listed by the built-in
// AutocompleteProvider.

const char kAccessibilitySubPage[] = "accessibility";
const char kAddressesSubPage[] = "addresses";
const char kAppearanceSubPage[] = "appearance";
const char kAutofillSubPage[] = "autofill";
const char kClearBrowserDataSubPage[] = "clearBrowserData";
const char kContentSettingsSubPage[] = "content";
const char kCookieSettingsSubPage[] = "cookies";
const char kDownloadsSubPage[] = "downloads";
const char kHandlerSettingsSubPage[] = "handlers";
const char kImportDataSubPage[] = "importData";
const char kLanguagesSubPage[] = "languages/details";
const char kLanguageOptionsSubPage[] = "languages";
const char kOnStartupSubPage[] = "onStartup";
const char kPasswordCheckSubPage[] = "passwords/check?start=true";
const char kPasswordManagerSubPage[] = "passwords";
const char kPaymentsSubPage[] = "payments";
const char kPrintingSettingsSubPage[] = "printing";
const char kPrivacyGuideSubPage[] = "privacy/guide";
const char kPrivacySubPage[] = "privacy";
const char kResetSubPage[] = "reset";
const char kResetProfileSettingsSubPage[] = "resetProfileSettings";
const char kSafeBrowsingEnhancedProtectionSubPage[] = "security?q=enhanced";
const char kSafetyCheckSubPage[] = "safetyCheck";
const char kSearchSubPage[] = "search";
const char kSearchEnginesSubPage[] = "searchEngines";
const char kSignOutSubPage[] = "signOut";
const char kSyncSetupSubPage[] = "syncSetup";
const char kTriggeredResetProfileSettingsSubPage[] =
    "triggeredResetProfileSettings";
const char kCreateProfileSubPage[] = "createProfile";
const char kManageProfileSubPage[] = "manageProfile";
const char kPeopleSubPage[] = "people";
const char kPrivacySandboxAdPersonalizationSubPage[] =
    "privacySandbox?view=adPersonalizationDialog";
const char kPrivacySandboxLearnMoreSubPage[] =
    "privacySandbox?view=learnMoreDialog";
const char kPrivacySandboxSubPage[] = "privacySandbox";

#if !BUILDFLAG(IS_ANDROID)
const char kPrivacySandboxSubPagePath[] = "/privacySandbox";
#endif

#if BUILDFLAG(IS_WIN) || BUILDFLAG(IS_MAC) || BUILDFLAG(IS_LINUX) || \
    BUILDFLAG(IS_FUCHSIA)
const char kChromeUIWebAppSettingsURL[] = "thorium://app-settings/";
const char kChromeUIWebAppSettingsHost[] = "app-settings";
#endif

#if BUILDFLAG(IS_WIN)
const char kCleanupSubPage[] = "cleanup";
#endif  // BUILDFLAG(IS_WIN)

#if !BUILDFLAG(IS_ANDROID) && BUILDFLAG(GOOGLE_CHROME_BRANDING)
const char kChromeUICastFeedbackHost[] = "cast-feedback";
#endif

// Extension sub pages.
const char kExtensionConfigureCommandsSubPage[] = "configureCommands";

// Add hosts here to be included in chrome://chrome-urls (about:about).
// These hosts will also be suggested by BuiltinProvider.
const char* const kChromeHostURLs[] = {
    kChromeUIAboutHost,
    kChromeUIAccessibilityHost,
#if !BUILDFLAG(IS_ANDROID)
    kChromeUIAppServiceInternalsHost,
#endif
    kChromeUIAutofillInternalsHost,
    kChromeUIBluetoothInternalsHost,
    kChromeUIBrowsingTopicsInternalsHost,
    kChromeUIChromeURLsHost,
    kChromeUIComponentsHost,
    kChromeUICrashesHost,
    kChromeUICreditsHost,
#if BUILDFLAG(IS_CHROMEOS_ASH) && !defined(OFFICIAL_BUILD)
    kChromeUIDeviceEmulatorHost,
#endif
    kChromeUIDeviceLogHost,
    kChromeUIDownloadInternalsHost,
#if BUILDFLAG(ENABLE_SUPERVISED_USERS)
    kChromeUIFamilyLinkUserInternalsHost,
#endif  // BUILDFLAG(ENABLE_SUPERVISED_USERS)
    kChromeUIFlagsHost,
    kChromeUIGCMInternalsHost,
    kChromeUIHistoryHost,
    history_clusters_internals::kChromeUIHistoryClustersInternalsHost,
#if BUILDFLAG(IS_CHROMEOS_ASH)
    kChromeUIHumanPresenceInternalsHost,
#endif
    kChromeUIInterstitialHost,
    kChromeUIInvalidationsHost,
    kChromeUILocalStateHost,
#if !BUILDFLAG(IS_ANDROID)
    kChromeUIManagementHost,
#endif
    kChromeUIMediaEngagementHost,
    kChromeUINetExportHost,
    kChromeUINetInternalsHost,
    kChromeUINewTabHost,
    kChromeUIOmniboxHost,
    optimization_guide_internals::kChromeUIOptimizationGuideInternalsHost,
    kChromeUIPasswordManagerInternalsHost,
    kChromeUIPolicyHost,
    kChromeUIPredictorsHost,
    kChromeUIPrefsInternalsHost,
    kChromeUIProfileInternalsHost,
    kChromeUIQuotaInternalsHost,
    kChromeUISignInInternalsHost,
    kChromeUISiteEngagementHost,
    kChromeUINTPTilesInternalsHost,
    safe_browsing::kChromeUISafeBrowsingHost,
    kChromeUISyncInternalsHost,
#if !BUILDFLAG(IS_ANDROID)
    kChromeUITermsHost,
#endif
    kChromeUITranslateInternalsHost,
    kChromeUIUsbInternalsHost,
    kChromeUIUserActionsHost,
    kChromeUIVersionHost,
#if !BUILDFLAG(IS_ANDROID)
    kChromeUIWebAppInternalsHost,
#endif
    content::kChromeUIAttributionInternalsHost,
    content::kChromeUIBlobInternalsHost,
    content::kChromeUIDinoHost,
    content::kChromeUIGpuHost,
    content::kChromeUIHistogramHost,
    content::kChromeUIIndexedDBInternalsHost,
    content::kChromeUIMediaInternalsHost,
    content::kChromeUINetworkErrorsListingHost,
    content::kChromeUIProcessInternalsHost,
    content::kChromeUIServiceWorkerInternalsHost,
#if !BUILDFLAG(IS_ANDROID)
    content::kChromeUITracingHost,
#endif
    content::kChromeUIUkmHost,
    content::kChromeUIWebRTCInternalsHost,
#if !BUILDFLAG(IS_ANDROID)
#if !BUILDFLAG(IS_CHROMEOS_ASH)
    kChromeUIAppLauncherPageHost,
#endif
    kChromeUIBookmarksHost,
    kChromeUIDownloadsHost,
    kChromeUIHelpHost,
    kChromeUIInspectHost,
    kChromeUINewTabPageHost,
    kChromeUINewTabPageThirdPartyHost,
    kChromeUISettingsHost,
    kChromeUISystemInfoHost,
    kChromeUIWhatsNewHost,
#endif
#if BUILDFLAG(IS_ANDROID)
    kChromeUIExploreSitesInternalsHost,
    kChromeUIOfflineInternalsHost,
    kChromeUISnippetsInternalsHost,
    kChromeUIWebApksHost,
#endif
#if BUILDFLAG(IS_CHROMEOS_ASH)
    kChromeUIBorealisCreditsHost,
    kChromeUICertificateManagerHost,
    kChromeUICrostiniCreditsHost,
    kChromeUICryptohomeHost,
    kChromeUIDriveInternalsHost,
    kChromeUINetworkHost,
    kChromeUILockScreenNetworkHost,
    kChromeUIOobeHost,
    kChromeUIOSCreditsHost,
    kChromeUIOSSettingsHost,
    kChromeUIPowerHost,
    kChromeUISysInternalsHost,
    kChromeUIInternetConfigDialogHost,
    kChromeUIInternetDetailDialogHost,
    kChromeUIAssistantOptInHost,
#endif
#if BUILDFLAG(IS_WIN) || BUILDFLAG(IS_MAC) || BUILDFLAG(IS_LINUX) || \
    BUILDFLAG(IS_CHROMEOS_ASH)
    kChromeUIConnectorsInternalsHost,
#endif
#if BUILDFLAG(IS_WIN) || BUILDFLAG(IS_MAC) || BUILDFLAG(IS_LINUX) || \
    BUILDFLAG(IS_CHROMEOS)
    kChromeUIDiscardsHost,
#endif
#if BUILDFLAG(IS_WIN) || BUILDFLAG(IS_MAC) || BUILDFLAG(IS_LINUX)
    kChromeUIWebAppSettingsHost,
#endif
#if BUILDFLAG(IS_POSIX) && !BUILDFLAG(IS_MAC) && !BUILDFLAG(IS_ANDROID)
    kChromeUILinuxProxyConfigHost,
#endif
#if BUILDFLAG(IS_WIN) || BUILDFLAG(IS_LINUX) || BUILDFLAG(IS_CHROMEOS) || \
    BUILDFLAG(IS_ANDROID)
    kChromeUISandboxHost,
#endif
#if BUILDFLAG(IS_WIN)
    kChromeUIConflictsHost,
#endif
#if BUILDFLAG(ENABLE_NACL)
    kChromeUINaClHost,
#endif
#if BUILDFLAG(ENABLE_EXTENSIONS)
    kChromeUIExtensionsHost,
    kChromeUIExtensionsInternalsHost,
#endif
#if BUILDFLAG(ENABLE_PRINT_PREVIEW)
    kChromeUIPrintHost,
#endif
    kChromeUIWebRtcLogsHost,
#if BUILDFLAG(PLATFORM_CFM)
    kCfmNetworkSettingsHost,
#endif  // BUILDFLAG(PLATFORM_CFM)
};
const size_t kNumberOfChromeHostURLs = std::size(kChromeHostURLs);

// Add chrome://internals/* subpages here to be included in chrome://chrome-urls
// (about:about).
const char* const kChromeInternalsPathURLs[] = {
#if BUILDFLAG(IS_ANDROID)
    kChromeUIInternalsQueryTilesPath,
#endif  // BUILDFLAG(IS_ANDROID)
#if BUILDFLAG(ENABLE_SESSION_SERVICE)
    kChromeUISessionServiceInternalsPath,
#endif
};
const size_t kNumberOfChromeInternalsPathURLs =
    std::size(kChromeInternalsPathURLs);

const char* const kChromeDebugURLs[] = {
    blink::kChromeUIBadCastCrashURL,
    blink::kChromeUIBrowserCrashURL,
    blink::kChromeUIBrowserDcheckURL,
    blink::kChromeUICrashURL,
    blink::kChromeUIDumpURL,
    blink::kChromeUIKillURL,
    blink::kChromeUIHangURL,
    blink::kChromeUIShorthangURL,
    blink::kChromeUIGpuCleanURL,
    blink::kChromeUIGpuCrashURL,
    blink::kChromeUIGpuHangURL,
    blink::kChromeUIMemoryExhaustURL,
    blink::kChromeUIMemoryPressureCriticalURL,
    blink::kChromeUIMemoryPressureModerateURL,
#if BUILDFLAG(IS_WIN)
    blink::kChromeUIBrowserHeapCorruptionURL,
    blink::kChromeUIHeapCorruptionCrashURL,
#endif
#if BUILDFLAG(IS_ANDROID)
    blink::kChromeUIGpuJavaCrashURL,
    kChromeUIJavaCrashURL,
#endif
#if BUILDFLAG(IS_LINUX) || BUILDFLAG(IS_CHROMEOS)
    kChromeUIWebUIJsErrorURL,
#endif
    kChromeUIQuitURL,
    kChromeUIRestartURL};
const size_t kNumberOfChromeDebugURLs = std::size(kChromeDebugURLs);

}  // namespace chrome
