// Copyright 2024 The Chromium Authors and Alex313031
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Instructions for adding new entries to this file:
// https://chromium.googlesource.com/chromium/src/+/main/docs/how_to_add_your_feature_flag.md#step-2_adding-the-feature-flag-to-the-chrome_flags-ui

#include "chrome/browser/about_flags.h"

#include <iterator>
#include <map>
#include <memory>
#include <set>
#include <utility>

#include "base/base_switches.h"
#include "base/command_line.h"
#include "base/feature_list.h"
#include "base/functional/bind.h"
#include "base/functional/callback.h"
#include "base/i18n/base_i18n_switches.h"
#include "base/memory/singleton.h"
#include "base/metrics/histogram_functions.h"
#include "base/no_destructor.h"
#include "base/strings/string_number_conversions.h"
#include "base/strings/utf_string_conversions.h"
#include "base/task/task_features.h"
#include "base/values.h"
#include "build/branding_buildflags.h"
#include "build/build_config.h"
#include "cc/base/features.h"
#include "cc/base/switches.h"
#include "chrome/browser/apps/app_discovery_service/app_discovery_service.h"
#include "chrome/browser/apps/link_capturing/link_capturing_features.h"
#include "chrome/browser/browser_features.h"
#include "chrome/browser/browser_process.h"
#include "chrome/browser/companion/core/features.h"
#include "chrome/browser/feature_guide/notifications/feature_notification_guide_service.h"
#include "chrome/browser/file_system_access/file_system_access_features.h"
#include "chrome/browser/flag_descriptions.h"
#include "chrome/browser/ip_protection/ip_protection_switches.h"
#include "chrome/browser/login_detection/login_detection_util.h"
#include "chrome/browser/media/router/discovery/access_code/access_code_cast_constants.h"
#include "chrome/browser/media/router/discovery/access_code/access_code_cast_feature.h"
#include "chrome/browser/navigation_predictor/navigation_predictor_features.h"
#include "chrome/browser/navigation_predictor/search_engine_preconnector.h"
#include "chrome/browser/net/stub_resolver_config_reader.h"
#include "chrome/browser/net/system_network_context_manager.h"
#include "chrome/browser/notifications/scheduler/public/features.h"
#include "chrome/browser/page_info/page_info_features.h"
#include "chrome/browser/permissions/notifications_permission_revocation_config.h"
#include "chrome/browser/permissions/quiet_notification_permission_ui_config.h"
#include "chrome/browser/predictors/loading_predictor_config.h"
#include "chrome/browser/preloading/prefetch/search_prefetch/field_trial_settings.h"
#include "chrome/browser/preloading/preloading_features.h"
#include "chrome/browser/resource_coordinator/tab_manager_features.h"
#include "chrome/browser/share/share_features.h"
#include "chrome/browser/sharing/features.h"
#include "chrome/browser/sharing_hub/sharing_hub_features.h"
#include "chrome/browser/signin/signin_features.h"
#include "chrome/browser/site_isolation/about_flags.h"
#include "chrome/browser/tpcd/experiment/tpcd_experiment_features.h"
#include "chrome/browser/ui/browser_navigator_params.h"
#include "chrome/browser/ui/ui_features.h"
#include "chrome/browser/unexpire_flags.h"
#include "chrome/browser/unexpire_flags_gen.h"
#include "chrome/browser/webauthn/webauthn_switches.h"
#include "chrome/common/buildflags.h"
#include "chrome/common/channel_info.h"
#include "chrome/common/chrome_content_client.h"
#include "chrome/common/chrome_features.h"
#include "chrome/common/chrome_switches.h"
#include "chrome/common/companion/visual_query/features.h"
#include "components/assist_ranker/predictor_config_definitions.h"
#include "components/autofill/core/browser/autofill_experiments.h"
#include "components/autofill/core/common/autofill_features.h"
#include "components/autofill/core/common/autofill_payments_features.h"
#include "components/autofill/core/common/autofill_switches.h"
#include "components/autofill/core/common/autofill_util.h"
#include "components/browser_sync/browser_sync_switches.h"
#include "components/browsing_data/core/features.h"
#include "components/commerce/core/commerce_feature_list.h"
#include "components/commerce/core/flag_descriptions.h"
#include "components/component_updater/component_updater_command_line_config_policy.h"
#include "components/component_updater/component_updater_switches.h"
#include "components/compose/buildflags.h"
#include "components/compose/core/browser/compose_features.h"
#include "components/content_settings/core/common/features.h"
#include "components/contextual_search/core/browser/contextual_search_field_trial.h"
#include "components/contextual_search/core/browser/public.h"
#include "components/dom_distiller/core/dom_distiller_features.h"
#include "components/dom_distiller/core/dom_distiller_switches.h"
#include "components/download/public/common/download_features.h"
#include "components/error_page/common/error_page_switches.h"
#include "components/feature_engagement/public/feature_constants.h"
#include "components/feature_engagement/public/feature_list.h"
#include "components/feed/feed_feature_list.h"
#include "components/flags_ui/feature_entry.h"
#include "components/flags_ui/feature_entry_macros.h"
#include "components/flags_ui/flags_state.h"
#include "components/flags_ui/flags_storage.h"
#include "components/flags_ui/flags_ui_metrics.h"
#include "components/flags_ui/flags_ui_switches.h"
#include "components/flags_ui/pref_service_flags_storage.h"
#include "components/heavy_ad_intervention/heavy_ad_features.h"
#include "components/history/core/browser/features.h"
#include "components/history_clusters/core/config.h"
#include "components/history_clusters/core/features.h"
#include "components/history_clusters/core/on_device_clustering_features.h"
#include "components/invalidation/impl/invalidation_switches.h"
#include "components/language/core/common/language_experiments.h"
#include "components/lens/buildflags.h"
#include "components/lens/lens_features.h"
#include "components/manta/features.h"
#include "components/mirroring/service/mirroring_features.h"
#include "components/nacl/common/buildflags.h"
#include "components/nacl/common/nacl_switches.h"
#include "components/network_session_configurator/common/network_features.h"
#include "components/network_session_configurator/common/network_switches.h"
#include "components/no_state_prefetch/browser/no_state_prefetch_field_trial.h"
#include "components/ntp_tiles/features.h"
#include "components/offline_pages/core/offline_page_feature.h"
#include "components/omnibox/browser/omnibox_feature_configs.h"
#include "components/omnibox/browser/omnibox_field_trial.h"
#include "components/omnibox/common/omnibox_features.h"
#include "components/open_from_clipboard/clipboard_recent_content_features.h"
#include "components/optimization_guide/core/model_execution/model_execution_features.h"
#include "components/optimization_guide/core/optimization_guide_features.h"
#include "components/optimization_guide/core/optimization_guide_switches.h"
#include "components/page_image_service/features.h"
#include "components/page_info/core/features.h"
#include "components/paint_preview/buildflags/buildflags.h"
#include "components/paint_preview/features/features.h"
#include "components/password_manager/core/browser/features/password_features.h"
#include "components/password_manager/core/common/password_manager_features.h"
#include "components/payments/core/features.h"
#include "components/performance_manager/public/features.h"
#include "components/permissions/features.h"
#include "components/policy/core/common/features.h"
#include "components/power_bookmarks/core/flag_descriptions.h"
#include "components/power_bookmarks/core/power_bookmark_features.h"
#include "components/privacy_sandbox/privacy_sandbox_features.h"
#include "components/query_tiles/switches.h"
#include "components/reading_list/features/reading_list_switches.h"
#include "components/safe_browsing/core/common/features.h"
#include "components/search/ntp_features.h"
#include "components/search_engines/search_engines_switches.h"
#include "components/security_interstitials/content/stateful_ssl_host_state_delegate.h"
#include "components/security_state/core/security_state.h"
#include "components/segmentation_platform/public/features.h"
#include "components/send_tab_to_self/features.h"
#include "components/services/heap_profiling/public/cpp/switches.h"
#include "components/services/storage/public/cpp/buckets/bucket_info.h"
#include "components/shared_highlighting/core/common/shared_highlighting_features.h"
#include "components/signin/core/browser/dice_account_reconcilor_delegate.h"
#include "components/signin/public/base/signin_buildflags.h"
#include "components/signin/public/base/signin_switches.h"
#include "components/site_isolation/features.h"
#include "components/spellcheck/common/spellcheck_features.h"
#include "components/spellcheck/spellcheck_buildflags.h"
#include "components/supervised_user/core/common/buildflags.h"
#include "components/sync/base/command_line_switches.h"
#include "components/sync/base/features.h"
#include "components/tracing/common/tracing_switches.h"
#include "components/translate/core/browser/translate_prefs.h"
#include "components/translate/core/browser/translate_ranker_impl.h"
#include "components/translate/core/common/translate_util.h"
#include "components/trusted_vault/features.h"
#include "components/ui_devtools/switches.h"
#include "components/variations/variations_switches.h"
#include "components/version_info/version_info.h"
#include "components/viz/common/features.h"
#include "components/viz/common/switches.h"
#include "components/webapps/browser/features.h"
#include "components/webapps/common/switches.h"
#include "content/public/common/content_features.h"
#include "content/public/common/content_switches.h"
#include "device/base/features.h"
#include "device/bluetooth/bluez/bluez_features.h"
#include "device/bluetooth/chromeos_platform_features.h"
#include "device/bluetooth/floss/floss_features.h"
#include "device/fido/features.h"
#include "device/gamepad/public/cpp/gamepad_features.h"
#include "device/vr/buildflags/buildflags.h"
#include "extensions/buildflags/buildflags.h"
#include "flag_descriptions.h"
#include "gpu/config/gpu_finch_features.h"
#include "gpu/config/gpu_switches.h"
#include "media/audio/audio_features.h"
#include "media/base/media_switches.h"
#include "media/capture/capture_switches.h"
#include "media/media_buildflags.h"
#include "media/midi/midi_switches.h"
#include "media/webrtc/webrtc_features.h"
#include "mojo/core/embedder/features.h"
#include "net/base/features.h"
#include "net/net_buildflags.h"
#include "net/nqe/effective_connection_type.h"
#include "net/nqe/network_quality_estimator_params.h"
#include "net/websockets/websocket_basic_handshake_stream.h"
#include "pdf/buildflags.h"
#include "ppapi/buildflags/buildflags.h"
#include "printing/buildflags/buildflags.h"
#include "sandbox/policy/features.h"
#include "sandbox/policy/switches.h"
#include "services/device/public/cpp/device_features.h"
#include "services/media_session/public/cpp/features.h"
#include "services/network/public/cpp/features.h"
#include "services/network/public/cpp/network_switches.h"
#include "services/tracing/public/cpp/tracing_features.h"
#include "storage/browser/quota/quota_features.h"
#include "third_party/blink/public/common/features.h"
#include "third_party/blink/public/common/features_generated.h"
#include "third_party/blink/public/common/forcedark/forcedark_switches.h"
#include "third_party/blink/public/common/switches.h"
#include "ui/accessibility/accessibility_features.h"
#include "ui/accessibility/accessibility_switches.h"
#include "ui/base/ui_base_features.h"
#include "ui/base/ui_base_switches.h"
#include "ui/compositor/compositor_switches.h"
#include "ui/display/display_features.h"
#include "ui/display/display_switches.h"
#include "ui/events/blink/blink_features.h"
#include "ui/events/event_switches.h"
#include "ui/events/events_features.h"
#include "ui/gfx/switches.h"
#include "ui/gl/buildflags.h"
#include "ui/gl/gl_features.h"
#include "ui/gl/gl_switches.h"
#include "ui/native_theme/native_theme_features.h"
#include "ui/ui_features.h"
#include "url/url_features.h"

#if BUILDFLAG(IS_LINUX) || BUILDFLAG(IS_CHROMEOS)
#include "base/allocator/buildflags.h"
#endif

#if BUILDFLAG(IS_CHROMEOS)
#include "base/process/process.h"
#include "chromeos/constants/chromeos_features.h"
#endif

#if BUILDFLAG(IS_ANDROID)
#include "chrome/browser/flags/android/chrome_feature_list.h"
#include "chrome/browser/notifications/chime/android/features.h"
#include "chrome/browser/push_messaging/push_messaging_features.h"
#include "components/browser_ui/photo_picker/android/features.h"
#include "components/browser_ui/site_settings/android/features.h"
#include "components/content_creation/notes/core/note_features.h"
#include "components/external_intents/android/external_intents_features.h"
#include "components/messages/android/messages_feature.h"
#include "components/translate/content/android/translate_message.h"
#include "ui/android/ui_android_features.h"
#else  // BUILDFLAG(IS_ANDROID)
#include "chrome/browser/media/router/discovery/access_code/access_code_cast_sink_service.h"
#include "chrome/browser/media/router/media_router_feature.h"
#include "chrome/browser/web_applications/preinstalled_app_install_features.h"
#include "components/user_notes/user_notes_features.h"
#endif  // BUILDFLAG(IS_ANDROID)

#if BUILDFLAG(IS_CHROMEOS_ASH)
#include "ash/components/arc/arc_features.h"
#include "ash/components/arc/arc_util.h"
#include "ash/constants/ash_features.h"
#include "ash/constants/ash_switches.h"
#include "ash/public/cpp/app_list/app_list_features.h"
#include "ash/public/cpp/keyboard/keyboard_switches.h"
#include "chrome/browser/ash/app_list/search/search_features.h"
#include "chrome/browser/ash/crosapi/browser_manager.h"
#include "chrome/browser/ash/crosapi/browser_util.h"
#include "chrome/browser/ash/crostini/crostini_util.h"
#include "chrome/browser/ash/file_suggest/item_suggest_cache.h"
#include "chrome/browser/ash/ownership/owner_settings_service_ash.h"
#include "chrome/browser/ash/ownership/owner_settings_service_ash_factory.h"
#include "chrome/browser/ash/policy/reporting/metrics_reporting/metric_reporting_manager.h"
#include "chrome/browser/ash/settings/about_flags.h"
#include "chrome/browser/component_updater/cros_component_installer_chromeos.h"
#include "chrome/browser/nearby_sharing/common/nearby_share_features.h"
#include "chrome/common/chrome_features.h"
#include "chrome/common/webui_url_constants.h"
#include "chromeos/ash/components/assistant/buildflags.h"
#include "chromeos/ash/components/login/hibernate/hibernate_manager.h"
#include "chromeos/ash/components/memory/swap_configuration.h"
#include "chromeos/ash/components/standalone_browser/lacros_availability.h"
#include "chromeos/ash/components/standalone_browser/standalone_browser_features.h"
#include "chromeos/ash/services/assistant/public/cpp/features.h"
#include "components/app_restore/features.h"
#include "components/metrics/structured/structured_metrics_features.h"  // nogncheck
#include "media/capture/video/chromeos/video_capture_features_chromeos.h"
#include "remoting/host/chromeos/features.h"
#include "third_party/cros_system_api/switches/chrome_switches.h"
#include "ui/events/ozone/features.h"
#endif  // BUILDFLAG(IS_CHROMEOS_ASH)

#if BUILDFLAG(IS_CHROMEOS_LACROS)
#include "base/i18n/icu_mergeable_data_file.h"
#include "chrome/browser/lacros/lacros_url_handling.h"
#include "chrome/common/webui_url_constants.h"
#endif

#if BUILDFLAG(IS_MAC)
#include "chrome/browser/ui/browser_dialogs.h"
#include "chrome/browser/ui/cocoa/screentime/screentime_features.h"
#endif  // BUILDFLAG(IS_MAC)

#if BUILDFLAG(ENABLE_CARDBOARD)
#include "device/vr/public/cpp/features.h"
#endif  // ENABLE_CARDBOARD

#if BUILDFLAG(ENABLE_EXTENSIONS)
#include "chrome/browser/extensions/cws_info_service.h"
#include "extensions/common/extension_features.h"
#include "extensions/common/switches.h"
#endif  // BUILDFLAG(ENABLE_EXTENSIONS)

#if BUILDFLAG(ENABLE_PDF)
#include "pdf/pdf_features.h"
#endif

#if BUILDFLAG(ENABLE_PRINTING)
#include "printing/printing_features.h"
#endif

#if BUILDFLAG(ENABLE_SUPERVISED_USERS)
#include "components/supervised_user/core/common/features.h"  // nogncheck
#endif  // ENABLE_SUPERVISED_USERS

#if BUILDFLAG(ENABLE_VR)
#include "device/vr/public/cpp/features.h"
#endif

#if BUILDFLAG(IS_LINUX) || BUILDFLAG(IS_CHROMEOS_ASH)
#include "ui/ozone/buildflags.h"
#include "ui/ozone/public/ozone_switches.h"
#endif  // BUILDFLAG(IS_LINUX) || BUILDFLAG(IS_CHROMEOS_ASH)

#if !BUILDFLAG(IS_ANDROID) && BUILDFLAG(GOOGLE_CHROME_BRANDING)
#include "chrome/browser/promos/promos_features.h"
#endif  // !BUILDFLAG(IS_ANDROID) && BUILDFLAG(GOOGLE_CHROME_BRANDING)

#if BUILDFLAG(IS_WIN)
#include "chrome/browser/enterprise/platform_auth/platform_auth_features.h"
#include "chrome/browser/win/titlebar_config.h"
#endif

#if BUILDFLAG(IS_LINUX) || BUILDFLAG(IS_MAC) || BUILDFLAG(IS_WIN)
#include "chrome/browser/enterprise/profile_management/profile_management_features.h"
#endif  // BUILDFLAG(IS_LINUX) || BUILDFLAG(IS_MAC) || BUILDFLAG(IS_WIN)

#if defined(TOOLKIT_VIEWS)
#include "ui/views/views_features.h"
#include "ui/views/views_switches.h"
#endif  // defined(TOOLKIT_VIEWS)

using flags_ui::FeatureEntry;
using flags_ui::kDeprecated;
using flags_ui::kOsAndroid;
using flags_ui::kOsCrOS;
using flags_ui::kOsCrOSOwnerOnly;
using flags_ui::kOsFuchsia;
using flags_ui::kOsLacros;
using flags_ui::kOsLinux;
using flags_ui::kOsMac;
using flags_ui::kOsWin;

namespace about_flags {

namespace {

const unsigned kOsAll =
    kOsMac | kOsWin | kOsLinux | kOsCrOS | kOsAndroid | kOsFuchsia | kOsLacros;
const unsigned kOsDesktop =
    kOsMac | kOsWin | kOsLinux | kOsCrOS | kOsFuchsia | kOsLacros;

#if defined(USE_AURA)
const unsigned kOsAura = kOsWin | kOsLinux | kOsCrOS | kOsFuchsia | kOsLacros;
#endif  // USE_AURA

#if defined(USE_AURA)
const FeatureEntry::Choice kPullToRefreshChoices[] = {
    {flags_ui::kGenericExperimentChoiceDefault, "", ""},
    {flags_ui::kGenericExperimentChoiceDisabled, switches::kPullToRefresh, "0"},
    {flags_ui::kGenericExperimentChoiceEnabled, switches::kPullToRefresh, "1"},
    {flag_descriptions::kPullToRefreshEnabledTouchscreen,
     switches::kPullToRefresh, "2"}};
#endif  // USE_AURA

const FeatureEntry::Choice kOverlayStrategiesChoices[] = {
    {flag_descriptions::kOverlayStrategiesDefault, "", ""},
    {flag_descriptions::kOverlayStrategiesNone,
     switches::kEnableHardwareOverlays, ""},
    {flag_descriptions::kOverlayStrategiesUnoccludedFullscreen,
     switches::kEnableHardwareOverlays, "single-fullscreen"},
    {flag_descriptions::kOverlayStrategiesUnoccluded,
     switches::kEnableHardwareOverlays, "single-fullscreen,single-on-top"},
    {flag_descriptions::kOverlayStrategiesOccludedAndUnoccluded,
     switches::kEnableHardwareOverlays,
     "single-fullscreen,single-on-top,underlay"},
};

const FeatureEntry::Choice kTouchTextSelectionStrategyChoices[] = {
    {flags_ui::kGenericExperimentChoiceDefault, "", ""},
    {flag_descriptions::kTouchSelectionStrategyCharacter,
     blink::switches::kTouchTextSelectionStrategy,
     blink::switches::kTouchTextSelectionStrategy_Character},
    {flag_descriptions::kTouchSelectionStrategyDirection,
     blink::switches::kTouchTextSelectionStrategy,
     blink::switches::kTouchTextSelectionStrategy_Direction}};

const FeatureEntry::Choice kEnablePasswordSharingChoices[] = {
    {"Default", "", ""},
    {"Bootstraping Only", switches::kEnableFeatures,
     "SharingOfferKeyPairBootstrap"},
    {"Enabled", switches::kEnableFeatures,
     "SharingOfferKeyPairBootstrap,SendPasswords,"
     "PasswordManagerEnableSenderService,"
     "PasswordManagerEnableReceiverService,SharedPasswordNotificationUI"},
};

const FeatureEntry::Choice kEnableSearchEngineChoice[] = {
    {"Default", "", ""},
    {"Enabled", switches::kEnableFeatures,
     "SearchEngineChoice,SearchEngineChoiceTrigger"},
    {"Disabled", switches::kDisableSearchEngineChoiceScreen, ""},
    {"Enabled - WithForcedEeaCountry", switches::kEnableFeatures,
     "SearchEngineChoice,SearchEngineChoiceTrigger:with_force_eea_country/"
     "true"},
};

#if BUILDFLAG(IS_WIN)
const FeatureEntry::FeatureParam kMediaFoundationClearStrategyUseFrameServer[] =
    {{"strategy", "frame-server"}};

const FeatureEntry::FeatureParam
    kMediaFoundationClearStrategyUseDirectComposition[] = {
        {"strategy", "direct-composition"}};

const FeatureEntry::FeatureParam kMediaFoundationClearStrategyUseDynamic[] = {
    {"strategy", "dynamic"}};

const FeatureEntry::FeatureVariation kMediaFoundationClearStrategyVariations[] =
    {{"Direct Composition", kMediaFoundationClearStrategyUseDirectComposition,
      std::size(kMediaFoundationClearStrategyUseDirectComposition), nullptr},
     {"Frame Server", kMediaFoundationClearStrategyUseFrameServer,
      std::size(kMediaFoundationClearStrategyUseFrameServer), nullptr},
     {"Dynamic", kMediaFoundationClearStrategyUseDynamic,
      std::size(kMediaFoundationClearStrategyUseDynamic), nullptr}};

const FeatureEntry::Choice kUseAngleChoicesWindows[] = {
    {flag_descriptions::kUseAngleDefault, "", ""},
    {flag_descriptions::kUseAngleGL, switches::kUseANGLE,
     gl::kANGLEImplementationOpenGLName},
    {flag_descriptions::kUseAngleD3D11, switches::kUseANGLE,
     gl::kANGLEImplementationD3D11Name},
    {flag_descriptions::kUseAngleD3D9, switches::kUseANGLE,
     gl::kANGLEImplementationD3D9Name},
    {flag_descriptions::kUseAngleD3D11on12, switches::kUseANGLE,
     gl::kANGLEImplementationD3D11on12Name}};
#elif BUILDFLAG(IS_MAC)
const FeatureEntry::Choice kUseAngleChoicesMac[] = {
    {flag_descriptions::kUseAngleDefault, "", ""},
    {flag_descriptions::kUseAngleGL, switches::kUseANGLE,
     gl::kANGLEImplementationOpenGLName},
    {flag_descriptions::kUseAngleMetal, switches::kUseANGLE,
     gl::kANGLEImplementationMetalName}};
#endif

#if BUILDFLAG(IS_WIN)
const FeatureEntry::FeatureParam kDXGIWaitableSwapChain1Frame = {
    "DXGIWaitableSwapChainMaxQueuedFrames", "1"};

const FeatureEntry::FeatureParam kDXGIWaitableSwapChain2Frames = {
    "DXGIWaitableSwapChainMaxQueuedFrames", "2"};

const FeatureEntry::FeatureParam kDXGIWaitableSwapChain3Frames = {
    "DXGIWaitableSwapChainMaxQueuedFrames", "3"};

const FeatureEntry::FeatureVariation kDXGIWaitableSwapChainVariations[] = {
    {"Max 1 Frame", &kDXGIWaitableSwapChain1Frame, 1, nullptr},
    {"Max 2 Frames", &kDXGIWaitableSwapChain2Frames, 1, nullptr},
    {"Max 3 Frames", &kDXGIWaitableSwapChain3Frames, 1, nullptr}};
#endif

#if BUILDFLAG(IS_LINUX)
const FeatureEntry::Choice kOzonePlatformHintRuntimeChoices[] = {
    {flag_descriptions::kOzonePlatformHintChoiceDefault, "", ""},
    {flag_descriptions::kOzonePlatformHintChoiceAuto,
     switches::kOzonePlatformHint, "auto"},
#if BUILDFLAG(OZONE_PLATFORM_X11)
    {flag_descriptions::kOzonePlatformHintChoiceX11,
     switches::kOzonePlatformHint, "x11"},
#endif
#if BUILDFLAG(OZONE_PLATFORM_WAYLAND)
    {flag_descriptions::kOzonePlatformHintChoiceWayland,
     switches::kOzonePlatformHint, "wayland"},
#endif
};
#endif

#if BUILDFLAG(ENABLE_VR)
const FeatureEntry::Choice kWebXrForceRuntimeChoices[] = {
    {flags_ui::kGenericExperimentChoiceDefault, "", ""},
    {flag_descriptions::kWebXrRuntimeChoiceNone, switches::kWebXrForceRuntime,
     switches::kWebXrRuntimeNone},
#if BUILDFLAG(ENABLE_CARDBOARD)
    {flag_descriptions::kWebXrRuntimeChoiceCardboard,
     switches::kWebXrForceRuntime, switches::kWebXrRuntimeCardboard},
#endif
#if BUILDFLAG(ENABLE_GVR_SERVICES)
    {flag_descriptions::kWebXrRuntimeChoiceGVR, switches::kWebXrForceRuntime,
     switches::kWebXrRuntimeGVR},
#endif
#if BUILDFLAG(ENABLE_OPENXR)
    {flag_descriptions::kWebXrRuntimeChoiceOpenXR, switches::kWebXrForceRuntime,
     switches::kWebXrRuntimeOpenXr},
#endif  // ENABLE_OPENXR
};
#endif  // ENABLE_VR

#if BUILDFLAG(IS_ANDROID)
const FeatureEntry::FeatureParam kCCTMinimizedDefaultIcon[] = {
    {"icon_variant", "0"}};
const FeatureEntry::FeatureParam kCCTMinimizedAlternativeIcon[] = {
    {"icon_variant", "1"}};

const FeatureEntry::FeatureVariation kCCTMinimizedIconVariations[] = {
    {"Use default minimize icon", kCCTMinimizedDefaultIcon,
     std::size(kCCTMinimizedDefaultIcon), nullptr},
    {"Use alternative minimize icon", kCCTMinimizedAlternativeIcon,
     std::size(kCCTMinimizedAlternativeIcon), nullptr}};

const FeatureEntry::FeatureParam kCCTResizablePolicyParamUseAllowlist[] = {
    {"default_policy", "use-allowlist"}};
const FeatureEntry::FeatureParam kCCTResizablePolicyParamUseDenylist[] = {
    {"default_policy", "use-denylist"}};

const FeatureEntry::FeatureVariation
    kCCTResizableThirdPartiesDefaultPolicyVariations[] = {
        {"Use Allowlist", kCCTResizablePolicyParamUseAllowlist,
         std::size(kCCTResizablePolicyParamUseAllowlist), nullptr},
        {"Use Denylist", kCCTResizablePolicyParamUseDenylist,
         std::size(kCCTResizablePolicyParamUseDenylist), nullptr}};

const FeatureEntry::FeatureParam kCCTBrandingTestFriendly[] = {
    {"use_temporary_storage", "true"},
    {"branding_cadence", "10000"}  // 10 seconds
};

const FeatureEntry::FeatureVariation kCctBrandTransparencyVariations[] = {
    {"Test friendly mode", kCCTBrandingTestFriendly,
     std::size(kCCTBrandingTestFriendly), nullptr}};

const FeatureEntry::FeatureParam
    kCCTRealTimeEngagementSignalsParamRealValues[] = {{"real_values", "true"}};
const FeatureEntry::FeatureParam
    kCCTRealTimeEngagementSignalsParamFakeValues[] = {{"real_values", "false"}};

const FeatureEntry::FeatureParam kCCTPageInsightsHubFastPeekTriggerParam = {
    "page_insights_can_autotrigger_after_end", "1000"};  // 1s
const FeatureEntry::FeatureParam kCCTPageInsightsHubShorterFullSizeParam = {
    "page_insights_full_height_ratio", "0.775"};
const FeatureEntry::FeatureParam kCCTPageInsightsHubShorterPeekSizeParam = {
    "page_insights_peek_height_ratio", "0.13"};
const FeatureEntry::FeatureParam
    kCCTPageInsightsHubShorterPeekWithPrivacySizeParam = {
        "page_insights_peek_with_privacy_height_ratio", "0.2"};
const FeatureEntry::FeatureParam kCCTPageInsightsHubFastPeekTriggerParams[] = {
    kCCTPageInsightsHubFastPeekTriggerParam};
const FeatureEntry::FeatureParam kCCTPageInsightsHubShorterSheetParams[] = {
    kCCTPageInsightsHubShorterFullSizeParam,
    kCCTPageInsightsHubShorterPeekSizeParam,
    kCCTPageInsightsHubShorterPeekWithPrivacySizeParam};
const FeatureEntry::FeatureParam kCCTPageInsightsHubBothParams[] = {
    kCCTPageInsightsHubFastPeekTriggerParam,
    kCCTPageInsightsHubShorterFullSizeParam,
    kCCTPageInsightsHubShorterPeekSizeParam,
    kCCTPageInsightsHubShorterPeekWithPrivacySizeParam};
const FeatureEntry::FeatureVariation kCCTPageInsightsHubVariations[] = {
    {"with fast peek trigger", kCCTPageInsightsHubFastPeekTriggerParams,
     std::size(kCCTPageInsightsHubFastPeekTriggerParams), nullptr},
    {"with shorter sheet", kCCTPageInsightsHubShorterSheetParams,
     std::size(kCCTPageInsightsHubShorterSheetParams), nullptr},
    {"with both", kCCTPageInsightsHubBothParams,
     std::size(kCCTPageInsightsHubBothParams), nullptr}};

const FeatureEntry::FeatureVariation kCCTRealTimeEngagementSignalsVariations[] =
    {{"Send real values", kCCTRealTimeEngagementSignalsParamRealValues,
      std::size(kCCTRealTimeEngagementSignalsParamRealValues), nullptr},
     {"Send fake values", kCCTRealTimeEngagementSignalsParamFakeValues,
      std::size(kCCTRealTimeEngagementSignalsParamFakeValues), nullptr}};

const FeatureEntry::FeatureParam
    kCCTRealTimeEngagementSignalsAlternativeImplParam300[] = {
        {"time_can_update_after_end", "300"}  // 300ms
};
const FeatureEntry::FeatureParam
    kCCTRealTimeEngagementSignalsAlternativeImplParam100[] = {
        {"time_can_update_after_end", "100"}  // 100ms
};

const FeatureEntry::FeatureVariation
    kCCTRealTimeEngagementSignalsAlternativeImplVariations[] = {
        {"Allow 300ms for scroll updates after scroll-end",
         kCCTRealTimeEngagementSignalsAlternativeImplParam300,
         std::size(kCCTRealTimeEngagementSignalsAlternativeImplParam300),
         nullptr},
        {"Allow 100ms for scroll updates after scroll-end",
         kCCTRealTimeEngagementSignalsAlternativeImplParam100,
         std::size(kCCTRealTimeEngagementSignalsAlternativeImplParam100),
         nullptr}};

const FeatureEntry::Choice kReaderModeHeuristicsChoices[] = {
    {flags_ui::kGenericExperimentChoiceDefault, "", ""},
    {flag_descriptions::kReaderModeHeuristicsMarkup,
     switches::kReaderModeHeuristics,
     switches::reader_mode_heuristics::kOGArticle},
    {flag_descriptions::kReaderModeHeuristicsAdaboost,
     switches::kReaderModeHeuristics,
     switches::reader_mode_heuristics::kAdaBoost},
    {flag_descriptions::kReaderModeHeuristicsAlwaysOn,
     switches::kReaderModeHeuristics,
     switches::reader_mode_heuristics::kAlwaysTrue},
    {flag_descriptions::kReaderModeHeuristicsAlwaysOff,
     switches::kReaderModeHeuristics, switches::reader_mode_heuristics::kNone},
    {flag_descriptions::kReaderModeHeuristicsAllArticles,
     switches::kReaderModeHeuristics,
     switches::reader_mode_heuristics::kAllArticles},
};

const FeatureEntry::Choice kForceUpdateMenuTypeChoices[] = {
    {flags_ui::kGenericExperimentChoiceDefault, "", ""},
    {flag_descriptions::kUpdateMenuTypeNone, switches::kForceUpdateMenuType,
     "none"},
    {flag_descriptions::kUpdateMenuTypeUpdateAvailable,
     switches::kForceUpdateMenuType, "update_available"},
    {flag_descriptions::kUpdateMenuTypeUnsupportedOSVersion,
     switches::kForceUpdateMenuType, "unsupported_os_version"},
};

const FeatureEntry::FeatureParam kOmahaMinSdkVersionAndroidMinSdk1[] = {
    {"min_sdk_version", "1"}};
const FeatureEntry::FeatureParam kOmahaMinSdkVersionAndroidMinSdk1000[] = {
    {"min_sdk_version", "1000"}};
const FeatureEntry::FeatureVariation kOmahaMinSdkVersionAndroidVariations[] = {
    {flag_descriptions::kOmahaMinSdkVersionAndroidMinSdk1Description,
     kOmahaMinSdkVersionAndroidMinSdk1,
     std::size(kOmahaMinSdkVersionAndroidMinSdk1), nullptr},
    {flag_descriptions::kOmahaMinSdkVersionAndroidMinSdk1000Description,
     kOmahaMinSdkVersionAndroidMinSdk1000,
     std::size(kOmahaMinSdkVersionAndroidMinSdk1000), nullptr},
};

const FeatureEntry::FeatureParam
    kOptimizationGuidePersonalizedFetchingAllowPageInsights[] = {
        {"allowed_contexts", "CONTEXT_PAGE_INSIGHTS_HUB"}};
const FeatureEntry::FeatureVariation
    kOptimizationGuidePersonalizedFetchingAllowPageInsightsVariations[] = {
        {"for Page Insights",
         kOptimizationGuidePersonalizedFetchingAllowPageInsights,
         std::size(kOptimizationGuidePersonalizedFetchingAllowPageInsights),
         nullptr}};

#else   // BUILDFLAG(IS_ANDROID)
const FeatureEntry::FeatureParam kReaderModeOfferInSettings[] = {
    {switches::kReaderModeDiscoverabilityParamName,
     switches::kReaderModeOfferInSettings}};

const FeatureEntry::FeatureVariation kReaderModeDiscoverabilityVariations[] = {
    {"available in settings", kReaderModeOfferInSettings,
     std::size(kReaderModeOfferInSettings), nullptr}};
#endif  // BUILDFLAG(IS_ANDROID)

#if BUILDFLAG(IS_ANDROID)
const FeatureEntry::FeatureParam kAdaptiveButton_AlwaysNone[] = {
    {"mode", "always-none"}};
const FeatureEntry::FeatureParam kAdaptiveButton_AlwaysNewTab[] = {
    {"mode", "always-new-tab"}};
const FeatureEntry::FeatureParam kAdaptiveButton_AlwaysShare[] = {
    {"mode", "always-share"}};
const FeatureEntry::FeatureParam kAdaptiveButton_AlwaysVoice[] = {
    {"mode", "always-voice"}};
const FeatureEntry::FeatureParam kAdaptiveButton_AlwaysTranslate[] = {
    {"mode", "always-translate"}};
const FeatureEntry::FeatureVariation kAdaptiveButtonInTopToolbarVariations[] = {
    {"Always None", kAdaptiveButton_AlwaysNone,
     std::size(kAdaptiveButton_AlwaysNone), nullptr},
    {"Always New Tab", kAdaptiveButton_AlwaysNewTab,
     std::size(kAdaptiveButton_AlwaysNewTab), nullptr},
    {"Always Share", kAdaptiveButton_AlwaysShare,
     std::size(kAdaptiveButton_AlwaysShare), nullptr},
    {"Always Voice", kAdaptiveButton_AlwaysVoice,
     std::size(kAdaptiveButton_AlwaysVoice), nullptr},
    {"Always Translate", kAdaptiveButton_AlwaysTranslate,
     std::size(kAdaptiveButton_AlwaysTranslate), nullptr},
};

const FeatureEntry::FeatureParam kAdaptiveButtonCustomization_NewTab[] = {
    {"default_segment", "new-tab"},
    {"show_ui_only_after_ready", "false"},
    {"ignore_segmentation_results", "true"}};
const FeatureEntry::FeatureParam kAdaptiveButtonCustomization_Share[] = {
    {"default_segment", "share"},
    {"show_ui_only_after_ready", "false"},
    {"ignore_segmentation_results", "true"}};
const FeatureEntry::FeatureParam kAdaptiveButtonCustomization_Voice[] = {
    {"default_segment", "voice"},
    {"show_ui_only_after_ready", "false"},
    {"ignore_segmentation_results", "true"}};
const FeatureEntry::FeatureVariation
    kAdaptiveButtonInTopToolbarCustomizationVariations[] = {
        {"New Tab", kAdaptiveButtonCustomization_NewTab,
         std::size(kAdaptiveButtonCustomization_NewTab), nullptr},
        {"Share", kAdaptiveButtonCustomization_Share,
         std::size(kAdaptiveButtonCustomization_Share), nullptr},
        {"Voice", kAdaptiveButtonCustomization_Voice,
         std::size(kAdaptiveButtonCustomization_Voice), nullptr},
};

const FeatureEntry::FeatureParam kContextualPageActionsUiParams_Quiet[] = {
    {"action_chip", "false"},
};
const FeatureEntry::FeatureParam kContextualPageActionsUiParams_ActionChip[] = {
    {"action_chip", "true"},
    {"action_chip_time_ms", "3000"},
};
const FeatureEntry::FeatureParam
    kContextualPageActionsUiParams_ActionChip_6s[] = {
        {"action_chip", "true"},
        {"action_chip_time_ms", "6000"},
};
const FeatureEntry::FeatureParam
    kContextualPageActionsUiParams_ActionChip_AltColor[] = {
        {"action_chip", "true"},
        {"action_chip_time_ms", "3000"},
        {"action_chip_with_different_color", "true"},
};
const FeatureEntry::FeatureParam
    kContextualPageActionsUiParams_ActionChip_AltColor_6s[] = {
        {"action_chip", "true"},
        {"action_chip_time_ms", "6000"},
        {"action_chip_with_different_color", "true"},
};

const FeatureEntry::FeatureParam kContextualPageActions_DisableUi[]{
    {"disable_ui", "true"},
};
const FeatureEntry::FeatureVariation kContextualPageActionsVariations[] = {
    {"Disable UI", kContextualPageActions_DisableUi},
};

const FeatureEntry::FeatureVariation
    kContextualPageActionPriceTrackingVariations[] = {
        {"Quiet", kContextualPageActionsUiParams_Quiet,
         std::size(kContextualPageActionsUiParams_Quiet), nullptr},
        {"Action Chip", kContextualPageActionsUiParams_ActionChip,
         std::size(kContextualPageActionsUiParams_ActionChip), nullptr},
        {"Action Chip - 6s", kContextualPageActionsUiParams_ActionChip_6s,
         std::size(kContextualPageActionsUiParams_ActionChip_6s), nullptr},
        {"Action Chip - Alternative Color",
         kContextualPageActionsUiParams_ActionChip_AltColor,
         std::size(kContextualPageActionsUiParams_ActionChip_AltColor),
         nullptr},
        {"Action Chip - Alternative Color - 6s",
         kContextualPageActionsUiParams_ActionChip_AltColor_6s,
         std::size(kContextualPageActionsUiParams_ActionChip_AltColor_6s),
         nullptr},
};

const FeatureEntry::FeatureParam
    kContextualPageActionReaderMode_ActionChip_NotRateLimited[] = {
        {"action_chip", "true"},
        {"action_chip_time_ms", "3000"},
        {"reader_mode_session_rate_limiting", "false"},
};
const FeatureEntry::FeatureParam
    kContextualPageActionReaderMode_ActionChip_NotRateLimited_6s[] = {
        {"action_chip", "true"},
        {"action_chip_time_ms", "6000"},
        {"reader_mode_session_rate_limiting", "false"},
};
const FeatureEntry::FeatureVariation
    kContextualPageActionReaderModeVariations[] = {
        {"Quiet", kContextualPageActionsUiParams_Quiet,
         std::size(kContextualPageActionsUiParams_Quiet), nullptr},
        {"Action Chip", kContextualPageActionsUiParams_ActionChip,
         std::size(kContextualPageActionsUiParams_ActionChip), nullptr},
        {"Action Chip - 6s", kContextualPageActionsUiParams_ActionChip_6s,
         std::size(kContextualPageActionsUiParams_ActionChip_6s), nullptr},
        {"Action Chip - Alternative Color",
         kContextualPageActionsUiParams_ActionChip_AltColor,
         std::size(kContextualPageActionsUiParams_ActionChip_AltColor),
         nullptr},
        {"Action Chip - Alternative Color - 6s",
         kContextualPageActionsUiParams_ActionChip_AltColor_6s,
         std::size(kContextualPageActionsUiParams_ActionChip_AltColor_6s),
         nullptr},
        {"Action Chip - Not rate limited - 3s",
         kContextualPageActionReaderMode_ActionChip_NotRateLimited,
         std::size(kContextualPageActionReaderMode_ActionChip_NotRateLimited),
         nullptr},
        {"Action Chip - Not rate limited - 6s",
         kContextualPageActionReaderMode_ActionChip_NotRateLimited_6s,
         std::size(
             kContextualPageActionReaderMode_ActionChip_NotRateLimited_6s),
         nullptr},
};

#endif  // BUILDFLAG(IS_ANDROID)

#if !BUILDFLAG(IS_CHROMEOS_ASH)
const FeatureEntry::FeatureParam kForceDark_SimpleHsl[] = {
    {"inversion_method", "hsl_based"},
    {"image_behavior", "none"},
    {"foreground_lightness_threshold", "150"},
    {"background_lightness_threshold", "205"}};

const FeatureEntry::FeatureParam kForceDark_SimpleCielab[] = {
    {"inversion_method", "cielab_based"},
    {"image_behavior", "none"},
    {"foreground_lightness_threshold", "150"},
    {"background_lightness_threshold", "205"}};

const FeatureEntry::FeatureParam kForceDark_SimpleRgb[] = {
    {"inversion_method", "rgb_based"},
    {"image_behavior", "none"},
    {"foreground_lightness_threshold", "150"},
    {"background_lightness_threshold", "205"}};

// Keep in sync with the kForceDark_SelectiveImageInversion
// in aw_feature_entries.cc if you tweak these parameters.
const FeatureEntry::FeatureParam kForceDark_SelectiveImageInversion[] = {
    {"inversion_method", "cielab_based"},
    {"image_behavior", "selective"},
    {"foreground_lightness_threshold", "150"},
    {"background_lightness_threshold", "205"}};

const FeatureEntry::FeatureParam kForceDark_SelectiveElementInversion[] = {
    {"inversion_method", "cielab_based"},
    {"image_behavior", "none"},
    {"foreground_lightness_threshold", "150"},
    {"background_lightness_threshold", "205"}};

const FeatureEntry::FeatureParam kForceDark_SelectiveGeneralInversion[] = {
    {"inversion_method", "cielab_based"},
    {"image_behavior", "selective"},
    {"foreground_lightness_threshold", "150"},
    {"background_lightness_threshold", "205"}};

const FeatureEntry::FeatureParam kForceDark_TransparencyAndNumColors[] = {
    {"classifier_policy", "transparency_and_num_colors"}};

const FeatureEntry::FeatureVariation kForceDarkVariations[] = {
    {"with simple HSL-based inversion", kForceDark_SimpleHsl,
     std::size(kForceDark_SimpleHsl), nullptr},
    {"with simple CIELAB-based inversion", kForceDark_SimpleCielab,
     std::size(kForceDark_SimpleCielab), nullptr},
    {"with simple RGB-based inversion", kForceDark_SimpleRgb,
     std::size(kForceDark_SimpleRgb), nullptr},
    {"with selective image inversion", kForceDark_SelectiveImageInversion,
     std::size(kForceDark_SelectiveImageInversion), nullptr},
    {"with selective inversion of non-image elements",
     kForceDark_SelectiveElementInversion,
     std::size(kForceDark_SelectiveElementInversion), nullptr},
    {"with selective inversion of everything",
     kForceDark_SelectiveGeneralInversion,
     std::size(kForceDark_SelectiveGeneralInversion), nullptr},
    {"with selective image inversion based on transparency and number of "
     "colors",
     kForceDark_TransparencyAndNumColors,
     std::size(kForceDark_TransparencyAndNumColors), nullptr}};
#endif  // !BUILDFLAG(IS_CHROMEOS)

const FeatureEntry::FeatureParam kClipboardMaximumAge60Seconds[] = {
    {"UIClipboardMaximumAge", "60"}};
const FeatureEntry::FeatureParam kClipboardMaximumAge90Seconds[] = {
    {"UIClipboardMaximumAge", "90"}};
const FeatureEntry::FeatureParam kClipboardMaximumAge120Seconds[] = {
    {"UIClipboardMaximumAge", "120"}};
const FeatureEntry::FeatureParam kClipboardMaximumAge150Seconds[] = {
    {"UIClipboardMaximumAge", "150"}};
const FeatureEntry::FeatureParam kClipboardMaximumAge180Seconds[] = {
    {"UIClipboardMaximumAge", "180"}};

const FeatureEntry::FeatureVariation kClipboardMaximumAgeVariations[] = {
    {"Enabled 60 seconds", kClipboardMaximumAge60Seconds,
     std::size(kClipboardMaximumAge60Seconds), nullptr},
    {"Enabled 90 seconds", kClipboardMaximumAge90Seconds,
     std::size(kClipboardMaximumAge90Seconds), nullptr},
    {"Enabled 120 seconds", kClipboardMaximumAge120Seconds,
     std::size(kClipboardMaximumAge120Seconds), nullptr},
    {"Enabled 150 seconds", kClipboardMaximumAge150Seconds,
     std::size(kClipboardMaximumAge150Seconds), nullptr},
    {"Enabled 180 seconds", kClipboardMaximumAge180Seconds,
     std::size(kClipboardMaximumAge180Seconds), nullptr},
};

const FeatureEntry::FeatureParam kMBIModeLegacy[] = {{"mode", "legacy"}};
const FeatureEntry::FeatureParam kMBIModeEnabledPerRenderProcessHost[] = {
    {"mode", "per_render_process_host"}};
const FeatureEntry::FeatureParam kMBIModeEnabledPerSiteInstance[] = {
    {"mode", "per_site_instance"}};

const FeatureEntry::FeatureVariation kMBIModeVariations[] = {
    {"legacy mode", kMBIModeLegacy, std::size(kMBIModeLegacy), nullptr},
    {"per render process host", kMBIModeEnabledPerRenderProcessHost,
     std::size(kMBIModeEnabledPerRenderProcessHost), nullptr},
    {"per site instance", kMBIModeEnabledPerSiteInstance,
     std::size(kMBIModeEnabledPerSiteInstance), nullptr}};

const FeatureEntry::FeatureParam kSearchPrefetchWithoutHoldback[] = {
    {"prefetch_holdback", "false"}};
const FeatureEntry::FeatureParam kSearchPrefetchWithHoldback[] = {
    {"prefetch_holdback", "true"}};

const FeatureEntry::FeatureVariation
    kSearchPrefetchServicePrefetchingVariations[] = {
        {"without holdback", kSearchPrefetchWithoutHoldback,
         std::size(kSearchPrefetchWithoutHoldback), nullptr},
        {"with holdback", kSearchPrefetchWithHoldback,
         std::size(kSearchPrefetchWithHoldback), nullptr}};

#if BUILDFLAG(IS_CHROMEOS)
// Note these strings must match the `kUserGroupParam` definition in
// web_applications/preinstalled_web_app_window_experiment.cc.
const FeatureEntry::FeatureParam kPreinstalledWebAppWindowExperimentControl[] =
    {{"user_group", "control"}};
const FeatureEntry::FeatureParam kPreinstalledWebAppWindowExperimentWindow[] = {
    {"user_group", "window"}};
const FeatureEntry::FeatureParam kPreinstalledWebAppWindowExperimentTab[] = {
    {"user_group", "tab"}};
const FeatureEntry::FeatureVariation
    kPreinstalledWebAppWindowExperimentVariations[] = {
        {"control", kPreinstalledWebAppWindowExperimentControl,
         std::size(kPreinstalledWebAppWindowExperimentControl), nullptr},
        {"window", kPreinstalledWebAppWindowExperimentWindow,
         std::size(kPreinstalledWebAppWindowExperimentWindow), nullptr},
        {"tab", kPreinstalledWebAppWindowExperimentTab,
         std::size(kPreinstalledWebAppWindowExperimentTab), nullptr}};

const FeatureEntry::FeatureParam kRoundedWindowRadius8 = {
    chromeos::features::kRoundedWindowsRadius, "8"};

const FeatureEntry::FeatureParam kRoundedWindowRadius10 = {
    chromeos::features::kRoundedWindowsRadius, "10"};

const FeatureEntry::FeatureParam kRoundedWindowRadius12 = {
    chromeos::features::kRoundedWindowsRadius, "12"};

const FeatureEntry::FeatureParam kRoundedWindowRadius14 = {
    chromeos::features::kRoundedWindowsRadius, "14"};

const FeatureEntry::FeatureParam kRoundedWindowRadius16 = {
    chromeos::features::kRoundedWindowsRadius, "16"};

const FeatureEntry::FeatureParam kRoundedWindowRadius18 = {
    chromeos::features::kRoundedWindowsRadius, "18"};

const FeatureEntry::FeatureVariation kRoundedWindowsRadiusVariation[] = {
    {"8", &kRoundedWindowRadius8, 1, nullptr},
    {"10", &kRoundedWindowRadius10, 1, nullptr},
    {"12", &kRoundedWindowRadius12, 1, nullptr},
    {"14", &kRoundedWindowRadius14, 1, nullptr},
    {"16", &kRoundedWindowRadius16, 1, nullptr},
    {"18", &kRoundedWindowRadius18, 1, nullptr},
};

#endif  // BUILDFLAG(IS_CHROMEOS)

#if BUILDFLAG(IS_CHROMEOS_ASH)
const FeatureEntry::FeatureParam kArcVmMemorySizeShift_200[] = {
    {"shift_mib", "-200"}};
const FeatureEntry::FeatureParam kArcVmMemorySizeShift_500[] = {
    {"shift_mib", "-500"}};
const FeatureEntry::FeatureParam kArcVmMemorySizeShift_800[] = {
    {"shift_mib", "-800"}};

const FeatureEntry::FeatureVariation kArcVmMemorySizeVariations[] = {
    {"shift -200MiB", kArcVmMemorySizeShift_200,
     std::size(kArcVmMemorySizeShift_200), nullptr},
    {"shift -500MiB", kArcVmMemorySizeShift_500,
     std::size(kArcVmMemorySizeShift_500), nullptr},
    {"shift -800MiB", kArcVmMemorySizeShift_800,
     std::size(kArcVmMemorySizeShift_800), nullptr},
};
#endif  // BUILDFLAG(IS_CHROMEOS_ASH)

#if BUILDFLAG(IS_ANDROID)
const FeatureEntry::FeatureParam kCriticalPersistedTabDataSaveAndRestore[] = {
    {"critical_persisted_tab_data_save_only", "false"},
    {"delay_saves_until_deferred_startup", "false"}};
const FeatureEntry::FeatureParam kCriticalPersistedTabDataSaveOnly[] = {
    {"critical_persisted_tab_data_save_only", "true"},
    {"delay_saves_until_deferred_startup", "false"}};
const FeatureEntry::FeatureParam kDelaySavesUntilDeferredStartup[] = {
    {"critical_persisted_tab_data_save_only", "false"},
    {"delay_saves_until_deferred_startup", "true"}};

const FeatureEntry::FeatureVariation kCriticalPersistedTabDataVariations[] = {
    {"Save and Restore", kCriticalPersistedTabDataSaveAndRestore,
     std::size(kCriticalPersistedTabDataSaveAndRestore), nullptr},
    {"Save Only", kCriticalPersistedTabDataSaveOnly,
     std::size(kCriticalPersistedTabDataSaveOnly), nullptr},
    {"Delay saves until DeferredStartup", kDelaySavesUntilDeferredStartup,
     std::size(kDelaySavesUntilDeferredStartup), nullptr}};

const FeatureEntry::FeatureParam kShowSingleRowMVTiles[] = {
    {"most_visited_max_rows_normal_screen", "1"},
    {"most_visited_max_rows_small_screen", "1"},
    {"small_screen_height_threshold_dp", "700"}};
const FeatureEntry::FeatureParam kShowTwoRowsMVTiles[] = {
    {"most_visited_max_rows_normal_screen", "2"},
    {"most_visited_max_rows_small_screen", "2"},
    {"small_screen_height_threshold_dp", "700"}};
const FeatureEntry::FeatureVariation kQueryTilesVariations[] = {
    {"(show single row of MV tiles)", kShowSingleRowMVTiles,
     std::size(kShowSingleRowMVTiles), nullptr},
    {"(show two rows of MV tiles)", kShowTwoRowsMVTiles,
     std::size(kShowTwoRowsMVTiles), nullptr}};

const FeatureEntry::FeatureParam kTangibleSyncGroupA[] = {{"group_id", "1"}};
const FeatureEntry::FeatureParam kTangibleSyncGroupB[] = {{"group_id", "2"}};
const FeatureEntry::FeatureParam kTangibleSyncGroupC[] = {{"group_id", "3"}};
const FeatureEntry::FeatureParam kTangibleSyncGroupD[] = {{"group_id", "4"}};
const FeatureEntry::FeatureParam kTangibleSyncGroupE[] = {{"group_id", "5"}};
const FeatureEntry::FeatureParam kTangibleSyncGroupF[] = {{"group_id", "6"}};
const FeatureEntry::FeatureVariation kTangibleSyncVariations[] = {
    {"(pick up where you left off)", kTangibleSyncGroupA,
     std::size(kTangibleSyncGroupA), nullptr},
    {"(browse across devices)", kTangibleSyncGroupB,
     std::size(kTangibleSyncGroupB), nullptr},
    {"(save time and type less)", kTangibleSyncGroupC,
     std::size(kTangibleSyncGroupC), nullptr},
    {"(get better suggestions)", kTangibleSyncGroupD,
     std::size(kTangibleSyncGroupD), nullptr},
    {"(sync your tabs and history)", kTangibleSyncGroupE,
     std::size(kTangibleSyncGroupE), nullptr},
    {"(Turn on sync?)", kTangibleSyncGroupF, std::size(kTangibleSyncGroupF),
     nullptr}};
#endif  // BUILDFLAG(IS_ANDROID)

const FeatureEntry::Choice kEnableGpuRasterizationChoices[] = {
    {flags_ui::kGenericExperimentChoiceDefault, "", ""},
    {flags_ui::kGenericExperimentChoiceEnabled,
     switches::kEnableGpuRasterization, ""},
    {flags_ui::kGenericExperimentChoiceDisabled,
     switches::kDisableGpuRasterization, ""},
};

const FeatureEntry::Choice kTopChromeTouchUiChoices[] = {
    {flags_ui::kGenericExperimentChoiceDefault, "", ""},
    {flags_ui::kGenericExperimentChoiceAutomatic, switches::kTopChromeTouchUi,
     switches::kTopChromeTouchUiAuto},
    {flags_ui::kGenericExperimentChoiceDisabled, switches::kTopChromeTouchUi,
     switches::kTopChromeTouchUiDisabled},
    {flags_ui::kGenericExperimentChoiceEnabled, switches::kTopChromeTouchUi,
     switches::kTopChromeTouchUiEnabled}};

#if BUILDFLAG(IS_CHROMEOS_ASH)

const FeatureEntry::FeatureParam
    kArcRoundedWindowCompatStrategyLeftRightBottomGesture = {
        arc::kRoundedWindowCompatStrategy,
        arc::kRoundedWindowCompatStrategy_LeftRightBottomGesture};

const FeatureEntry::FeatureParam
    kArcRoundedWindowCompatStrategyBottomOnlyGesture = {
        arc::kRoundedWindowCompatStrategy,
        arc::kRoundedWindowCompatStrategy_BottomOnlyGesture};

const FeatureEntry::FeatureVariation kArcRoundedWindowCompatVariation[] = {
    {"Left-Right-Bottom Gesture Exclusion",
     &kArcRoundedWindowCompatStrategyLeftRightBottomGesture, 1, nullptr},
    {"Bottom-only Gesture Exclusion",
     &kArcRoundedWindowCompatStrategyBottomOnlyGesture, 1, nullptr},
};

const FeatureEntry::FeatureParam kHibernate6 = {"HibernateAfterTimeHours", "6"};
const FeatureEntry::FeatureParam kHibernate8 = {"HibernateAfterTimeHours", "8"};
const FeatureEntry::FeatureParam kHibernate12 = {"HibernateAfterTimeHours",
                                                 "12"};
const FeatureEntry::FeatureParam kHibernate24 = {"HibernateAfterTimeHours",
                                                 "24"};

const FeatureEntry::FeatureVariation kHibernateFeatureVariations[] = {
    {"Hibernate after 6 hours", &kHibernate6, 1, nullptr},
    {"Hibernate after 8 hours", &kHibernate8, 1, nullptr},
    {"Hibernate after 12 hours", &kHibernate12, 1, nullptr},
    {"Hibernate after 24 hours", &kHibernate24, 1, nullptr},
};

const char kPreferDcheckInternalName[] = "prefer-dcheck";

const char kLacrosAvailabilityIgnoreInternalName[] =
    "lacros-availability-ignore";
const char kLacrosOnlyInternalName[] = "lacros-only";
const char kLacrosStabilityInternalName[] = "lacros-stability";
const char kLacrosWaylandLoggingInternalName[] = "lacros-wayland-logging";
const char kArcEnableVirtioBlkForDataInternalName[] =
    "arc-enable-virtio-blk-for-data";

const FeatureEntry::Choice kPreferDcheckChoices[] = {
    {flags_ui::kGenericExperimentChoiceDefault, "", ""},
    {component_updater::kPreferDcheckOptIn,
     component_updater::kPreferDcheckSwitch,
     component_updater::kPreferDcheckOptIn},
    {component_updater::kPreferDcheckOptOut,
     component_updater::kPreferDcheckSwitch,
     component_updater::kPreferDcheckOptOut},
};

const FeatureEntry::Choice kLacrosStabilityChoices[] = {
    {flags_ui::kGenericExperimentChoiceDefault, "", ""},
    {crosapi::browser_util::kLacrosStabilityChannelCanary,
     crosapi::browser_util::kLacrosStabilitySwitch,
     crosapi::browser_util::kLacrosStabilityChannelCanary},
    {crosapi::browser_util::kLacrosStabilityChannelDev,
     crosapi::browser_util::kLacrosStabilitySwitch,
     crosapi::browser_util::kLacrosStabilityChannelDev},
    {crosapi::browser_util::kLacrosStabilityChannelBeta,
     crosapi::browser_util::kLacrosStabilitySwitch,
     crosapi::browser_util::kLacrosStabilityChannelBeta},
    {crosapi::browser_util::kLacrosStabilityChannelStable,
     crosapi::browser_util::kLacrosStabilitySwitch,
     crosapi::browser_util::kLacrosStabilityChannelStable},
};

const char kLacrosSelectionInternalName[] = "lacros-selection";
const char kProjectorServerSideSpeechRecognition[] =
    "enable-projector-server-side-speech-recognition";

const FeatureEntry::Choice kLacrosSelectionChoices[] = {
    {flags_ui::kGenericExperimentChoiceDefault, "", ""},
    {flag_descriptions::kLacrosSelectionStatefulDescription,
     crosapi::browser_util::kLacrosSelectionSwitch,
     crosapi::browser_util::kLacrosSelectionStateful},
    {flag_descriptions::kLacrosSelectionRootfsDescription,
     crosapi::browser_util::kLacrosSelectionSwitch,
     crosapi::browser_util::kLacrosSelectionRootfs},
};

const char kLacrosSelectionPolicyIgnoreInternalName[] =
    "lacros-selection-ignore";

const FeatureEntry::Choice kLacrosAvailabilityPolicyChoices[] = {
    {flags_ui::kGenericExperimentChoiceDefault, "", ""},
    {ash::standalone_browser::kLacrosAvailabilityPolicyUserChoice,
     ash::standalone_browser::kLacrosAvailabilityPolicySwitch,
     ash::standalone_browser::kLacrosAvailabilityPolicyUserChoice},
    {ash::standalone_browser::kLacrosAvailabilityPolicyLacrosDisabled,
     ash::standalone_browser::kLacrosAvailabilityPolicySwitch,
     ash::standalone_browser::kLacrosAvailabilityPolicyLacrosDisabled},
    {ash::standalone_browser::kLacrosAvailabilityPolicyLacrosOnly,
     ash::standalone_browser::kLacrosAvailabilityPolicySwitch,
     ash::standalone_browser::kLacrosAvailabilityPolicyLacrosOnly},
};

const FeatureEntry::Choice kLacrosDataBackwardMigrationModePolicyChoices[] = {
    {flags_ui::kGenericExperimentChoiceDefault, "", ""},
    {crosapi::browser_util::kLacrosDataBackwardMigrationModePolicyNone,
     crosapi::browser_util::kLacrosDataBackwardMigrationModePolicySwitch,
     crosapi::browser_util::kLacrosDataBackwardMigrationModePolicyNone},
    {crosapi::browser_util::kLacrosDataBackwardMigrationModePolicyKeepNone,
     crosapi::browser_util::kLacrosDataBackwardMigrationModePolicySwitch,
     crosapi::browser_util::kLacrosDataBackwardMigrationModePolicyKeepNone},
    {crosapi::browser_util::kLacrosDataBackwardMigrationModePolicyKeepSafeData,
     crosapi::browser_util::kLacrosDataBackwardMigrationModePolicySwitch,
     crosapi::browser_util::kLacrosDataBackwardMigrationModePolicyKeepSafeData},
    {crosapi::browser_util::kLacrosDataBackwardMigrationModePolicyKeepAll,
     crosapi::browser_util::kLacrosDataBackwardMigrationModePolicySwitch,
     crosapi::browser_util::kLacrosDataBackwardMigrationModePolicyKeepAll},
};

#endif  // BUILDFLAG(IS_CHROMEOS_ASH)

const FeatureEntry::Choice kForceUIDirectionChoices[] = {
    {flags_ui::kGenericExperimentChoiceDefault, "", ""},
    {flag_descriptions::kForceDirectionLtr, switches::kForceUIDirection,
     switches::kForceDirectionLTR},
    {flag_descriptions::kForceDirectionRtl, switches::kForceUIDirection,
     switches::kForceDirectionRTL},
};

const FeatureEntry::Choice kForceTextDirectionChoices[] = {
    {flags_ui::kGenericExperimentChoiceDefault, "", ""},
    {flag_descriptions::kForceDirectionLtr, switches::kForceTextDirection,
     switches::kForceDirectionLTR},
    {flag_descriptions::kForceDirectionRtl, switches::kForceTextDirection,
     switches::kForceDirectionRTL},
};

const FeatureEntry::Choice kIpProtectionProxyOptOutChoices[] = {
    {flag_descriptions::kIpProtectionProxyOptOutChoiceDefault, "", ""},
    {flag_descriptions::kIpProtectionProxyOptOutChoiceOptOut,
     switches::kDisableIpProtectionProxy, ""},
};

#if BUILDFLAG(IS_CHROMEOS_ASH)
const FeatureEntry::Choice kSchedulerConfigurationChoices[] = {
    {flags_ui::kGenericExperimentChoiceDefault, "", ""},
    {flag_descriptions::kSchedulerConfigurationConservative,
     switches::kSchedulerConfiguration,
     switches::kSchedulerConfigurationConservative},
    {flag_descriptions::kSchedulerConfigurationPerformance,
     switches::kSchedulerConfiguration,
     switches::kSchedulerConfigurationPerformance},
};
#endif  // BUILDFLAG(IS_CHROMEOS_ASH)

#if BUILDFLAG(IS_CHROMEOS_ASH)
const FeatureEntry::FeatureParam kDynamicSearchUpdateAnimationDuration_50[] = {
    {"search_result_translation_duration", "50"}};
const FeatureEntry::FeatureParam kDynamicSearchUpdateAnimationDuration_100[] = {
    {"search_result_translation_duration", "100"}};
const FeatureEntry::FeatureParam kDynamicSearchUpdateAnimationDuration_150[] = {
    {"search_result_translation_duration", "150"}};

const FeatureEntry::FeatureVariation kDynamicSearchUpdateAnimationVariations[] =
    {{"50ms", kDynamicSearchUpdateAnimationDuration_50,
      std::size(kDynamicSearchUpdateAnimationDuration_50), nullptr},
     {"100ms", kDynamicSearchUpdateAnimationDuration_100,
      std::size(kDynamicSearchUpdateAnimationDuration_100), nullptr},
     {"150ms", kDynamicSearchUpdateAnimationDuration_150,
      std::size(kDynamicSearchUpdateAnimationDuration_150), nullptr}};
#endif  // BUILDFLAG(IS_CHROMEOS_ASH)

#if BUILDFLAG(ENABLE_NACL)
// Note: This needs to be kept in sync with parsing in
// content/common/zygote/zygote_communication_linux.cc
const FeatureEntry::Choice kVerboseLoggingInNaclChoices[] = {
    {flag_descriptions::kVerboseLoggingInNaclChoiceDefault, "", ""},
    {flag_descriptions::kVerboseLoggingInNaclChoiceLow,
     switches::kVerboseLoggingInNacl, switches::kVerboseLoggingInNaclChoiceLow},
    {flag_descriptions::kVerboseLoggingInNaclChoiceMedium,
     switches::kVerboseLoggingInNacl,
     switches::kVerboseLoggingInNaclChoiceMedium},
    {flag_descriptions::kVerboseLoggingInNaclChoiceHigh,
     switches::kVerboseLoggingInNacl,
     switches::kVerboseLoggingInNaclChoiceHigh},
    {flag_descriptions::kVerboseLoggingInNaclChoiceHighest,
     switches::kVerboseLoggingInNacl,
     switches::kVerboseLoggingInNaclChoiceHighest},
    {flag_descriptions::kVerboseLoggingInNaclChoiceDisabled,
     switches::kVerboseLoggingInNacl,
     switches::kVerboseLoggingInNaclChoiceDisabled},
};
#endif  // ENABLE_NACL

const FeatureEntry::Choice kSiteIsolationOptOutChoices[] = {
    {flag_descriptions::kSiteIsolationOptOutChoiceDefault, "", ""},
    {flag_descriptions::kSiteIsolationOptOutChoiceOptOut,
     switches::kDisableSiteIsolation, ""},
};

const FeatureEntry::Choice kForceColorProfileChoices[] = {
    {flags_ui::kGenericExperimentChoiceDefault, "", ""},
    {flag_descriptions::kForceColorProfileSRGB,
     switches::kForceDisplayColorProfile, "srgb"},
    {flag_descriptions::kForceColorProfileP3,
     switches::kForceDisplayColorProfile, "display-p3-d65"},
    {flag_descriptions::kForceColorProfileRec2020,
     switches::kForceDisplayColorProfile, "rec2020"},
    {flag_descriptions::kForceColorProfileColorSpin,
     switches::kForceDisplayColorProfile, "color-spin-gamma24"},
    {flag_descriptions::kForceColorProfileSCRGBLinear,
     switches::kForceDisplayColorProfile, "scrgb-linear"},
    {flag_descriptions::kForceColorProfileHDR10,
     switches::kForceDisplayColorProfile, "hdr10"},
};

const FeatureEntry::Choice kMemlogModeChoices[] = {
    {flags_ui::kGenericExperimentChoiceDisabled, "", ""},
    {flag_descriptions::kMemlogModeMinimal, heap_profiling::kMemlogMode,
     heap_profiling::kMemlogModeMinimal},
    {flag_descriptions::kMemlogModeAll, heap_profiling::kMemlogMode,
     heap_profiling::kMemlogModeAll},
    {flag_descriptions::kMemlogModeBrowser, heap_profiling::kMemlogMode,
     heap_profiling::kMemlogModeBrowser},
    {flag_descriptions::kMemlogModeGpu, heap_profiling::kMemlogMode,
     heap_profiling::kMemlogModeGpu},
    {flag_descriptions::kMemlogModeAllRenderers, heap_profiling::kMemlogMode,
     heap_profiling::kMemlogModeAllRenderers},
    {flag_descriptions::kMemlogModeRendererSampling,
     heap_profiling::kMemlogMode, heap_profiling::kMemlogModeRendererSampling},
};

const FeatureEntry::Choice kMemlogStackModeChoices[] = {
    {flags_ui::kGenericExperimentChoiceDefault, "", ""},
    {flag_descriptions::kMemlogStackModeNative,
     heap_profiling::kMemlogStackMode, heap_profiling::kMemlogStackModeNative},
    {flag_descriptions::kMemlogStackModeNativeWithThreadNames,
     heap_profiling::kMemlogStackMode,
     heap_profiling::kMemlogStackModeNativeWithThreadNames},
};

const FeatureEntry::Choice kMemlogSamplingRateChoices[] = {
    {flags_ui::kGenericExperimentChoiceDefault, "", ""},
    {flag_descriptions::kMemlogSamplingRate10KB,
     heap_profiling::kMemlogSamplingRate,
     heap_profiling::kMemlogSamplingRate10KB},
    {flag_descriptions::kMemlogSamplingRate50KB,
     heap_profiling::kMemlogSamplingRate,
     heap_profiling::kMemlogSamplingRate50KB},
    {flag_descriptions::kMemlogSamplingRate100KB,
     heap_profiling::kMemlogSamplingRate,
     heap_profiling::kMemlogSamplingRate100KB},
    {flag_descriptions::kMemlogSamplingRate500KB,
     heap_profiling::kMemlogSamplingRate,
     heap_profiling::kMemlogSamplingRate500KB},
    {flag_descriptions::kMemlogSamplingRate1MB,
     heap_profiling::kMemlogSamplingRate,
     heap_profiling::kMemlogSamplingRate1MB},
    {flag_descriptions::kMemlogSamplingRate5MB,
     heap_profiling::kMemlogSamplingRate,
     heap_profiling::kMemlogSamplingRate5MB},
};
const FeatureEntry::FeatureParam kPageContentAnnotationsContentParams[] = {
    {"annotate_title_instead_of_page_content", "false"},
    {"extract_related_searches", "true"},
    {"max_size_for_text_dump_in_bytes", "5120"},
    {"write_to_history_service", "true"},
};
const FeatureEntry::FeatureParam kPageContentAnnotationsTitleParams[] = {
    {"annotate_title_instead_of_page_content", "true"},
    {"extract_related_searches", "true"},
    {"write_to_history_service", "true"},
};
const FeatureEntry::FeatureVariation kPageContentAnnotationsVariations[] = {
    {"All Annotations and Persistence on Content",
     kPageContentAnnotationsContentParams,
     std::size(kPageContentAnnotationsContentParams), nullptr},
    {"All Annotations and Persistence on Title",
     kPageContentAnnotationsTitleParams,
     std::size(kPageContentAnnotationsTitleParams), nullptr},
};
const FeatureEntry::FeatureParam kJourneysShowAllVisitsParams[] = {
    {"JourneysLocaleOrLanguageAllowlist", "*"},
    // To show all visits, set the number of visits above the fold to a very
    // high number.
    {"JourneysNumVisitsToAlwaysShowAboveTheFold", "200"},
};
const FeatureEntry::FeatureParam kJourneysAllLocalesParams[] = {
    {"JourneysLocaleOrLanguageAllowlist", "*"},
};
const FeatureEntry::FeatureVariation kJourneysVariations[] = {
    {"No 'Show More' - Show all visits", kJourneysShowAllVisitsParams,
     std::size(kJourneysShowAllVisitsParams), nullptr},
    {"All Supported Locales", kJourneysAllLocalesParams,
     std::size(kJourneysAllLocalesParams), nullptr},
};

const FeatureEntry::FeatureVariation
    kImageServiceOptimizationGuideSalientImagesVariations[] = {
        {"High Performance Canonicalization", nullptr, 0, "3362133"},
};

const FeatureEntry::FeatureParam kSidePanelJourneysOpensFromOmniboxParams[] = {
    {"SidePanelJourneysOpensFromOmnibox", "true"},
};
const FeatureEntry::FeatureVariation
    kSidePanelJourneysOpensFromOmniboxVariations[] = {
        {"Omnibox opens Side Panel Journeys",
         kSidePanelJourneysOpensFromOmniboxParams,
         std::size(kSidePanelJourneysOpensFromOmniboxParams), nullptr},
};

const FeatureEntry::FeatureParam
    kJourneysLabelsWithSearchVisitEntitiesParams[] = {
        {"labels_from_search_visit_entities", "true"},
};
const FeatureEntry::FeatureParam kJourneysLabelsWithEntitiesParams[] = {
    {"labels_from_entities", "true"},
};
const FeatureEntry::FeatureParam
    kJourneysLabelsWithEntitiesNoHostnamesParams[] = {
        {"labels_from_hostnames", "false"},
        {"labels_from_entities", "true"},
};
const FeatureEntry::FeatureVariation kJourneysLabelsVariations[] = {
    {"With Entities", kJourneysLabelsWithEntitiesParams,
     std::size(kJourneysLabelsWithEntitiesParams), nullptr},
    {"With Entities, No Hostnames",
     kJourneysLabelsWithEntitiesNoHostnamesParams,
     std::size(kJourneysLabelsWithEntitiesNoHostnamesParams), nullptr},
    {"With Search Entities", kJourneysLabelsWithSearchVisitEntitiesParams,
     std::size(kJourneysLabelsWithSearchVisitEntitiesParams), nullptr},
};

const FeatureEntry::FeatureParam
    kOmniboxCompanyEntityIconAdjustmentLeastAggressive[] = {
        {"OmniboxCompanyEntityAdjustmentGroup", "least-aggressive"}};
const FeatureEntry::FeatureParam kOmniboxCompanyEntityIconAdjustmentModerate[] =
    {{"OmniboxCompanyEntityAdjustmentGroup", "moderate"}};
const FeatureEntry::FeatureParam
    kOmniboxCompanyEntityIconAdjustmentMostAggressive[] = {
        {"OmniboxCompanyEntityAdjustmentGroup", "most-aggressive"}};

const FeatureEntry::FeatureVariation
    kOmniboxCompanyEntityIconAdjustmentVariations[] = {
        {"Least Aggressive", kOmniboxCompanyEntityIconAdjustmentLeastAggressive,
         std::size(kOmniboxCompanyEntityIconAdjustmentLeastAggressive),
         nullptr},
        {"Moderate", kOmniboxCompanyEntityIconAdjustmentModerate,
         std::size(kOmniboxCompanyEntityIconAdjustmentModerate), nullptr},
        {"Most Aggressive", kOmniboxCompanyEntityIconAdjustmentMostAggressive,
         std::size(kOmniboxCompanyEntityIconAdjustmentMostAggressive), nullptr},
};

const FeatureEntry::FeatureParam kChromeRefresh2023Level1[] = {{"level", "1"}};

// "Enabled" is equivalent to "Enabled with Omnibox", therefore we don't need to
// make a separate variation for it.
const FeatureEntry::FeatureVariation kChromeRefresh2023Variations[] = {
    {"without Omnibox", kChromeRefresh2023Level1,
     std::size(kChromeRefresh2023Level1), nullptr}};

const FeatureEntry::FeatureParam kChromeRefresh2023NTBVariationGM2Full[] = {
    {features::kChromeRefresh2023NTBVariationKey, "GM2Full"}};
const FeatureEntry::FeatureParam
    kChromeRefresh2023NTBVariationGM3OldIconNoBackground[] = {
        {features::kChromeRefresh2023NTBVariationKey,
         "GM3OldIconNoBackground"}};
const FeatureEntry::FeatureParam
    kChromeRefresh2023NTBVariationGM3OldIconWithBackground[] = {
        {features::kChromeRefresh2023NTBVariationKey,
         "GM3OldIconWithBackground"}};
const FeatureEntry::FeatureParam
    kChromeRefresh2023NTBVariationGM3NewIconNoBackground[] = {
        {features::kChromeRefresh2023NTBVariationKey,
         "GM3NewIconNoBackground"}};
const FeatureEntry::FeatureParam
    kChromeRefresh2023NTBVariationGM3NewIconWithBackground[] = {
        {features::kChromeRefresh2023NTBVariationKey,
         "GM3NewIconWithBackground"}};

const FeatureEntry::FeatureVariation kChromeRefresh2023NTBVariations[] = {
    {"GM2 Implementation", kChromeRefresh2023NTBVariationGM2Full,
     std::size(kChromeRefresh2023NTBVariationGM2Full), nullptr},
    {"GM3 Old Icon No Background",
     kChromeRefresh2023NTBVariationGM3OldIconNoBackground,
     std::size(kChromeRefresh2023NTBVariationGM3OldIconNoBackground), nullptr},
    {"GM3 Old Icon With Background",
     kChromeRefresh2023NTBVariationGM3OldIconWithBackground,
     std::size(kChromeRefresh2023NTBVariationGM3OldIconWithBackground),
     nullptr},
    {"GM3 New Icon No Background",
     kChromeRefresh2023NTBVariationGM3NewIconNoBackground,
     std::size(kChromeRefresh2023NTBVariationGM3NewIconNoBackground), nullptr},
    {"GM3 New Icon With Background",
     kChromeRefresh2023NTBVariationGM3NewIconWithBackground,
     std::size(kChromeRefresh2023NTBVariationGM3NewIconWithBackground),
     nullptr}};

#if BUILDFLAG(IS_LINUX) || BUILDFLAG(IS_CHROMEOS) || BUILDFLAG(IS_MAC) || \
    BUILDFLAG(IS_WIN) || BUILDFLAG(IS_FUCHSIA)
const FeatureEntry::FeatureParam kShortcutBoostSingleUrl[] = {
    {"ShortcutBoostSearchScore", "0"},
    {"ShortcutBoostNonTopHitThreshold", "0"},
    {"ShortcutBoostGroupWithSearches", "false"},
};
const FeatureEntry::FeatureParam kShortcutBoostMultipleUrls[] = {
    {"ShortcutBoostSearchScore", "0"},
    {"ShortcutBoostNonTopHitThreshold", "2"},
    {"ShortcutBoostGroupWithSearches", "true"},
};
const FeatureEntry::FeatureParam kShortcutBoostMultipleSearchesAndUrls[] = {
    {"ShortcutBoostSearchScore", "1414"},
    {"ShortcutBoostNonTopHitThreshold", "2"},
    {"ShortcutBoostNonTopHitSearchThreshold", "3"},
    {"ShortcutBoostGroupWithSearches", "true"},
};

const FeatureEntry::FeatureVariation kOmniboxShortcutBoostVariations[] = {
    {"Single URL", kShortcutBoostSingleUrl, std::size(kShortcutBoostSingleUrl),
     nullptr},
    {"Multiple URLs", kShortcutBoostMultipleUrls,
     std::size(kShortcutBoostMultipleUrls), nullptr},
    {"Multiple Searches and URLs", kShortcutBoostMultipleSearchesAndUrls,
     std::size(kShortcutBoostMultipleSearchesAndUrls), nullptr},
};

// A limited number of combinations of the rich autocompletion params.
const FeatureEntry::FeatureParam
    kOmniboxRichAutocompletionConservativeModerate[] = {
        {"RichAutocompletionAutocompleteTitles", "true"},
        {"RichAutocompletionAutocompleteNonPrefixShortcutProvider", "true"},
        {"RichAutocompletionAutocompleteTitlesMinChar", "3"},
        {"RichAutocompletionAutocompleteNonPrefixMinChar", "5"}};
const FeatureEntry::FeatureParam
    kOmniboxRichAutocompletionConservativeModerate2[] = {
        {"RichAutocompletionAutocompleteTitlesShortcutProvider", "true"},
        {"RichAutocompletionAutocompleteNonPrefixShortcutProvider", "true"},
        {"RichAutocompletionAutocompleteTitlesMinChar", "3"},
        {"RichAutocompletionAutocompleteNonPrefixMinChar", "5"}};
const FeatureEntry::FeatureParam kOmniboxRichAutocompletionAggressive2[] = {
    {"RichAutocompletionAutocompleteTitlesShortcutProvider", "true"},
    {"RichAutocompletionAutocompleteTitlesMinChar", "2"},
    {"RichAutocompletionAutocompleteShortcutText", "true"},
    {"RichAutocompletionAutocompleteShortcutTextMinChar", "2"}};
const FeatureEntry::FeatureParam kOmniboxRichAutocompletionAggressive3[] = {
    {"RichAutocompletionAutocompleteTitlesShortcutProvider", "true"},
    {"RichAutocompletionAutocompleteTitlesMinChar", "3"},
    {"RichAutocompletionAutocompleteShortcutText", "true"},
    {"RichAutocompletionAutocompleteShortcutTextMinChar", "3"}};
const FeatureEntry::FeatureParam kOmniboxRichAutocompletionAggressive4[] = {
    {"RichAutocompletionAutocompleteTitlesShortcutProvider", "true"},
    {"RichAutocompletionAutocompleteTitlesMinChar", "4"},
    {"RichAutocompletionAutocompleteShortcutText", "true"},
    {"RichAutocompletionAutocompleteShortcutTextMinChar", "4"}};

const FeatureEntry::FeatureVariation
    kOmniboxRichAutocompletionPromisingVariations[] = {
        {"Conservative Moderate - Title, Shortcut Non-Prefix, min 3/5",
         kOmniboxRichAutocompletionConservativeModerate,
         std::size(kOmniboxRichAutocompletionConservativeModerate), nullptr},
        {"Conservative Moderate 2 - Shortcut Title, Shortcut Non-Prefix, min "
         "3/5",
         kOmniboxRichAutocompletionConservativeModerate2,
         std::size(kOmniboxRichAutocompletionConservativeModerate2), nullptr},
        {"Aggressive 2 - Title Shortcut Title 2, Shortcut Text 2",
         kOmniboxRichAutocompletionAggressive2,
         std::size(kOmniboxRichAutocompletionAggressive2), nullptr},
        {"Aggressive 3 - Title Shortcut Title 3, Shortcut Text 3",
         kOmniboxRichAutocompletionAggressive3,
         std::size(kOmniboxRichAutocompletionAggressive3), nullptr},
        {"Aggressive 4 - Title Shortcut Title 4, Shortcut Text 4",
         kOmniboxRichAutocompletionAggressive4,
         std::size(kOmniboxRichAutocompletionAggressive4), nullptr},
};

const FeatureEntry::FeatureParam kOmniboxMlUrlScoringRerankFinalMatchesOnly[] =
    {
        {"MlUrlScoringRerankFinalMatchesOnly", "true"},
        {"enable_scoring_signals_annotators_for_ml_scoring", "true"},
        {"MlUrlScoringShortcutDocumentSignals", "true"},
};
const FeatureEntry::FeatureParam kOmniboxMlUrlScoringPreserveDefault[] = {
    {"MlUrlScoringRerankFinalMatchesOnly", "true"},
    {"MlUrlScoringPreserveDefault", "true"},
    {"enable_scoring_signals_annotators_for_ml_scoring", "true"},
    {"MlUrlScoringShortcutDocumentSignals", "true"},
};
const FeatureEntry::FeatureParam kOmniboxMlUrlScoringUnlimitedNumCandidates[] =
    {
        {"MlUrlScoringUnlimitedNumCandidates", "true"},
        {"enable_scoring_signals_annotators_for_ml_scoring", "true"},
        {"MlUrlScoringShortcutDocumentSignals", "true"},
};
// Sets Bookmark(1), History Quick(4), History URL(8), Shortcuts(64), and
// History Fuzzy(65536) providers max matches to 10.
const FeatureEntry::FeatureParam kOmniboxMlUrlScoringMaxMatchesByProvider10[] =
    {
        {"MlUrlScoringMaxMatchesByProvider", "1:10,4:10,8:10,64:10,65536:10"},
        {"enable_scoring_signals_annotators_for_ml_scoring", "true"},
        {"MlUrlScoringShortcutDocumentSignals", "true"},
};

const FeatureEntry::FeatureVariation kOmniboxMlUrlScoringVariations[] = {
    {"final matches only", kOmniboxMlUrlScoringRerankFinalMatchesOnly,
     std::size(kOmniboxMlUrlScoringRerankFinalMatchesOnly), nullptr},
    {"final matches only, preserve legacy default match",
     kOmniboxMlUrlScoringPreserveDefault,
     std::size(kOmniboxMlUrlScoringPreserveDefault), nullptr},
    {"unlimited suggestion candidates",
     kOmniboxMlUrlScoringUnlimitedNumCandidates,
     std::size(kOmniboxMlUrlScoringUnlimitedNumCandidates), nullptr},
    {"Increase provider max limit to 10",
     kOmniboxMlUrlScoringMaxMatchesByProvider10,
     std::size(kOmniboxMlUrlScoringMaxMatchesByProvider10), nullptr},
};

const FeatureEntry::FeatureParam kRealboxTwoPreviousSearchRelatedSuggestions[] =
    {
        {"RealboxMaxPreviousSearchRelatedSuggestions", "2"},
};
const FeatureEntry::FeatureParam kRealboxSecondaryZeroSuggestCounterfactual[] =
    {
        {"RealboxSecondaryZeroSuggestCounterfactual", "true"},
};

const FeatureEntry::FeatureVariation kRealboxSecondaryZeroSuggestVariations[] =
    {{"2 secondary suggestions (default is 3)",
      kRealboxTwoPreviousSearchRelatedSuggestions,
      std::size(kRealboxTwoPreviousSearchRelatedSuggestions), nullptr},
     {"counterfactual (don't show secondary suggestions)",
      kRealboxSecondaryZeroSuggestCounterfactual,
      std::size(kRealboxSecondaryZeroSuggestCounterfactual), nullptr}};

const FeatureEntry::FeatureParam kActionsUISimplificationTrimExtraFalse[]{
    {"ActionsUISimplificationTrimExtra", "false"},
};
const FeatureEntry::FeatureVariation
    kOmniboxActionsUISimplificationVariations[] = {
        {"with TrimExtra false", kActionsUISimplificationTrimExtraFalse,
         std::size(kActionsUISimplificationTrimExtraFalse), nullptr},
};

const FeatureEntry::FeatureParam
    kOmniboxDriveSuggestionsIgnoreWhenDebouncing[] = {
        {"DocumentProviderIgnoreWhenDebouncing", "true"}};
const FeatureEntry::FeatureVariation kOmniboxDriveSuggestionsVariations[] = {
    {"ignore when debouncing", kOmniboxDriveSuggestionsIgnoreWhenDebouncing,
     std::size(kOmniboxDriveSuggestionsIgnoreWhenDebouncing), nullptr}};

#endif  // BUILDFLAG(IS_LINUX) || BUILDFLAG(IS_CHROMEOS) || BUILDFLAG(IS_MAC) ||
        // BUILDFLAG(IS_WIN) || BUILDFLAG(IS_FUCHSIA)

#if BUILDFLAG(IS_ANDROID)
constexpr FeatureEntry::FeatureParam kOmniboxActionsInSuggestTreatment1[] = {
    {OmniboxFieldTrial::kActionsInSuggestPromoteEntitySuggestion.name, "true"},
    {OmniboxFieldTrial::kActionsInSuggestPromoteReviewsAction.name, "true"},
    {OmniboxFieldTrial::kActionsInSuggestRemoveActionTypes.name, ""}};
constexpr FeatureEntry::FeatureParam kOmniboxActionsInSuggestTreatment2[] = {
    {OmniboxFieldTrial::kActionsInSuggestPromoteEntitySuggestion.name, "false"},
    {OmniboxFieldTrial::kActionsInSuggestPromoteReviewsAction.name, "true"},
    {OmniboxFieldTrial::kActionsInSuggestRemoveActionTypes.name, ""}};
constexpr FeatureEntry::FeatureParam kOmniboxActionsInSuggestTreatment3[] = {
    {OmniboxFieldTrial::kActionsInSuggestPromoteEntitySuggestion.name, "true"},
    {OmniboxFieldTrial::kActionsInSuggestPromoteReviewsAction.name, "false"},
    {OmniboxFieldTrial::kActionsInSuggestRemoveActionTypes.name, ""}};
constexpr FeatureEntry::FeatureParam kOmniboxActionsInSuggestTreatment4[] = {
    {OmniboxFieldTrial::kActionsInSuggestPromoteEntitySuggestion.name, "true"},
    {OmniboxFieldTrial::kActionsInSuggestPromoteReviewsAction.name, "true"},
    {OmniboxFieldTrial::kActionsInSuggestRemoveActionTypes.name, "reviews"}};
constexpr FeatureEntry::FeatureParam kOmniboxActionsInSuggestTreatment5[] = {
    {OmniboxFieldTrial::kActionsInSuggestPromoteEntitySuggestion.name, "true"},
    {OmniboxFieldTrial::kActionsInSuggestPromoteReviewsAction.name, "true"},
    {OmniboxFieldTrial::kActionsInSuggestRemoveActionTypes.name, "call"}};
constexpr FeatureEntry::FeatureParam kOmniboxActionsInSuggestTreatment6[] = {
    {OmniboxFieldTrial::kActionsInSuggestPromoteEntitySuggestion.name, "true"},
    {OmniboxFieldTrial::kActionsInSuggestPromoteReviewsAction.name, "true"},
    {OmniboxFieldTrial::kActionsInSuggestRemoveActionTypes.name, "directions"}};
constexpr FeatureEntry::FeatureParam kOmniboxActionsInSuggestTreatment7[] = {
    {OmniboxFieldTrial::kActionsInSuggestPromoteEntitySuggestion.name, "true"},
    {OmniboxFieldTrial::kActionsInSuggestPromoteReviewsAction.name, "false"},
    {OmniboxFieldTrial::kActionsInSuggestRemoveActionTypes.name, "call"}};
constexpr FeatureEntry::FeatureParam kOmniboxActionsInSuggestTreatment8[] = {
    {OmniboxFieldTrial::kActionsInSuggestPromoteEntitySuggestion.name, "false"},
    {OmniboxFieldTrial::kActionsInSuggestPromoteReviewsAction.name, "false"},
    {OmniboxFieldTrial::kActionsInSuggestRemoveActionTypes.name, "call"}};

constexpr FeatureEntry::FeatureVariation kOmniboxActionsInSuggestVariants[] = {
    {"T1: Promote, Reviews, Directions, Calls",
     kOmniboxActionsInSuggestTreatment1,
     std::size(kOmniboxActionsInSuggestTreatment1), "t3366528"},
    {"T2: Reviews, Directions, Calls", kOmniboxActionsInSuggestTreatment2,
     std::size(kOmniboxActionsInSuggestTreatment2), "t3366528"},
    {"T3: Promote, Calls, Directions, Reviews",
     kOmniboxActionsInSuggestTreatment3,
     std::size(kOmniboxActionsInSuggestTreatment3), "t3366528"},
    {"T4: Promote, Directions, Calls", kOmniboxActionsInSuggestTreatment4,
     std::size(kOmniboxActionsInSuggestTreatment4), "t3366528"},
    {"T5: Promote, Reviews, Directions", kOmniboxActionsInSuggestTreatment5,
     std::size(kOmniboxActionsInSuggestTreatment5), "t3366528"},
    {"T6: Promote, Reviews, Calls", kOmniboxActionsInSuggestTreatment6,
     std::size(kOmniboxActionsInSuggestTreatment6), "t3366528"},
    {"T7: Promote, Directions, Reviews", kOmniboxActionsInSuggestTreatment7,
     std::size(kOmniboxActionsInSuggestTreatment7), "t3366528"},
    {"T8: Directions, Reviews", kOmniboxActionsInSuggestTreatment8,
     std::size(kOmniboxActionsInSuggestTreatment8), "t3366528"},
};

constexpr FeatureEntry::FeatureParam kOmniboxQueryTilesShowAboveTrends[] = {
    {OmniboxFieldTrial::kQueryTilesShowAboveTrends.name, "true"}};
constexpr FeatureEntry::FeatureParam kOmniboxQueryTilesShowBelowTrends[] = {
    {OmniboxFieldTrial::kQueryTilesShowAboveTrends.name, "false"}};

constexpr FeatureEntry::FeatureVariation kOmniboxQueryTilesVariations[] = {
    {"Above Trends", kOmniboxQueryTilesShowAboveTrends,
     std::size(kOmniboxQueryTilesShowAboveTrends), nullptr},
    {"Below Trends", kOmniboxQueryTilesShowBelowTrends,
     std::size(kOmniboxQueryTilesShowBelowTrends), nullptr},
};
#endif  // BUILDFLAG(IS_ANDROID)

const FeatureEntry::FeatureParam kOmniboxSquareSuggestionIconFavicons[] = {
    {"OmniboxSquareSuggestIconIcons", "true"}};
const FeatureEntry::FeatureParam kOmniboxSquareSuggestionIconAnswers[] = {
    {"OmniboxSquareSuggestIconAnswers", "true"}};
const FeatureEntry::FeatureParam
    kOmniboxSquareSuggestionIconFaviconsAndAnswers[] = {
        {"OmniboxSquareSuggestIconIcons", "true"},
        {"OmniboxSquareSuggestIconAnswers", "true"},
};
const FeatureEntry::FeatureParam kOmniboxSquareSuggestionIconEntities[] = {
    {"OmniboxSquareSuggestIconEntities", "true"}};
const FeatureEntry::FeatureParam kOmniboxSquareSuggestionIconWeather[] = {
    {"OmniboxSquareSuggestIconWeather", "true"}};
const FeatureEntry::FeatureParam kOmniboxSquareSuggestionIconAll[] = {
    {"OmniboxSquareSuggestIconIcons", "true"},
    {"OmniboxSquareSuggestIconAnswers", "true"},
    {"OmniboxSquareSuggestIconEntities", "true"},
    {"OmniboxSquareSuggestIconWeather", "true"},
};
const FeatureEntry::FeatureParam kOmniboxSquareSuggestionIconAllFullEntity[] = {
    {"OmniboxSquareSuggestIconIcons", "true"},
    {"OmniboxSquareSuggestIconAnswers", "true"},
    {"OmniboxSquareSuggestIconEntities", "true"},
    {"OmniboxSquareSuggestIconEntitiesScale", "1"},
    {"OmniboxSquareSuggestIconWeather", "true"},
};

const FeatureEntry::FeatureVariation kOmniboxSquareSuggestionIconVariations[] =
    {
        {"Favicons", kOmniboxSquareSuggestionIconFavicons,
         std::size(kOmniboxSquareSuggestionIconFavicons), nullptr},
        {"Answers", kOmniboxSquareSuggestionIconAnswers,
         std::size(kOmniboxSquareSuggestionIconAnswers), nullptr},
        {"Favicons and answers", kOmniboxSquareSuggestionIconFaviconsAndAnswers,
         std::size(kOmniboxSquareSuggestionIconFaviconsAndAnswers), nullptr},
        {"Entities", kOmniboxSquareSuggestionIconEntities,
         std::size(kOmniboxSquareSuggestionIconEntities), nullptr},
        {"Weather", kOmniboxSquareSuggestionIconWeather,
         std::size(kOmniboxSquareSuggestionIconWeather), nullptr},
        {"All", kOmniboxSquareSuggestionIconAll,
         std::size(kOmniboxSquareSuggestionIconAll), nullptr},
        {"All with full entities", kOmniboxSquareSuggestionIconAllFullEntity,
         std::size(kOmniboxSquareSuggestionIconAllFullEntity), nullptr},
};

const FeatureEntry::FeatureParam kMaxZeroSuggestMatches5[] = {
    {"MaxZeroSuggestMatches", "5"}};
const FeatureEntry::FeatureParam kMaxZeroSuggestMatches6[] = {
    {"MaxZeroSuggestMatches", "6"}};
const FeatureEntry::FeatureParam kMaxZeroSuggestMatches7[] = {
    {"MaxZeroSuggestMatches", "7"}};
const FeatureEntry::FeatureParam kMaxZeroSuggestMatches8[] = {
    {"MaxZeroSuggestMatches", "8"}};
const FeatureEntry::FeatureParam kMaxZeroSuggestMatches9[] = {
    {"MaxZeroSuggestMatches", "9"}};
const FeatureEntry::FeatureParam kMaxZeroSuggestMatches10[] = {
    {"MaxZeroSuggestMatches", "10"}};
const FeatureEntry::FeatureParam kMaxZeroSuggestMatches11[] = {
    {"MaxZeroSuggestMatches", "11"}};
const FeatureEntry::FeatureParam kMaxZeroSuggestMatches12[] = {
    {"MaxZeroSuggestMatches", "12"}};
const FeatureEntry::FeatureParam kMaxZeroSuggestMatches13[] = {
    {"MaxZeroSuggestMatches", "13"}};
const FeatureEntry::FeatureParam kMaxZeroSuggestMatches14[] = {
    {"MaxZeroSuggestMatches", "14"}};
const FeatureEntry::FeatureParam kMaxZeroSuggestMatches15[] = {
    {"MaxZeroSuggestMatches", "15"}};

const FeatureEntry::FeatureVariation kMaxZeroSuggestMatchesVariations[] = {
    {"5", kMaxZeroSuggestMatches5, std::size(kMaxZeroSuggestMatches5), nullptr},
    {"6", kMaxZeroSuggestMatches6, std::size(kMaxZeroSuggestMatches6), nullptr},
    {"7", kMaxZeroSuggestMatches7, std::size(kMaxZeroSuggestMatches7), nullptr},
    {"8", kMaxZeroSuggestMatches8, std::size(kMaxZeroSuggestMatches8), nullptr},
    {"9", kMaxZeroSuggestMatches9, std::size(kMaxZeroSuggestMatches9), nullptr},
    {"10", kMaxZeroSuggestMatches10, std::size(kMaxZeroSuggestMatches10),
     nullptr},
    {"11", kMaxZeroSuggestMatches11, std::size(kMaxZeroSuggestMatches11),
     nullptr},
    {"12", kMaxZeroSuggestMatches12, std::size(kMaxZeroSuggestMatches12),
     nullptr},
    {"13", kMaxZeroSuggestMatches13, std::size(kMaxZeroSuggestMatches13),
     nullptr},
    {"14", kMaxZeroSuggestMatches14, std::size(kMaxZeroSuggestMatches14),
     nullptr},
    {"15", kMaxZeroSuggestMatches15, std::size(kMaxZeroSuggestMatches15),
     nullptr}};

const FeatureEntry::FeatureParam kOmniboxUIMaxAutocompleteMatches3[] = {
    {OmniboxFieldTrial::kUIMaxAutocompleteMatchesParam, "3"}};
const FeatureEntry::FeatureParam kOmniboxUIMaxAutocompleteMatches4[] = {
    {OmniboxFieldTrial::kUIMaxAutocompleteMatchesParam, "4"}};
const FeatureEntry::FeatureParam kOmniboxUIMaxAutocompleteMatches5[] = {
    {OmniboxFieldTrial::kUIMaxAutocompleteMatchesParam, "5"}};
const FeatureEntry::FeatureParam kOmniboxUIMaxAutocompleteMatches6[] = {
    {OmniboxFieldTrial::kUIMaxAutocompleteMatchesParam, "6"}};
const FeatureEntry::FeatureParam kOmniboxUIMaxAutocompleteMatches7[] = {
    {OmniboxFieldTrial::kUIMaxAutocompleteMatchesParam, "7"}};
const FeatureEntry::FeatureParam kOmniboxUIMaxAutocompleteMatches8[] = {
    {OmniboxFieldTrial::kUIMaxAutocompleteMatchesParam, "8"}};
const FeatureEntry::FeatureParam kOmniboxUIMaxAutocompleteMatches9[] = {
    {OmniboxFieldTrial::kUIMaxAutocompleteMatchesParam, "9"}};
const FeatureEntry::FeatureParam kOmniboxUIMaxAutocompleteMatches10[] = {
    {OmniboxFieldTrial::kUIMaxAutocompleteMatchesParam, "10"}};
const FeatureEntry::FeatureParam kOmniboxUIMaxAutocompleteMatches12[] = {
    {OmniboxFieldTrial::kUIMaxAutocompleteMatchesParam, "12"}};

const FeatureEntry::FeatureVariation
    kOmniboxUIMaxAutocompleteMatchesVariations[] = {
        {"3 matches", kOmniboxUIMaxAutocompleteMatches3,
         std::size(kOmniboxUIMaxAutocompleteMatches3), nullptr},
        {"4 matches", kOmniboxUIMaxAutocompleteMatches4,
         std::size(kOmniboxUIMaxAutocompleteMatches4), nullptr},
        {"5 matches", kOmniboxUIMaxAutocompleteMatches5,
         std::size(kOmniboxUIMaxAutocompleteMatches5), nullptr},
        {"6 matches", kOmniboxUIMaxAutocompleteMatches6,
         std::size(kOmniboxUIMaxAutocompleteMatches6), nullptr},
        {"7 matches", kOmniboxUIMaxAutocompleteMatches7,
         std::size(kOmniboxUIMaxAutocompleteMatches7), nullptr},
        {"8 matches", kOmniboxUIMaxAutocompleteMatches8,
         std::size(kOmniboxUIMaxAutocompleteMatches8), nullptr},
        {"9 matches", kOmniboxUIMaxAutocompleteMatches9,
         std::size(kOmniboxUIMaxAutocompleteMatches9), nullptr},
        {"10 matches", kOmniboxUIMaxAutocompleteMatches10,
         std::size(kOmniboxUIMaxAutocompleteMatches10), nullptr},
        {"12 matches", kOmniboxUIMaxAutocompleteMatches12,
         std::size(kOmniboxUIMaxAutocompleteMatches12), nullptr}};

const FeatureEntry::FeatureParam kOmniboxMaxURLMatches2[] = {
    {OmniboxFieldTrial::kOmniboxMaxURLMatchesParam, "2"}};
const FeatureEntry::FeatureParam kOmniboxMaxURLMatches3[] = {
    {OmniboxFieldTrial::kOmniboxMaxURLMatchesParam, "3"}};
const FeatureEntry::FeatureParam kOmniboxMaxURLMatches4[] = {
    {OmniboxFieldTrial::kOmniboxMaxURLMatchesParam, "4"}};
const FeatureEntry::FeatureParam kOmniboxMaxURLMatches5[] = {
    {OmniboxFieldTrial::kOmniboxMaxURLMatchesParam, "5"}};
const FeatureEntry::FeatureParam kOmniboxMaxURLMatches6[] = {
    {OmniboxFieldTrial::kOmniboxMaxURLMatchesParam, "6"}};

const FeatureEntry::FeatureVariation kOmniboxMaxURLMatchesVariations[] = {
    {"2 matches", kOmniboxMaxURLMatches2, std::size(kOmniboxMaxURLMatches2),
     nullptr},
    {"3 matches", kOmniboxMaxURLMatches3, std::size(kOmniboxMaxURLMatches3),
     nullptr},
    {"4 matches", kOmniboxMaxURLMatches4, std::size(kOmniboxMaxURLMatches4),
     nullptr},
    {"5 matches", kOmniboxMaxURLMatches5, std::size(kOmniboxMaxURLMatches5),
     nullptr},
    {"6 matches", kOmniboxMaxURLMatches6, std::size(kOmniboxMaxURLMatches6),
     nullptr}};

const FeatureEntry::FeatureParam kOmniboxDynamicMaxAutocomplete90[] = {
    {"OmniboxDynamicMaxAutocompleteUrlCutoff", "0"},
    {"OmniboxDynamicMaxAutocompleteIncreasedLimit", "9"}};
const FeatureEntry::FeatureParam kOmniboxDynamicMaxAutocomplete91[] = {
    {"OmniboxDynamicMaxAutocompleteUrlCutoff", "1"},
    {"OmniboxDynamicMaxAutocompleteIncreasedLimit", "9"}};
const FeatureEntry::FeatureParam kOmniboxDynamicMaxAutocomplete92[] = {
    {"OmniboxDynamicMaxAutocompleteUrlCutoff", "2"},
    {"OmniboxDynamicMaxAutocompleteIncreasedLimit", "9"}};
const FeatureEntry::FeatureParam kOmniboxDynamicMaxAutocomplete100[] = {
    {"OmniboxDynamicMaxAutocompleteUrlCutoff", "0"},
    {"OmniboxDynamicMaxAutocompleteIncreasedLimit", "10"}};
const FeatureEntry::FeatureParam kOmniboxDynamicMaxAutocomplete101[] = {
    {"OmniboxDynamicMaxAutocompleteUrlCutoff", "1"},
    {"OmniboxDynamicMaxAutocompleteIncreasedLimit", "10"}};
const FeatureEntry::FeatureParam kOmniboxDynamicMaxAutocomplete102[] = {
    {"OmniboxDynamicMaxAutocompleteUrlCutoff", "2"},
    {"OmniboxDynamicMaxAutocompleteIncreasedLimit", "10"}};

const FeatureEntry::FeatureVariation
    kOmniboxDynamicMaxAutocompleteVariations[] = {
        {"9 suggestions if 0 or fewer URLs", kOmniboxDynamicMaxAutocomplete90,
         std::size(kOmniboxDynamicMaxAutocomplete90), nullptr},
        {"9 suggestions if 1 or fewer URLs", kOmniboxDynamicMaxAutocomplete91,
         std::size(kOmniboxDynamicMaxAutocomplete91), nullptr},
        {"9 suggestions if 2 or fewer URLs", kOmniboxDynamicMaxAutocomplete92,
         std::size(kOmniboxDynamicMaxAutocomplete92), nullptr},
        {"10 suggestions if 0 or fewer URLs", kOmniboxDynamicMaxAutocomplete100,
         std::size(kOmniboxDynamicMaxAutocomplete100), nullptr},
        {"10 suggestions if 1 or fewer URLs", kOmniboxDynamicMaxAutocomplete101,
         std::size(kOmniboxDynamicMaxAutocomplete101), nullptr},
        {"10 suggestions if 2 or fewer URLs", kOmniboxDynamicMaxAutocomplete102,
         std::size(kOmniboxDynamicMaxAutocomplete102), nullptr}};

const FeatureEntry::FeatureParam kOmniboxUniformRowHeight36[] = {
    {"OmniboxRichSuggestionVerticalMargin", "4"}};
const FeatureEntry::FeatureParam kOmniboxUniformRowHeight40[] = {
    {"OmniboxRichSuggestionVerticalMargin", "6"}};

const FeatureEntry::FeatureVariation kOmniboxSuggestionHeightVariations[] = {
    {"36px omnibox suggestions", kOmniboxUniformRowHeight36,
     std::size(kOmniboxUniformRowHeight36), nullptr},
    {"40px omnibox suggestions", kOmniboxUniformRowHeight40,
     std::size(kOmniboxUniformRowHeight40), nullptr},
};

const FeatureEntry::FeatureParam kOmniboxFontSize12[] = {
    {"OmniboxFontSizeNonTouchUI", "12"}};
const FeatureEntry::FeatureParam kOmniboxFontSize13[] = {
    {"OmniboxFontSizeNonTouchUI", "13"}};
const FeatureEntry::FeatureParam kOmniboxFontSize14[] = {
    {"OmniboxFontSizeNonTouchUI", "14"}};

const FeatureEntry::FeatureVariation kOmniboxFontSizeVariations[] = {
    {"12pt omnibox font", kOmniboxFontSize12, std::size(kOmniboxFontSize12),
     nullptr},
    {"13pt omnibox font", kOmniboxFontSize13, std::size(kOmniboxFontSize13),
     nullptr},
    {"14pt omnibox font", kOmniboxFontSize14, std::size(kOmniboxFontSize14),
     nullptr},
};

const FeatureEntry::FeatureParam kRepeatableQueries_6Searches_90Days[] = {
    {"RepeatableQueriesIgnoreDuplicateVisits", "true"},
    {"RepeatableQueriesMinVisitCount", "6"},
};
const FeatureEntry::FeatureParam kRepeatableQueries_12Searches_90Days[] = {
    {"RepeatableQueriesIgnoreDuplicateVisits", "true"},
    {"RepeatableQueriesMinVisitCount", "12"},
};
const FeatureEntry::FeatureParam kRepeatableQueries_6Searches_7Days[] = {
    {"RepeatableQueriesIgnoreDuplicateVisits", "true"},
    {"RepeatableQueriesMinVisitCount", "6"},
    {"RepeatableQueriesMaxAgeDays", "7"},
};
const FeatureEntry::FeatureParam kRepeatableQueries_12Searches_7Days[] = {
    {"RepeatableQueriesIgnoreDuplicateVisits", "true"},
    {"RepeatableQueriesMinVisitCount", "12"},
    {"RepeatableQueriesMaxAgeDays", "7"},
};

const FeatureEntry::FeatureVariation kOrganicRepeatableQueriesVariations[] = {
    {"6+ uses, once in last 90d", kRepeatableQueries_6Searches_90Days,
     std::size(kRepeatableQueries_6Searches_90Days), nullptr},
    {"12+ uses, once in last 90d", kRepeatableQueries_12Searches_90Days,
     std::size(kRepeatableQueries_12Searches_90Days), nullptr},
    {"6+ uses, once in last 7d", kRepeatableQueries_6Searches_7Days,
     std::size(kRepeatableQueries_6Searches_7Days), nullptr},
    {"12+ uses, once in last 7d", kRepeatableQueries_12Searches_7Days,
     std::size(kRepeatableQueries_12Searches_7Days), nullptr},
};

const FeatureEntry::FeatureParam kMinimumTabWidthSettingPinned[] = {
    {features::kMinimumTabWidthFeatureParameterName, "54"}};
const FeatureEntry::FeatureParam kMinimumTabWidthSettingMedium[] = {
    {features::kMinimumTabWidthFeatureParameterName, "72"}};
const FeatureEntry::FeatureParam kMinimumTabWidthSettingLarge[] = {
    {features::kMinimumTabWidthFeatureParameterName, "140"}};
const FeatureEntry::FeatureParam kMinimumTabWidthSettingFull[] = {
    {features::kMinimumTabWidthFeatureParameterName, "256"}};

const FeatureEntry::FeatureVariation kTabScrollingVariations[] = {
    {" - tabs shrink to pinned tab width", kMinimumTabWidthSettingPinned,
     std::size(kMinimumTabWidthSettingPinned), nullptr},
    {" - tabs shrink to a medium width", kMinimumTabWidthSettingMedium,
     std::size(kMinimumTabWidthSettingMedium), nullptr},
    {" - tabs shrink to a large width", kMinimumTabWidthSettingLarge,
     std::size(kMinimumTabWidthSettingLarge), nullptr},
    {" - tabs don't shrink", kMinimumTabWidthSettingFull,
     std::size(kMinimumTabWidthSettingFull), nullptr}};

const FeatureEntry::FeatureParam kTabScrollingButtonPositionRight[] = {
    {features::kTabScrollingButtonPositionParameterName, "0"}};
const FeatureEntry::FeatureParam kTabScrollingButtonPositionLeft[] = {
    {features::kTabScrollingButtonPositionParameterName, "1"}};
const FeatureEntry::FeatureParam kTabScrollingButtonPositionSplit[] = {
    {features::kTabScrollingButtonPositionParameterName, "2"}};

const FeatureEntry::FeatureVariation kTabScrollingButtonPositionVariations[] = {
    {" - to the right of the tabstrip", kTabScrollingButtonPositionRight,
     std::size(kTabScrollingButtonPositionRight), nullptr},
    {" - to the left of the tabstrip", kTabScrollingButtonPositionLeft,
     std::size(kTabScrollingButtonPositionLeft), nullptr},
    {" - on both sides of the tabstrip", kTabScrollingButtonPositionSplit,
     std::size(kTabScrollingButtonPositionSplit), nullptr}};

const FeatureEntry::FeatureParam kTabScrollingWithDraggingWithConstantSpeed[] =
    {{features::kTabScrollingWithDraggingModeName, "1"}};
const FeatureEntry::FeatureParam kTabScrollingWithDraggingWithVariableSpeed[] =
    {{features::kTabScrollingWithDraggingModeName, "2"}};

const FeatureEntry::FeatureVariation kTabScrollingWithDraggingVariations[] = {
    {" - tabs scrolling with constant speed",
     kTabScrollingWithDraggingWithConstantSpeed,
     std::size(kTabScrollingWithDraggingWithConstantSpeed), nullptr},
    {" - tabs scrolling with variable speed region",
     kTabScrollingWithDraggingWithVariableSpeed,
     std::size(kTabScrollingWithDraggingWithVariableSpeed), nullptr}};

const FeatureEntry::FeatureParam kScrollableTabStripOverflowDivider[] = {
    {features::kScrollableTabStripOverflowModeName, "1"}};
const FeatureEntry::FeatureParam kScrollableTabStripOverflowFade[] = {
    {features::kScrollableTabStripOverflowModeName, "2"}};
const FeatureEntry::FeatureParam kScrollableTabStripOverflowShadow[] = {
    {features::kScrollableTabStripOverflowModeName, "3"}};

const FeatureEntry::FeatureVariation kScrollableTabStripOverflowVariations[] = {
    {" - Divider", kScrollableTabStripOverflowDivider,
     std::size(kScrollableTabStripOverflowDivider), nullptr},  // Divider
    {" - Fade", kScrollableTabStripOverflowFade,
     std::size(kScrollableTabStripOverflowFade), nullptr},  // Fade
    {" - Shadow", kScrollableTabStripOverflowShadow,
     std::size(kScrollableTabStripOverflowShadow), nullptr},  // Shadow
};

const FeatureEntry::FeatureParam kTabSearchSearchThresholdSmall[] = {
    {features::kTabSearchSearchThresholdName, "0.3"}};
const FeatureEntry::FeatureParam kTabSearchSearchThresholdMedium[] = {
    {features::kTabSearchSearchThresholdName, "0.6"}};
const FeatureEntry::FeatureParam kTabSearchSearchThresholdLarge[] = {
    {features::kTabSearchSearchThresholdName, "0.8"}};

const FeatureEntry::FeatureVariation kTabSearchSearchThresholdVariations[] = {
    {" - fuzzy level: small", kTabSearchSearchThresholdSmall,
     std::size(kTabSearchSearchThresholdSmall), nullptr},
    {" - fuzzy level: medium", kTabSearchSearchThresholdMedium,
     std::size(kTabSearchSearchThresholdMedium), nullptr},
    {" - fuzzy level: large", kTabSearchSearchThresholdLarge,
     std::size(kTabSearchSearchThresholdLarge), nullptr}};

const FeatureEntry::FeatureParam kChromeLabsEnabledInFlags[] = {
    {features::kChromeLabsActivationParameterName, "100"}};

const FeatureEntry::FeatureVariation kChromeLabsVariations[] = {
    {" use this one!", kChromeLabsEnabledInFlags,
     std::size(kChromeLabsEnabledInFlags), nullptr}};

#if !BUILDFLAG(IS_ANDROID)
const FeatureEntry::FeatureParam kNtpChromeCartModuleFakeData[] = {
    {ntp_features::kNtpChromeCartModuleDataParam, "fake"},
    {ntp_features::kNtpChromeCartModuleAbandonedCartDiscountParam, "true"}};
const FeatureEntry::FeatureParam kNtpChromeCartModuleAbandonedCartDiscount[] = {
    {ntp_features::kNtpChromeCartModuleAbandonedCartDiscountParam, "true"},
    {ntp_features::kNtpChromeCartModuleAbandonedCartDiscountUseUtmParam,
     "true"},
    {"partner-merchant-pattern",
     "(electronicexpress.com|zazzle.com|wish.com|homesquare.com|iherb.com|"
     "zappos.com|otterbox.com)"}};
const FeatureEntry::FeatureParam kNtpChromeCartModuleHeuristicsImprovement[] = {
    {ntp_features::kNtpChromeCartModuleHeuristicsImprovementParam, "true"}};
const FeatureEntry::FeatureParam kNtpChromeCartModuleRBDAndCouponDiscount[] = {
    {ntp_features::kNtpChromeCartModuleHeuristicsImprovementParam, "true"},
    {ntp_features::kNtpChromeCartModuleAbandonedCartDiscountParam, "true"},
    {ntp_features::kNtpChromeCartModuleAbandonedCartDiscountUseUtmParam,
     "true"},
    {"partner-merchant-pattern",
     "(electronicexpress.com|zazzle.com|wish.com|homesquare.com)"},
    {ntp_features::kNtpChromeCartModuleCouponParam, "true"}};
const FeatureEntry::FeatureVariation kNtpChromeCartModuleVariations[] = {
    {"- Fake Data And Discount", kNtpChromeCartModuleFakeData,
     std::size(kNtpChromeCartModuleFakeData), nullptr},
    {"- Abandoned Cart Discount", kNtpChromeCartModuleAbandonedCartDiscount,
     std::size(kNtpChromeCartModuleAbandonedCartDiscount), nullptr},
    {"- Heuristics Improvement", kNtpChromeCartModuleHeuristicsImprovement,
     std::size(kNtpChromeCartModuleHeuristicsImprovement), nullptr},
    {"- RBD and Coupons", kNtpChromeCartModuleRBDAndCouponDiscount,
     std::size(kNtpChromeCartModuleRBDAndCouponDiscount), nullptr},
};

// The following are consent v2 variations in the Chrome Cart module.
const flags_ui::FeatureEntry::FeatureParam kDiscountConsentNtpDialog[] = {
    {commerce::kNtpChromeCartModuleDiscountConsentNtpVariationParam, "3"}};
const flags_ui::FeatureEntry::FeatureParam kDiscountConsentNtpNativeDialog[] = {
    {commerce::kNtpChromeCartModuleDiscountConsentNtpVariationParam, "4"}};

const FeatureEntry::FeatureVariation kDiscountConsentV2Variations[] = {
    {"WebUi Dialog Consent", kDiscountConsentNtpDialog,
     std::size(kDiscountConsentNtpDialog), nullptr},
    {"Native Dialog Consent", kDiscountConsentNtpNativeDialog,
     std::size(kDiscountConsentNtpNativeDialog), nullptr},
};

// The following is Code-based RBD variation.
const flags_ui::FeatureEntry::FeatureParam kCodeBasedRBDEnabled[] = {
    {commerce::kCodeBasedRuleDiscountParam, "true"}};

const FeatureEntry::FeatureVariation kCodeBasedRBDVariations[] = {
    {"code-based RBD", kCodeBasedRBDEnabled, std::size(kCodeBasedRBDEnabled),
     "t3362898"},
};

// The following is Merchant wide promotion variation.
const flags_ui::FeatureEntry::FeatureParam kMerchantWideFetchEnabled[] = {
    {commerce::kReadyToFetchMerchantWidePromotionParam, "true"}};

const FeatureEntry::FeatureVariation kMerchantWidePromotionVariations[] = {
    {"- Enable fetch", kMerchantWideFetchEnabled,
     std::size(kMerchantWideFetchEnabled), nullptr},
};

const FeatureEntry::FeatureParam kNtpDriveModuleFakeData[] = {
    {ntp_features::kNtpDriveModuleDataParam, "fake"}};
const FeatureEntry::FeatureParam kNtpDriveModuleManagedUsersOnly[] = {
    {ntp_features::kNtpDriveModuleManagedUsersOnlyParam, "true"}};
const FeatureEntry::FeatureVariation kNtpDriveModuleVariations[] = {
    {"- Fake Data", kNtpDriveModuleFakeData, std::size(kNtpDriveModuleFakeData),
     nullptr},
    {"- Managed Users Only", kNtpDriveModuleManagedUsersOnly,
     std::size(kNtpDriveModuleManagedUsersOnly), nullptr},
};

// History clusters fake data params are expressed as a comma separated tuple
// consisting of a number desired clusters, a number of desired visits, and the
// number of such visits to be marked as having url keyed images. The number of
// visits must be greater than or equal to the number of visits marked as having
// images.
const FeatureEntry::FeatureParam
    kNtpHistoryClustersModuleFakeData2Visits2Images[] = {
        {ntp_features::kNtpHistoryClustersModuleDataParam, "1,2,2"}};
const FeatureEntry::FeatureParam
    kNtpHistoryClustersModuleFakeData3Visits1Image[] = {
        {ntp_features::kNtpHistoryClustersModuleDataParam, "1,3,1"}};
const FeatureEntry::FeatureParam
    kNtpHistoryClustersModuleFakeData4Visits2Images[] = {
        {ntp_features::kNtpHistoryClustersModuleDataParam, "1,4,2"}};
const FeatureEntry::FeatureParam kNtpHistoryClustersModuleV2OneJourney[] = {
    {ntp_features::kNtpHistoryClustersModuleDataParam, "1,2,2"}};
const FeatureEntry::FeatureParam kNtpHistoryClustersModuleV2TwoJourneys[] = {
    {ntp_features::kNtpHistoryClustersModuleDataParam, "2,2,2"}};
const FeatureEntry::FeatureParam kNtpHistoryClustersModuleV2ThreeJourneys[] = {
    {ntp_features::kNtpHistoryClustersModuleDataParam, "3,2,2"}};
const FeatureEntry::FeatureParam
    kNtpHistoryClustersModuleV2ThreeJourneysTextOnly[] = {
        {ntp_features::kNtpHistoryClustersModuleDataParam, "3,2,0"}};
const FeatureEntry::FeatureVariation kNtpHistoryClustersModuleVariations[] = {
    {"- Fake Data - Layout 1", kNtpHistoryClustersModuleFakeData2Visits2Images,
     std::size(kNtpHistoryClustersModuleFakeData2Visits2Images), nullptr},
    {"- Fake Data - Layout 2", kNtpHistoryClustersModuleFakeData3Visits1Image,
     std::size(kNtpHistoryClustersModuleFakeData3Visits1Image), nullptr},
    {"- Fake Data - Layout 3", kNtpHistoryClustersModuleFakeData4Visits2Images,
     std::size(kNtpHistoryClustersModuleFakeData4Visits2Images), nullptr},
    {"- v2 Fake Data - 1 Journey", kNtpHistoryClustersModuleV2OneJourney,
     std::size(kNtpHistoryClustersModuleV2OneJourney), nullptr},
    {"- v2 Fake Data - 2 Journeys", kNtpHistoryClustersModuleV2TwoJourneys,
     std::size(kNtpHistoryClustersModuleV2TwoJourneys), nullptr},
    {"- v2 Fake Data - 3 Journeys", kNtpHistoryClustersModuleV2ThreeJourneys,
     std::size(kNtpHistoryClustersModuleV2ThreeJourneys), nullptr},
    {"- v2 Fake Data - 3 Journeys - Text Only",
     kNtpHistoryClustersModuleV2ThreeJourneysTextOnly,
     std::size(kNtpHistoryClustersModuleV2ThreeJourneysTextOnly), nullptr},
};

const FeatureEntry::FeatureParam
    kNtpChromeCartInHistoryClustersModuleFakeData0[] = {
        {ntp_features::kNtpChromeCartInHistoryClustersModuleDataParam, "0"}};
const FeatureEntry::FeatureParam
    kNtpChromeCartInHistoryClustersModuleFakeData1[] = {
        {ntp_features::kNtpChromeCartInHistoryClustersModuleDataParam, "1"}};
const FeatureEntry::FeatureParam
    kNtpChromeCartInHistoryClustersModuleFakeData2[] = {
        {ntp_features::kNtpChromeCartInHistoryClustersModuleDataParam, "3"}};
const FeatureEntry::FeatureParam
    kNtpChromeCartInHistoryClustersModuleFakeData3[] = {
        {ntp_features::kNtpChromeCartInHistoryClustersModuleDataParam, "6"}};

const FeatureEntry::FeatureVariation
    kNtpChromeCartInHistoryClustersModuleVariations[] = {
        {" - Fake cart: 0 product image",
         kNtpChromeCartInHistoryClustersModuleFakeData0,
         std::size(kNtpChromeCartInHistoryClustersModuleFakeData0), nullptr},
        {" - Fake cart: 1 product image",
         kNtpChromeCartInHistoryClustersModuleFakeData1,
         std::size(kNtpChromeCartInHistoryClustersModuleFakeData1), nullptr},
        {" - Fake cart: 3 product images",
         kNtpChromeCartInHistoryClustersModuleFakeData2,
         std::size(kNtpChromeCartInHistoryClustersModuleFakeData2), nullptr},
        {" - Fake cart: 6 product images",
         kNtpChromeCartInHistoryClustersModuleFakeData3,
         std::size(kNtpChromeCartInHistoryClustersModuleFakeData3), nullptr}};

const FeatureEntry::FeatureParam kNtpMiddleSlotPromoDismissalFakeData[] = {
    {ntp_features::kNtpMiddleSlotPromoDismissalParam, "fake"}};
const FeatureEntry::FeatureVariation kNtpMiddleSlotPromoDismissalVariations[] =
    {
        {"- Fake Data", kNtpMiddleSlotPromoDismissalFakeData,
         std::size(kNtpMiddleSlotPromoDismissalFakeData), nullptr},
};

const FeatureEntry::FeatureParam kNtpPhotosModuleFakeData0[] = {
    {ntp_features::kNtpPhotosModuleDataParam, "0"}};
const FeatureEntry::FeatureParam kNtpPhotosModuleFakeData1[] = {
    {ntp_features::kNtpPhotosModuleDataParam, "1"}};
const FeatureEntry::FeatureParam kNtpPhotosModuleFakeData2[] = {
    {ntp_features::kNtpPhotosModuleDataParam, "2"}};
const FeatureEntry::FeatureParam kNtpPhotosModuleFakeData3[] = {
    {ntp_features::kNtpPhotosModuleDataParam, "3"}};
const FeatureEntry::FeatureParam kNtpPhotosModuleFakeData4[] = {
    {ntp_features::kNtpPhotosModuleDataParam, "4"}};

const FeatureEntry::FeatureVariation kNtpPhotosModuleVariations[] = {
    {" - Fake memories: 0", kNtpPhotosModuleFakeData0,
     std::size(kNtpPhotosModuleFakeData0), nullptr},
    {" - Fake memories: 1", kNtpPhotosModuleFakeData1,
     std::size(kNtpPhotosModuleFakeData1), nullptr},
    {" - Fake memories: 2", kNtpPhotosModuleFakeData2,
     std::size(kNtpPhotosModuleFakeData2), nullptr},
    {" - Fake memories: 3", kNtpPhotosModuleFakeData3,
     std::size(kNtpPhotosModuleFakeData3), nullptr},
    {" - Fake memories: 4", kNtpPhotosModuleFakeData4,
     std::size(kNtpPhotosModuleFakeData4), nullptr}};

const FeatureEntry::FeatureParam kNtpPhotosModuleOptInRHTitle[] = {
    {ntp_features::kNtpPhotosModuleOptInTitleParam, "0"}};
const FeatureEntry::FeatureParam kNtpPhotosModuleOptInFavoriteTitle[] = {
    {ntp_features::kNtpPhotosModuleOptInTitleParam, "1"}};
const FeatureEntry::FeatureParam kNtpPhotosModuleOptInPersonalizedTitle[] = {
    {ntp_features::kNtpPhotosModuleOptInTitleParam, "2"}};
const FeatureEntry::FeatureParam kNtpPhotosModuleOptInTripsTitle[] = {
    {ntp_features::kNtpPhotosModuleOptInTitleParam, "3"}};

const FeatureEntry::FeatureVariation kNtpPhotosModuleOptInTitleVariations[] = {
    {" - Recent Highlights", kNtpPhotosModuleOptInRHTitle,
     std::size(kNtpPhotosModuleOptInRHTitle), nullptr},
    {" - Favorite people", kNtpPhotosModuleOptInFavoriteTitle,
     std::size(kNtpPhotosModuleOptInFavoriteTitle), nullptr},
    {" - Personalized title", kNtpPhotosModuleOptInPersonalizedTitle,
     std::size(kNtpPhotosModuleOptInPersonalizedTitle), nullptr},
    {" - Trips title", kNtpPhotosModuleOptInTripsTitle,
     std::size(kNtpPhotosModuleOptInTripsTitle), nullptr}};

const FeatureEntry::FeatureParam kNtpPhotosModuleLogo1ArtWork[] = {
    {ntp_features::kNtpPhotosModuleOptInArtWorkParam, "1"}};
const FeatureEntry::FeatureParam kNtpPhotosModuleLogo2ArtWork[] = {
    {ntp_features::kNtpPhotosModuleOptInArtWorkParam, "2"}};
const FeatureEntry::FeatureParam kNtpPhotosModuleIllustrationsArtWork[] = {
    {ntp_features::kNtpPhotosModuleOptInArtWorkParam, "3"}};
const FeatureEntry::FeatureParam kNtpPhotosModuleStockpileArtWork[] = {
    {ntp_features::kNtpPhotosModuleOptInArtWorkParam, "4"}};

const FeatureEntry::FeatureVariation kNtpPhotosModuleOptInArtWorkVariations[] =
    {{" - Artwork with Logo - 1", kNtpPhotosModuleLogo1ArtWork,
      std::size(kNtpPhotosModuleLogo1ArtWork), nullptr},
     {" - Artwork with Logo - 2", kNtpPhotosModuleLogo2ArtWork,
      std::size(kNtpPhotosModuleLogo2ArtWork), nullptr},
     {" - Artwork with Illustrations", kNtpPhotosModuleIllustrationsArtWork,
      std::size(kNtpPhotosModuleIllustrationsArtWork), nullptr},
     {" - Artwork with Stockpile", kNtpPhotosModuleStockpileArtWork,
      std::size(kNtpPhotosModuleStockpileArtWork), nullptr}};

const FeatureEntry::FeatureParam kNtpRealboxRevertWidthOnBlur[] = {
    {ntp_features::kNtpRealboxWidthBehaviorParam, "revert"}};
const FeatureEntry::FeatureParam kNtpRealboxAlwaysWide[] = {
    {ntp_features::kNtpRealboxWidthBehaviorParam, "wide"}};
const FeatureEntry::FeatureVariation kNtpRealboxWidthBehaviorVariations[] = {
    {" - Reverts back on blur if there is secondary column",
     kNtpRealboxRevertWidthOnBlur, std::size(kNtpRealboxRevertWidthOnBlur),
     nullptr},
    {" - Always wide", kNtpRealboxAlwaysWide, std::size(kNtpRealboxAlwaysWide),
     nullptr}};

const FeatureEntry::FeatureParam kNtpRecipeTasksModuleFakeData[] = {
    {ntp_features::kNtpRecipeTasksModuleDataParam, "fake"}};
const FeatureEntry::FeatureParam kNtpRecipeTasksModuleHistorical7Days[] = {
    {ntp_features::kNtpRecipeTasksModuleExperimentGroupParam, "historical-7"}};
const FeatureEntry::FeatureParam kNtpRecipeTasksModuleHistorical14Days[] = {
    {ntp_features::kNtpRecipeTasksModuleExperimentGroupParam, "historical-14"}};
const FeatureEntry::FeatureParam kNtpRecipeTasksModuleMix7Days[] = {
    {ntp_features::kNtpRecipeTasksModuleExperimentGroupParam, "mix-7"}};
const FeatureEntry::FeatureParam kNtpRecipeTasksModuleMix14Days[] = {
    {ntp_features::kNtpRecipeTasksModuleExperimentGroupParam, "mix-14"}};
const FeatureEntry::FeatureVariation kNtpRecipeTasksModuleVariations[] = {
    {"- Fake Data", kNtpRecipeTasksModuleFakeData,
     std::size(kNtpRecipeTasksModuleFakeData), nullptr},
    {"- Historical Arm (7 days)", kNtpRecipeTasksModuleHistorical7Days,
     std::size(kNtpRecipeTasksModuleHistorical7Days), "t3349934"},
    {"- Historical Arm (14 days)", kNtpRecipeTasksModuleHistorical14Days,
     std::size(kNtpRecipeTasksModuleHistorical14Days), "t3349935"},
    {"- Recommended Mix Arm (7 days)", kNtpRecipeTasksModuleMix7Days,
     std::size(kNtpRecipeTasksModuleMix7Days), "t3349936"},
    {"- Recommended Mix Arm (14 days)", kNtpRecipeTasksModuleMix14Days,
     std::size(kNtpRecipeTasksModuleMix14Days), "t3349937"},
};

const FeatureEntry::FeatureParam kNtpSafeBrowsingModuleFastCooldown[] = {
    {ntp_features::kNtpSafeBrowsingModuleCooldownPeriodDaysParam, "0.001"},
    {ntp_features::kNtpSafeBrowsingModuleCountMaxParam, "1"}};
const FeatureEntry::FeatureVariation kNtpSafeBrowsingModuleVariations[] = {
    {"(Fast Cooldown)", kNtpSafeBrowsingModuleFastCooldown,
     std::size(kNtpSafeBrowsingModuleFastCooldown), nullptr},
};

const FeatureEntry::FeatureParam kNtpTabResumptionModuleFakeData[] = {
    {ntp_features::kNtpTabResumptionModuleDataParam, "Fake Data"}};
const FeatureEntry::FeatureVariation kNtpTabResumptionModuleVariations[] = {
    {"- Fake Data", kNtpTabResumptionModuleFakeData,
     std::size(kNtpTabResumptionModuleFakeData), nullptr}};
#endif  // !BUILDFLAG(IS_ANDROID)

#if BUILDFLAG(IS_ANDROID)

const FeatureEntry::FeatureParam kRelatedSearchesUrl = {"stamp", "1Ru"};
const FeatureEntry::FeatureParam kRelatedSearchesContent = {"stamp", "1Rc"};
const FeatureEntry::FeatureVariation kRelatedSearchesVariations[] = {
    {"from URL", &kRelatedSearchesUrl, 1, nullptr},
    {"from content", &kRelatedSearchesContent, 1, nullptr},
};

const FeatureEntry::FeatureParam kContextualSearchSuppressShortViewWith300Dp[] =
    {{"contextual_search_minimum_page_height_dp", "300"}};
const FeatureEntry::FeatureParam kContextualSearchSuppressShortViewWith400Dp[] =
    {{"contextual_search_minimum_page_height_dp", "400"}};
const FeatureEntry::FeatureParam kContextualSearchSuppressShortViewWith500Dp[] =
    {{"contextual_search_minimum_page_height_dp", "500"}};
const FeatureEntry::FeatureParam kContextualSearchSuppressShortViewWith600Dp[] =
    {{"contextual_search_minimum_page_height_dp", "600"}};
const FeatureEntry::FeatureVariation
    kContextualSearchSuppressShortViewVariations[] = {
        {"(300 dp)", kContextualSearchSuppressShortViewWith300Dp,
         std::size(kContextualSearchSuppressShortViewWith300Dp), nullptr},
        {"(400 dp)", kContextualSearchSuppressShortViewWith400Dp,
         std::size(kContextualSearchSuppressShortViewWith400Dp), nullptr},
        {"(500 dp)", kContextualSearchSuppressShortViewWith500Dp,
         std::size(kContextualSearchSuppressShortViewWith500Dp), nullptr},
        {"(600 dp)", kContextualSearchSuppressShortViewWith600Dp,
         std::size(kContextualSearchSuppressShortViewWith600Dp), nullptr},
};

#endif  // BUILDFLAG(IS_ANDROID)

const FeatureEntry::FeatureParam
    kResamplingScrollEventsPredictionTimeBasedEnabled[] = {
        {"mode", features::kPredictionTypeTimeBased},
        {"latency", features::kPredictionTypeDefaultTime}};
const FeatureEntry::FeatureParam
    kResamplingScrollEventsPredictionFramesBasedEnabled[] = {
        {"mode", features::kPredictionTypeFramesBased},
        {"latency", features::kPredictionTypeDefaultFramesRatio}};
const FeatureEntry::FeatureVariation
    kResamplingScrollEventsExperimentalPredictionVariations[] = {
        {features::kPredictionTypeTimeBased,
         kResamplingScrollEventsPredictionTimeBasedEnabled,
         std::size(kResamplingScrollEventsPredictionTimeBasedEnabled), nullptr},
        {features::kPredictionTypeFramesBased,
         kResamplingScrollEventsPredictionFramesBasedEnabled,
         std::size(kResamplingScrollEventsPredictionFramesBasedEnabled),
         nullptr}};

#if BUILDFLAG(IS_ANDROID)
const FeatureEntry::FeatureParam kStartSurfaceReturnTime_Immediate[] = {
    {"start_surface_return_time_seconds", "0"},
    {"start_surface_return_time_on_tablet_seconds", "0"}};
const FeatureEntry::FeatureParam kStartSurfaceReturnTime_10Seconds[] = {
    {"start_surface_return_time_seconds", "10"},
    {"start_surface_return_time_on_tablet_seconds", "10"}};
const FeatureEntry::FeatureParam kStartSurfaceReturnTime_1Minute[] = {
    {"start_surface_return_time_seconds", "60"},
    {"start_surface_return_time_on_tablet_seconds", "60"}};
const FeatureEntry::FeatureParam kStartSurfaceReturnTime_5Minute[] = {
    {"start_surface_return_time_seconds", "300"},
    {"start_surface_return_time_on_tablet_seconds", "300"}};
const FeatureEntry::FeatureParam kStartSurfaceReturnTime_60Minute[] = {
    {"start_surface_return_time_seconds", "3600"},
    {"start_surface_return_time_on_tablet_seconds", "3600"}};
const FeatureEntry::FeatureVariation kStartSurfaceReturnTimeVariations[] = {
    {"Immediate", kStartSurfaceReturnTime_Immediate,
     std::size(kStartSurfaceReturnTime_Immediate), nullptr},
    {"10 seconds", kStartSurfaceReturnTime_10Seconds,
     std::size(kStartSurfaceReturnTime_10Seconds), nullptr},
    {"1 minute", kStartSurfaceReturnTime_1Minute,
     std::size(kStartSurfaceReturnTime_1Minute), nullptr},
    {"5 minute", kStartSurfaceReturnTime_5Minute,
     std::size(kStartSurfaceReturnTime_5Minute), nullptr},
    {"60 minute", kStartSurfaceReturnTime_60Minute,
     std::size(kStartSurfaceReturnTime_60Minute), nullptr},
};

const FeatureEntry::FeatureParam
    kAccountReauthenticationRecentTimeWindow_0Minutes[] = {
        {"account_reauthentication_recent_time_window_minutes", "0"},
};
const FeatureEntry::FeatureParam
    kAccountReauthenticationRecentTimeWindow_1Minutes[] = {
        {"account_reauthentication_recent_time_window_minutes", "1"},
};
const FeatureEntry::FeatureParam
    kAccountReauthenticationRecentTimeWindow_5Minutes[] = {
        {"account_reauthentication_recent_time_window_minutes", "5"},
};
const FeatureEntry::FeatureParam
    kAccountReauthenticationRecentTimeWindow_10Minutes[] = {
        {"account_reauthentication_recent_time_window_minutes", "10"},
};
const FeatureEntry::FeatureVariation
    kAccountReauthenticationRecentTimeWindowVariations[] = {
        {"0 minutes", kAccountReauthenticationRecentTimeWindow_0Minutes,
         std::size(kAccountReauthenticationRecentTimeWindow_0Minutes), nullptr},
        {"1 minutes", kAccountReauthenticationRecentTimeWindow_1Minutes,
         std::size(kAccountReauthenticationRecentTimeWindow_1Minutes), nullptr},
        {"5 minutes", kAccountReauthenticationRecentTimeWindow_5Minutes,
         std::size(kAccountReauthenticationRecentTimeWindow_5Minutes), nullptr},
        {"10 minutes", kAccountReauthenticationRecentTimeWindow_10Minutes,
         std::size(kAccountReauthenticationRecentTimeWindow_10Minutes),
         nullptr},
};
#endif  // BUILDFLAG(IS_ANDROID)
#if BUILDFLAG(IS_ANDROID)
const FeatureEntry::FeatureParam
    kNewTabSearchEngineUrlAndroid_EeaCountryOnly[] = {
        {"eea_country_only", "true"}};

const FeatureEntry::FeatureVariation kNewTabSearchEngineUrlAndroidVariations[] =
    {
        {"EEA Country Only", kNewTabSearchEngineUrlAndroid_EeaCountryOnly,
         std::size(kNewTabSearchEngineUrlAndroid_EeaCountryOnly), nullptr},
};

const FeatureEntry::FeatureParam kStartSurfaceAndroid_SingleSurface[] = {
    {"open_ntp_instead_of_start", "false"},
    {"open_start_as_homepage", "true"},
    {"show_last_active_tab_only", "false"},
    {"show_tabs_in_mru_order", "true"}};

const FeatureEntry::FeatureParam kStartSurfaceAndroid_CandidateA[] = {
    {"open_ntp_instead_of_start", "false"},
    {"open_start_as_homepage", "true"}};

const FeatureEntry::FeatureParam kStartSurfaceAndroid_CandidateA_SyncCheck[] = {
    {"open_ntp_instead_of_start", "false"},
    {"open_start_as_homepage", "true"},
    {"check_sync_before_show_start_at_startup", "true"}};

const FeatureEntry::FeatureParam
    kStartSurfaceAndroid_CandidateA_SigninPromoTimeLimit[] = {
        {"open_ntp_instead_of_start", "false"},
        {"open_start_as_homepage", "true"},
        {"sign_in_promo_show_since_last_background_limit_ms", "30000"}};

const FeatureEntry::FeatureParam kStartSurfaceAndroid_CandidateB[] = {
    {"open_ntp_instead_of_start", "true"}};

const FeatureEntry::FeatureParam
    kStartSurfaceAndroid_CandidateB_AlwaysShowIncognito[] = {
        {"hide_switch_when_no_incognito_tabs", "false"}};

const FeatureEntry::FeatureVariation kStartSurfaceAndroidVariations[] = {
    {"Candidate A", kStartSurfaceAndroid_CandidateA,
     std::size(kStartSurfaceAndroid_CandidateA), nullptr},
    {"Candidate A + Sync check", kStartSurfaceAndroid_CandidateA_SyncCheck,
     std::size(kStartSurfaceAndroid_CandidateA_SyncCheck), nullptr},
    {"Candidate A + Sign in promo backgrounded time limit",
     kStartSurfaceAndroid_CandidateA_SigninPromoTimeLimit,
     std::size(kStartSurfaceAndroid_CandidateA_SigninPromoTimeLimit), nullptr},
    {"Candidate B", kStartSurfaceAndroid_CandidateB,
     std::size(kStartSurfaceAndroid_CandidateB), nullptr},
    {"Candidate B + Always show Incognito icon",
     kStartSurfaceAndroid_CandidateB_AlwaysShowIncognito,
     std::size(kStartSurfaceAndroid_CandidateB_AlwaysShowIncognito), nullptr},
    {"Single Surface", kStartSurfaceAndroid_SingleSurface,
     std::size(kStartSurfaceAndroid_SingleSurface), nullptr},
};

const FeatureEntry::FeatureParam kSurfacePolish_mvp[] = {
    {"polish_omnibox_color", "true"},
    {"scrollable_mvt", "true"},
    {"move_down_logo", "true"}};

const FeatureEntry::FeatureParam kSurfacePolish_white_omnibox[] = {
    {"scrollable_mvt", "true"},
    {"move_down_logo", "true"}};

const FeatureEntry::FeatureParam kSurfacePolish_less_brand_space[] = {
    {"polish_omnibox_color", "true"},
    {"scrollable_mvt", "true"},
    {"move_down_logo", "true"},
    {"less_brand_space", "true"}};

const FeatureEntry::FeatureParam kSurfacePolish_mvp_2row_mvt[] = {
    {"polish_omnibox_color", "true"},
    {"move_down_logo", "true"}};

const FeatureEntry::FeatureParam kSurfacePolish_logo_in_toolbar[] = {
    {"polish_omnibox_color", "true"}};

const FeatureEntry::FeatureParam kSurfacePolish_use_magic_space[] = {
    {"polish_omnibox_color", "true"},
    {"move_down_logo", "true"},
    {"use_magic_space", "true"}};

const FeatureEntry::FeatureVariation kSurfacePolishVariations[] = {
    {"Arm 1: MVP", kSurfacePolish_mvp, std::size(kSurfacePolish_mvp), nullptr},
    {"Arm 2: White Omnibox", kSurfacePolish_white_omnibox,
     std::size(kSurfacePolish_white_omnibox), nullptr},
    {"Arm 3: Less brand space", kSurfacePolish_less_brand_space,
     std::size(kSurfacePolish_less_brand_space), nullptr},
    {"Arm 4: MVP with 2 row MVT", kSurfacePolish_mvp_2row_mvt,
     std::size(kSurfacePolish_mvp_2row_mvt), nullptr},
    {"Arm 5: Logo in toolbar", kSurfacePolish_logo_in_toolbar,
     std::size(kSurfacePolish_logo_in_toolbar), nullptr},
    {"Use magic space", kSurfacePolish_use_magic_space,
     std::size(kSurfacePolish_use_magic_space), nullptr},
};

const FeatureEntry::FeatureParam kFeedPositionAndroid_push_down_feed_small[] = {
    {"push_down_feed_small", "true"}};

const FeatureEntry::FeatureParam kFeedPositionAndroid_push_down_feed_large[] = {
    {"push_down_feed_large", "true"}};

const FeatureEntry::FeatureParam kFeedPositionAndroid_pull_up_feed[] = {
    {"pull_up_feed", "true"}};

const FeatureEntry::FeatureParam
    kFeedPositionAndroid_push_down_feed_large_target_feed_active[] = {
        {"push_down_feed_large", "true"},
        {"feed_active_targeting", "active"}};

const FeatureEntry::FeatureParam
    kFeedPositionAndroid_push_down_feed_large_target_non_feed_active[] = {
        {"push_down_feed_large", "true"},
        {"feed_active_targeting", "non-active"}};

const FeatureEntry::FeatureParam
    kFeedPositionAndroid_pull_up_feed_target_feed_active[] = {
        {"pull_up_feed", "true"},
        {"feed_active_targeting", "active"}};

const FeatureEntry::FeatureParam
    kFeedPositionAndroid_pull_up_feed_target_non_feed_active[] = {
        {"pull_up_feed", "true"},
        {"feed_active_targeting", "non-active"}};

const FeatureEntry::FeatureVariation kFeedPositionAndroidVariations[] = {
    {"Push down Feed (small)", kFeedPositionAndroid_push_down_feed_small,
     std::size(kFeedPositionAndroid_push_down_feed_small), nullptr},
    {"Push down Feed (large)", kFeedPositionAndroid_push_down_feed_large,
     std::size(kFeedPositionAndroid_push_down_feed_large), nullptr},
    {"Pull up Feed", kFeedPositionAndroid_pull_up_feed,
     std::size(kFeedPositionAndroid_pull_up_feed), nullptr},
    {"Push down Feed (large) with targeting Feed active users",
     kFeedPositionAndroid_push_down_feed_large_target_feed_active,
     std::size(kFeedPositionAndroid_push_down_feed_large_target_feed_active),
     nullptr},
    {"Push down Feed (large) with targeting non-Feed active users",
     kFeedPositionAndroid_push_down_feed_large_target_non_feed_active,
     std::size(
         kFeedPositionAndroid_push_down_feed_large_target_non_feed_active),
     nullptr},
    {"Pull up Feed with targeting Feed active users",
     kFeedPositionAndroid_pull_up_feed_target_feed_active,
     std::size(kFeedPositionAndroid_pull_up_feed_target_feed_active), nullptr},
    {"Pull up Feed with targeting non-Feed active users",
     kFeedPositionAndroid_pull_up_feed_target_non_feed_active,
     std::size(kFeedPositionAndroid_pull_up_feed_target_non_feed_active),
     nullptr},
};

const FeatureEntry::FeatureParam kSearchResumption_use_new_service[] = {
    {"use_new_service", "true"}};
const FeatureEntry::FeatureVariation
    kSearchResumptionModuleAndroidVariations[] = {
        {"Use New Service", kSearchResumption_use_new_service,
         std::size(kSearchResumption_use_new_service), nullptr},
};

const FeatureEntry::FeatureParam kFeatureNotificationGuide_low_engaged[] = {
    {"enable_feature_incognito_tab", "true"},
    {"enable_feature_ntp_suggestion_card", "true"},
    {"enable_feature_voice_search", "true"}};

const FeatureEntry::FeatureParam kFeatureNotificationGuide_default_browser[] = {
    {"enable_feature_default_browser", "true"}};

const FeatureEntry::FeatureVariation kFeatureNotificationGuideVariations[] = {
    {"Low engaged users", kFeatureNotificationGuide_low_engaged,
     std::size(kFeatureNotificationGuide_low_engaged), nullptr},
    {"Default browser", kFeatureNotificationGuide_default_browser,
     std::size(kFeatureNotificationGuide_default_browser), nullptr},
};

const FeatureEntry::FeatureParam
    kNotificationPermissionRationale_show_dialog_next_start[] = {
        {"always_show_rationale_before_requesting_permission", "true"},
        {"permission_request_interval_days", "0"},
};

const FeatureEntry::FeatureVariation
    kNotificationPermissionRationaleVariations[] = {
        {"- Show rationale UI on next startup",
         kNotificationPermissionRationale_show_dialog_next_start,
         std::size(kNotificationPermissionRationale_show_dialog_next_start),
         nullptr},
};

const FeatureEntry::FeatureParam kWebFeed_accelerator[] = {
    {"intro_style", "accelerator"}};

const FeatureEntry::FeatureParam kWebFeed_IPH[] = {{"intro_style", "IPH"}};

const FeatureEntry::FeatureVariation kWebFeedVariations[] = {
    {"accelerator recommendations", kWebFeed_accelerator,
     std::size(kWebFeed_accelerator), nullptr},
    {"IPH recommendations", kWebFeed_IPH, std::size(kWebFeed_IPH), nullptr},
};

const FeatureEntry::FeatureParam kWebFeedAwareness_new_animation[] = {
    {"awareness_style", "new_animation"}};
const FeatureEntry::FeatureParam kWebFeedAwareness_new_animation_no_limit[] = {
    {"awareness_style", "new_animation_no_limit"}};

const FeatureEntry::FeatureParam kWebFeedAwareness_IPH[] = {
    {"awareness_style", "IPH"}};

const FeatureEntry::FeatureVariation kWebFeedAwarenessVariations[] = {
    {"new animation", kWebFeedAwareness_new_animation,
     std::size(kWebFeedAwareness_new_animation), nullptr},
    {"new animation rate limit off", kWebFeedAwareness_new_animation_no_limit,
     std::size(kWebFeedAwareness_new_animation_no_limit), nullptr},
    {"IPH and dot", kWebFeedAwareness_IPH, std::size(kWebFeedAwareness_IPH),
     nullptr},
};

const FeatureEntry::FeatureParam kFeedCloseRefresh_Open[] = {
    {"require_interaction", "false"}};

const FeatureEntry::FeatureParam kFeedCloseRefresh_Interact[] = {
    {"require_interaction", "true"}};

const FeatureEntry::FeatureVariation kFeedCloseRefreshVariations[] = {
    {"Open", kFeedCloseRefresh_Open, std::size(kFeedCloseRefresh_Open),
     nullptr},
    {"Interact", kFeedCloseRefresh_Interact,
     std::size(kFeedCloseRefresh_Interact), nullptr},
};

#endif  // BUILDFLAG(IS_ANDROID)

#if BUILDFLAG(IS_ANDROID)
const FeatureEntry::FeatureParam kAddToHomescreen_UseTextBubble[] = {
    {"use_text_bubble", "true"}};
const FeatureEntry::FeatureParam kAddToHomescreen_UseMessage[] = {
    {"use_message", "true"}};

const FeatureEntry::FeatureVariation kAddToHomescreenIPHVariations[] = {
    {"Use Text Bubble", kAddToHomescreen_UseTextBubble,
     std::size(kAddToHomescreen_UseTextBubble), nullptr},
    {"Use Message", kAddToHomescreen_UseMessage,
     std::size(kAddToHomescreen_UseMessage), nullptr}};
#endif

#if BUILDFLAG(IS_ANDROID)
const FeatureEntry::FeatureParam
    kAutofillUseMobileLabelDisambiguationShowAll[] = {
        {autofill::features::kAutofillUseMobileLabelDisambiguationParameterName,
         autofill::features::
             kAutofillUseMobileLabelDisambiguationParameterShowAll}};
const FeatureEntry::FeatureParam
    kAutofillUseMobileLabelDisambiguationShowOne[] = {
        {autofill::features::kAutofillUseMobileLabelDisambiguationParameterName,
         autofill::features::
             kAutofillUseMobileLabelDisambiguationParameterShowOne}};

const FeatureEntry::FeatureVariation
    kAutofillUseMobileLabelDisambiguationVariations[] = {
        {"(show all)", kAutofillUseMobileLabelDisambiguationShowAll,
         std::size(kAutofillUseMobileLabelDisambiguationShowAll), nullptr},
        {"(show one)", kAutofillUseMobileLabelDisambiguationShowOne,
         std::size(kAutofillUseMobileLabelDisambiguationShowOne), nullptr}};
#endif  // BUILDFLAG(IS_ANDROID)

constexpr FeatureEntry::FeatureParam kStorageAccessAPI_WithPrompt[] = {
    {"storage_access_api_auto_deny_outside_fps", "false"}};

const FeatureEntry::FeatureVariation kStorageAccessAPIVariations[] = {
    {"(with prompt)", kStorageAccessAPI_WithPrompt,
     std::size(kStorageAccessAPI_WithPrompt), nullptr}};

#if BUILDFLAG(IS_ANDROID)
const FeatureEntry::FeatureParam kLensCameraAssistedSearchLensButtonStart[] = {
    {"searchBoxStartVariantForLensCameraAssistedSearch", "true"}};

const FeatureEntry::FeatureParam kLensCameraAssistedSearchLensButtonEnd[] = {
    {"searchBoxStartVariantForLensCameraAssistedSearch", "false"}};

const FeatureEntry::FeatureParam
    kLensCameraAssistedSkipAgsaVersionCheckEnabled[] = {
        {"skipAgsaVersionCheck", "true"}};

const FeatureEntry::FeatureParam
    kLensCameraAssistedSkipAgsaVersionCheckDisabled[] = {
        {"skipAgsaVersionCheck", "false"}};

const FeatureEntry::FeatureParam kLensCameraAssistedSearchOnTablet[] = {
    {"enableCameraAssistedSearchOnTablet", "true"}};

const FeatureEntry::FeatureVariation kLensCameraAssistedSearchVariations[] = {
    {"(Lens then Mic)", kLensCameraAssistedSearchLensButtonStart,
     std::size(kLensCameraAssistedSearchLensButtonStart), nullptr},
    {"(Mic then Lens)", kLensCameraAssistedSearchLensButtonEnd,
     std::size(kLensCameraAssistedSearchLensButtonEnd), nullptr},
    {"(without AGSA version check)",
     kLensCameraAssistedSkipAgsaVersionCheckEnabled,
     std::size(kLensCameraAssistedSkipAgsaVersionCheckEnabled), nullptr},
    {"(with AGSA version check )",
     kLensCameraAssistedSkipAgsaVersionCheckDisabled,
     std::size(kLensCameraAssistedSkipAgsaVersionCheckDisabled), nullptr},
    {"(on Tablet)", kLensCameraAssistedSearchOnTablet,
     std::size(kLensCameraAssistedSearchOnTablet), nullptr}};
#endif  // BUILDFLAG(IS_ANDROID)

const FeatureEntry::Choice kNotificationSchedulerChoices[] = {
    {flags_ui::kGenericExperimentChoiceDefault, "", ""},
    {flag_descriptions::
         kNotificationSchedulerImmediateBackgroundTaskDescription,
     notifications::switches::kNotificationSchedulerImmediateBackgroundTask,
     ""},
};

#if BUILDFLAG(IS_ANDROID)

const FeatureEntry::FeatureParam
    kOmniboxModernizeVisualUpdateExcludeTabletsSmallMargin[] = {
        {"enable_modernize_visual_update_on_tablet", "false"},
        {"modernize_visual_update_active_color_on_omnibox", "true"},
        {"modernize_visual_update_small_bottom_margin", "true"}};

const FeatureEntry::FeatureParam
    kOmniboxModernizeVisualUpdateExcludeTabletsSmallerMarginsMergeClipboard[] =
        {{"enable_modernize_visual_update_on_tablet", "false"},
         {"modernize_visual_update_active_color_on_omnibox", "true"},
         {"modernize_visual_update_smaller_margins", "true"},
         {"modernize_visual_update_merge_clipboard_on_ntp", "true"}};

const FeatureEntry::FeatureParam
    kOmniboxModernizeVisualUpdateExcludeTabletsSmallestMargins[] = {
        {"enable_modernize_visual_update_on_tablet", "false"},
        {"modernize_visual_update_active_color_on_omnibox", "true"},
        {"modernize_visual_update_smallest_margins", "true"},
        {"modernize_visual_update_merge_clipboard_on_ntp", "false"}};

const FeatureEntry::FeatureParam
    kOmniboxModernizeVisualUpdateExcludeTabletsSmallestMarginsMergeClipboard[] =
        {{"enable_modernize_visual_update_on_tablet", "false"},
         {"modernize_visual_update_active_color_on_omnibox", "true"},
         {"modernize_visual_update_smallest_margins", "true"},
         {"modernize_visual_update_merge_clipboard_on_ntp", "true"}};

const FeatureEntry::FeatureParam kOmniboxModernizeVisualUpdateIncludeTablets[] =
    {{"enable_modernize_visual_update_on_tablet", "true"},
     {"modernize_visual_update_active_color_on_omnibox", "true"},
     {"modernize_visual_update_smallest_margins", "true"},
     {"modernize_visual_update_merge_clipboard_on_ntp", "true"}};

const FeatureEntry::FeatureVariation kOmniboxModernizeVisualUpdateVariations[] = {
    {"(Small vertical margin)",
     kOmniboxModernizeVisualUpdateExcludeTabletsSmallMargin,
     std::size(kOmniboxModernizeVisualUpdateExcludeTabletsSmallMargin),
     nullptr},
    {"(Smaller margins, merge clipboard)",
     kOmniboxModernizeVisualUpdateExcludeTabletsSmallerMarginsMergeClipboard,
     std::size(
         kOmniboxModernizeVisualUpdateExcludeTabletsSmallerMarginsMergeClipboard),
     nullptr},
    {"(Smallest margins, merge clipboard)",
     kOmniboxModernizeVisualUpdateExcludeTabletsSmallestMarginsMergeClipboard,
     std::size(
         kOmniboxModernizeVisualUpdateExcludeTabletsSmallestMarginsMergeClipboard),
     nullptr},
    {"(Smallest margins, don't merge clipboard)",
     kOmniboxModernizeVisualUpdateExcludeTabletsSmallestMargins,
     std::size(kOmniboxModernizeVisualUpdateExcludeTabletsSmallestMargins),
     nullptr},
    {"(Tablet)", kOmniboxModernizeVisualUpdateIncludeTablets,
     std::size(kOmniboxModernizeVisualUpdateIncludeTablets), nullptr},
};

const FeatureEntry::FeatureParam kPhotoPickerAdoptionStudyActionGetContent[] = {
    {"use_action_get_content", "true"}};
const FeatureEntry::FeatureParam kPhotoPickerAdoptionStudyActionPickImages[] = {
    {"use_action_pick_images", "true"}};
const FeatureEntry::FeatureParam
    kPhotoPickerAdoptionStudyActionPickImagesPlus[] = {
        {"use_action_pick_images_plus", "true"}};
const FeatureEntry::FeatureParam
    kPhotoPickerAdoptionStudyChromePickerWithoutBrowse[] = {
        {"chrome_picker_suppress_browse", "true"}};

const FeatureEntry::FeatureVariation
    kPhotoPickerAdoptionStudyFeatureVariations[] = {
        {"(Android Picker w/ACTION_GET_CONTENT)",
         kPhotoPickerAdoptionStudyActionGetContent,
         std::size(kPhotoPickerAdoptionStudyActionGetContent), nullptr},
        {"(Android Picker w/ACTION_PICK_IMAGES)",
         kPhotoPickerAdoptionStudyActionPickImages,
         std::size(kPhotoPickerAdoptionStudyActionPickImages), nullptr},
        {"(Android Picker w/ACTION_PICK_IMAGES Plus)",
         kPhotoPickerAdoptionStudyActionPickImagesPlus,
         std::size(kPhotoPickerAdoptionStudyActionPickImagesPlus), nullptr},
        {"(Chrome Picker without Browse)",
         kPhotoPickerAdoptionStudyChromePickerWithoutBrowse,
         std::size(kPhotoPickerAdoptionStudyChromePickerWithoutBrowse),
         nullptr}};

const FeatureEntry::FeatureParam kAuxiliarySearchDonation_MaxDonation_20[] = {
    {chrome::android::kAuxiliarySearchMaxBookmarksCountParam.name, "20"},
    {chrome::android::kAuxiliarySearchMaxTabsCountParam.name, "20"}};
const FeatureEntry::FeatureParam kAuxiliarySearchDonation_MaxDonation_100[] = {
    {chrome::android::kAuxiliarySearchMaxBookmarksCountParam.name, "100"},
    {chrome::android::kAuxiliarySearchMaxTabsCountParam.name, "100"}};
const FeatureEntry::FeatureParam kAuxiliarySearchDonation_MaxDonation_200[] = {
    {chrome::android::kAuxiliarySearchMaxBookmarksCountParam.name, "200"},
    {chrome::android::kAuxiliarySearchMaxTabsCountParam.name, "200"}};
const FeatureEntry::FeatureParam kAuxiliarySearchDonation_MaxDonation_500[] = {
    {chrome::android::kAuxiliarySearchMaxBookmarksCountParam.name, "500"},
    {chrome::android::kAuxiliarySearchMaxTabsCountParam.name, "500"}};
const FeatureEntry::FeatureVariation kAuxiliarySearchDonationVariations[] = {
    {"50 counts", kAuxiliarySearchDonation_MaxDonation_20,
     std::size(kAuxiliarySearchDonation_MaxDonation_20), nullptr},
    {"100 counts", kAuxiliarySearchDonation_MaxDonation_100,
     std::size(kAuxiliarySearchDonation_MaxDonation_100), nullptr},
    {"200 counts", kAuxiliarySearchDonation_MaxDonation_200,
     std::size(kAuxiliarySearchDonation_MaxDonation_200), nullptr},
    {"500 counts", kAuxiliarySearchDonation_MaxDonation_500,
     std::size(kAuxiliarySearchDonation_MaxDonation_500), nullptr},
};

const FeatureEntry::FeatureParam kBoardingPassDetectorUrl_AA[] = {
    {features::kBoardingPassDetectorUrlParamName,
     "https://www.aa.com/checkin/viewMobileBoardingPass"}};
const FeatureEntry::FeatureParam kBoardingPassDetectorUrl_All[] = {
    {features::kBoardingPassDetectorUrlParamName,
     "https://www.aa.com/checkin/viewMobileBoardingPass,https://united.com"}};
const FeatureEntry::FeatureParam kBoardingPassDetectorUrl_Test[] = {
    {features::kBoardingPassDetectorUrlParamName, "http"}};
const FeatureEntry::FeatureVariation kBoardingPassDetectorVariations[] = {
    {"AA", kBoardingPassDetectorUrl_AA, std::size(kBoardingPassDetectorUrl_AA),
     nullptr},
    {"All", kBoardingPassDetectorUrl_All,
     std::size(kBoardingPassDetectorUrl_All), nullptr},
    {"Test", kBoardingPassDetectorUrl_Test,
     std::size(kBoardingPassDetectorUrl_Test), nullptr},
};
#endif  // BUILDFLAG(IS_ANDROID)

// TODO(crbug.com/991082,1015377): Remove after proper support for back/forward
// cache is implemented.
const FeatureEntry::FeatureParam kBackForwardCache_ForceCaching[] = {
    {"TimeToLiveInBackForwardCacheInSeconds", "300"},
    {"should_ignore_blocklists", "true"}};

const FeatureEntry::FeatureVariation kBackForwardCacheVariations[] = {
    {"force caching all pages (experimental)", kBackForwardCache_ForceCaching,
     std::size(kBackForwardCache_ForceCaching), nullptr},
};

const FeatureEntry::FeatureParam kPerformanceSettingsPreloadingSubpage_V1[] = {
    {"use_v2_preloading_subpage", "false"}};

const FeatureEntry::FeatureVariation
    kPerformanceSettingsPreloadingSubpageVariations[] = {
        {"without toggle button", kPerformanceSettingsPreloadingSubpage_V1,
         std::size(kPerformanceSettingsPreloadingSubpage_V1), nullptr},
};

const FeatureEntry::FeatureParam kRenderDocument_Subframe[] = {
    {"level", "subframe"}};
const FeatureEntry::FeatureParam kRenderDocument_AllFrames[] = {
    {"level", "all-frames"}};

const FeatureEntry::FeatureVariation kRenderDocumentVariations[] = {
    {"Swap RenderFrameHosts on same-site navigations from subframes and "
     "crashed frames (experimental)",
     kRenderDocument_Subframe, std::size(kRenderDocument_Subframe), nullptr},
    {"Swap RenderFrameHosts on same-site navigations from any frame "
     "(experimental)",
     kRenderDocument_AllFrames, std::size(kRenderDocument_AllFrames), nullptr},
};

#if BUILDFLAG(IS_ANDROID)
const FeatureEntry::Choice kWebOtpBackendChoices[] = {
    {flags_ui::kGenericExperimentChoiceDefault, "", ""},
    {flag_descriptions::kWebOtpBackendSmsVerification, switches::kWebOtpBackend,
     switches::kWebOtpBackendSmsVerification},
    {flag_descriptions::kWebOtpBackendUserConsent, switches::kWebOtpBackend,
     switches::kWebOtpBackendUserConsent},
    {flag_descriptions::kWebOtpBackendAuto, switches::kWebOtpBackend,
     switches::kWebOtpBackendAuto},
};

const FeatureEntry::Choice kQueryTilesCountryChoices[] = {
    {flags_ui::kGenericExperimentChoiceDefault, "", ""},
    {flag_descriptions::kQueryTilesCountryCodeUS,
     query_tiles::switches::kQueryTilesCountryCode, "US"},
    {flag_descriptions::kQueryTilesCountryCodeIndia,
     query_tiles::switches::kQueryTilesCountryCode, "IN"},
    {flag_descriptions::kQueryTilesCountryCodeBrazil,
     query_tiles::switches::kQueryTilesCountryCode, "BR"},
    {flag_descriptions::kQueryTilesCountryCodeNigeria,
     query_tiles::switches::kQueryTilesCountryCode, "NG"},
    {flag_descriptions::kQueryTilesCountryCodeIndonesia,
     query_tiles::switches::kQueryTilesCountryCode, "ID"},
};

#endif  // BUILDFLAG(IS_ANDROID)

// The choices for --enable-download-warning-improvements. This really should
// just be a SINGLE_VALUE_TYPE, but it is misleading to have the choices be
// labeled "Disabled"/"Enabled". So instead this is made to be a
// MULTI_VALUE_TYPE with choices "Default"/"Enabled".
const FeatureEntry::Choice kDownloadWarningImprovementsChoices[] = {
    {flags_ui::kGenericExperimentChoiceDefault, "", ""},
    {flags_ui::kGenericExperimentChoiceEnabled,
     switches::kEnableDownloadWarningImprovements, ""},
};

// The choices for --enable-experimental-cookie-features. This really should
// just be a SINGLE_VALUE_TYPE, but it is misleading to have the choices be
// labeled "Disabled"/"Enabled". So instead this is made to be a
// MULTI_VALUE_TYPE with choices "Default"/"Enabled".
const FeatureEntry::Choice kEnableExperimentalCookieFeaturesChoices[] = {
    {flags_ui::kGenericExperimentChoiceDefault, "", ""},
    {flags_ui::kGenericExperimentChoiceEnabled,
     switches::kEnableExperimentalCookieFeatures, ""},
};

#if BUILDFLAG(IS_CHROMEOS_ASH)
const FeatureEntry::Choice kFrameThrottleFpsChoices[] = {
    {flag_descriptions::kFrameThrottleFpsDefault, "", ""},
    {flag_descriptions::kFrameThrottleFps5, ash::switches::kFrameThrottleFps,
     "5"},
    {flag_descriptions::kFrameThrottleFps10, ash::switches::kFrameThrottleFps,
     "10"},
    {flag_descriptions::kFrameThrottleFps15, ash::switches::kFrameThrottleFps,
     "15"},
    {flag_descriptions::kFrameThrottleFps20, ash::switches::kFrameThrottleFps,
     "20"},
    {flag_descriptions::kFrameThrottleFps25, ash::switches::kFrameThrottleFps,
     "25"},
    {flag_descriptions::kFrameThrottleFps30, ash::switches::kFrameThrottleFps,
     "30"}};
#endif  // BUILDFLAG(IS_CHROMEOS_ASH)

const FeatureEntry::FeatureParam kDrawPredictedPointExperiment1Point12Ms[] = {
    {"predicted_points", features::kDraw1Point12Ms}};
const FeatureEntry::FeatureParam kDrawPredictedPointExperiment2Points6Ms[] = {
    {"predicted_points", features::kDraw2Points6Ms}};
const FeatureEntry::FeatureParam kDrawPredictedPointExperiment1Point6Ms[] = {
    {"predicted_points", features::kDraw1Point6Ms}};
const FeatureEntry::FeatureParam kDrawPredictedPointExperiment2Points3Ms[] = {
    {"predicted_points", features::kDraw2Points3Ms}};

const FeatureEntry::FeatureVariation kDrawPredictedPointVariations[] = {
    {flag_descriptions::kDraw1PredictedPoint12Ms,
     kDrawPredictedPointExperiment1Point12Ms,
     std::size(kDrawPredictedPointExperiment1Point12Ms), nullptr},
    {flag_descriptions::kDraw2PredictedPoints6Ms,
     kDrawPredictedPointExperiment2Points6Ms,
     std::size(kDrawPredictedPointExperiment2Points6Ms), nullptr},
    {flag_descriptions::kDraw1PredictedPoint6Ms,
     kDrawPredictedPointExperiment1Point6Ms,
     std::size(kDrawPredictedPointExperiment1Point6Ms), nullptr},
    {flag_descriptions::kDraw2PredictedPoints3Ms,
     kDrawPredictedPointExperiment2Points3Ms,
     std::size(kDrawPredictedPointExperiment2Points3Ms), nullptr}};

#if BUILDFLAG(IS_CHROMEOS_ASH)
const FeatureEntry::Choice kForceControlFaceAeChoices[] = {
    {"Default", "", ""},
    {"Enable", media::switches::kForceControlFaceAe, "enable"},
    {"Disable", media::switches::kForceControlFaceAe, "disable"}};

const FeatureEntry::Choice kAutoFramingOverrideChoices[] = {
    {"Default", "", ""},
    {"Force enabled", media::switches::kAutoFramingOverride,
     media::switches::kAutoFramingForceEnabled},
    {"Force disabled", media::switches::kAutoFramingOverride,
     media::switches::kAutoFramingForceDisabled}};
#endif

#if BUILDFLAG(IS_CHROMEOS_ASH)
const FeatureEntry::Choice kCrostiniContainerChoices[] = {
    {"Default", "", ""},
    {"Buster", crostini::kCrostiniContainerFlag, "buster"},
    {"Bullseye", crostini::kCrostiniContainerFlag, "bullseye"},
    {"Bookworm", crostini::kCrostiniContainerFlag, "bookworm"},
};
#endif  // BUILDFLAG(IS_CHROMEOS_ASH)

#if !BUILDFLAG(IS_ANDROID)
// SCT Auditing feature variations.
const FeatureEntry::FeatureParam kSCTAuditingSamplingRateNone[] = {
    {"sampling_rate", "0.0"}};
const FeatureEntry::FeatureParam kSCTAuditingSamplingRateAlternativeOne[] = {
    {"sampling_rate", "0.0001"}};
const FeatureEntry::FeatureParam kSCTAuditingSamplingRateAlternativeTwo[] = {
    {"sampling_rate", "0.001"}};

const FeatureEntry::FeatureVariation kSCTAuditingVariations[] = {
    {"Sampling rate 0%", kSCTAuditingSamplingRateNone,
     std::size(kSCTAuditingSamplingRateNone), nullptr},
    {"Sampling rate 0.01%", kSCTAuditingSamplingRateAlternativeOne,
     std::size(kSCTAuditingSamplingRateAlternativeOne), nullptr},
    {"Sampling rate 0.1%", kSCTAuditingSamplingRateAlternativeTwo,
     std::size(kSCTAuditingSamplingRateAlternativeTwo), nullptr},
};
#endif  // !BUILDFLAG(IS_ANDROID)

#if BUILDFLAG(IS_ANDROID)
// The variations of ContentLanguagesInLanguagePicker.
const FeatureEntry::FeatureParam
    kContentLanguagesInLanguagePickerDisableObservers[] = {
        {language::kContentLanguagesDisableObserversParam, "true"}};

const FeatureEntry::FeatureVariation
    kContentLanguagesInLanguaePickerVariations[] = {
        {"Without observers", kContentLanguagesInLanguagePickerDisableObservers,
         std::size(kContentLanguagesInLanguagePickerDisableObservers), nullptr},
};
#endif  // BUILDFLAG(IS_ANDROID)

#if BUILDFLAG(IS_ANDROID)
// The variations of TranslateMessageUI
const FeatureEntry::FeatureParam kTranslateMessageUISnackbar[] = {
    {translate::kTranslateMessageUISnackbarParam, "true"}};

const FeatureEntry::FeatureVariation kTranslateMessageUIVariations[] = {
    {"With Snackbar", kTranslateMessageUISnackbar,
     std::size(kTranslateMessageUISnackbar), nullptr}};
#endif  // BUILDFLAG(IS_ANDROID)

#if BUILDFLAG(IS_CHROMEOS_ASH)
const FeatureEntry::FeatureParam kProductivityLauncher_WithoutContinue[] = {
    {"enable_continue", "false"}};

const FeatureEntry::FeatureVariation kProductivityLauncherVariations[] = {
    {"without Continue", kProductivityLauncher_WithoutContinue,
     std::size(kProductivityLauncher_WithoutContinue), nullptr}};

const FeatureEntry::FeatureParam kLauncherItemSuggest_LongDelay10Mins[] = {
    {"long_delay_minutes", "10"}};
const FeatureEntry::FeatureParam kLauncherItemSuggest_LongDelay12Hours[] = {
    {"long_delay_minutes", "720"}};
const FeatureEntry::FeatureParam kLauncherItemSuggest_LongDelay24Hours[] = {
    {"long_delay_minutes", "1440"}};
const FeatureEntry::FeatureParam kLauncherItemSuggest_LongDelay36Hours[] = {
    {"long_delay_minutes", "2160"}};
const FeatureEntry::FeatureParam kLauncherItemSuggest_LongDelay48Hours[] = {
    {"long_delay_minutes", "2880"}};
const FeatureEntry::FeatureParam kLauncherItemSuggest_LongDelay60Hours[] = {
    {"long_delay_minutes", "3600"}};
const FeatureEntry::FeatureParam kLauncherItemSuggest_LongDelay72Hours[] = {
    {"long_delay_minutes", "4320"}};

const FeatureEntry::FeatureVariation kLauncherItemSuggestVariations[] = {
    {"with 10 minute long delay", kLauncherItemSuggest_LongDelay10Mins,
     std::size(kLauncherItemSuggest_LongDelay10Mins), nullptr},
    {"with 12 hour long delay", kLauncherItemSuggest_LongDelay12Hours,
     std::size(kLauncherItemSuggest_LongDelay12Hours), nullptr},
    {"with 24 hour long delay", kLauncherItemSuggest_LongDelay24Hours,
     std::size(kLauncherItemSuggest_LongDelay24Hours), nullptr},
    {"with 36 hour long delay", kLauncherItemSuggest_LongDelay36Hours,
     std::size(kLauncherItemSuggest_LongDelay36Hours), nullptr},
    {"with 48 hour long delay", kLauncherItemSuggest_LongDelay48Hours,
     std::size(kLauncherItemSuggest_LongDelay48Hours), nullptr},
    {"with 60 hour long delay", kLauncherItemSuggest_LongDelay60Hours,
     std::size(kLauncherItemSuggest_LongDelay60Hours), nullptr},
    {"with 72 hour long delay", kLauncherItemSuggest_LongDelay72Hours,
     std::size(kLauncherItemSuggest_LongDelay72Hours), nullptr}};

const FeatureEntry::FeatureParam kEolIncentiveOffer[] = {
    {"incentive_type", "offer"}};
const FeatureEntry::FeatureParam kEolIncentiveNoOffer[] = {
    {"incentive_type", "no_offer"}};

const FeatureEntry::FeatureVariation kEolIncentiveVariations[] = {
    {"with offer", kEolIncentiveOffer, std::size(kEolIncentiveOffer), nullptr},
    {"with no offer", kEolIncentiveNoOffer, std::size(kEolIncentiveNoOffer),
     nullptr}};

const FeatureEntry::FeatureParam kCaptureModeEducationShortcutNudge[] = {
    {"CaptureModeEducationParam", "ShortcutNudge"}};
const FeatureEntry::FeatureParam kCaptureModeEducationShortcutTutorial[] = {
    {"CaptureModeEducationParam", "ShortcutTutorial"}};
const FeatureEntry::FeatureParam kCaptureModeEducationQuickSettingsNudge[] = {
    {"CaptureModeEducationParam", "QuickSettingsNudge"}};

const FeatureEntry::FeatureVariation kCaptureModeEducationVariations[] = {
    {"Shortcut Nudge", kCaptureModeEducationShortcutNudge,
     std::size(kCaptureModeEducationShortcutNudge), nullptr},
    {"Shortcut Tutorial", kCaptureModeEducationShortcutTutorial,
     std::size(kCaptureModeEducationShortcutTutorial), nullptr},
    {"Quick Settings Nudge", kCaptureModeEducationQuickSettingsNudge,
     std::size(kCaptureModeEducationQuickSettingsNudge), nullptr}};

#endif  // BUILDFLAG(IS_CHROMEOS_ASH)

#if BUILDFLAG(IS_CHROMEOS)
constexpr char kTaskManagerEndProcessDisabledForExtensionInternalName[] =
    "enable-task-manager-end-process-disabled-for-extension";
#endif  // BUILDFLAG(IS_CHROMEOS)

#if BUILDFLAG(IS_CHROMEOS_ASH)
constexpr char kWallpaperFastRefreshInternalName[] = "wallpaper-fast-refresh";
constexpr char kWallpaperGooglePhotosSharedAlbumsInternalName[] =
    "wallpaper-google-photos-shared-albums";
constexpr char kWallpaperPerDeskName[] = "per-desk-wallpaper";
constexpr char kTimeOfDayWallpaperInternalName[] = "time-of-day-wallpaper";
constexpr char kTimeOfDayWallpaperForcedAutoScheduleInternalName[] =
    "time-of-day-wallpaper-forced-auto-schedule";
constexpr char kTimeOfDayScreenSaverInternalName[] = "time-of-day-screen-saver";
constexpr char kTimeOfDayDlcInternalName[] = "time-of-day-dlc";
constexpr char kGlanceablesV2InternalName[] = "glanceables-v2";
constexpr char kGlanceablesV2CalendarViewInternalName[] =
    "glanceables-v2-calendar-view";
#endif  // BUILDFLAG(IS_CHROMEOS_ASH)

#if BUILDFLAG(IS_CHROMEOS_ASH)
constexpr char kBorealisBigGlInternalName[] = "borealis-big-gl";
constexpr char kBorealisDGPUInternalName[] = "borealis-dgpu";
constexpr char kBorealisForceBetaClientInternalName[] =
    "borealis-force-beta-client";
constexpr char kBorealisForceDoubleScaleInternalName[] =
    "borealis-force-double-scale";
constexpr char kBorealisLinuxModeInternalName[] = "borealis-linux-mode";
// This differs slightly from its symbol's name since "enabled" is used
// internally to refer to whether borealis is installed or not.
constexpr char kBorealisPermittedInternalName[] = "borealis-enabled";
constexpr char kBorealisProvisionInternalName[] = "borealis-provision";
constexpr char kBorealisScaleClientByDPIInternalName[] =
    "borealis-scale-client-by-dpi";
constexpr char kBorealisWebUIInstallerInternalName[] =
    "borealis-web-ui-installer";
constexpr char kClipboardHistoryLongpressInternalName[] =
    "clipboard-history-longpress";
constexpr char kClipboardHistoryRefreshInternalName[] =
    "clipboard-history-refresh";
constexpr char kClipboardHistoryUrlTitlesInternalName[] =
    "clipboard-history-url-titles";
constexpr char kBluetoothUseFlossInternalName[] = "bluetooth-use-floss";
constexpr char kEnableSuspendToDiskInternalName[] = "enable-suspend-to-disk";
#endif  // BUILDFLAG(IS_CHROMEOS_ASH)

#if BUILDFLAG(IS_ANDROID)
const FeatureEntry::FeatureParam kBookmarksRefreshVisuals[] = {
    {"bookmark_visuals_enabled", "true"}};
const FeatureEntry::FeatureParam kBookmarksRefreshCompactVisuals[] = {
    {"bookmark_visuals_enabled", "true"},
    {"bookmark_compact_visuals_enabled", "true"}};
const FeatureEntry::FeatureParam kBookmarksRefreshAppMenu[] = {
    {"bookmark_in_app_menu", "true"}};
const FeatureEntry::FeatureParam kBookmarksRefreshNormal[] = {
    {"bookmark_visuals_enabled", "true"},
    {"bookmark_in_app_menu", "true"}};
const FeatureEntry::FeatureParam kBookmarksRefreshCompact[] = {
    {"bookmark_visuals_enabled", "true"},
    {"bookmark_compact_visuals_enabled", "true"},
    {"bookmark_in_app_menu", "true"}};

const FeatureEntry::FeatureVariation kBookmarksRefreshVariations[] = {
    {"(enabled w/ visuals)", kBookmarksRefreshNormal,
     std::size(kBookmarksRefreshNormal), nullptr},
    {"(enabled w/ compact visuals)", kBookmarksRefreshCompact,
     std::size(kBookmarksRefreshCompact), nullptr},
    {"(visuals only)", kBookmarksRefreshVisuals,
     std::size(kBookmarksRefreshVisuals), nullptr},
    {"(compact visuals only)", kBookmarksRefreshCompactVisuals,
     std::size(kBookmarksRefreshCompactVisuals), nullptr},
    {"(app menu item only)", kBookmarksRefreshAppMenu,
     std::size(kBookmarksRefreshAppMenu), nullptr}};
#endif  // BUILDFLAG(IS_ANDROID)

const FeatureEntry::FeatureParam kLargeFaviconFromGoogle96[] = {
    {"favicon_size_in_dip", "96"}};
const FeatureEntry::FeatureParam kLargeFaviconFromGoogle128[] = {
    {"favicon_size_in_dip", "128"}};

const FeatureEntry::FeatureVariation kLargeFaviconFromGoogleVariations[] = {
    {"(96dip)", kLargeFaviconFromGoogle96, std::size(kLargeFaviconFromGoogle96),
     nullptr},
    {"(128dip)", kLargeFaviconFromGoogle128,
     std::size(kLargeFaviconFromGoogle128), nullptr}};

#if BUILDFLAG(IS_ANDROID)
const FeatureEntry::FeatureParam kRestoreTabsOnFRE_skipFeatureEngagement[] = {
    {"skip_feature_engagement", "true"}};
const FeatureEntry::FeatureVariation kRestoreTabsOnFREVariations[] = {
    {"- skip feature engagement", kRestoreTabsOnFRE_skipFeatureEngagement,
     std::size(kRestoreTabsOnFRE_skipFeatureEngagement), nullptr},
};
#endif  // BUILDFLAG(IS_ANDROID)

#if !BUILDFLAG(IS_ANDROID) && BUILDFLAG(GOOGLE_CHROME_BRANDING)
constexpr FeatureEntry::FeatureParam kIOSPromoPasswordBubbleContextualDirect[] =
    {{"activation", "contextual-direct"}};
constexpr FeatureEntry::FeatureParam
    kIOSPromoPasswordBubbleContextualIndirect[] = {
        {"activation", "contextual-indirect"}};
constexpr FeatureEntry::FeatureParam
    kIOSPromoPasswordBubbleNonContextualDirect[] = {
        {"activation", "non-contextual-direct"}};
constexpr FeatureEntry::FeatureParam
    kIOSPromoPasswordBubbleNonContextualIndirect[] = {
        {"activation", "non-contextual-indirect"}};
constexpr FeatureEntry::FeatureParam kIOSPromoPasswordBubbleAlwaysShowDirect[] =
    {{"activation", "always-show-direct"}};
constexpr FeatureEntry::FeatureParam
    kIOSPromoPasswordBubbleAlwaysShowIndirect[] = {
        {"activation", "always-show-indirect"}};

constexpr FeatureEntry::FeatureVariation kIOSPromoPasswordBubbleVariations[] = {
    {"contextual & direct activation", kIOSPromoPasswordBubbleContextualDirect,
     std::size(kIOSPromoPasswordBubbleContextualDirect), nullptr},
    {"contextual & indirect activation",
     kIOSPromoPasswordBubbleContextualIndirect,
     std::size(kIOSPromoPasswordBubbleContextualIndirect), nullptr},
    {"non-contextual & direct activation",
     kIOSPromoPasswordBubbleNonContextualDirect,
     std::size(kIOSPromoPasswordBubbleNonContextualDirect), nullptr},
    {"non-contextual & indirect activation",
     kIOSPromoPasswordBubbleNonContextualIndirect,
     std::size(kIOSPromoPasswordBubbleNonContextualIndirect), nullptr},
    {"always show direct activation", kIOSPromoPasswordBubbleAlwaysShowDirect,
     std::size(kIOSPromoPasswordBubbleAlwaysShowDirect), nullptr},
    {"always show indirect activation",
     kIOSPromoPasswordBubbleAlwaysShowIndirect,
     std::size(kIOSPromoPasswordBubbleAlwaysShowIndirect), nullptr}};
#endif

const FeatureEntry::FeatureParam kUnthrottledNestedTimeout_NestingLevel = {
    "nesting", "15"};

const FeatureEntry::FeatureVariation kUnthrottledNestedTimeout_Variations[] = {
    {"15", &kUnthrottledNestedTimeout_NestingLevel, 1, nullptr},
};

constexpr FeatureEntry::FeatureParam kLensFormatOptimizationWebp[] = {
    {"use-webp-for-image-search", "true"},
    {"use-jpeg-for-image-search", "false"}};
constexpr FeatureEntry::FeatureVariation kLensImageFormatVariations[] = {
    {"use Webp", kLensFormatOptimizationWebp,
     std::size(kLensFormatOptimizationWebp), nullptr},
};

constexpr FeatureEntry::FeatureParam kPingLensSequentially[] = {
    {"ping-lens-sequentially", "true"}};
constexpr FeatureEntry::FeatureParam kLensPingURL[] = {
    {"lens-ping-url", "https://lens.google.com/_/LensWebStandaloneUi/gen204/"}};
constexpr FeatureEntry::FeatureVariation kLensPingVariations[] = {
    {"ping sequentially", kPingLensSequentially,
     std::size(kPingLensSequentially), nullptr},
    {"ping url", kLensPingURL, std::size(kLensPingURL), nullptr},
};

#if BUILDFLAG(ENABLE_LENS_DESKTOP_GOOGLE_BRANDED_FEATURES)
constexpr FeatureEntry::FeatureParam kCscStagingEnvVariation[] = {
    {"companion-homepage-url",
     "https://lens-staging.corp.google.com/companion"},
    {"companion-image-upload-url",
     "https://lens-staging.corp.google.com/v2/upload"}};
constexpr FeatureEntry::FeatureParam kCscClobberVariation[] = {
    {"open-links-in-current-tab", "true"},
};
constexpr FeatureEntry::FeatureParam kCscNewTabVariation[] = {
    {"open-links-in-current-tab", "false"},
};

constexpr FeatureEntry::FeatureVariation kSidePanelCompanionVariations[] = {
    {"with staging URL", kCscStagingEnvVariation,
     std::size(kCscStagingEnvVariation), nullptr},
    {"with clobber", kCscClobberVariation, std::size(kCscClobberVariation),
     nullptr},
    {"with new tab", kCscNewTabVariation, std::size(kCscNewTabVariation),
     nullptr},
};

const FeatureEntry::Choice kForceCompanionPinnedStateChoices[] = {
    {flags_ui::kGenericExperimentChoiceDefault, "", ""},
    {"Forced Pinned", companion::switches::kForceCompanionPinnedState,
     "pinned"},
    {"Forced Unpinned", companion::switches::kForceCompanionPinnedState,
     "unpinned"},
};
#endif  // BUILDFLAG(ENABLE_LENS_DESKTOP_GOOGLE_BRANDED_FEATURES)

#if BUILDFLAG(IS_CHROMEOS_ASH)
const FeatureEntry::Choice kAlwaysEnableHdcpChoices[] = {
    {flag_descriptions::kAlwaysEnableHdcpDefault, "", ""},
    {flag_descriptions::kAlwaysEnableHdcpType0,
     ash::switches::kAlwaysEnableHdcp, "type0"},
    {flag_descriptions::kAlwaysEnableHdcpType1,
     ash::switches::kAlwaysEnableHdcp, "type1"},
};

const FeatureEntry::Choice kPrintingPpdChannelChoices[] = {
    {flags_ui::kGenericExperimentChoiceDefault, "", ""},
    {ash::switches::kPrintingPpdChannelProduction,
     ash::switches::kPrintingPpdChannel,
     ash::switches::kPrintingPpdChannelProduction},
    {ash::switches::kPrintingPpdChannelStaging,
     ash::switches::kPrintingPpdChannel,
     ash::switches::kPrintingPpdChannelStaging},
    {ash::switches::kPrintingPpdChannelDev, ash::switches::kPrintingPpdChannel,
     ash::switches::kPrintingPpdChannelDev},
    {ash::switches::kPrintingPpdChannelLocalhost,
     ash::switches::kPrintingPpdChannel,
     ash::switches::kPrintingPpdChannelLocalhost}};
#endif  // BUILDFLAG(IS_CHROMEOS_ASH)

// Feature variations for kIsolateSandboxedIframes.
#if !BUILDFLAG(IS_ANDROID)
// TODO(wjmaclean): Add FeatureParams for a per-frame grouping when support
// for it is added.
const FeatureEntry::FeatureParam kIsolateSandboxedIframesGroupingPerSite{
    "grouping", "per-site"};
const FeatureEntry::FeatureParam kIsolateSandboxedIframesGroupingPerOrigin{
    "grouping", "per-origin"};
const FeatureEntry::FeatureParam kIsolateSandboxedIframesGroupingPerDocument{
    "grouping", "per-document"};
const FeatureEntry::FeatureVariation
    kIsolateSandboxedIframesGroupingVariations[] = {
        {"with grouping by URL's site",
         &kIsolateSandboxedIframesGroupingPerSite, 1, nullptr},
        {"with grouping by URL's origin",
         &kIsolateSandboxedIframesGroupingPerOrigin, 1, nullptr},
        {"with each sandboxed frame document in its own process",
         &kIsolateSandboxedIframesGroupingPerDocument, 1, nullptr},
};
#endif  // !BUILDFLAG(IS_ANDROID)

const FeatureEntry::FeatureParam
    kServiceWorkerSkipIgnorableFetchHandler_NotSkip[] = {
        {"SkipEmptyFetchHandler", "false"}};
const FeatureEntry::FeatureParam
    kServiceWorkerSkipIgnorableFetchHandler_SkipEmpty[] = {
        {"SkipEmptyFetchHandler", "true"}};
const FeatureEntry::FeatureVariation
    kServiceWorkerSkipIgnorableFetchHandlerVariations[] = {
        {"Not Skip", kServiceWorkerSkipIgnorableFetchHandler_NotSkip,
         std::size(kServiceWorkerSkipIgnorableFetchHandler_NotSkip), nullptr},
        {"Skip Empty", kServiceWorkerSkipIgnorableFetchHandler_SkipEmpty,
         std::size(kServiceWorkerSkipIgnorableFetchHandler_SkipEmpty), nullptr},
};

const FeatureEntry::FeatureParam
    kServiceWorkerBypassFetchHandler_MainResource[] = {
        {"bypass_for", "main_resource"},
        {"strategy", "optin"}};
const FeatureEntry::FeatureParam
    kServiceWorkerBypassFetchHandler_RaceNetworkRequest[] = {
        {"bypass_for", "all_with_race_network_request"},
        {"strategy", "optin"}};
const FeatureEntry::FeatureParam
    kServiceWorkerBypassFetchHandler_Subresource[] = {
        {"bypass_for", "sub_resource"},
        {"strategy", "optin"}};
const FeatureEntry::FeatureVariation
    kServiceWorkerBypassFetchHandlerVariations[] = {
        {"Main Resource", kServiceWorkerBypassFetchHandler_MainResource,
         std::size(kServiceWorkerBypassFetchHandler_MainResource), nullptr},
        {"Subesource", kServiceWorkerBypassFetchHandler_Subresource,
         std::size(kServiceWorkerBypassFetchHandler_Subresource), nullptr},
        {"Race Network Request and Fetch Handler",
         kServiceWorkerBypassFetchHandler_RaceNetworkRequest,
         std::size(kServiceWorkerBypassFetchHandler_RaceNetworkRequest),
         nullptr},
};

#if !BUILDFLAG(IS_ANDROID)

const FeatureEntry::FeatureParam
    kHighEfficiencyMultistateModeShowRecommendedBadge[] = {
        {"show_recommended_badge", "true"},
};
const FeatureEntry::FeatureVariation kHighEfficiencyMultistateModeVariations[] =
    {
        {"With Recommended Badge",
         kHighEfficiencyMultistateModeShowRecommendedBadge,
         std::size(kHighEfficiencyMultistateModeShowRecommendedBadge), nullptr},
};

const FeatureEntry::FeatureParam kDiscardedTabTreatmentWithRing30Opacity[] = {
    {"discard_tab_treatment_option", "2"},
    {"discard_tab_treatment_opacity", "0.3"}};
const FeatureEntry::FeatureParam kDiscardedTabTreatmentWithRing40Opacity[] = {
    {"discard_tab_treatment_option", "2"},
    {"discard_tab_treatment_opacity", "0.4"}};
const FeatureEntry::FeatureParam kDiscardedTabTreatmentWithRing50Opacity[] = {
    {"discard_tab_treatment_option", "2"},
    {"discard_tab_treatment_opacity", "0.5"}};
const FeatureEntry::FeatureParam kDiscardedTabTreatmentWithRing75Opacity[] = {
    {"discard_tab_treatment_option", "2"},
    {"discard_tab_treatment_opacity", "0.75"}};
const FeatureEntry::FeatureParam kDiscardedTabTreatmentWithRing90Opacity[] = {
    {"discard_tab_treatment_option", "2"},
    {"discard_tab_treatment_opacity", "0.9"}};
const FeatureEntry::FeatureParam kDiscardedTabTreatmentWithRing100Opacity[] = {
    {"discard_tab_treatment_option", "2"},
    {"discard_tab_treatment_opacity", "1.0"}};
const FeatureEntry::FeatureParam kDiscardedTabTreatmentWithoutRing30Opacity[] =
    {{"discard_tab_treatment_option", "1"},
     {"discard_tab_treatment_opacity", "0.3"}};
const FeatureEntry::FeatureParam kDiscardedTabTreatmentWithoutRing40Opacity[] =
    {{"discard_tab_treatment_option", "1"},
     {"discard_tab_treatment_opacity", "0.4"}};
const FeatureEntry::FeatureParam kDiscardedTabTreatmentWithoutRing50Opacity[] =
    {{"discard_tab_treatment_option", "1"},
     {"discard_tab_treatment_opacity", "0.5"}};

const FeatureEntry::FeatureVariation kDiscardedTabTreatmentVariations[] = {
    {"With Ring and 30% Opacity", kDiscardedTabTreatmentWithRing30Opacity,
     std::size(kDiscardedTabTreatmentWithRing30Opacity), nullptr},
    {"With Ring and 40% Opacity", kDiscardedTabTreatmentWithRing40Opacity,
     std::size(kDiscardedTabTreatmentWithRing40Opacity), nullptr},
    {"With Ring and 50% Opacity", kDiscardedTabTreatmentWithRing50Opacity,
     std::size(kDiscardedTabTreatmentWithRing50Opacity), nullptr},
    {"With Ring and 75% Opacity", kDiscardedTabTreatmentWithRing75Opacity,
     std::size(kDiscardedTabTreatmentWithRing75Opacity), nullptr},
    {"With Ring and 90% Opacity", kDiscardedTabTreatmentWithRing90Opacity,
     std::size(kDiscardedTabTreatmentWithRing90Opacity), nullptr},
    {"With Ring and 100% Opacity", kDiscardedTabTreatmentWithRing100Opacity,
     std::size(kDiscardedTabTreatmentWithRing100Opacity), nullptr},
    {"Without Ring and 30% Opacity", kDiscardedTabTreatmentWithoutRing30Opacity,
     std::size(kDiscardedTabTreatmentWithoutRing30Opacity), nullptr},
    {"Without Ring and 40% Opacity", kDiscardedTabTreatmentWithoutRing40Opacity,
     std::size(kDiscardedTabTreatmentWithoutRing40Opacity), nullptr},
    {"Without Ring and 50% Opacity", kDiscardedTabTreatmentWithoutRing50Opacity,
     std::size(kDiscardedTabTreatmentWithoutRing50Opacity), nullptr},
};

const FeatureEntry::FeatureParam
    kMemoryUsageInHovercardsUpdateTriggerVariations[] = {
        {"memory_update_trigger", "navigation"},
};
const FeatureEntry::FeatureVariation kMemoryUsageInHovercardsVariations[] = {
    {"With Update on Navigation",
     kMemoryUsageInHovercardsUpdateTriggerVariations,
     std::size(kMemoryUsageInHovercardsUpdateTriggerVariations), nullptr},
};

const FeatureEntry::FeatureParam kMemorySavingsReportingFrequent[] = {
    // 100 * 1024 * 1024
    {"expanded_high_efficiency_chip_threshold_bytes", "104857600"},
    {"expanded_high_efficiency_chip_frequency", "2h"},
    {"expanded_high_efficiency_chip_discarded_duration", "1h"}};

const FeatureEntry::FeatureParam kMemorySavingsReportingInfrequent[] = {
    // 200 * 1024 * 1024
    {"expanded_high_efficiency_chip_threshold_bytes", "209715200"},
    {"expanded_high_efficiency_chip_frequency", "1d"},
    {"expanded_high_efficiency_chip_discarded_duration", "6h"}};

const FeatureEntry::FeatureParam kMemorySavingsReportingVeryFrequent[] = {
    // 100 * 1024 * 1024
    {"expanded_high_efficiency_chip_threshold_bytes", "104857600"},
    {"expanded_high_efficiency_chip_frequency", "5m"},
    {"expanded_high_efficiency_chip_discarded_duration", "5m"}};

const FeatureEntry::FeatureVariation
    kHighEfficiencyMemorySavingsReportingVariations[] = {
        {"With Frequent Memory Savings Reporting",
         kMemorySavingsReportingFrequent,
         std::size(kMemorySavingsReportingFrequent), nullptr},
        {"With Infrequent Memory Savings Reporting",
         kMemorySavingsReportingInfrequent,
         std::size(kMemorySavingsReportingInfrequent), nullptr},
        {"With Very Frequent Memory Savings Reporting",
         kMemorySavingsReportingVeryFrequent,
         std::size(kMemorySavingsReportingVeryFrequent), nullptr},
};

const FeatureEntry::FeatureParam kProbabilisticMemorySaverSimulated[] = {
    {"proactive_discarding_target_false_positive_percent", "35"},
    {"proactive_discarding_sampling_interval", "10m"},
    {"proactive_discarding_simulation_mode", "true"},
};

const FeatureEntry::FeatureParam kProbabilisticMemorySaverActive[] = {
    {"proactive_discarding_target_false_positive_percent", "35"},
    {"proactive_discarding_sampling_interval", "10m"},
    {"proactive_discarding_simulation_mode", "false"},
};

const FeatureEntry::FeatureVariation kProbabilisticMemorySaverMode[] = {
    {"Simulated", kProbabilisticMemorySaverSimulated,
     std::size(kProbabilisticMemorySaverSimulated), nullptr},
    {"Active", kProbabilisticMemorySaverActive,
     std::size(kProbabilisticMemorySaverActive), nullptr},
};

const FeatureEntry::FeatureParam kModalMemorySaverConservative[] = {
    {"modal_memory_saver_mode", "1"}};
const FeatureEntry::FeatureParam kModalMemorySaverMedium[] = {
    {"modal_memory_saver_mode", "2"}};
const FeatureEntry::FeatureParam kModalMemorySaverAggressive[] = {
    {"modal_memory_saver_mode", "3"}};

const FeatureEntry::FeatureVariation kModalMemorySaverMode[] = {
    {"Conservative (6 hours, only tabs visited < 5 times)",
     kModalMemorySaverConservative, std::size(kModalMemorySaverConservative),
     nullptr},
    {"Medium (4 hours, only tabs visited < 15 times)", kModalMemorySaverMedium,
     std::size(kModalMemorySaverMedium), nullptr},
    {"Aggressive (2 hours, only tabs visited < 15 times)",
     kModalMemorySaverAggressive, std::size(kModalMemorySaverAggressive),
     nullptr},
};
#endif  // !BUILDFLAG(IS_ANDROID)

#if BUILDFLAG(IS_CHROMEOS_ASH)
const FeatureEntry::FeatureParam kGalleryAppPdfEditNotificationEditAndSign[] = {
    {"text", "Edit and Sign"}};
const FeatureEntry::FeatureParam
    kGalleryAppPdfEditNotificationOpenWithGalleryApp[] = {
        {"text", "Open with Gallery app"}};
const FeatureEntry::FeatureVariation
    kGalleryAppPdfEditNotificationVariations[] = {
        {"Edit and Sign", kGalleryAppPdfEditNotificationEditAndSign,
         std::size(kGalleryAppPdfEditNotificationEditAndSign), nullptr},
        {"Open with Gallery app",
         kGalleryAppPdfEditNotificationOpenWithGalleryApp,
         std::size(kGalleryAppPdfEditNotificationOpenWithGalleryApp), nullptr}};
#endif

#if !BUILDFLAG(IS_ANDROID)
const FeatureEntry::FeatureParam kOsIntegrationSubManagersWriteConfig[] = {
    {"stage", "write_config"}};
const FeatureEntry::FeatureParam
    kOsIntegrationSubManagersExecuteAndWriteConfig[] = {
        {"stage", "execute_and_write_config"}};

const FeatureEntry::FeatureVariation
    kOsIntegrationSubManagersConfigVariations[] = {
        {"Write Config only", kOsIntegrationSubManagersWriteConfig,
         std::size(kOsIntegrationSubManagersWriteConfig), nullptr},
        {"Execute and Write Config",
         kOsIntegrationSubManagersExecuteAndWriteConfig,
         std::size(kOsIntegrationSubManagersExecuteAndWriteConfig), nullptr}};
#endif  // !BUILDFLAG(IS_ANDROID)

const FeatureEntry::FeatureParam kWebRtcApmDownmixMethodAverage[] = {
    {"method", "average"}};
const FeatureEntry::FeatureParam kWebRtcApmDownmixMethodFirstChannel[] = {
    {"method", "first"}};
const FeatureEntry::FeatureVariation kWebRtcApmDownmixMethodVariations[] = {
    {"- Average all the input channels", kWebRtcApmDownmixMethodAverage,
     std::size(kWebRtcApmDownmixMethodAverage), nullptr},
    {"- Use first channel", kWebRtcApmDownmixMethodFirstChannel,
     std::size(kWebRtcApmDownmixMethodFirstChannel), nullptr}};

#if !BUILDFLAG(IS_ANDROID)
const FeatureEntry::FeatureParam
    kSafetyCheckUnusedSitePermissionsNoDelayParam[] = {
        {"unused-site-permissions-no-delay-for-testing", "true"}};

const FeatureEntry::FeatureParam
    kSafetyCheckUnusedSitePermissionsWithDelayParam[] = {
        {"unused-site-permissions-with-delay-for-testing", "true"}};

const FeatureEntry::FeatureVariation
    kSafetyCheckUnusedSitePermissionsVariations[] = {
        {"for testing no delay", kSafetyCheckUnusedSitePermissionsNoDelayParam,
         std::size(kSafetyCheckUnusedSitePermissionsNoDelayParam), nullptr},
        {"for testing with delay",
         kSafetyCheckUnusedSitePermissionsWithDelayParam,
         std::size(kSafetyCheckUnusedSitePermissionsWithDelayParam), nullptr},
};
#endif

const FeatureEntry::FeatureParam kTPCPhaseOutFacilitatedTestingControl1[] = {
    {tpcd::experiment::kForceEligibleForTestingName, "false"},
    {tpcd::experiment::kDisable3PCookiesName, "false"},
    {features::kCookieDeprecationTestingDisableAdsAPIsName, "false"},
    {features::kCookieDeprecationLabelName, "fake_control_1.1"},
    {tpcd::experiment::kVersionName, "9990"}};
const FeatureEntry::FeatureParam kTPCPhaseOutFacilitatedTestingLabelOnly[] = {
    {tpcd::experiment::kForceEligibleForTestingName, "false"},
    {tpcd::experiment::kDisable3PCookiesName, "false"},
    {features::kCookieDeprecationTestingDisableAdsAPIsName, "false"},
    {features::kCookieDeprecationLabelName, "fake_label_only_1.1"},
    {tpcd::experiment::kVersionName, "9991"}};
const FeatureEntry::FeatureParam kTPCPhaseOutFacilitatedTestingTreatment[] = {
    {tpcd::experiment::kForceEligibleForTestingName, "false"},
    {tpcd::experiment::kDisable3PCookiesName, "true"},
    {features::kCookieDeprecationTestingDisableAdsAPIsName, "false"},
    {features::kCookieDeprecationLabelName, "fake_treatment_1.1"},
    {tpcd::experiment::kVersionName, "9992"}};
const FeatureEntry::FeatureParam kTPCPhaseOutFacilitatedTestingControl2[] = {
    {tpcd::experiment::kForceEligibleForTestingName, "false"},
    {tpcd::experiment::kDisable3PCookiesName, "true"},
    {features::kCookieDeprecationTestingDisableAdsAPIsName, "true"},
    {features::kCookieDeprecationLabelName, "fake_control_2"},
    {tpcd::experiment::kVersionName, "9993"}};
const FeatureEntry::FeatureParam kTPCPhaseOutFacilitatedTestingForceControl1[] =
    {{tpcd::experiment::kForceEligibleForTestingName, "true"},
     {tpcd::experiment::kDisable3PCookiesName, "false"},
     {features::kCookieDeprecationTestingDisableAdsAPIsName, "false"},
     {features::kCookieDeprecationLabelName, "fake_control_1.1"},
     {tpcd::experiment::kVersionName, "9994"}};
const FeatureEntry::FeatureParam
    kTPCPhaseOutFacilitatedTestingForceLabelOnly[] = {
        {tpcd::experiment::kForceEligibleForTestingName, "true"},
        {tpcd::experiment::kDisable3PCookiesName, "false"},
        {features::kCookieDeprecationTestingDisableAdsAPIsName, "false"},
        {features::kCookieDeprecationLabelName, "fake_label_only_1.1"},
        {tpcd::experiment::kVersionName, "9995"}};
const FeatureEntry::FeatureParam
    kTPCPhaseOutFacilitatedTestingForceTreatment[] = {
        {tpcd::experiment::kForceEligibleForTestingName, "true"},
        {tpcd::experiment::kDisable3PCookiesName, "true"},
        {features::kCookieDeprecationTestingDisableAdsAPIsName, "false"},
        {features::kCookieDeprecationLabelName, "fake_treatment_1.1"},
        {tpcd::experiment::kVersionName, "9996"}};
const FeatureEntry::FeatureParam kTPCPhaseOutFacilitatedTestingForceControl2[] =
    {{tpcd::experiment::kForceEligibleForTestingName, "true"},
     {tpcd::experiment::kDisable3PCookiesName, "true"},
     {features::kCookieDeprecationTestingDisableAdsAPIsName, "true"},
     {features::kCookieDeprecationLabelName, "fake_control_2"},
     {tpcd::experiment::kVersionName, "9997"}};

const FeatureEntry::FeatureVariation
    kTPCPhaseOutFacilitatedTestingVariations[] = {
        {"Control 1", kTPCPhaseOutFacilitatedTestingControl1,
         std::size(kTPCPhaseOutFacilitatedTestingControl1), nullptr},
        {"LabelOnly", kTPCPhaseOutFacilitatedTestingLabelOnly,
         std::size(kTPCPhaseOutFacilitatedTestingLabelOnly), nullptr},
        {"Treatment", kTPCPhaseOutFacilitatedTestingTreatment,
         std::size(kTPCPhaseOutFacilitatedTestingTreatment), nullptr},
        {"Control 2", kTPCPhaseOutFacilitatedTestingControl2,
         std::size(kTPCPhaseOutFacilitatedTestingControl2), nullptr},
        {"Force Control 1", kTPCPhaseOutFacilitatedTestingForceControl1,
         std::size(kTPCPhaseOutFacilitatedTestingForceControl1), nullptr},
        {"Force LabelOnly", kTPCPhaseOutFacilitatedTestingForceLabelOnly,
         std::size(kTPCPhaseOutFacilitatedTestingForceLabelOnly), nullptr},
        {"Force Treatment", kTPCPhaseOutFacilitatedTestingForceTreatment,
         std::size(kTPCPhaseOutFacilitatedTestingForceTreatment), nullptr},
        {"Force Control 2", kTPCPhaseOutFacilitatedTestingForceControl2,
         std::size(kTPCPhaseOutFacilitatedTestingForceControl2), nullptr},
};

const FeatureEntry::FeatureParam
    kTpcdHeuristicsGrants_CurrentInteraction_ShortRedirect_MainFrameInitiator
        [] = {
            {content_settings::features::kTpcdReadHeuristicsGrantsName, "true"},
            {tpcd::experiment::
                 kTpcdWritePopupCurrentInteractionHeuristicsGrantsName,
             "30d"},
            {tpcd::experiment::kTpcdBackfillPopupHeuristicsGrantsName, "30d"},
            {tpcd::experiment::kTpcdPopupHeuristicEnableForIframeInitiatorName,
             "none"},
            {tpcd::experiment::kTpcdWriteRedirectHeuristicGrantsName, "15m"},
            {tpcd::experiment::kTpcdRedirectHeuristicRequireABAFlowName,
             "true"},
            {tpcd::experiment::
                 kTpcdRedirectHeuristicRequireCurrentInteractionName,
             "true"}};
const FeatureEntry::FeatureParam
    kTpcdHeuristicsGrants_CurrentInteraction_LongRedirect_MainFrameInitiator[] =
        {{content_settings::features::kTpcdReadHeuristicsGrantsName, "true"},
         {tpcd::experiment::
              kTpcdWritePopupCurrentInteractionHeuristicsGrantsName,
          "30d"},
         {tpcd::experiment::kTpcdBackfillPopupHeuristicsGrantsName, "30d"},
         {tpcd::experiment::kTpcdPopupHeuristicEnableForIframeInitiatorName,
          "none"},
         {tpcd::experiment::kTpcdWriteRedirectHeuristicGrantsName, "30d"},
         {tpcd::experiment::kTpcdRedirectHeuristicRequireABAFlowName, "true"},
         {tpcd::experiment::kTpcdRedirectHeuristicRequireCurrentInteractionName,
          "true"}};
const FeatureEntry::FeatureParam
    kTpcdHeuristicsGrants_CurrentInteraction_ShortRedirect_AllFrameInitiator[] =
        {{content_settings::features::kTpcdReadHeuristicsGrantsName, "true"},
         {tpcd::experiment::
              kTpcdWritePopupCurrentInteractionHeuristicsGrantsName,
          "30d"},
         {tpcd::experiment::kTpcdBackfillPopupHeuristicsGrantsName, "30d"},
         {tpcd::experiment::kTpcdPopupHeuristicEnableForIframeInitiatorName,
          "all"},
         {tpcd::experiment::kTpcdWriteRedirectHeuristicGrantsName, "15m"},
         {tpcd::experiment::kTpcdRedirectHeuristicRequireABAFlowName, "true"},
         {tpcd::experiment::kTpcdRedirectHeuristicRequireCurrentInteractionName,
          "true"}};
const FeatureEntry::FeatureParam
    kTpcdHeuristicsGrants_CurrentInteraction_LongRedirect_AllFrameInitiator[] =
        {{content_settings::features::kTpcdReadHeuristicsGrantsName, "true"},
         {tpcd::experiment::
              kTpcdWritePopupCurrentInteractionHeuristicsGrantsName,
          "30d"},
         {tpcd::experiment::kTpcdBackfillPopupHeuristicsGrantsName, "30d"},
         {tpcd::experiment::kTpcdPopupHeuristicEnableForIframeInitiatorName,
          "all"},
         {tpcd::experiment::kTpcdWriteRedirectHeuristicGrantsName, "30d"},
         {tpcd::experiment::kTpcdRedirectHeuristicRequireABAFlowName, "true"},
         {tpcd::experiment::kTpcdRedirectHeuristicRequireCurrentInteractionName,
          "true"}};

const FeatureEntry::FeatureVariation kTpcdHeuristicsGrantsVariations[] = {
    {"CurrentInteraction_ShortRedirect_MainFrameInitiator",
     kTpcdHeuristicsGrants_CurrentInteraction_ShortRedirect_MainFrameInitiator,
     std::size(
         kTpcdHeuristicsGrants_CurrentInteraction_ShortRedirect_MainFrameInitiator),
     nullptr},
    {"CurrentInteraction_LongRedirect_MainFrameInitiator",
     kTpcdHeuristicsGrants_CurrentInteraction_LongRedirect_MainFrameInitiator,
     std::size(
         kTpcdHeuristicsGrants_CurrentInteraction_LongRedirect_MainFrameInitiator),
     nullptr},
    {"CurrentInteraction_ShortRedirect_AllFrameInitiator",
     kTpcdHeuristicsGrants_CurrentInteraction_ShortRedirect_AllFrameInitiator,
     std::size(
         kTpcdHeuristicsGrants_CurrentInteraction_ShortRedirect_AllFrameInitiator),
     nullptr},
    {"CurrentInteraction_LongRedirect_AllFrameInitiator",
     kTpcdHeuristicsGrants_CurrentInteraction_LongRedirect_AllFrameInitiator,
     std::size(
         kTpcdHeuristicsGrants_CurrentInteraction_LongRedirect_AllFrameInitiator),
     nullptr}};

#if BUILDFLAG(IS_CHROMEOS_ASH)
const FeatureEntry::FeatureParam kVcSegmentationModelHighResolution[] = {
    {"segmentation_model", "high_resolution"},
};

const FeatureEntry::FeatureParam kVcSegmentationModelLowerResolution[] = {
    {"segmentation_model", "lower_resolution"},
};

const FeatureEntry::FeatureVariation kVcSegmentationModelVariations[] = {
    {"High resolution model", kVcSegmentationModelHighResolution,
     std::size(kVcSegmentationModelHighResolution), nullptr},
    {"Lower resolution model", kVcSegmentationModelLowerResolution,
     std::size(kVcSegmentationModelLowerResolution), nullptr},
};

const FeatureEntry::FeatureParam kVcLightIntensity10[] = {
    {"light_intensity", "1.0"},
};

const FeatureEntry::FeatureParam kVcLightIntensity13[] = {
    {"light_intensity", "1.3"},
};

const FeatureEntry::FeatureParam kVcLightIntensity15[] = {
    {"light_intensity", "1.5"},
};

const FeatureEntry::FeatureParam kVcLightIntensity17[] = {
    {"light_intensity", "1.7"},
};

const FeatureEntry::FeatureParam kVcLightIntensity18[] = {
    {"light_intensity", "1.8"},
};

const FeatureEntry::FeatureParam kVcLightIntensity20[] = {
    {"light_intensity", "2.0"},
};

const FeatureEntry::FeatureVariation kVcLightIntensityVariations[] = {
    {"1.0", kVcLightIntensity10, std::size(kVcLightIntensity10), nullptr},
    {"1.3", kVcLightIntensity13, std::size(kVcLightIntensity13), nullptr},
    {"1.5", kVcLightIntensity15, std::size(kVcLightIntensity15), nullptr},
    {"1.7", kVcLightIntensity17, std::size(kVcLightIntensity17), nullptr},
    {"1.8", kVcLightIntensity18, std::size(kVcLightIntensity18), nullptr},
    {"2.0", kVcLightIntensity20, std::size(kVcLightIntensity20), nullptr},
};

#endif  // BUILDFLAG(IS_CHROMEOS_ASH)

#if BUILDFLAG(IS_CHROMEOS_ASH)
const FeatureEntry::FeatureParam
    kCrOSLateBootMissiveDisableStorageDegradation[] = {
        {"controlled_degradation", "false"}};
const FeatureEntry::FeatureParam
    kCrOSLateBootMissiveEnableStorageDegradation[] = {
        {"controlled_degradation", "true"}};
const FeatureEntry::FeatureParam kCrOSLateBootMissiveDisableLegacyStorage[] = {
    {"legacy_storage_enabled",
     "UNDEFINED_PRIORITY"}};  // All others are multi-generation action state.
const FeatureEntry::FeatureParam kCrOSLateBootMissiveEnableLegacyStorage[] = {
    {"legacy_storage_enabled",
     "SECURITY,"
     "IMMEDIATE,"
     "FAST_BATCH,"
     "SLOW_BATCH,"
     "BACKGROUND_BATCH,"
     "MANUAL_BATCH,"
     "MANUAL_BATCH_LACROS,"}};
const FeatureEntry::FeatureParam kCrOSLateBootMissivePartialLegacyStorage[] = {
    {"legacy_storage_enabled",
     "SECURITY,"
     "IMMEDIATE,"}};
const FeatureEntry::FeatureParam kCrOSLateBootMissiveSecurityLegacyStorage[] = {
    {"legacy_storage_enabled", "SECURITY,"}};

const FeatureEntry::FeatureVariation
    kCrOSLateBootMissiveStorageDefaultVariations[] = {
        {"Enable storage degradation",
         kCrOSLateBootMissiveEnableStorageDegradation,
         std::size(kCrOSLateBootMissiveEnableStorageDegradation), nullptr},
        {"Disable storage degradation",
         kCrOSLateBootMissiveDisableStorageDegradation,
         std::size(kCrOSLateBootMissiveDisableStorageDegradation), nullptr},
        {"Enable all queues legacy", kCrOSLateBootMissiveEnableLegacyStorage,
         std::size(kCrOSLateBootMissiveEnableLegacyStorage), nullptr},
        {"Disable all queues legacy", kCrOSLateBootMissiveDisableLegacyStorage,
         std::size(kCrOSLateBootMissiveDisableLegacyStorage), nullptr},
        {"Enable SECURITY and IMMEDIATE queues legacy only",
         kCrOSLateBootMissivePartialLegacyStorage,
         std::size(kCrOSLateBootMissivePartialLegacyStorage), nullptr},
        {"Enable SECURITY queues legacy only",
         kCrOSLateBootMissiveSecurityLegacyStorage,
         std::size(kCrOSLateBootMissiveSecurityLegacyStorage), nullptr},
};
#endif  // BUILDFLAG(IS_CHROMEOS_ASH)

#if !BUILDFLAG(IS_ANDROID)
const FeatureEntry::Choice kCastMirroringTargetPlayoutDelayChoices[] = {
    {flag_descriptions::kCastMirroringTargetPlayoutDelayDefault, "", ""},
    {flag_descriptions::kCastMirroringTargetPlayoutDelay100ms,
     switches::kCastMirroringTargetPlayoutDelay, "100"},
    {flag_descriptions::kCastMirroringTargetPlayoutDelay150ms,
     switches::kCastMirroringTargetPlayoutDelay, "150"},
    {flag_descriptions::kCastMirroringTargetPlayoutDelay200ms,
     switches::kCastMirroringTargetPlayoutDelay, "200"},
    {flag_descriptions::kCastMirroringTargetPlayoutDelay250ms,
     switches::kCastMirroringTargetPlayoutDelay, "250"},
    {flag_descriptions::kCastMirroringTargetPlayoutDelay300ms,
     switches::kCastMirroringTargetPlayoutDelay, "300"},
    {flag_descriptions::kCastMirroringTargetPlayoutDelay350ms,
     switches::kCastMirroringTargetPlayoutDelay, "350"}};

#endif  // !BUILDFLAG(IS_ANDROID)

#if !BUILDFLAG(IS_ANDROID)
const FeatureEntry::FeatureParam kPasswordGenerationTrustedAdvice[] = {
    {password_manager::features::kPasswordGenerationExperimentVariationParam
         .name,
     password_manager::features::kPasswordGenerationExperimentVariationOption[0]
         .name}};
const FeatureEntry::FeatureParam kPasswordGenerationSafetyFirst[] = {
    {password_manager::features::kPasswordGenerationExperimentVariationParam
         .name,
     password_manager::features::kPasswordGenerationExperimentVariationOption[1]
         .name}};
const FeatureEntry::FeatureParam kPasswordGenerationTrySomethingNew[] = {
    {password_manager::features::kPasswordGenerationExperimentVariationParam
         .name,
     password_manager::features::kPasswordGenerationExperimentVariationOption[2]
         .name}};
const FeatureEntry::FeatureParam kPasswordGenerationConvenience[] = {
    {password_manager::features::kPasswordGenerationExperimentVariationParam
         .name,
     password_manager::features::kPasswordGenerationExperimentVariationOption[3]
         .name}};
const FeatureEntry::FeatureParam kPasswordGenerationCrossDevice[] = {
    {password_manager::features::kPasswordGenerationExperimentVariationParam
         .name,
     password_manager::features::kPasswordGenerationExperimentVariationOption[4]
         .name}};
const FeatureEntry::FeatureParam kPasswordGenerationEditPassword[] = {
    {password_manager::features::kPasswordGenerationExperimentVariationParam
         .name,
     password_manager::features::kPasswordGenerationExperimentVariationOption[5]
         .name}};
const FeatureEntry::FeatureParam kPasswordGenerationChunkPassword[] = {
    {password_manager::features::kPasswordGenerationExperimentVariationParam
         .name,
     password_manager::features::kPasswordGenerationExperimentVariationOption[6]
         .name}};
const FeatureEntry::FeatureParam kPasswordGenerationNudgePassword[] = {
    {password_manager::features::kPasswordGenerationExperimentVariationParam
         .name,
     password_manager::features::kPasswordGenerationExperimentVariationOption[7]
         .name}};

const FeatureEntry::FeatureVariation kPasswordGenerationExperimentVariations[] =
    {
        {"Trusted advice", kPasswordGenerationTrustedAdvice,
         std::size(kPasswordGenerationTrustedAdvice), nullptr},
        {"Safety first", kPasswordGenerationSafetyFirst,
         std::size(kPasswordGenerationSafetyFirst), nullptr},
        {"Try something new", kPasswordGenerationTrySomethingNew,
         std::size(kPasswordGenerationTrySomethingNew), nullptr},
        {"Convenience", kPasswordGenerationConvenience,
         std::size(kPasswordGenerationConvenience), nullptr},
        {"Cross device", kPasswordGenerationCrossDevice,
         std::size(kPasswordGenerationCrossDevice), nullptr},
        {"Edit password", kPasswordGenerationEditPassword,
         std::size(kPasswordGenerationEditPassword), nullptr},
        {"Chunk password", kPasswordGenerationChunkPassword,
         std::size(kPasswordGenerationChunkPassword), nullptr},
        {"Nudge password", kPasswordGenerationNudgePassword,
         std::size(kPasswordGenerationNudgePassword), nullptr},
};
#endif  // !BUILDFLAG(IS_ANDROID)

#if BUILDFLAG(ENABLE_BOUND_SESSION_CREDENTIALS)
const FeatureEntry::FeatureParam kEnableBoundSessionCredentialsWithDice[] = {
    {"dice-support", "enabled"}};

const FeatureEntry::FeatureVariation
    kEnableBoundSessionCredentialsVariations[] = {
        {"including DICE profiles", kEnableBoundSessionCredentialsWithDice,
         std::size(kEnableBoundSessionCredentialsWithDice), nullptr}};
#endif  // BUILDFLAG(ENABLE_BOUND_SESSION_CREDENTIALS)

#if BUILDFLAG(IS_ANDROID)
const FeatureEntry::FeatureParam kWebAuthnAndroidCredManGpmInCredManParam = {
    device::kWebAuthnAndroidGpmInCredMan.name, "true"};
const FeatureEntry::FeatureParam kWebAuthnAndroidCredManGpmNotInCredManParam = {
    device::kWebAuthnAndroidGpmInCredMan.name, "false"};
const FeatureEntry::FeatureVariation kWebAuthnAndroidCredManVariations[] = {
    {"for Google Password Manager and 3rd party passkeys",
     &kWebAuthnAndroidCredManGpmInCredManParam, 1, nullptr},
    {"for 3rd party passkeys", &kWebAuthnAndroidCredManGpmNotInCredManParam, 1,
     nullptr}};

const FeatureEntry::FeatureParam kHubPhase1WithFab[] = {
    {"floating_action_button", "true"}};
const FeatureEntry::FeatureParam kHubPhase1WithoutFab[] = {
    {"floating_action_button", "false"}};
const FeatureEntry::FeatureParam kHubPhase2WithIcons[] = {
    {"floating_action_button", "true"},
    {"supports_other_tabs", "true"}};
const FeatureEntry::FeatureParam kHubPhase2WithText[] = {
    {"floating_action_button", "true"},
    {"pane_switcher_uses_text", "true"},
    {"supports_other_tabs", "true"}};
const FeatureEntry::FeatureParam kHubPhase3[] = {
    {"floating_action_button", "true"},
    {"pane_switcher_uses_text", "true"},
    {"supports_other_tabs", "true"},
    {"supports_search", "true"}};
const FeatureEntry::FeatureParam kHubPhase4[] = {
    {"floating_action_button", "true"},
    {"pane_switcher_uses_text", "true"},
    {"supports_other_tabs", "true"},
    {"supports_search", "true"},
    {"supports_bookmarks", "true"}};

const FeatureEntry::FeatureVariation kAndroidHubVariations[] = {
    {"Phase 1 w/ FAB", kHubPhase1WithFab, std::size(kHubPhase1WithFab),
     nullptr},
    {"Phase 1 w/o FAB", kHubPhase1WithoutFab, std::size(kHubPhase1WithoutFab),
     nullptr},
    {"Phase 2 w/ Icons", kHubPhase2WithIcons, std::size(kHubPhase2WithIcons),
     nullptr},
    {"Phase 2 w/ Text", kHubPhase2WithText, std::size(kHubPhase2WithText),
     nullptr},
    {"Phase 3", kHubPhase3, std::size(kHubPhase3), nullptr},
    {"Phase 4", kHubPhase4, std::size(kHubPhase4), nullptr}};
#endif  // BUILDFLAG(IS_ANDROID)

const flags_ui::FeatureEntry::FeatureParam kParcelTrackingTestDataDelivered[] =
    {{commerce::kParcelTrackingTestDataParam,
      commerce::kParcelTrackingTestDataParamDelivered}};
const flags_ui::FeatureEntry::FeatureParam kParcelTrackingTestDataInProgress[] =
    {{commerce::kParcelTrackingTestDataParam,
      commerce::kParcelTrackingTestDataParamInProgress}};
const flags_ui::FeatureEntry::FeatureParam
    kParcelTrackingTestDataOutForDelivery[] = {
        {commerce::kParcelTrackingTestDataParam,
         commerce::kParcelTrackingTestDataParamOutForDelivery}};
const flags_ui::FeatureEntry::FeatureVariation
    kParcelTrackingTestDataVariations[] = {
        {"Delivered", kParcelTrackingTestDataDelivered,
         std::size(kParcelTrackingTestDataDelivered), nullptr},
        {"In progress", kParcelTrackingTestDataInProgress,
         std::size(kParcelTrackingTestDataInProgress), nullptr},
        {"Out for delivery", kParcelTrackingTestDataOutForDelivery,
         std::size(kParcelTrackingTestDataOutForDelivery), nullptr},
};

#if BUILDFLAG(IS_LINUX) || BUILDFLAG(IS_MAC) || BUILDFLAG(IS_WIN)
const flags_ui::FeatureEntry::FeatureParam
    kDesktopPWAsLinkCapturingDefaultOn[] = {{"on_by_default", "true"}};
const flags_ui::FeatureEntry::FeatureParam
    kDesktopPWAsLinkCapturingDefaultOff[] = {{"on_by_default", "false"}};
const flags_ui::FeatureEntry::FeatureVariation
    kDesktopPWAsLinkCapturingVariations[] = {
        {"On by default", kDesktopPWAsLinkCapturingDefaultOn,
         std::size(kDesktopPWAsLinkCapturingDefaultOn), nullptr},
        {"Off by default", kDesktopPWAsLinkCapturingDefaultOff,
         std::size(kDesktopPWAsLinkCapturingDefaultOff), nullptr}};
#endif  // BUILDFLAG(IS_LINUX) || BUILDFLAG(IS_MAC) || BUILDFLAG(IS_WIN)

// RECORDING USER METRICS FOR FLAGS:
// -----------------------------------------------------------------------------
// The first line of the entry is the internal name.
//
// To add a new entry, add to the end of kFeatureEntries. There are two
// distinct types of entries:
// . SINGLE_VALUE: entry is either on or off. Use the SINGLE_VALUE_TYPE
//   macro for this type supplying the command line to the macro.
// . MULTI_VALUE: a list of choices, the first of which should correspond to a
//   deactivated state for this lab (i.e. no command line option). To specify
//   this type of entry use the macro MULTI_VALUE_TYPE supplying it the
//   array of choices.
// See the documentation of FeatureEntry for details on the fields.
//
// Usage of about:flags is logged on startup via the "Launch.FlagsAtStartup"
// UMA histogram. This histogram shows the number of startups with a given flag
// enabled. If you'd like to see user counts instead, make sure to switch to
// "count users" view on the dashboard. When adding new entries, the enum
// "LoginCustomFlags" must be updated in histograms/enums.xml. See note in
// enums.xml and don't forget to run AboutFlagsHistogramTest unit test to
// calculate and verify checksum.
//
// When adding a new choice, add it to the end of the list.
#include "chrome/browser/thorium_flag_choices.h"
const FeatureEntry kFeatureEntries[] = {
#include "chrome/browser/thorium_flag_entries.h"
// Include generated flags for flag unexpiry; see //docs/flag_expiry.md and
// //tools/flags/generate_unexpire_flags.py.
#include "build/chromeos_buildflags.h"
#include "chrome/browser/unexpire_flags_gen.inc"
    {variations::switches::kEnableBenchmarking,
     flag_descriptions::kEnableBenchmarkingName,
     flag_descriptions::kEnableBenchmarkingDescription, kOsAll,
     SINGLE_VALUE_TYPE(variations::switches::kEnableBenchmarking)},
    {"ignore-gpu-blocklist", flag_descriptions::kIgnoreGpuBlocklistName,
     flag_descriptions::kIgnoreGpuBlocklistDescription, kOsAll,
     SINGLE_VALUE_TYPE(switches::kIgnoreGpuBlocklist)},
    {"disable-accelerated-2d-canvas",
     flag_descriptions::kAccelerated2dCanvasName,
     flag_descriptions::kAccelerated2dCanvasDescription, kOsAll,
     SINGLE_DISABLE_VALUE_TYPE(switches::kDisableAccelerated2dCanvas)},
    {"overlay-strategies", flag_descriptions::kOverlayStrategiesName,
     flag_descriptions::kOverlayStrategiesDescription, kOsAll,
     MULTI_VALUE_TYPE(kOverlayStrategiesChoices)},
    {"tint-composited-content", flag_descriptions::kTintCompositedContentName,
     flag_descriptions::kTintCompositedContentDescription, kOsAll,
     SINGLE_VALUE_TYPE(switches::kTintCompositedContent)},
    {"show-overdraw-feedback", flag_descriptions::kShowOverdrawFeedbackName,
     flag_descriptions::kShowOverdrawFeedbackDescription, kOsAll,
     SINGLE_VALUE_TYPE(switches::kShowOverdrawFeedback)},
    {"ui-disable-partial-swap", flag_descriptions::kUiPartialSwapName,
     flag_descriptions::kUiPartialSwapDescription, kOsAll,
     SINGLE_DISABLE_VALUE_TYPE(switches::kUIDisablePartialSwap)},
    {"disable-webrtc-hw-decoding", flag_descriptions::kWebrtcHwDecodingName,
     flag_descriptions::kWebrtcHwDecodingDescription, kOsAndroid | kOsCrOS,
     SINGLE_DISABLE_VALUE_TYPE(switches::kDisableWebRtcHWDecoding)},
    {"disable-webrtc-hw-encoding", flag_descriptions::kWebrtcHwEncodingName,
     flag_descriptions::kWebrtcHwEncodingDescription, kOsAndroid | kOsCrOS,
     SINGLE_DISABLE_VALUE_TYPE(switches::kDisableWebRtcHWEncoding)},
#if !BUILDFLAG(IS_ANDROID)
    {"enable-reader-mode", flag_descriptions::kEnableReaderModeName,
     flag_descriptions::kEnableReaderModeDescription, kOsDesktop,
     FEATURE_WITH_PARAMS_VALUE_TYPE(dom_distiller::kReaderMode,
                                    kReaderModeDiscoverabilityVariations,
                                    "ReaderMode")},
#endif  // !BUILDFLAG(IS_ANDROID)
#if defined(WEBRTC_USE_PIPEWIRE)
    {"enable-webrtc-pipewire-capturer",
     flag_descriptions::kWebrtcPipeWireCapturerName,
     flag_descriptions::kWebrtcPipeWireCapturerDescription, kOsLinux,
     FEATURE_VALUE_TYPE(features::kWebRtcPipeWireCapturer)},
#endif  // defined(WEBRTC_USE_PIPEWIRE)
#if BUILDFLAG(IS_CHROMEOS_ASH)
    {"enable-lacros-in-chrome-kiosk",
     flag_descriptions::kChromeKioskEnableLacrosName,
     flag_descriptions::kChromeKioskEnableLacrosDescription, kOsCrOS,
     FEATURE_VALUE_TYPE(
         ash::standalone_browser::features::kChromeKioskEnableLacros)},
    {"enable-lacros-in-web-kiosk", flag_descriptions::kWebKioskEnableLacrosName,
     flag_descriptions::kWebKioskEnableLacrosDescription, kOsCrOS,
     FEATURE_VALUE_TYPE(
         ash::standalone_browser::features::kWebKioskEnableLacros)},
#endif  // BUILDFLAG(IS_CHROMEOS_ASH)
#if !BUILDFLAG(IS_ANDROID)
    {"enable-webrtc-remote-event-log",
     flag_descriptions::kWebRtcRemoteEventLogName,
     flag_descriptions::kWebRtcRemoteEventLogDescription, kOsDesktop,
     FEATURE_VALUE_TYPE(features::kWebRtcRemoteEventLog)},
#endif
    {"enable-webrtc-allow-input-volume-adjustment",
     flag_descriptions::kWebRtcAllowInputVolumeAdjustmentName,
     flag_descriptions::kWebRtcAllowInputVolumeAdjustmentDescription,
     kOsWin | kOsMac | kOsLinux,
     FEATURE_VALUE_TYPE(features::kWebRtcAllowInputVolumeAdjustment)},
    {"enable-webrtc-apm-downmix-capture-audio-method",
     flag_descriptions::kWebRtcApmDownmixCaptureAudioMethodName,
     flag_descriptions::kWebRtcApmDownmixCaptureAudioMethodDescription,
     kOsDesktop,
     FEATURE_WITH_PARAMS_VALUE_TYPE(
         features::kWebRtcApmDownmixCaptureAudioMethod,
         kWebRtcApmDownmixMethodVariations,
         "WebRtcApmDownmixCaptureAudioMethod")},
    {"enable-webrtc-hide-local-ips-with-mdns",
     flag_descriptions::kWebrtcHideLocalIpsWithMdnsName,
     flag_descriptions::kWebrtcHideLocalIpsWithMdnsDecription, kOsDesktop,
     FEATURE_VALUE_TYPE(blink::features::kWebRtcHideLocalIpsWithMdns)},
    {"enable-webrtc-use-min-max-vea-dimensions",
     flag_descriptions::kWebrtcUseMinMaxVEADimensionsName,
     flag_descriptions::kWebrtcUseMinMaxVEADimensionsDescription, kOsAll,
     FEATURE_VALUE_TYPE(blink::features::kWebRtcUseMinMaxVEADimensions)},
#if BUILDFLAG(ENABLE_NACL)
    {"enable-nacl", flag_descriptions::kNaclName,
     flag_descriptions::kNaclDescription, kOsAll,
     SINGLE_VALUE_TYPE(switches::kEnableNaCl)},
    {"verbose-logging-in-nacl", flag_descriptions::kVerboseLoggingInNaclName,
     flag_descriptions::kVerboseLoggingInNaclDescription, kOsAll,
     MULTI_VALUE_TYPE(kVerboseLoggingInNaclChoices)},
#endif  // ENABLE_NACL
#if BUILDFLAG(ENABLE_CARDBOARD)
    {"enable-cardboard", flag_descriptions::kEnableCardboardName,
     flag_descriptions::kEnableCardboardDescription, kOsAndroid,
     FEATURE_VALUE_TYPE(device::features::kEnableCardboard)},
#endif  // ENABLE_CARDBOARD
#if BUILDFLAG(ENABLE_EXTENSIONS)
    {"extensions-on-chrome-urls",
     flag_descriptions::kExtensionsOnChromeUrlsName,
     flag_descriptions::kExtensionsOnChromeUrlsDescription, kOsAll,
     SINGLE_VALUE_TYPE(extensions::switches::kExtensionsOnChromeURLs)},
#endif  // ENABLE_EXTENSIONS
#if BUILDFLAG(IS_ANDROID)
    {"contextual-search-suppress-short-view",
     flag_descriptions::kContextualSearchSuppressShortViewName,
     flag_descriptions::kContextualSearchSuppressShortViewDescription,
     kOsAndroid,
     FEATURE_WITH_PARAMS_VALUE_TYPE(
         chrome::android::kContextualSearchSuppressShortView,
         kContextualSearchSuppressShortViewVariations,
         "ContextualSearchSuppressShortView")},
    {"related-searches", flag_descriptions::kRelatedSearchesName,
     flag_descriptions::kRelatedSearchesDescription, kOsAndroid,
     FEATURE_WITH_PARAMS_VALUE_TYPE(chrome::android::kRelatedSearches,
                                    kRelatedSearchesVariations,
                                    "RelatedSearches")},
    {"related-searches-all-language",
     flag_descriptions::kRelatedSearchesAllLanguageName,
     flag_descriptions::kRelatedSearchesAllLanguageDescription, kOsAndroid,
     FEATURE_VALUE_TYPE(chrome::android::kRelatedSearchesAllLanguage)},
#endif  // BUILDFLAG(IS_ANDROID)
    {"show-autofill-type-predictions",
     flag_descriptions::kShowAutofillTypePredictionsName,
     flag_descriptions::kShowAutofillTypePredictionsDescription, kOsAll,
     FEATURE_VALUE_TYPE(
         autofill::features::test::kAutofillShowTypePredictions)},
    {"autofill-undo", flag_descriptions::kUndoAutofillName,
     flag_descriptions::kUndoAutofillDescription, kOsAll,
     FEATURE_VALUE_TYPE(autofill::features::kAutofillUndo)},
    {"autofill-more-prominent-popup",
     flag_descriptions::kAutofillMoreProminentPopupName,
     flag_descriptions::kAutofillMoreProminentPopupDescription, kOsDesktop,
     FEATURE_VALUE_TYPE(autofill::features::kAutofillMoreProminentPopup)},
    {"smooth-scrolling", flag_descriptions::kSmoothScrollingName,
     flag_descriptions::kSmoothScrollingDescription,
     // Mac has a separate implementation with its own setting to disable.
     kOsLinux | kOsLacros | kOsCrOS | kOsWin | kOsAndroid | kOsFuchsia,
     ENABLE_DISABLE_VALUE_TYPE(switches::kEnableSmoothScrolling,
                               switches::kDisableSmoothScrolling)},
    {"fractional-scroll-offsets",
     flag_descriptions::kFractionalScrollOffsetsName,
     flag_descriptions::kFractionalScrollOffsetsDescription, kOsAll,
     FEATURE_VALUE_TYPE(features::kFractionalScrollOffsets)},
#if defined(USE_AURA)
    {"overlay-scrollbars", flag_descriptions::kOverlayScrollbarsName,
     flag_descriptions::kOverlayScrollbarsDescription,
     // Uses the system preference on Mac (a different implementation).
     // On Android, this is always enabled.
     kOsAura, FEATURE_VALUE_TYPE(features::kOverlayScrollbar)},
#endif  // USE_AURA
    {"enable-quic", flag_descriptions::kQuicName,
     flag_descriptions::kQuicDescription, kOsAll,
     ENABLE_DISABLE_VALUE_TYPE(switches::kEnableQuic, switches::kDisableQuic)},
    {"webtransport-developer-mode",
     flag_descriptions::kWebTransportDeveloperModeName,
     flag_descriptions::kWebTransportDeveloperModeDescription, kOsAll,
     SINGLE_VALUE_TYPE(switches::kWebTransportDeveloperMode)},
    {"disable-javascript-harmony-shipping",
     flag_descriptions::kJavascriptHarmonyShippingName,
     flag_descriptions::kJavascriptHarmonyShippingDescription, kOsAll,
     SINGLE_DISABLE_VALUE_TYPE(switches::kDisableJavaScriptHarmonyShipping)},
    {"enable-javascript-harmony", flag_descriptions::kJavascriptHarmonyName,
     flag_descriptions::kJavascriptHarmonyDescription, kOsAll,
     SINGLE_VALUE_TYPE(switches::kJavaScriptHarmony)},
    {"enable-javascript-experimental-shared-memory",
     flag_descriptions::kJavascriptExperimentalSharedMemoryName,
     flag_descriptions::kJavascriptExperimentalSharedMemoryDescription, kOsAll,
     FEATURE_VALUE_TYPE(features::kJavaScriptExperimentalSharedMemory)},
    {"enable-experimental-webassembly-features",
     flag_descriptions::kExperimentalWebAssemblyFeaturesName,
     flag_descriptions::kExperimentalWebAssemblyFeaturesDescription, kOsAll,
     SINGLE_VALUE_TYPE(switches::kEnableExperimentalWebAssemblyFeatures)},
#if defined(ARCH_CPU_X86_64) || defined(ARCH_CPU_ARM64)
    {"enable-experimental-webassembly-stack-switching",
     flag_descriptions::kExperimentalWebAssemblyJSPIName,
     flag_descriptions::kExperimentalWebAssemblyJSPIDescription, kOsAll,
     FEATURE_VALUE_TYPE(features::kEnableExperimentalWebAssemblyJSPI)},
#endif  // defined(ARCH_CPU_X86_64) || defined(ARCH_CPU_ARM64)
    {"enable-webassembly-baseline", flag_descriptions::kEnableWasmBaselineName,
     flag_descriptions::kEnableWasmBaselineDescription, kOsAll,
     FEATURE_VALUE_TYPE(features::kWebAssemblyBaseline)},
    {"enable-webassembly-garbage-collection",
     flag_descriptions::kEnableWasmGarbageCollectionName,
     flag_descriptions::kEnableWasmGarbageCollectionDescription, kOsAll,
     FEATURE_VALUE_TYPE(features::kWebAssemblyGarbageCollection)},
    {"enable-webassembly-lazy-compilation",
     flag_descriptions::kEnableWasmLazyCompilationName,
     flag_descriptions::kEnableWasmLazyCompilationDescription, kOsAll,
     FEATURE_VALUE_TYPE(features::kWebAssemblyLazyCompilation)},
    {"enable-webassembly-relaxed-simd",
     flag_descriptions::kEnableWasmRelaxedSimdName,
     flag_descriptions::kEnableWasmRelaxedSimdDescription, kOsAll,
     FEATURE_VALUE_TYPE(features::kWebAssemblyRelaxedSimd)},
    {"enable-webassembly-stringref",
     flag_descriptions::kEnableWasmStringrefName,
     flag_descriptions::kEnableWasmStringrefDescription, kOsAll,
     FEATURE_VALUE_TYPE(features::kWebAssemblyStringref)},
    {"enable-webassembly-tiering", flag_descriptions::kEnableWasmTieringName,
     flag_descriptions::kEnableWasmTieringDescription, kOsAll,
     FEATURE_VALUE_TYPE(features::kWebAssemblyTiering)},
    {"enable-future-v8-vm-features", flag_descriptions::kV8VmFutureName,
     flag_descriptions::kV8VmFutureDescription, kOsAll,
     FEATURE_VALUE_TYPE(features::kV8VmFuture)},
    {"enable-gpu-rasterization", flag_descriptions::kGpuRasterizationName,
     flag_descriptions::kGpuRasterizationDescription, kOsAll,
     MULTI_VALUE_TYPE(kEnableGpuRasterizationChoices)},
#if BUILDFLAG(USE_FONTATIONS_BACKEND)
    {"enable-fontations-backend", flag_descriptions::kFontationsFontBackendName,
     flag_descriptions::kFontationsFontBackendDescription, kOsAll,
     FEATURE_VALUE_TYPE(blink::features::kFontationsFontBackend)},
#endif
    {"enable-experimental-web-platform-features",
     flag_descriptions::kExperimentalWebPlatformFeaturesName,
     flag_descriptions::kExperimentalWebPlatformFeaturesDescription, kOsAll,
     SINGLE_VALUE_TYPE(switches::kEnableExperimentalWebPlatformFeatures)},
    {"top-chrome-touch-ui", flag_descriptions::kTopChromeTouchUiName,
     flag_descriptions::kTopChromeTouchUiDescription, kOsDesktop,
     MULTI_VALUE_TYPE(kTopChromeTouchUiChoices)},
#if BUILDFLAG(ENABLE_WEBUI_TAB_STRIP)
    {flag_descriptions::kWebUITabStripFlagId,
     flag_descriptions::kWebUITabStripName,
     flag_descriptions::kWebUITabStripDescription, kOsDesktop,
     FEATURE_VALUE_TYPE(features::kWebUITabStrip)},
    {"webui-tab-strip-context-menu-after-tap",
     flag_descriptions::kWebUITabStripContextMenuAfterTapName,
     flag_descriptions::kWebUITabStripContextMenuAfterTapDescription,
     kOsDesktop,
     FEATURE_VALUE_TYPE(features::kWebUITabStripContextMenuAfterTap)},
#endif  // BUILDFLAG(ENABLE_WEBUI_TAB_STRIP)
#if BUILDFLAG(IS_CHROMEOS_ASH)
    {"allow-eap-default-cas-without-subject-verification",
     flag_descriptions::kAllowEapDefaultCasWithoutSubjectVerificationName,
     flag_descriptions::
         kAllowEapDefaultCasWithoutSubjectVerificationDescription,
     kOsCrOS,
     FEATURE_VALUE_TYPE(
         ash::features::kAllowEapDefaultCasWithoutSubjectVerification)},
    {"alt-click-and-six-pack-customization",
     flag_descriptions::kAltClickAndSixPackCustomizationName,
     flag_descriptions::kAltClickAndSixPackCustomizationDescription, kOsCrOS,
     FEATURE_VALUE_TYPE(ash::features::kAltClickAndSixPackCustomization)},
    {"apn-revamp", flag_descriptions::kApnRevampName,
     flag_descriptions::kApnRevampDescription, kOsCrOS,
     FEATURE_VALUE_TYPE(ash::features::kApnRevamp)},
    {"audio-a2dp-advanced-codecs",
     flag_descriptions::kAudioA2DPAdvancedCodecsName,
     flag_descriptions::kAudioA2DPAdvancedCodecsDescription, kOsCrOS,
     PLATFORM_FEATURE_NAME_TYPE("CrOSLateBootAudioA2DPAdvancedCodecs")},
    {"audio-ap-noise-cancellation",
     flag_descriptions::kAudioAPNoiseCancellationName,
     flag_descriptions::kAudioAPNoiseCancellationDescription, kOsCrOS,
     PLATFORM_FEATURE_NAME_TYPE("CrOSLateBootAudioAPNoiseCancellation")},
    {"audio-flexible-loopback", flag_descriptions::kAudioFlexibleLoopbackName,
     flag_descriptions::kAudioFlexibleLoopbackDescription, kOsCrOS,
     PLATFORM_FEATURE_NAME_TYPE("CrOSLateBootAudioFlexibleLoopback")},
    {"audio-hfp-mic-sr", flag_descriptions::kAudioHFPMicSRName,
     flag_descriptions::kAudioHFPMicSRDescription, kOsCrOS,
     PLATFORM_FEATURE_NAME_TYPE("CrOSLateBootAudioHFPMicSR")},
    {"audio-hfp-mic-sr-toggle", flag_descriptions::kAudioHFPMicSRToggleName,
     flag_descriptions::kAudioHFPMicSRToggleDescription, kOsCrOS,
     FEATURE_VALUE_TYPE(ash::features::kAudioHFPMicSRToggle)},
    {"audio-hfp-offload", flag_descriptions::kAudioHFPOffloadName,
     flag_descriptions::kAudioHFPOffloadDescription, kOsCrOS,
     PLATFORM_FEATURE_NAME_TYPE("CrOSLateBootAudioHFPOffload")},
    {"audio-hfp-swb", flag_descriptions::kAudioHFPSwbName,
     flag_descriptions::kAudioHFPSwbDescription, kOsCrOS,
     PLATFORM_FEATURE_NAME_TYPE("CrOSLateBootAudioHFPSwb")},
    {"audio-suppress-set-rtc-audio-active",
     flag_descriptions::kAudioSuppressSetRTCAudioActiveName,
     flag_descriptions::kAudioSuppressSetRTCAudioActiveDescription, kOsCrOS,
     PLATFORM_FEATURE_NAME_TYPE("CrOSLateBootAudioSuppressSetRTCAudioActive")},
    {"cras-split-alsa-usb-internal",
     flag_descriptions::kCrasSplitAlsaUsbInternalName,
     flag_descriptions::kCrasSplitAlsaUsbInternalDescription, kOsCrOS,
     PLATFORM_FEATURE_NAME_TYPE("CrOSLateBootCrasSplitAlsaUSBInternal")},
    {"disable-explicit-dma-fences",
     flag_descriptions::kDisableExplicitDmaFencesName,
     flag_descriptions::kDisableExplicitDmaFencesDescription, kOsCrOS,
     SINGLE_VALUE_TYPE(switches::kDisableExplicitDmaFences)},
    // TODO(crbug.com/1012846): Remove this flag and provision when HDR is fully
    //  supported on ChromeOS.
    {"use-hdr-transfer-function",
     flag_descriptions::kUseHDRTransferFunctionName,
     flag_descriptions::kUseHDRTransferFunctionDescription, kOsCrOS,
     FEATURE_VALUE_TYPE(display::features::kUseHDRTransferFunction)},
    {"enable-external-display-hdr10",
     flag_descriptions::kEnableExternalDisplayHdr10Name,
     flag_descriptions::kEnableExternalDisplayHdr10Description, kOsCrOS,
     FEATURE_VALUE_TYPE(display::features::kEnableExternalDisplayHDR10Mode)},
    {"adaptive-charging", flag_descriptions::kAdaptiveChargingName,
     flag_descriptions::kAdaptiveChargingDescription, kOsCrOS,
     FEATURE_VALUE_TYPE(ash::features::kAdaptiveCharging)},
    {"adaptive-charging-for-testing",
     flag_descriptions::kAdaptiveChargingForTestingName,
     flag_descriptions::kAdaptiveChargingForTestingDescription, kOsCrOS,
     FEATURE_VALUE_TYPE(ash::features::kAdaptiveChargingForTesting)},
    {"ash-capture-mode-audio-mixing",
     flag_descriptions::kCaptureModeAudioMixingName,
     flag_descriptions::kCaptureModeAudioMixingDescription, kOsCrOS,
     FEATURE_VALUE_TYPE(ash::features::kCaptureModeAudioMixing)},
    {"ash-capture-mode-education", flag_descriptions::kCaptureModeEducationName,
     flag_descriptions::kCaptureModeEducationDescription, kOsCrOS,
     FEATURE_WITH_PARAMS_VALUE_TYPE(ash::features::kCaptureModeEducation,
                                    kCaptureModeEducationVariations,
                                    "CaptureModeEducation")},
    {"ash-capture-mode-education-bypass-limits",
     flag_descriptions::kCaptureModeEducationBypassLimitsName,
     flag_descriptions::kCaptureModeEducationBypassLimitsDescription, kOsCrOS,
     FEATURE_VALUE_TYPE(ash::features::kCaptureModeEducationBypassLimits)},
    {"ash-capture-mode-gif-recording",
     flag_descriptions::kCaptureModeGifRecordingName,
     flag_descriptions::kCaptureModeGifRecordingDescription, kOsCrOS,
     FEATURE_VALUE_TYPE(ash::features::kGifRecording)},
    {"ash-overview-button", flag_descriptions::kOverviewButtonName,
     flag_descriptions::kOverviewButtonDescription, kOsCrOS,
     FEATURE_VALUE_TYPE(ash::features::kOverviewButton)},
    {"ash-limit-shelf-items-to-active-desk",
     flag_descriptions::kLimitShelfItemsToActiveDeskName,
     flag_descriptions::kLimitShelfItemsToActiveDeskDescription, kOsCrOS,
     FEATURE_VALUE_TYPE(ash::features::kPerDeskShelf)},
    {"ash-enable-unified-desktop",
     flag_descriptions::kAshEnableUnifiedDesktopName,
     flag_descriptions::kAshEnableUnifiedDesktopDescription, kOsCrOS,
     SINGLE_VALUE_TYPE(switches::kEnableUnifiedDesktop)},
    {"ash-faster-split-screen-setup",
     flag_descriptions::kFasterSplitScreenSetupName,
     flag_descriptions::kFasterSplitScreenSetupDescription, kOsCrOS,
     FEATURE_VALUE_TYPE(ash::features::kFasterSplitScreenSetup)},
    {"ash-snap-groups", flag_descriptions::kSnapGroupsName,
     flag_descriptions::kSnapGroupsDescription, kOsCrOS,
     FEATURE_VALUE_TYPE(ash::features::kSnapGroup)},
    {"rounded-display", flag_descriptions::kRoundedDisplay,
     flag_descriptions::kRoundedDisplayDescription, kOsCrOS,
     FEATURE_VALUE_TYPE(display::features::kRoundedDisplay)},
    {"bluetooth-fix-a2dp-packet-size",
     flag_descriptions::kBluetoothFixA2dpPacketSizeName,
     flag_descriptions::kBluetoothFixA2dpPacketSizeDescription, kOsCrOS,
     FEATURE_VALUE_TYPE(ash::features::kBluetoothFixA2dpPacketSize)},
    {"bluetooth-quality-report", flag_descriptions::kBluetoothQualityReportName,
     flag_descriptions::kBluetoothQualityReportDescription, kOsCrOS,
     FEATURE_VALUE_TYPE(ash::features::kBluetoothQualityReport)},
    {"bluetooth-coredump", flag_descriptions::kBluetoothCoredumpName,
     flag_descriptions::kBluetoothCoredumpDescription, kOsCrOS,
     FEATURE_VALUE_TYPE(chromeos::bluetooth::features::kBluetoothCoredump)},
    {"bluetooth-floss-coredump", flag_descriptions::kBluetoothFlossCoredumpName,
     flag_descriptions::kBluetoothFlossCoredumpDescription, kOsCrOS,
     FEATURE_VALUE_TYPE(
         chromeos::bluetooth::features::kBluetoothFlossCoredump)},
    {"bluetooth-floss-telephony",
     flag_descriptions::kBluetoothFlossTelephonyName,
     flag_descriptions::kBluetoothFlossTelephonyDescription, kOsCrOS,
     FEATURE_VALUE_TYPE(
         chromeos::bluetooth::features::kBluetoothFlossTelephony)},
    {kBluetoothUseFlossInternalName, flag_descriptions::kBluetoothUseFlossName,
     flag_descriptions::kBluetoothUseFlossDescription, kOsCrOS,
     FEATURE_VALUE_TYPE(floss::features::kFlossEnabled)},
    {"bluetooth-floss-availability-check",
     flag_descriptions::kBluetoothFlossIsAvailabilityCheckNeededName,
     flag_descriptions::kBluetoothFlossIsAvailabilityCheckNeededDescription,
     kOsCrOS,
     FEATURE_VALUE_TYPE(floss::features::kFlossIsAvailabilityCheckNeeded)},
    {"bluetooth-use-llprivacy", flag_descriptions::kBluetoothUseLLPrivacyName,
     flag_descriptions::kBluetoothUseLLPrivacyDescription, kOsCrOS,
     FEATURE_VALUE_TYPE(bluez::features::kLinkLayerPrivacy)},
    {"cellular-bypass-esim-installation-connectivity-check",
     flag_descriptions::kCellularBypassESimInstallationConnectivityCheckName,
     flag_descriptions::
         kCellularBypassESimInstallationConnectivityCheckDescription,
     kOsCrOS,
     FEATURE_VALUE_TYPE(
         ash::features::kCellularBypassESimInstallationConnectivityCheck)},
    {"cellular-carrier-lock", flag_descriptions::kCellularCarrierLockName,
     flag_descriptions::kCellularCarrierLockDescription, kOsCrOS,
     FEATURE_VALUE_TYPE(ash::features::kCellularCarrierLock)},
    {"cellular-use-second-euicc",
     flag_descriptions::kCellularUseSecondEuiccName,
     flag_descriptions::kCellularUseSecondEuiccDescription, kOsCrOS,
     FEATURE_VALUE_TYPE(ash::features::kCellularUseSecondEuicc)},
    {"cros-privacy-hub-v0", flag_descriptions::kCrosPrivacyHubV0Name,
     flag_descriptions::kCrosPrivacyHubV0Description, kOsCrOS,
     FEATURE_VALUE_TYPE(ash::features::kCrosPrivacyHubV0)},
    {"cros-privacy-hub-app-permissions",
     flag_descriptions::kCrosPrivacyHubAppPermissionsName,
     flag_descriptions::kCrosPrivacyHubAppPermissionsDescription, kOsCrOS,
     FEATURE_VALUE_TYPE(ash::features::kCrosPrivacyHubAppPermissions)},
    {"enable-cros-privacy-hub", flag_descriptions::kCrosPrivacyHubName,
     flag_descriptions::kCrosPrivacyHubDescription, kOsCrOS,
     FEATURE_VALUE_TYPE(ash::features::kCrosPrivacyHub)},
    {"cros-components", flag_descriptions::kCrosComponentsName,
     flag_descriptions::kCrosComponentsDescription, kOsCrOS | kOsLacros,
     FEATURE_VALUE_TYPE(chromeos::features::kCrosComponents)},
    {"oobe-hid-detection-revamp",
     flag_descriptions::kOobeHidDetectionRevampName,
     flag_descriptions::kOobeHidDetectionRevampDescription, kOsCrOS,
     FEATURE_VALUE_TYPE(ash::features::kOobeHidDetectionRevamp)},
    {"os-feedback-dialog", flag_descriptions::kOsFeedbackDialogName,
     flag_descriptions::kOsFeedbackDialogDescription, kOsCrOS,
     FEATURE_VALUE_TYPE(ash::features::kOsFeedbackDialog)},
    {"os-feedback-jelly", flag_descriptions::kOsFeedbackJellyName,
     flag_descriptions::kOsFeedbackJellyDescription, kOsCrOS,
     FEATURE_VALUE_TYPE(ash::features::kOsFeedbackJelly)},
    {"os-settings-app-badging-toggle",
     flag_descriptions::kOsSettingsAppBadgingToggleName,
     flag_descriptions::kOsSettingsAppBadgingToggleDescription, kOsCrOS,
     FEATURE_VALUE_TYPE(ash::features::kOsSettingsAppBadgingToggle)},
    {"os-settings-deprecate-sync-metrics-toggle",
     flag_descriptions::kOsSettingsDeprecateSyncMetricsToggleName,
     flag_descriptions::kOsSettingsDeprecateSyncMetricsToggleDescription,
     kOsCrOS,
     FEATURE_VALUE_TYPE(ash::features::kOsSettingsDeprecateSyncMetricsToggle)},
    {"os-settings-revamp-wayfinding",
     flag_descriptions::kOsSettingsRevampWayfindingName,
     flag_descriptions::kOsSettingsRevampWayfindingDescription, kOsCrOS,
     FEATURE_VALUE_TYPE(ash::features::kOsSettingsRevampWayfinding)},
    {"disable-cancel-all-touches",
     flag_descriptions::kDisableCancelAllTouchesName,
     flag_descriptions::kDisableCancelAllTouchesDescription, kOsCrOS,
     SINGLE_VALUE_TYPE(switches::kDisableCancelAllTouches)},
    {
        "enable-background-blur",
        flag_descriptions::kEnableBackgroundBlurName,
        flag_descriptions::kEnableBackgroundBlurDescription,
        kOsCrOS,
        FEATURE_VALUE_TYPE(ash::features::kEnableBackgroundBlur),
    },
    {"enable-zram-writeback", flag_descriptions::kEnableZramWriteback,
     flag_descriptions::kEnableZramWritebackDescription, kOsCrOS,
     FEATURE_VALUE_TYPE(ash::kCrOSEnableZramWriteback)},
    {kEnableSuspendToDiskInternalName, flag_descriptions::kEnableSuspendToDisk,
     flag_descriptions::kEnableSuspendToDiskDescription, kOsCrOS,
     FEATURE_WITH_PARAMS_VALUE_TYPE(ash::features::kSuspendToDisk,
                                    kHibernateFeatureVariations,
                                    "SuspendToDisk")},
    // Used to carry the policy value crossing the Chrome process lifetime.
    {ash::standalone_browser::kLacrosAvailabilityPolicyInternalName, "", "",
     kOsCrOS, MULTI_VALUE_TYPE(kLacrosAvailabilityPolicyChoices)},
    // Used to carry the policy value crossing the Chrome process lifetime.
    {crosapi::browser_util::kLacrosDataBackwardMigrationModePolicyInternalName,
     "", "", kOsCrOS,
     MULTI_VALUE_TYPE(kLacrosDataBackwardMigrationModePolicyChoices)},
    {kLacrosStabilityInternalName, flag_descriptions::kLacrosStabilityName,
     flag_descriptions::kLacrosStabilityDescription, kOsCrOS,
     MULTI_VALUE_TYPE(kLacrosStabilityChoices)},
    {kLacrosWaylandLoggingInternalName,
     flag_descriptions::kLacrosWaylandLoggingName,
     flag_descriptions::kLacrosWaylandLoggingDescription, kOsCrOS,
     FEATURE_VALUE_TYPE(ash::features::kLacrosWaylandLogging)},
    {kPreferDcheckInternalName, flag_descriptions::kPreferDcheckName,
     flag_descriptions::kPreferDcheckDescription, kOsCrOS,
     MULTI_VALUE_TYPE(kPreferDcheckChoices)},
    {"lacros-profile-migration-force-off",
     flag_descriptions::kLacrosProfileMigrationForceOffName,
     flag_descriptions::kLacrosProfileMigrationForceOffDescription, kOsCrOS,
     FEATURE_VALUE_TYPE(
         ash::standalone_browser::features::kLacrosProfileMigrationForceOff)},
    {"lacros-trigger-profile-backward-migration",
     flag_descriptions::kLacrosProfileBackwardMigrationName,
     flag_descriptions::kLacrosProfileBackwardMigrationDescription, kOsCrOS,
     FEATURE_VALUE_TYPE(ash::features::kLacrosProfileBackwardMigration)},
    {kLacrosSelectionInternalName, flag_descriptions::kLacrosSelectionName,
     flag_descriptions::kLacrosSelectionDescription, kOsCrOS,
     MULTI_VALUE_TYPE(kLacrosSelectionChoices)},
    {kLacrosSelectionPolicyIgnoreInternalName,
     flag_descriptions::kLacrosSelectionPolicyIgnoreName,
     flag_descriptions::kLacrosSelectionPolicyIgnoreDescription, kOsCrOS,
     SINGLE_VALUE_TYPE(ash::switches::kLacrosSelectionPolicyIgnore)},
    {kLacrosOnlyInternalName, flag_descriptions::kLacrosOnlyName,
     flag_descriptions::kLacrosOnlyDescription, kOsCrOS,
     FEATURE_VALUE_TYPE(ash::standalone_browser::features::kLacrosOnly)},
    {kLacrosAvailabilityIgnoreInternalName,
     flag_descriptions::kLacrosAvailabilityIgnoreName,
     flag_descriptions::kLacrosAvailabilityIgnoreDescription, kOsCrOS,
     SINGLE_VALUE_TYPE(ash::switches::kLacrosAvailabilityIgnore)},
    {"list-all-display-modes", flag_descriptions::kListAllDisplayModesName,
     flag_descriptions::kListAllDisplayModesDescription, kOsCrOS,
     FEATURE_VALUE_TYPE(display::features::kListAllDisplayModes)},
    {"enable-hardware_mirror-mode",
     flag_descriptions::kEnableHardwareMirrorModeName,
     flag_descriptions::kEnableHardwareMirrorModeDescription, kOsCrOS,
     FEATURE_VALUE_TYPE(display::features::kEnableHardwareMirrorMode)},
    {"enable-edid-based-display-ids",
     flag_descriptions::kEnableEdidBasedDisplayIdsName,
     flag_descriptions::kEnableEdidBasedDisplayIdsDescription, kOsCrOS,
     FEATURE_VALUE_TYPE(display::features::kEnableEdidBasedDisplayIds)},
    {"enable-wifi-qos", flag_descriptions::kEnableWifiQosName,
     flag_descriptions::kEnableWifiQosDescription, kOsCrOS,
     FEATURE_VALUE_TYPE(ash::features::kEnableWifiQos)},
    {"enforce-ash-extension-keeplist",
     flag_descriptions::kEnforceAshExtensionKeeplistName,
     flag_descriptions::kEnforceAshExtensionKeeplistDescription, kOsCrOS,
     FEATURE_VALUE_TYPE(ash::features::kEnforceAshExtensionKeeplist)},
    {"hotspot", flag_descriptions::kHotspotName,
     flag_descriptions::kHotspotDescription, kOsCrOS,
     FEATURE_VALUE_TYPE(ash::features::kHotspot)},
    {"instant-hotspot-rebrand", flag_descriptions::kInstantHotspotRebrandName,
     flag_descriptions::kInstantHotspotRebrandDescription, kOsCrOS,
     FEATURE_VALUE_TYPE(ash::features::kInstantHotspotRebrand)},
    {"instant-tethering", flag_descriptions::kTetherName,
     flag_descriptions::kTetherDescription, kOsCrOS,
     FEATURE_VALUE_TYPE(ash::features::kInstantTethering)},
    {"improved-keyboard-shortcuts",
     flag_descriptions::kImprovedKeyboardShortcutsName,
     flag_descriptions::kImprovedKeyboardShortcutsDescription, kOsCrOS,
     FEATURE_VALUE_TYPE(features::kImprovedKeyboardShortcuts)},
    {"deprecate-alt-click", flag_descriptions::kDeprecateAltClickName,
     flag_descriptions::kDeprecateAltClickDescription, kOsCrOS,
     FEATURE_VALUE_TYPE(features::kDeprecateAltClick)},
    {"deprecate-old-keyboard-shortcuts-accelerator",
     flag_descriptions::kDeprecateOldKeyboardShortcutsAcceleratorName,
     flag_descriptions::kDeprecateOldKeyboardShortcutsAcceleratorDescription,
     kOsCrOS,
     FEATURE_VALUE_TYPE(
         ash::features::kDeprecateOldKeyboardShortcutsAccelerator)},
    {"show-bluetooth-debug-log-toggle",
     flag_descriptions::kShowBluetoothDebugLogToggleName,
     flag_descriptions::kShowBluetoothDebugLogToggleDescription, kOsCrOS,
     FEATURE_VALUE_TYPE(ash::features::kShowBluetoothDebugLogToggle)},
    {"show-taps", flag_descriptions::kShowTapsName,
     flag_descriptions::kShowTapsDescription, kOsCrOS,
     SINGLE_VALUE_TYPE(ash::switches::kShowTaps)},
    {"show-touch-hud", flag_descriptions::kShowTouchHudName,
     flag_descriptions::kShowTouchHudDescription, kOsCrOS,
     SINGLE_VALUE_TYPE(ash::switches::kAshTouchHud)},
    {"stylus-battery-status", flag_descriptions::kStylusBatteryStatusName,
     flag_descriptions::kStylusBatteryStatusDescription, kOsCrOS,
     FEATURE_VALUE_TYPE(ash::features::kStylusBatteryStatus)},
    {"suppress-text-messages", flag_descriptions::kSuppressTextMessagesName,
     flag_descriptions::kSuppressTextMessagesDescription, kOsCrOS,
     FEATURE_VALUE_TYPE(ash::features::kSuppressTextMessages)},
    {"wake-on-wifi-allowed", flag_descriptions::kWakeOnWifiAllowedName,
     flag_descriptions::kWakeOnWifiAllowedDescription, kOsCrOS,
     FEATURE_VALUE_TYPE(ash::features::kWakeOnWifiAllowed)},
    {"microphone-mute-notifications",
     flag_descriptions::kMicrophoneMuteNotificationsName,
     flag_descriptions::kMicrophoneMuteNotificationsDescription, kOsCrOS,
     FEATURE_VALUE_TYPE(ash::features::kMicMuteNotifications)},
    {"microphone-mute-switch-device",
     flag_descriptions::kMicrophoneMuteSwitchDeviceName,
     flag_descriptions::kMicrophoneMuteSwitchDeviceDescription, kOsCrOS,
     SINGLE_VALUE_TYPE("enable-microphone-mute-switch-device")},
    {"wifi-connect-mac-address-randomization",
     flag_descriptions::kWifiConnectMacAddressRandomizationName,
     flag_descriptions::kWifiConnectMacAddressRandomizationDescription, kOsCrOS,
     FEATURE_VALUE_TYPE(ash::features::kWifiConnectMacAddressRandomization)},
    {"consumer-auto-update-toggle-allowed",
     flag_descriptions::kConsumerAutoUpdateToggleAllowedName,
     flag_descriptions::kConsumerAutoUpdateToggleAllowedDescription, kOsCrOS,
     FEATURE_VALUE_TYPE(ash::features::kConsumerAutoUpdateToggleAllowed)},
    {"diagnostics-app-jelly", flag_descriptions::kDiagnosticsAppJellyName,
     flag_descriptions::kDiagnosticsAppJellyDescription, kOsCrOS,
     FEATURE_VALUE_TYPE(ash::features::kDiagnosticsAppJelly)},
    {"disable-lacros-tts-support",
     flag_descriptions::kDisableLacrosTtsSupportName,
     flag_descriptions::kDisableLacrosTtsSupportDescription, kOsCrOS,
     FEATURE_VALUE_TYPE(ash::features::kDisableLacrosTtsSupport)},
    {"disable-dns-proxy", flag_descriptions::kDisableDnsProxyName,
     flag_descriptions::kDisableDnsProxyDescription, kOsCrOS,
     FEATURE_VALUE_TYPE(ash::features::kDisableDnsProxy)},
    {"firmware-update-jelly", flag_descriptions::kFirmwareUpdateJellyName,
     flag_descriptions::kFirmwareUpdateJellyDescription, kOsCrOS,
     FEATURE_VALUE_TYPE(ash::features::kFirmwareUpdateJelly)},
    {"firmware-update-ui-v2", flag_descriptions::kFirmwareUpdateUIV2Name,
     flag_descriptions::kFirmwareUpdateUIV2Description, kOsCrOS,
     FEATURE_VALUE_TYPE(ash::features::kFirmwareUpdateUIV2)},
    {"screen-saver-duration", flag_descriptions::kScreenSaverDurationName,
     flag_descriptions::kScreenSaverDurationDescription, kOsCrOS,
     FEATURE_VALUE_TYPE(ash::features::kScreenSaverDuration)},
    {"multi-zone-rgb-keyboard", flag_descriptions::kMultiZoneRgbKeyboardName,
     flag_descriptions::kMultiZoneRgbKeyboardDescription, kOsCrOS,
     FEATURE_VALUE_TYPE(ash::features::kMultiZoneRgbKeyboard)},
    {"passpoint-arc-support", flag_descriptions::kPasspointARCSupportName,
     flag_descriptions::kPasspointARCSupportDescription, kOsCrOS,
     FEATURE_VALUE_TYPE(ash::features::kPasspointARCSupport)},
    {"passpoint-settings", flag_descriptions::kPasspointSettingsName,
     flag_descriptions::kPasspointSettingsDescription, kOsCrOS,
     FEATURE_VALUE_TYPE(ash::features::kPasspointSettings)},
    {"policy-provided-trust-anchors-allowed-at-lock-screen",
     flag_descriptions::kPolicyProvidedTrustAnchorsAllowedAtLockScreenName,
     flag_descriptions::
         kPolicyProvidedTrustAnchorsAllowedAtLockScreenDescription,
     kOsCrOS,
     FEATURE_VALUE_TYPE(
         ash::features::kPolicyProvidedTrustAnchorsAllowedAtLockScreen)},
    {kTimeOfDayScreenSaverInternalName,
     flag_descriptions::kTimeOfDayScreenSaverName,
     flag_descriptions::kTimeOfDayScreenSaverDescription, kOsCrOS,
     FEATURE_VALUE_TYPE(ash::features::kTimeOfDayScreenSaver)},
    {kTimeOfDayWallpaperInternalName,
     flag_descriptions::kTimeOfDayWallpaperName,
     flag_descriptions::kTimeOfDayWallpaperDescription, kOsCrOS,
     FEATURE_VALUE_TYPE(ash::features::kTimeOfDayWallpaper)},
    {kTimeOfDayWallpaperForcedAutoScheduleInternalName,
     flag_descriptions::kTimeOfDayWallpaperForcedAutoScheduleName,
     flag_descriptions::kTimeOfDayWallpaperForcedAutoScheduleDescription,
     kOsCrOS,
     FEATURE_VALUE_TYPE(ash::features::kTimeOfDayWallpaperForcedAutoSchedule)},
    {kTimeOfDayDlcInternalName, flag_descriptions::kTimeOfDayDlcName,
     flag_descriptions::kTimeOfDayDlcDescription, kOsCrOS,
     FEATURE_VALUE_TYPE(ash::features::kTimeOfDayDlc)},
    {"enable-rfc-8925", flag_descriptions::kEnableRFC8925Name,
     flag_descriptions::kEnableRFC8925Description, kOsCrOS,
     FEATURE_VALUE_TYPE(ash::features::kEnableRFC8925)},
    {"support-f11-and-f12-shortcuts",
     flag_descriptions::kSupportF11AndF12ShortcutsName,
     flag_descriptions::kSupportF11AndF12ShortcutsDescription, kOsCrOS,
     FEATURE_VALUE_TYPE(features::kSupportF11AndF12KeyShortcuts)},
#endif  // BUILDFLAG(IS_CHROMEOS_ASH)

#if BUILDFLAG(IS_CHROMEOS)
    {"cros-apps-background-event-handling",
     flag_descriptions::kCrosAppsBackgroundEventHandlingName,
     flag_descriptions::kCrosAppsBackgroundEventHandlingDescription,
     kOsCrOS | kOsLacros,
     FEATURE_VALUE_TYPE(chromeos::features::kCrosAppsBackgroundEventHandling)},
    {"cros-legacy-media-formats",
     flag_descriptions::kCrOSLegacyMediaFormatsName,
     flag_descriptions::kCrOSLegacyMediaFormatsDescription, kOsCrOS | kOsLacros,
     FEATURE_VALUE_TYPE(media::kCrOSLegacyMediaFormats)},
    {"disable-idle-sockets-close-on-memory-pressure",
     flag_descriptions::kDisableIdleSocketsCloseOnMemoryPressureName,
     flag_descriptions::kDisableIdleSocketsCloseOnMemoryPressureDescription,
     kOsCrOS | kOsLacros,
     FEATURE_VALUE_TYPE(
         chromeos::features::kDisableIdleSocketsCloseOnMemoryPressure)},
    {"disable-office-editing-component-app",
     flag_descriptions::kDisableOfficeEditingComponentAppName,
     flag_descriptions::kDisableOfficeEditingComponentAppDescription,
     kOsCrOS | kOsLacros,
     FEATURE_VALUE_TYPE(chromeos::features::kDisableOfficeEditingComponentApp)},
    {"one-group-per-renderer", flag_descriptions::kOneGroupPerRendererName,
     flag_descriptions::kOneGroupPerRendererDescription, kOsCrOS | kOsLacros,
     FEATURE_VALUE_TYPE(base::kOneGroupPerRenderer)},
    {"rounded-windows", flag_descriptions::kRoundedWindows,
     flag_descriptions::kRoundedWindowsDescription, kOsCrOS | kOsLacros,
     FEATURE_WITH_PARAMS_VALUE_TYPE(chromeos::features::kRoundedWindows,
                                    kRoundedWindowsRadiusVariation,
                                    "RoundedWindows")},
    {"cros-web-app-install-dialog",
     flag_descriptions::kCrosWebAppInstallDialogName,
     flag_descriptions::kCrosWebAppInstallDialogDescription,
     kOsCrOS | kOsLacros,
     FEATURE_VALUE_TYPE(chromeos::features::kCrosWebAppInstallDialog)},
#endif  // BUILDFLAG(IS_CHROMEOS)
    {
        "disable-accelerated-video-decode",
        flag_descriptions::kAcceleratedVideoDecodeName,
        flag_descriptions::kAcceleratedVideoDecodeDescription,
        kOsMac | kOsWin | kOsCrOS | kOsAndroid | kOsLacros | kOsFuchsia |
            kOsLinux,
        SINGLE_DISABLE_VALUE_TYPE(switches::kDisableAcceleratedVideoDecode),
    },
    {
        "disable-accelerated-video-encode",
        flag_descriptions::kAcceleratedVideoEncodeName,
        flag_descriptions::kAcceleratedVideoEncodeDescription,
        kOsMac | kOsWin | kOsCrOS | kOsAndroid,
        SINGLE_DISABLE_VALUE_TYPE(switches::kDisableAcceleratedVideoEncode),
    },
#if BUILDFLAG(IS_WIN)
    {
        "enable-hardware-secure-decryption",
        flag_descriptions::kHardwareSecureDecryptionName,
        flag_descriptions::kHardwareSecureDecryptionDescription,
        kOsWin,
        FEATURE_VALUE_TYPE(media::kHardwareSecureDecryption),
    },
    {
        "enable-hardware-secure-decryption-experiment",
        flag_descriptions::kHardwareSecureDecryptionExperimentName,
        flag_descriptions::kHardwareSecureDecryptionExperimentDescription,
        kOsWin,
        FEATURE_VALUE_TYPE(media::kHardwareSecureDecryptionExperiment),
    },
    {
        "enable-hardware-secure-decryption-fallback",
        flag_descriptions::kHardwareSecureDecryptionFallbackName,
        flag_descriptions::kHardwareSecureDecryptionFallbackDescription,
        kOsWin,
        FEATURE_VALUE_TYPE(media::kHardwareSecureDecryptionFallback),
    },
    {
        "enable-media-foundation-clear",
        flag_descriptions::kMediaFoundationClearName,
        flag_descriptions::kMediaFoundationClearDescription,
        kOsWin,
        FEATURE_VALUE_TYPE(media::kMediaFoundationClearPlayback),
    },
    {"enable-media-foundation-clear-rendering-strategy",
     flag_descriptions::kMediaFoundationClearStrategyName,
     flag_descriptions::kMediaFoundationClearStrategyDescription, kOsWin,
     FEATURE_WITH_PARAMS_VALUE_TYPE(media::kMediaFoundationClearRendering,
                                    kMediaFoundationClearStrategyVariations,
                                    "MediaFoundationClearRendering")},
    {
        "enable-waitable-swap-chain",
        flag_descriptions::kUseWaitableSwapChainName,
        flag_descriptions::kUseWaitableSwapChainDescription,
        kOsWin,
        FEATURE_WITH_PARAMS_VALUE_TYPE(features::kDXGIWaitableSwapChain,
                                       kDXGIWaitableSwapChainVariations,
                                       "DXGIWaitableSwapChain"),
    },
#endif  // BUILDFLAG(IS_WIN)
#if BUILDFLAG(IS_WIN) || BUILDFLAG(IS_LINUX)
    {
        "fluent-overlay-scrollbars",
        flag_descriptions::kFluentOverlayScrollbarsName,
        flag_descriptions::kFluentOverlayScrollbarsDescription,
        kOsWin | kOsLinux,
        FEATURE_VALUE_TYPE(features::kFluentOverlayScrollbar),
    },
    {
        "fluent-scrollbars",
        flag_descriptions::kFluentScrollbarsName,
        flag_descriptions::kFluentScrollbarsDescription,
        kOsWin | kOsLinux,
        FEATURE_VALUE_TYPE(features::kFluentScrollbar),
    },
#endif  // BUILDFLAG(IS_WIN) || BUILDFLAG(IS_LINUX)
#if BUILDFLAG(IS_CHROMEOS_ASH)
    {
        "zero-copy-video-capture",
        flag_descriptions::kZeroCopyVideoCaptureName,
        flag_descriptions::kZeroCopyVideoCaptureDescription,
        kOsCrOS,
        ENABLE_DISABLE_VALUE_TYPE_AND_VALUE(
            switches::kVideoCaptureUseGpuMemoryBuffer,
            "1",
            switches::kDisableVideoCaptureUseGpuMemoryBuffer,
            "1"),
    },
    {
        "ash-debug-shortcuts",
        flag_descriptions::kDebugShortcutsName,
        flag_descriptions::kDebugShortcutsDescription,
        kOsAll,
        SINGLE_VALUE_TYPE(ash::switches::kAshDebugShortcuts),
    },
    {"ui-slow-animations", flag_descriptions::kUiSlowAnimationsName,
     flag_descriptions::kUiSlowAnimationsDescription, kOsCrOS,
     SINGLE_VALUE_TYPE(switches::kUISlowAnimations)},
#endif  // BUILDFLAG(IS_CHROMEOS_ASH)
#if BUILDFLAG(IS_WIN)
    {
        "zero-copy-video-capture",
        flag_descriptions::kZeroCopyVideoCaptureName,
        flag_descriptions::kZeroCopyVideoCaptureDescription,
        kOsWin,
        FEATURE_VALUE_TYPE(media::kMediaFoundationD3D11VideoCapture),
    },
#endif  // BUILDFLAG(IS_WIN)
    {"debug-packed-apps", flag_descriptions::kDebugPackedAppName,
     flag_descriptions::kDebugPackedAppDescription, kOsDesktop,
     SINGLE_VALUE_TYPE(switches::kDebugPackedApps)},
    {"username-first-flow-fallback-crowdsourcing",
     flag_descriptions::kUsernameFirstFlowFallbackCrowdsourcingName,
     flag_descriptions::kUsernameFirstFlowFallbackCrowdsourcingDescription,
     kOsAll,
     FEATURE_VALUE_TYPE(
         password_manager::features::kUsernameFirstFlowFallbackCrowdsourcing)},
    {"username-first-flow-store-several-values",
     flag_descriptions::kUsernameFirstFlowStoreSeveralValuesName,
     flag_descriptions::kUsernameFirstFlowStoreSeveralValuesDescription, kOsAll,
     FEATURE_VALUE_TYPE(
         password_manager::features::kUsernameFirstFlowStoreSeveralValues)},
    {"username-first-flow-with-intermediate-values",
     flag_descriptions::kUsernameFirstFlowWithIntermediateValuesName,
     flag_descriptions::kUsernameFirstFlowWithIntermediateValuesDescription,
     kOsAll,
     FEATURE_VALUE_TYPE(
         password_manager::features::kUsernameFirstFlowWithIntermediateValues)},
    {"username-first-flow-with-intermediate-values-predictions",
     flag_descriptions::kUsernameFirstFlowWithIntermediateValuesPredictionsName,
     flag_descriptions::
         kUsernameFirstFlowWithIntermediateValuesPredictionsDescription,
     kOsAll,
     FEATURE_VALUE_TYPE(
         password_manager::features::
             kUsernameFirstFlowWithIntermediateValuesPredictions)},
    {"username-first-flow-with-intermediate-values-voting",
     flag_descriptions::kUsernameFirstFlowWithIntermediateValuesVotingName,
     flag_descriptions::
         kUsernameFirstFlowWithIntermediateValuesVotingDescription,
     kOsAll,
     FEATURE_VALUE_TYPE(password_manager::features::
                            kUsernameFirstFlowWithIntermediateValuesVoting)},
    {"enable-show-autofill-signatures",
     flag_descriptions::kShowAutofillSignaturesName,
     flag_descriptions::kShowAutofillSignaturesDescription, kOsAll,
     SINGLE_VALUE_TYPE(autofill::switches::kShowAutofillSignatures)},
    {"wallet-service-use-sandbox",
     flag_descriptions::kWalletServiceUseSandboxName,
     flag_descriptions::kWalletServiceUseSandboxDescription,
     kOsAndroid | kOsDesktop,
     ENABLE_DISABLE_VALUE_TYPE_AND_VALUE(
         autofill::switches::kWalletServiceUseSandbox,
         "1",
         autofill::switches::kWalletServiceUseSandbox,
         "0")},
    {"web-authentication-screen-reader-mode",
     flag_descriptions::kWebAuthnScreenReaderModeName,
     flag_descriptions::kWebAuthnScreenReaderModeDescription, kOsDesktop,
     FEATURE_VALUE_TYPE(device::kWebAuthnScreenReaderMode)},
    {"enable-web-bluetooth", flag_descriptions::kWebBluetoothName,
     flag_descriptions::kWebBluetoothDescription, kOsLinux,
     FEATURE_VALUE_TYPE(features::kWebBluetooth)},
    {"enable-web-bluetooth-new-permissions-backend",
     flag_descriptions::kWebBluetoothNewPermissionsBackendName,
     flag_descriptions::kWebBluetoothNewPermissionsBackendDescription,
     kOsAndroid | kOsDesktop,
     FEATURE_VALUE_TYPE(features::kWebBluetoothNewPermissionsBackend)},
    {"enable-webusb-device-detection",
     flag_descriptions::kWebUsbDeviceDetectionName,
     flag_descriptions::kWebUsbDeviceDetectionDescription, kOsDesktop,
     FEATURE_VALUE_TYPE(features::kWebUsbDeviceDetection)},
#if defined(USE_AURA)
    {"overscroll-history-navigation",
     flag_descriptions::kOverscrollHistoryNavigationName,
     flag_descriptions::kOverscrollHistoryNavigationDescription, kOsAura,
     FEATURE_VALUE_TYPE(features::kOverscrollHistoryNavigation)},
    {"pull-to-refresh", flag_descriptions::kPullToRefreshName,
     flag_descriptions::kPullToRefreshDescription, kOsAura,
     MULTI_VALUE_TYPE(kPullToRefreshChoices)},
#endif  // USE_AURA
    {"overscroll-history-navigation-setting",
     flag_descriptions::kOverscrollHistoryNavigationSettingName,
     flag_descriptions::kOverscrollHistoryNavigationSettingDescription,
     kOsDesktop,
     FEATURE_VALUE_TYPE(features::kOverscrollHistoryNavigationSetting)},
    {"enable-touch-drag-drop", flag_descriptions::kTouchDragDropName,
     flag_descriptions::kTouchDragDropDescription, kOsWin | kOsCrOS,
     ENABLE_DISABLE_VALUE_TYPE(switches::kEnableTouchDragDrop,
                               switches::kDisableTouchDragDrop)},
    {"touch-selection-strategy", flag_descriptions::kTouchSelectionStrategyName,
     flag_descriptions::kTouchSelectionStrategyDescription,
     kOsAndroid,  // TODO(mfomitchev): Add CrOS/Win/Linux support soon.
     MULTI_VALUE_TYPE(kTouchTextSelectionStrategyChoices)},
#if BUILDFLAG(IS_CHROMEOS_ASH)
    {"enable-virtual-keyboard", flag_descriptions::kVirtualKeyboardName,
     flag_descriptions::kVirtualKeyboardDescription, kOsCrOS,
     SINGLE_VALUE_TYPE(keyboard::switches::kEnableVirtualKeyboard)},
    {"disable-virtual-keyboard",
     flag_descriptions::kVirtualKeyboardDisabledName,
     flag_descriptions::kVirtualKeyboardDisabledDescription, kOsCrOS,
     SINGLE_VALUE_TYPE(keyboard::switches::kDisableVirtualKeyboard)},
#endif  // BUILDFLAG(IS_CHROMEOS_ASH)
    {"enable-webgl-developer-extensions",
     flag_descriptions::kWebglDeveloperExtensionsName,
     flag_descriptions::kWebglDeveloperExtensionsDescription, kOsAll,
     SINGLE_VALUE_TYPE(switches::kEnableWebGLDeveloperExtensions)},
    {"enable-webgl-draft-extensions",
     flag_descriptions::kWebglDraftExtensionsName,
     flag_descriptions::kWebglDraftExtensionsDescription, kOsAll,
     SINGLE_VALUE_TYPE(switches::kEnableWebGLDraftExtensions)},
    {"enable-zero-copy", flag_descriptions::kZeroCopyName,
     flag_descriptions::kZeroCopyDescription, kOsAll,
     ENABLE_DISABLE_VALUE_TYPE(blink::switches::kEnableZeroCopy,
                               blink::switches::kDisableZeroCopy)},
    {"enable-vulkan", flag_descriptions::kEnableVulkanName,
     flag_descriptions::kEnableVulkanDescription,
     kOsWin | kOsLinux | kOsAndroid | kOsCrOS | kOsLacros,
     FEATURE_VALUE_TYPE(features::kVulkan)},
    {"default-angle-vulkan", flag_descriptions::kDefaultAngleVulkanName,
     flag_descriptions::kDefaultAngleVulkanDescription,
     kOsLinux | kOsAndroid | kOsCrOS | kOsLacros,
     FEATURE_VALUE_TYPE(features::kDefaultANGLEVulkan)},
    {"vulkan-from-angle", flag_descriptions::kVulkanFromAngleName,
     flag_descriptions::kVulkanFromAngleDescription,
     kOsLinux | kOsAndroid | kOsCrOS | kOsLacros,
     FEATURE_VALUE_TYPE(features::kVulkanFromANGLE)},
#if BUILDFLAG(IS_ANDROID)
    {"translate-message-ui", flag_descriptions::kTranslateMessageUIName,
     flag_descriptions::kTranslateMessageUIDescription, kOsAndroid,
     FEATURE_WITH_PARAMS_VALUE_TYPE(translate::kTranslateMessageUI,
                                    kTranslateMessageUIVariations,
                                    "TranslateMessageUI")},
#endif  // BUILDFLAG(IS_ANDROID)

#if BUILDFLAG(ENABLE_SYSTEM_NOTIFICATIONS) && !BUILDFLAG(IS_CHROMEOS_ASH)
    {"enable-system-notifications",
     flag_descriptions::kNotificationsSystemFlagName,
     flag_descriptions::kNotificationsSystemFlagDescription,
     kOsMac | kOsLinux | kOsWin,
     FEATURE_VALUE_TYPE(features::kSystemNotifications)},
#endif  // BUILDFLAG(ENABLE_SYSTEM_NOTIFICATIONS) && !BUILDFLAG(IS_CHROMEOS_ASH)
#if BUILDFLAG(IS_ANDROID)
    {"adaptive-button-in-top-toolbar",
     flag_descriptions::kAdaptiveButtonInTopToolbarName,
     flag_descriptions::kAdaptiveButtonInTopToolbarDescription, kOsAndroid,
     FEATURE_WITH_PARAMS_VALUE_TYPE(
         chrome::android::kAdaptiveButtonInTopToolbar,
         kAdaptiveButtonInTopToolbarVariations,
         "OptionalToolbarButton")},
    {"adaptive-button-in-top-toolbar-translate",
     flag_descriptions::kAdaptiveButtonInTopToolbarTranslateName,
     flag_descriptions::kAdaptiveButtonInTopToolbarTranslateDescription,
     kOsAndroid,
     FEATURE_VALUE_TYPE(chrome::android::kAdaptiveButtonInTopToolbarTranslate)},
    {"adaptive-button-in-top-toolbar-add-to-bookmarks",
     flag_descriptions::kAdaptiveButtonInTopToolbarAddToBookmarksName,
     flag_descriptions::kAdaptiveButtonInTopToolbarAddToBookmarksDescription,
     kOsAndroid,
     FEATURE_VALUE_TYPE(
         chrome::android::kAdaptiveButtonInTopToolbarAddToBookmarks)},
    {"adaptive-button-in-top-toolbar-customization",
     flag_descriptions::kAdaptiveButtonInTopToolbarCustomizationName,
     flag_descriptions::kAdaptiveButtonInTopToolbarCustomizationDescription,
     kOsAndroid,
     FEATURE_WITH_PARAMS_VALUE_TYPE(
         chrome::android::kAdaptiveButtonInTopToolbarCustomizationV2,
         kAdaptiveButtonInTopToolbarCustomizationVariations,
         "OptionalToolbarButtonCustomization")},
    {"contextual-page-actions", flag_descriptions::kContextualPageActionsName,
     flag_descriptions::kContextualPageActionsDescription, kOsAndroid,
     FEATURE_WITH_PARAMS_VALUE_TYPE(
         segmentation_platform::features::kContextualPageActions,
         kContextualPageActionsVariations,
         "ContextualPageActions")},
    {"contextual-page-actions-with-price-tracking",
     flag_descriptions::kContextualPageActionsPriceTrackingName,
     flag_descriptions::kContextualPageActionsPriceTrackingDescription,
     kOsAndroid,
     FEATURE_WITH_PARAMS_VALUE_TYPE(
         segmentation_platform::features::kContextualPageActionPriceTracking,
         kContextualPageActionPriceTrackingVariations,
         "ContextualPageActionPriceTracking")},
    {"contextual-page-actions-reader-mode",
     flag_descriptions::kContextualPageActionsReaderModeName,
     flag_descriptions::kContextualPageActionsReaderModeDescription, kOsAndroid,
     FEATURE_WITH_PARAMS_VALUE_TYPE(
         segmentation_platform::features::kContextualPageActionReaderMode,
         kContextualPageActionReaderModeVariations,
         "ContextualPageActionReaderMode")},
    {"contextual-page-actions-share-model",
     flag_descriptions::kContextualPageActionsShareModelName,
     flag_descriptions::kContextualPageActionsShareModelDescription, kOsAndroid,
     FEATURE_VALUE_TYPE(
         segmentation_platform::features::kContextualPageActionShareModel)},
    {"reader-mode-heuristics", flag_descriptions::kReaderModeHeuristicsName,
     flag_descriptions::kReaderModeHeuristicsDescription, kOsAndroid,
     MULTI_VALUE_TYPE(kReaderModeHeuristicsChoices)},
    {"screenshots-for-android-v2",
     flag_descriptions::kScreenshotsForAndroidV2Name,
     flag_descriptions::kScreenshotsForAndroidV2Description, kOsAndroid,
     FEATURE_VALUE_TYPE(share::kScreenshotsForAndroidV2)},
    {"default-viewport-is-device-width",
     flag_descriptions::kDefaultViewportIsDeviceWidthName,
     flag_descriptions::kDefaultViewportIsDeviceWidthDescription, kOsAndroid,
     FEATURE_VALUE_TYPE(blink::features::kDefaultViewportIsDeviceWidth)},
#endif  // BUILDFLAG(IS_ANDROID)
    {"in-product-help-demo-mode-choice",
     flag_descriptions::kInProductHelpDemoModeChoiceName,
     flag_descriptions::kInProductHelpDemoModeChoiceDescription, kOsAll,
     FEATURE_WITH_PARAMS_VALUE_TYPE(
         feature_engagement::kIPHDemoMode,
         feature_engagement::kIPHDemoModeChoiceVariations,
         "IPH_DemoMode")},
    {"in-product-help-use-client-config",
     flag_descriptions::kInProductHelpUseClientConfigName,
     flag_descriptions::kInProductHelpUseClientConfigDescription, kOsAll,
     FEATURE_VALUE_TYPE(feature_engagement::kUseClientConfigIPH)},
#if BUILDFLAG(IS_CHROMEOS_ASH)
    {"enable-lock-screen-notification",
     flag_descriptions::kLockScreenNotificationName,
     flag_descriptions::kLockScreenNotificationDescription, kOsCrOS,
     FEATURE_VALUE_TYPE(ash::features::kLockScreenNotifications)},
#endif  // BUILDFLAG(IS_CHROMEOS_ASH)
#if BUILDFLAG(IS_CHROMEOS_ASH)
    {"system-extensions", flag_descriptions::kSystemExtensionsName,
     flag_descriptions::kSystemExtensionsDescription, kOsCrOS,
     FEATURE_VALUE_TYPE(ash::features::kSystemExtensions)},
    {"enable-service-workers-for-chrome-untrusted",
     flag_descriptions::kEnableServiceWorkersForChromeUntrustedName,
     flag_descriptions::kEnableServiceWorkersForChromeUntrustedDescription,
     kOsCrOS,
     FEATURE_VALUE_TYPE(features::kEnableServiceWorkersForChromeUntrusted)},
    {"enterprise-reporting-ui", flag_descriptions::kEnterpriseReportingUIName,
     flag_descriptions::kEnterpriseReportingUIDescription, kOsCrOS,
     FEATURE_VALUE_TYPE(ash::features::kEnterpriseReportingUI)},
    {"crostini-reset-lxd-db", flag_descriptions::kCrostiniResetLxdDbName,
     flag_descriptions::kCrostiniResetLxdDbDescription, kOsCrOS,
     FEATURE_VALUE_TYPE(ash::features::kCrostiniResetLxdDb)},
    {"terminal-alternative-emulator",
     flag_descriptions::kTerminalAlternativeEmulatorName,
     flag_descriptions::kTerminalAlternativeEmulatorDescription, kOsCrOS,
     FEATURE_VALUE_TYPE(ash::features::kTerminalAlternativeEmulator)},
    {"terminal-dev", flag_descriptions::kTerminalDevName,
     flag_descriptions::kTerminalDevDescription, kOsCrOS,
     FEATURE_VALUE_TYPE(ash::features::kTerminalDev)},
    {"permissive-usb-passthrough",
     flag_descriptions::kPermissiveUsbPassthroughName,
     flag_descriptions::kPermissiveUsbPassthroughDescription, kOsCrOS,
     PLATFORM_FEATURE_NAME_TYPE("CrOSLateBootPermissiveUsbPassthrough")},
    {"crostini-multi-container", flag_descriptions::kCrostiniMultiContainerName,
     flag_descriptions::kCrostiniMultiContainerDescription, kOsCrOS,
     FEATURE_VALUE_TYPE(ash::features::kCrostiniMultiContainer)},
    {"crostini-qt-ime-support", flag_descriptions::kCrostiniQtImeSupportName,
     flag_descriptions::kCrostiniQtImeSupportDescription, kOsCrOS,
     FEATURE_VALUE_TYPE(ash::features::kCrostiniQtImeSupport)},
    {"crostini-virtual-keyboard-support",
     flag_descriptions::kCrostiniVirtualKeyboardSupportName,
     flag_descriptions::kCrostiniVirtualKeyboardSupportDescription, kOsCrOS,
     FEATURE_VALUE_TYPE(ash::features::kCrostiniVirtualKeyboardSupport)},
    {"crostini-use-lxd-5", flag_descriptions::kCrostiniUseLxd5Name,
     flag_descriptions::kCrostiniUseLxd5Description, kOsCrOS,
     FEATURE_VALUE_TYPE(ash::features::kCrostiniUseLxd5)},
#endif  // BUILDFLAG(IS_CHROMEOS_ASH)
#if (BUILDFLAG(IS_CHROMEOS) || BUILDFLAG(IS_LINUX) || \
     BUILDFLAG(IS_ANDROID)) &&                        \
    !BUILDFLAG(IS_NACL)
    {"mojo-linux-sharedmem", flag_descriptions::kMojoLinuxChannelSharedMemName,
     flag_descriptions::kMojoLinuxChannelSharedMemDescription,
     kOsCrOS | kOsLinux | kOsLacros | kOsAndroid,
     FEATURE_VALUE_TYPE(mojo::core::kMojoLinuxChannelSharedMem)},
#endif
#if BUILDFLAG(IS_ANDROID)
    {"enable-site-isolation-for-password-sites",
     flag_descriptions::kSiteIsolationForPasswordSitesName,
     flag_descriptions::kSiteIsolationForPasswordSitesDescription, kOsAndroid,
     FEATURE_VALUE_TYPE(
         site_isolation::features::kSiteIsolationForPasswordSites)},
    {"enable-site-per-process", flag_descriptions::kStrictSiteIsolationName,
     flag_descriptions::kStrictSiteIsolationDescription, kOsAndroid,
     SINGLE_VALUE_TYPE(switches::kSitePerProcess)},
#endif
    {"enable-isolated-web-apps", flag_descriptions::kEnableIsolatedWebAppsName,
     flag_descriptions::kEnableIsolatedWebAppsDescription, kOsDesktop,
     FEATURE_VALUE_TYPE(features::kIsolatedWebApps)},
#if BUILDFLAG(IS_CHROMEOS)
    {"enable-isolated-web-app-automatic-updates",
     flag_descriptions::kEnableIsolatedWebAppAutomaticUpdatesName,
     flag_descriptions::kEnableIsolatedWebAppAutomaticUpdatesDescription,
     kOsCrOS | kOsLacros,
     FEATURE_VALUE_TYPE(features::kIsolatedWebAppAutomaticUpdates)},
#endif
    {"enable-isolated-web-app-dev-mode",
     flag_descriptions::kEnableIsolatedWebAppDevModeName,
     flag_descriptions::kEnableIsolatedWebAppDevModeDescription, kOsDesktop,
     FEATURE_VALUE_TYPE(features::kIsolatedWebAppDevMode)},
#if BUILDFLAG(IS_CHROMEOS)
    {"install-isolated-web-app-from-url",
     flag_descriptions::kInstallIsolatedWebAppFromUrl,
     flag_descriptions::kInstallIsolatedWebAppFromUrlDescription, kOsAll,
     ORIGIN_LIST_VALUE_TYPE(switches::kInstallIsolatedWebAppFromUrl, "")},
#endif
    {"isolate-origins", flag_descriptions::kIsolateOriginsName,
     flag_descriptions::kIsolateOriginsDescription, kOsAll,
     ORIGIN_LIST_VALUE_TYPE(switches::kIsolateOrigins, "")},
    {about_flags::kSiteIsolationTrialOptOutInternalName,
     flag_descriptions::kSiteIsolationOptOutName,
     flag_descriptions::kSiteIsolationOptOutDescription, kOsAll,
     MULTI_VALUE_TYPE(kSiteIsolationOptOutChoices)},
    {"isolation-by-default", flag_descriptions::kIsolationByDefaultName,
     flag_descriptions::kIsolationByDefaultDescription, kOsAll,
     SINGLE_VALUE_TYPE(switches::kIsolationByDefault)},
    {"allow-insecure-localhost", flag_descriptions::kAllowInsecureLocalhostName,
     flag_descriptions::kAllowInsecureLocalhostDescription, kOsAll,
     SINGLE_VALUE_TYPE(switches::kAllowInsecureLocalhost)},
    {"text-based-audio-descriptions",
     flag_descriptions::kTextBasedAudioDescriptionName,
     flag_descriptions::kTextBasedAudioDescriptionDescription, kOsAll,
     FEATURE_VALUE_TYPE(features::kTextBasedAudioDescription)},
    {"bypass-app-banner-engagement-checks",
     flag_descriptions::kBypassAppBannerEngagementChecksName,
     flag_descriptions::kBypassAppBannerEngagementChecksDescription, kOsAll,
     SINGLE_VALUE_TYPE(webapps::switches::kBypassAppBannerEngagementChecks)},
    {"enable-desktop-pwas-elided-extensions-menu",
     flag_descriptions::kDesktopPWAsElidedExtensionsMenuName,
     flag_descriptions::kDesktopPWAsElidedExtensionsMenuDescription, kOsDesktop,
     FEATURE_VALUE_TYPE(features::kDesktopPWAsElidedExtensionsMenu)},
    {"enable-desktop-pwas-tab-strip",
     flag_descriptions::kDesktopPWAsTabStripName,
     flag_descriptions::kDesktopPWAsTabStripDescription, kOsDesktop,
     FEATURE_VALUE_TYPE(blink::features::kDesktopPWAsTabStrip)},
    {"enable-desktop-pwas-tab-strip-settings",
     flag_descriptions::kDesktopPWAsTabStripSettingsName,
     flag_descriptions::kDesktopPWAsTabStripSettingsDescription, kOsDesktop,
     FEATURE_VALUE_TYPE(features::kDesktopPWAsTabStripSettings)},
    {"enable-desktop-pwas-tab-strip-customizations",
     flag_descriptions::kDesktopPWAsTabStripCustomizationsName,
     flag_descriptions::kDesktopPWAsTabStripCustomizationsDescription,
     kOsDesktop,
     FEATURE_VALUE_TYPE(blink::features::kDesktopPWAsTabStripCustomizations)},
    {"enable-desktop-pwas-launch-handler",
     flag_descriptions::kDesktopPWAsLaunchHandlerName,
     flag_descriptions::kDesktopPWAsLaunchHandlerDescription, kOsDesktop,
     FEATURE_VALUE_TYPE(blink::features::kWebAppEnableLaunchHandler)},
    {"enable-desktop-pwas-sub-apps", flag_descriptions::kDesktopPWAsSubAppsName,
     flag_descriptions::kDesktopPWAsSubAppsDescription,
     kOsWin | kOsLinux | kOsLacros | kOsMac | kOsCrOS | kOsFuchsia,
     FEATURE_VALUE_TYPE(blink::features::kDesktopPWAsSubApps)},
    {"enable-desktop-pwas-scope-extensions",
     flag_descriptions::kDesktopPWAsScopeExtensionsName,
     flag_descriptions::kDesktopPWAsScopeExtensionsDescription, kOsDesktop,
     FEATURE_VALUE_TYPE(blink::features::kWebAppEnableScopeExtensions)},
    {"enable-desktop-pwas-borderless",
     flag_descriptions::kDesktopPWAsBorderlessName,
     flag_descriptions::kDesktopPWAsBorderlessDescription,
     kOsLinux | kOsLacros | kOsCrOS,
     FEATURE_VALUE_TYPE(blink::features::kWebAppBorderless)},
    {"enable-desktop-pwas-additional-windowing-controls",
     flag_descriptions::kDesktopPWAsAdditionalWindowingControlsName,
     flag_descriptions::kDesktopPWAsAdditionalWindowingControlsDescription,
     kOsWin | kOsLinux | kOsLacros | kOsMac | kOsCrOS | kOsFuchsia,
     FEATURE_VALUE_TYPE(
         blink::features::kDesktopPWAsAdditionalWindowingControls)},
    {"enable-desktop-pwas-web-bundles",
     flag_descriptions::kDesktopPWAsWebBundlesName,
     flag_descriptions::kDesktopPWAsWebBundlesDescription, kOsDesktop,
     FEATURE_VALUE_TYPE(features::kDesktopPWAsWebBundles)},
    {"record-web-app-debug-info", flag_descriptions::kRecordWebAppDebugInfoName,
     flag_descriptions::kRecordWebAppDebugInfoDescription, kOsDesktop,
     FEATURE_VALUE_TYPE(features::kRecordWebAppDebugInfo)},
#if !BUILDFLAG(IS_ANDROID)
    {"web-app-dedupe-install-urls",
     flag_descriptions::kWebAppDedupeInstallUrlsName,
     flag_descriptions::kWebAppDedupeInstallUrlsDescription, kOsDesktop,
     FEATURE_VALUE_TYPE(features::kWebAppDedupeInstallUrls)},
    {"web-app-manifest-immediate-updating",
     flag_descriptions::kWebAppManifestImmediateUpdatingName,
     flag_descriptions::kWebAppManifestImmediateUpdatingDescription, kOsDesktop,
     FEATURE_VALUE_TYPE(features::kWebAppManifestImmediateUpdating)},
    {"web-app-sync-generated-icon-background-fix",
     flag_descriptions::kWebAppSyncGeneratedIconBackgroundFixName,
     flag_descriptions::kWebAppSyncGeneratedIconBackgroundFixDescription,
     kOsDesktop,
     FEATURE_VALUE_TYPE(features::kWebAppSyncGeneratedIconBackgroundFix)},
    {"web-app-sync-generated-icon-retroactive-fix",
     flag_descriptions::kWebAppSyncGeneratedIconRetroactiveFixName,
     flag_descriptions::kWebAppSyncGeneratedIconRetroactiveFixDescription,
     kOsDesktop,
     FEATURE_VALUE_TYPE(features::kWebAppSyncGeneratedIconRetroactiveFix)},
    {"web-app-sync-generated-icon-update-fix",
     flag_descriptions::kWebAppSyncGeneratedIconUpdateFixName,
     flag_descriptions::kWebAppSyncGeneratedIconUpdateFixDescription,
     kOsDesktop,
     FEATURE_VALUE_TYPE(features::kWebAppSyncGeneratedIconUpdateFix)},
#endif  // !BUILDFLAG(IS_ANDROID)
    {"use-sync-sandbox", flag_descriptions::kSyncSandboxName,
     flag_descriptions::kSyncSandboxDescription, kOsAll,
     SINGLE_VALUE_TYPE_AND_VALUE(
         syncer::kSyncServiceURL,
         "https://chrome-sync.sandbox.google.com/chrome-sync/alpha")},
#if !BUILDFLAG(IS_ANDROID)
    {"media-router-cast-allow-all-ips",
     flag_descriptions::kMediaRouterCastAllowAllIPsName,
     flag_descriptions::kMediaRouterCastAllowAllIPsDescription, kOsDesktop,
     FEATURE_VALUE_TYPE(media_router::kCastAllowAllIPsFeature)},
    {"global-media-controls-cast-start-stop",
     flag_descriptions::kGlobalMediaControlsCastStartStopName,
     flag_descriptions::kGlobalMediaControlsCastStartStopDescription,
     kOsDesktop,
     FEATURE_VALUE_TYPE(media_router::kGlobalMediaControlsCastStartStop)},
    {"media-remoting-without-fullscreen",
     flag_descriptions::kMediaRemotingWithoutFullscreenName,
     flag_descriptions::kMediaRemotingWithoutFullscreenDescription, kOsDesktop,
     FEATURE_VALUE_TYPE(media::kMediaRemotingWithoutFullscreen)},
    {"remote-playback-backend", flag_descriptions::kRemotePlaybackBackendName,
     flag_descriptions::kRemotePlaybackBackendDescription, kOsDesktop,
     FEATURE_VALUE_TYPE(blink::features::kRemotePlaybackBackend)},
    {"allow-all-sites-to-initiate-mirroring",
     flag_descriptions::kAllowAllSitesToInitiateMirroringName,
     flag_descriptions::kAllowAllSitesToInitiateMirroringDescription,
     kOsDesktop,
     FEATURE_VALUE_TYPE(media_router::kAllowAllSitesToInitiateMirroring)},
    {"media-route-dial-provider",
     flag_descriptions::kDialMediaRouteProviderName,
     flag_descriptions::kDialMediaRouteProviderDescription, kOsDesktop,
     FEATURE_VALUE_TYPE(media_router::kDialMediaRouteProvider)},

    {"cast-streaming-hardware-h264",
     flag_descriptions::kCastStreamingHardwareH264Name,
     flag_descriptions::kCastStreamingHardwareH264Description, kOsDesktop,
     ENABLE_DISABLE_VALUE_TYPE(
         switches::kCastStreamingForceEnableHardwareH264,
         switches::kCastStreamingForceDisableHardwareH264)},

    {"cast-streaming-hardware-vp8",
     flag_descriptions::kCastStreamingHardwareVp8Name,
     flag_descriptions::kCastStreamingHardwareVp8Description, kOsDesktop,
     ENABLE_DISABLE_VALUE_TYPE(
         switches::kCastStreamingForceEnableHardwareVp8,
         switches::kCastStreamingForceDisableHardwareVp8)},

    {"cast-streaming-performance-overlay",
     flag_descriptions::kCastStreamingPerformanceOverlayName,
     flag_descriptions::kCastStreamingPerformanceOverlayDescription, kOsDesktop,
     FEATURE_VALUE_TYPE(media::kCastStreamingPerformanceOverlay)},

    {"enable-openscreen-cast-streaming-session",
     flag_descriptions::kOpenscreenCastStreamingSessionName,
     flag_descriptions::kOpenscreenCastStreamingSessionDescription, kOsDesktop,
     FEATURE_VALUE_TYPE(media::kOpenscreenCastStreamingSession)},

    {"enable-cast-streaming-av1", flag_descriptions::kCastStreamingAv1Name,
     flag_descriptions::kCastStreamingAv1Description, kOsDesktop,
     FEATURE_VALUE_TYPE(media::kCastStreamingAv1)},

    {"enable-cast-streaming-vp9", flag_descriptions::kCastStreamingVp9Name,
     flag_descriptions::kCastStreamingVp9Description, kOsDesktop,
     FEATURE_VALUE_TYPE(media::kCastStreamingVp9)},

    {"enable-cast-streaming-with-hidpi",
     flag_descriptions::kCastEnableStreamingWithHiDPIName,
     flag_descriptions::kCastEnableStreamingWithHiDPIDescription, kOsDesktop,
     FEATURE_VALUE_TYPE(mirroring::features::kCastEnableStreamingWithHiDPI)},

    {"enable-os-integration-sub-managers",
     flag_descriptions::kEnableOsIntegrationSubManagersName,
     flag_descriptions::kEnableOsIntegrationSubManagersDescription, kOsDesktop,
     FEATURE_WITH_PARAMS_VALUE_TYPE(features::kOsIntegrationSubManagers,
                                    kOsIntegrationSubManagersConfigVariations,
                                    "OsIntegrationSubManagers")},

#endif  // !BUILDFLAG(IS_ANDROID)

    {"password-sharing", flag_descriptions::kEnablePasswordSharingName,
     flag_descriptions::kEnablePasswordSharingDescription, kOsAll,
     MULTI_VALUE_TYPE(kEnablePasswordSharingChoices)},

    {"enable-search-engine-choice",
     flag_descriptions::kEnableSearchEngineChoiceName,
     flag_descriptions::kEnableSearchEngineChoiceDescription,
     kOsMac | kOsWin | kOsLinux | kOsCrOS | kOsAndroid | kOsLacros,
     MULTI_VALUE_TYPE(kEnableSearchEngineChoice)},

#if BUILDFLAG(IS_MAC)
    {"mac-syscall-sandbox", flag_descriptions::kMacSyscallSandboxName,
     flag_descriptions::kMacSyscallSandboxDescription, kOsMac,
     FEATURE_VALUE_TYPE(features::kMacSyscallSandbox)},

    {"mac-loopback-audio-for-cast",
     flag_descriptions::kMacLoopbackAudioForCastName,
     flag_descriptions::kMacLoopbackAudioForCastDescription, kOsMac,
     FEATURE_VALUE_TYPE(media::kMacLoopbackAudioForCast)},

    {"mac-loopback-audio-for-screen-share",
     flag_descriptions::kMacLoopbackAudioForScreenShareName,
     flag_descriptions::kMacLoopbackAudioForScreenShareDescription, kOsMac,
     FEATURE_VALUE_TYPE(media::kMacLoopbackAudioForScreenShare)},
#endif  // BUILDFLAG(IS_MAC)
#if BUILDFLAG(IS_CHROMEOS) || BUILDFLAG(IS_WIN) || BUILDFLAG(IS_MAC)
    {"web-share", flag_descriptions::kWebShareName,
     flag_descriptions::kWebShareDescription, kOsWin | kOsCrOS | kOsMac,
     FEATURE_VALUE_TYPE(features::kWebShare)},
#endif  // BUILDFLAG(IS_CHROMEOS) || BUILDFLAG(IS_WIN) || BUILDFLAG(IS_MAC)

#if BUILDFLAG(IS_LINUX)
    {"ozone-platform-hint", flag_descriptions::kOzonePlatformHintName,
     flag_descriptions::kOzonePlatformHintDescription, kOsLinux,
     MULTI_VALUE_TYPE(kOzonePlatformHintRuntimeChoices)},
#endif  // BUILDFLAG(IS_LINUX)

#if BUILDFLAG(IS_LINUX) || BUILDFLAG(IS_MAC)
    {"skip-undecryptable-passwords",
     flag_descriptions::kSkipUndecryptablePasswordsName,
     flag_descriptions::kSkipUndecryptablePasswordsDescription,
     kOsLinux | kOsMac,
     FEATURE_VALUE_TYPE(
         password_manager::features::kSkipUndecryptablePasswords)},

    {"force-password-initial-sync-when-decryption-fails",
     flag_descriptions::kForcePasswordInitialSyncWhenDecryptionFailsName,
     flag_descriptions::kForcePasswordInitialSyncWhenDecryptionFailsDescription,
     kOsLinux | kOsMac,
     FEATURE_VALUE_TYPE(
         password_manager::features::kForceInitialSyncWhenDecryptionFails)},
#endif  // BUILDFLAG(IS_LINUX) || BUILDFLAG(IS_MAC)

#if BUILDFLAG(ENABLE_VR)
    {"webxr-incubations", flag_descriptions::kWebXrIncubationsName,
     flag_descriptions::kWebXrIncubationsDescription, kOsAll,
     FEATURE_VALUE_TYPE(device::features::kWebXrIncubations)},
    {"webxr-internals", flag_descriptions::kWebXrInternalsName,
     flag_descriptions::kWebXrInternalsDescription, kOsDesktop | kOsAndroid,
     FEATURE_VALUE_TYPE(device::features::kWebXrInternals)},
    {"webxr-runtime", flag_descriptions::kWebXrForceRuntimeName,
     flag_descriptions::kWebXrForceRuntimeDescription, kOsDesktop | kOsAndroid,
     MULTI_VALUE_TYPE(kWebXrForceRuntimeChoices)},
#if BUILDFLAG(IS_ANDROID)
    {"webxr-shared-buffers", flag_descriptions::kWebXrSharedBuffersName,
     flag_descriptions::kWebXrSharedBuffersDescription, kOsAndroid,
     FEATURE_VALUE_TYPE(device::features::kWebXrSharedBuffers)},
#if BUILDFLAG(ENABLE_OPENXR)
    {"enable-openxr-android", flag_descriptions::kOpenXRName,
     flag_descriptions::kOpenXRDescription, kOsAndroid,
     FEATURE_VALUE_TYPE(device::features::kOpenXR)},
#endif
#endif  // BUILDFLAG(IS_ANDROID)
#endif  // ENABLE_VR
#if BUILDFLAG(IS_CHROMEOS_ASH)
    {"disable-accelerated-mjpeg-decode",
     flag_descriptions::kAcceleratedMjpegDecodeName,
     flag_descriptions::kAcceleratedMjpegDecodeDescription, kOsCrOS,
     SINGLE_DISABLE_VALUE_TYPE(switches::kDisableAcceleratedMjpegDecode)},
#endif  // BUILDFLAG(IS_CHROMEOS_ASH)
    {"system-keyboard-lock", flag_descriptions::kSystemKeyboardLockName,
     flag_descriptions::kSystemKeyboardLockDescription, kOsDesktop,
     FEATURE_VALUE_TYPE(features::kSystemKeyboardLock)},
#if BUILDFLAG(IS_ANDROID)
    {"add-to-homescreen-iph", flag_descriptions::kAddToHomescreenIPHName,
     flag_descriptions::kAddToHomescreenIPHDescription, kOsAndroid,
     FEATURE_WITH_PARAMS_VALUE_TYPE(chrome::android::kAddToHomescreenIPH,
                                    kAddToHomescreenIPHVariations,
                                    "AddToHomescreen")},
    {"feature-notification-guide",
     flag_descriptions::kFeatureNotificationGuideName,
     flag_descriptions::kFeatureNotificationGuideDescription, kOsAndroid,
     FEATURE_WITH_PARAMS_VALUE_TYPE(
         feature_guide::features::kFeatureNotificationGuide,
         kFeatureNotificationGuideVariations,
         "FeatureNotificationGuide")},
    {"notification-permission-rationale-dialog",
     flag_descriptions::kNotificationPermissionRationaleName,
     flag_descriptions::kNotificationPermissionRationaleDescription, kOsAndroid,
     FEATURE_WITH_PARAMS_VALUE_TYPE(
         chrome::android::kNotificationPermissionVariant,
         kNotificationPermissionRationaleVariations,
         "NotificationPermissionVariant")},
    {"notification-permission-rationale-bottom-sheet",
     flag_descriptions::kNotificationPermissionRationaleBottomSheetName,
     flag_descriptions::kNotificationPermissionRationaleBottomSheetDescription,
     kOsAndroid,
     FEATURE_VALUE_TYPE(chrome::android::kNotificationPermissionBottomSheet)},
    {"feature-notification-guide-skip-check-for-low-engaged-users",
     flag_descriptions::
         kFeatureNotificationGuideSkipCheckForLowEngagedUsersName,
     flag_descriptions::
         kFeatureNotificationGuideSkipCheckForLowEngagedUsersDescription,
     kOsAll,
     FEATURE_VALUE_TYPE(feature_guide::features::kSkipCheckForLowEngagedUsers)},
    {"offline-pages-live-page-sharing",
     flag_descriptions::kOfflinePagesLivePageSharingName,
     flag_descriptions::kOfflinePagesLivePageSharingDescription, kOsAndroid,
     FEATURE_VALUE_TYPE(offline_pages::kOfflinePagesLivePageSharingFeature)},
    {"query-tiles", flag_descriptions::kQueryTilesName,
     flag_descriptions::kQueryTilesDescription, kOsAndroid,
     FEATURE_WITH_PARAMS_VALUE_TYPE(query_tiles::features::kQueryTiles,
                                    kQueryTilesVariations,
                                    "QueryTilesVariations")},
    {"query-tiles-ntp", flag_descriptions::kQueryTilesNTPName,
     flag_descriptions::kQueryTilesNTPDescription, kOsAndroid,
     FEATURE_VALUE_TYPE(query_tiles::features::kQueryTilesInNTP)},
    {"query-tiles-on-start", flag_descriptions::kQueryTilesOnStartName,
     flag_descriptions::kQueryTilesOnStartDescription, kOsAndroid,
     FEATURE_VALUE_TYPE(query_tiles::features::kQueryTilesOnStart)},
    {"query-tiles-single-tier", flag_descriptions::kQueryTilesSingleTierName,
     flag_descriptions::kQueryTilesSingleTierDescription, kOsAndroid,
     SINGLE_VALUE_TYPE(query_tiles::switches::kQueryTilesSingleTier)},
    {"query-tiles-enable-query-editing",
     flag_descriptions::kQueryTilesEnableQueryEditingName,
     flag_descriptions::kQueryTilesEnableQueryEditingDescription, kOsAndroid,
     FEATURE_VALUE_TYPE(query_tiles::features::kQueryTilesEnableQueryEditing)},
    {"query-tiles-enable-trending",
     flag_descriptions::kQueryTilesEnableTrendingName,
     flag_descriptions::kQueryTilesEnableTrendingDescription, kOsAndroid,
     SINGLE_VALUE_TYPE(query_tiles::switches::kQueryTilesEnableTrending)},
    {"query-tiles-country-code", flag_descriptions::kQueryTilesCountryCode,
     flag_descriptions::kQueryTilesCountryCodeDescription, kOsAndroid,
     MULTI_VALUE_TYPE(kQueryTilesCountryChoices)},
    {"query-tiles-disable-country-override",
     flag_descriptions::kQueryTilesDisableCountryOverrideName,
     flag_descriptions::kQueryTilesDisableCountryOverrideDescription,
     kOsAndroid,
     FEATURE_VALUE_TYPE(
         query_tiles::features::kQueryTilesDisableCountryOverride)},
    {"query-tiles-instant-fetch",
     flag_descriptions::kQueryTilesInstantFetchName,
     flag_descriptions::kQueryTilesInstantFetchDescription, kOsAndroid,
     SINGLE_VALUE_TYPE(
         query_tiles::switches::kQueryTilesInstantBackgroundTask)},
    {"query-tiles-rank-tiles", flag_descriptions::kQueryTilesRankTilesName,
     flag_descriptions::kQueryTilesRankTilesDescription, kOsAndroid,
     SINGLE_VALUE_TYPE(query_tiles::switches::kQueryTilesRankTiles)},
    {"query-tiles-segmentation", flag_descriptions::kQueryTilesSegmentationName,
     flag_descriptions::kQueryTilesSegmentationDescription, kOsAndroid,
     FEATURE_VALUE_TYPE(query_tiles::features::kQueryTilesSegmentation)},
    {"query-tiles-swap-trending",
     flag_descriptions::kQueryTilesSwapTrendingName,
     flag_descriptions::kQueryTilesSwapTrendingDescription, kOsAndroid,
     FEATURE_VALUE_TYPE(
         query_tiles::features::kQueryTilesRemoveTrendingTilesAfterInactivity)},
    {"reengagement-notification",
     flag_descriptions::kReengagementNotificationName,
     flag_descriptions::kReengagementNotificationDescription, kOsAndroid,
     FEATURE_VALUE_TYPE(chrome::android::kReengagementNotification)},
    {"back-gesture-activity-tab-provider",
     flag_descriptions::kBackGestureActivityTabProviderName,
     flag_descriptions::kBackGestureActivityTabProviderDescription, kOsAndroid,
     FEATURE_VALUE_TYPE(chrome::android::kBackGestureRefactorAndroid)},
    {"back-gesture-refactor-android",
     flag_descriptions::kBackGestureRefactorAndroidName,
     flag_descriptions::kBackGestureRefactorAndroidDescription, kOsAndroid,
     FEATURE_VALUE_TYPE(chrome::android::kBackGestureRefactorAndroid)},
    {"back-gesture-refactor-activity-android",
     flag_descriptions::kBackGestureRefactorActivityAndroidName,
     flag_descriptions::kBackGestureRefactorActivityAndroidDescription,
     kOsAndroid,
     FEATURE_VALUE_TYPE(chrome::android::kBackGestureRefactorActivityAndroid)},
    {"draw-cutout-edge-to-edge", flag_descriptions::kDrawCutoutEdgeToEdgeName,
     flag_descriptions::kDrawCutoutEdgeToEdgeDescription, kOsAndroid,
     FEATURE_VALUE_TYPE(features::kDrawCutoutEdgeToEdge)},
    {"draw-edge-to-edge", flag_descriptions::kDrawEdgeToEdgeName,
     flag_descriptions::kDrawEdgeToEdgeDescription, kOsAndroid,
     FEATURE_VALUE_TYPE(chrome::android::kDrawEdgeToEdge)},
    {"draw-native-edge-to-edge", flag_descriptions::kDrawNativeEdgeToEdgeName,
     flag_descriptions::kDrawNativeEdgeToEdgeDescription, kOsAndroid,
     FEATURE_VALUE_TYPE(chrome::android::kDrawNativeEdgeToEdge)},
    {"draw-web-edge-to-edge", flag_descriptions::kDrawWebEdgeToEdgeName,
     flag_descriptions::kDrawWebEdgeToEdgeDescription, kOsAndroid,
     FEATURE_VALUE_TYPE(chrome::android::kDrawWebEdgeToEdge)},
    {"infobar-scroll-optimization",
     flag_descriptions::kInfobarScrollOptimizationName,
     flag_descriptions::kInfobarScrollOptimizationDescription, kOsAndroid,
     FEATURE_VALUE_TYPE(chrome::android::kInfobarScrollOptimization)},
    {"share-sheet-migration-android",
     flag_descriptions::kShareSheetMigrationAndroidName,
     flag_descriptions::kShareSheetMigrationAndroidDescription, kOsAndroid,
     FEATURE_VALUE_TYPE(chrome::android::kShareSheetMigrationAndroid)},
    {"share-sheet-custom-actions-polish",
     flag_descriptions::kShareSheetCustomActionsPolishName,
     flag_descriptions::kShareSheetCustomActionsPolishDescription, kOsAndroid,
     FEATURE_VALUE_TYPE(chrome::android::kShareSheetCustomActionsPolish)},

#endif  // BUILDFLAG(IS_ANDROID)
    {"disallow-doc-written-script-loads",
     flag_descriptions::kDisallowDocWrittenScriptsUiName,
     flag_descriptions::kDisallowDocWrittenScriptsUiDescription, kOsAll,
     // NOTE: if we want to add additional experiment entries for other
     // features controlled by kBlinkSettings, we'll need to add logic to
     // merge the flag values.
     ENABLE_DISABLE_VALUE_TYPE_AND_VALUE(
         blink::switches::kBlinkSettings,
         "disallowFetchForDocWrittenScriptsInMainFrame=true",
         blink::switches::kBlinkSettings,
         "disallowFetchForDocWrittenScriptsInMainFrame=false")},
#if !BUILDFLAG(IS_ANDROID) && !BUILDFLAG(IS_CHROMEOS_ASH)
    {"disallow-managed-profile-signout",
     flag_descriptions::kDisallowManagedProfileSignoutName,
     flag_descriptions::kDisallowManagedProfileSignoutDescription,
     kOsMac | kOsWin | kOsLinux | kOsLacros,
     FEATURE_VALUE_TYPE(kDisallowManagedProfileSignout)},
#endif  // !BUILDFLAG(IS_ANDROID) && !BUILDFLAG(IS_CHROMEOS_ASH)
    {"view-transition-on-navigation",
     flag_descriptions::kViewTransitionOnNavigationName,
     flag_descriptions::kViewTransitionOnNavigationDescription, kOsAll,
     FEATURE_VALUE_TYPE(blink::features::kViewTransitionOnNavigation)},
#if BUILDFLAG(IS_WIN)
    {"use-winrt-midi-api", flag_descriptions::kUseWinrtMidiApiName,
     flag_descriptions::kUseWinrtMidiApiDescription, kOsWin,
     FEATURE_VALUE_TYPE(midi::features::kMidiManagerWinrt)},
    {"webrtc-allow-wgc-screen-capturer",
     flag_descriptions::kWebRtcAllowWgcScreenCapturerName,
     flag_descriptions::kWebRtcAllowWgcScreenCapturerDescription, kOsWin,
     FEATURE_VALUE_TYPE(features::kWebRtcAllowWgcScreenCapturer)},
    {"webrtc-allow-wgc-window-capturer",
     flag_descriptions::kWebRtcAllowWgcWindowCapturerName,
     flag_descriptions::kWebRtcAllowWgcWindowCapturerDescription, kOsWin,
     FEATURE_VALUE_TYPE(features::kWebRtcAllowWgcWindowCapturer)},
#endif  // BUILDFLAG(IS_WIN)
#if defined(TOOLKIT_VIEWS) || BUILDFLAG(IS_ANDROID)
    {"enable-autofill-credit-card-upload",
     flag_descriptions::kAutofillCreditCardUploadName,
     flag_descriptions::kAutofillCreditCardUploadDescription, kOsAll,
     FEATURE_VALUE_TYPE(autofill::features::kAutofillUpstream)},
#endif  // defined(TOOLKIT_VIEWS) || BUILDFLAG(IS_ANDROID)
    {"force-ui-direction", flag_descriptions::kForceUiDirectionName,
     flag_descriptions::kForceUiDirectionDescription, kOsAll,
     MULTI_VALUE_TYPE(kForceUIDirectionChoices)},
    {"force-text-direction", flag_descriptions::kForceTextDirectionName,
     flag_descriptions::kForceTextDirectionDescription, kOsAll,
     MULTI_VALUE_TYPE(kForceTextDirectionChoices)},
#if BUILDFLAG(IS_ANDROID)
    {"force-update-menu-type", flag_descriptions::kUpdateMenuTypeName,
     flag_descriptions::kUpdateMenuTypeDescription, kOsAndroid,
     MULTI_VALUE_TYPE(kForceUpdateMenuTypeChoices)},
    {"update-menu-item-custom-summary",
     flag_descriptions::kUpdateMenuItemCustomSummaryName,
     flag_descriptions::kUpdateMenuItemCustomSummaryDescription, kOsAndroid,
     SINGLE_VALUE_TYPE_AND_VALUE(
         switches::kForceShowUpdateMenuItemCustomSummary,
         "Custom Summary")},
    {"force-show-update-menu-badge", flag_descriptions::kUpdateMenuBadgeName,
     flag_descriptions::kUpdateMenuBadgeDescription, kOsAndroid,
     SINGLE_VALUE_TYPE(switches::kForceShowUpdateMenuBadge)},
    {"set-market-url-for-testing",
     flag_descriptions::kSetMarketUrlForTestingName,
     flag_descriptions::kSetMarketUrlForTestingDescription, kOsAndroid,
     SINGLE_VALUE_TYPE_AND_VALUE(switches::kMarketUrlForTesting,
                                 "https://play.google.com/store/apps/"
                                 "details?id=com.android.chrome")},
    {"omaha-min-sdk-version-android",
     flag_descriptions::kOmahaMinSdkVersionAndroidName,
     flag_descriptions::kOmahaMinSdkVersionAndroidDescription, kOsAndroid,
     FEATURE_WITH_PARAMS_VALUE_TYPE(chrome::android::kOmahaMinSdkVersionAndroid,
                                    kOmahaMinSdkVersionAndroidVariations,
                                    "OmahaMinSdkVersionAndroidStudy")},
#endif  // BUILDFLAG(IS_ANDROID)
    {"enable-tls13-early-data", flag_descriptions::kEnableTLS13EarlyDataName,
     flag_descriptions::kEnableTLS13EarlyDataDescription, kOsAll,
     FEATURE_VALUE_TYPE(net::features::kEnableTLS13EarlyData)},
    {"enable-tls13-kyber", flag_descriptions::kEnableTLS13KyberName,
     flag_descriptions::kEnableTLS13KyberDescription, kOsAll,
     FEATURE_VALUE_TYPE(net::features::kPostQuantumKyber)},
#if BUILDFLAG(IS_ANDROID)
    {"feed-back-to-top", flag_descriptions::kFeedBackToTopName,
     flag_descriptions::kFeedBackToTopDescription, kOsAndroid,
     FEATURE_VALUE_TYPE(feed::kFeedBackToTop)},
    {"feed-loading-placeholder", flag_descriptions::kFeedLoadingPlaceholderName,
     flag_descriptions::kFeedLoadingPlaceholderDescription, kOsAndroid,
     FEATURE_VALUE_TYPE(feed::kFeedLoadingPlaceholder)},
    {"feed-signed-out-view-demotion",
     flag_descriptions::kFeedSignedOutViewDemotionName,
     flag_descriptions::kFeedSignedOutViewDemotionDescription, kOsAndroid,
     FEATURE_VALUE_TYPE(feed::kFeedSignedOutViewDemotion)},
    {"feed-v2-hearts", flag_descriptions::kInterestFeedV2HeartsName,
     flag_descriptions::kInterestFeedV2HeartsDescription, kOsAndroid,
     FEATURE_VALUE_TYPE(feed::kInterestFeedV2Hearts)},
    {"info-card-acknowledgement-tracking",
     flag_descriptions::kInfoCardAcknowledgementTrackingName,
     flag_descriptions::kInfoCardAcknowledgementTrackingDescription, kOsAndroid,
     FEATURE_VALUE_TYPE(feed::kInfoCardAcknowledgementTracking)},
    {"web-feed", flag_descriptions::kWebFeedName,
     flag_descriptions::kWebFeedDescription, kOsAndroid,
     FEATURE_WITH_PARAMS_VALUE_TYPE(feed::kWebFeed,
                                    kWebFeedVariations,
                                    "WebFeed")},
    {"web-feed-awareness", flag_descriptions::kWebFeedAwarenessName,
     flag_descriptions::kWebFeedAwarenessDescription, kOsAndroid,
     FEATURE_WITH_PARAMS_VALUE_TYPE(feed::kWebFeedAwareness,
                                    kWebFeedAwarenessVariations,
                                    "WebFeedAwareness")},
    {"web-feed-onboarding", flag_descriptions::kWebFeedOnboardingName,
     flag_descriptions::kWebFeedOnboardingDescription, kOsAndroid,
     FEATURE_VALUE_TYPE(feed::kWebFeedOnboarding)},
    {"web-feed-sort", flag_descriptions::kWebFeedSortName,
     flag_descriptions::kWebFeedSortDescription, kOsAndroid,
     FEATURE_VALUE_TYPE(feed::kWebFeedSort)},
    {"feed-header-stick-to-top", flag_descriptions::kFeedHeaderStickToTopName,
     flag_descriptions::kFeedHeaderStickToTopDescription, kOsAndroid,
     FEATURE_VALUE_TYPE(feed::kFeedHeaderStickToTop)},
    {"xsurface-metrics-reporting",
     flag_descriptions::kXsurfaceMetricsReportingName,
     flag_descriptions::kXsurfaceMetricsReportingDescription, kOsAndroid,
     FEATURE_VALUE_TYPE(feed::kXsurfaceMetricsReporting)},
    {"feed-close-refresh", flag_descriptions::kFeedCloseRefreshName,
     flag_descriptions::kFeedCloseRefreshDescription, kOsAndroid,
     FEATURE_WITH_PARAMS_VALUE_TYPE(feed::kFeedCloseRefresh,
                                    kFeedCloseRefreshVariations,
                                    "FeedCloseRefresh")},
    {"feed-discofeed-endpoint", flag_descriptions::kFeedDiscoFeedEndpointName,
     flag_descriptions::kFeedDiscoFeedEndpointDescription, kOsAndroid,
     FEATURE_VALUE_TYPE(feed::kDiscoFeedEndpoint)},
    {"feed-dynamic-colors", flag_descriptions::kFeedDynamicColorsName,
     flag_descriptions::kFeedDynamicColorsDescription, kOsAndroid,
     FEATURE_VALUE_TYPE(feed::kFeedDynamicColors)},
    {"feed-follow-ui-update", flag_descriptions::kFeedFollowUiUpdateName,
     flag_descriptions::kFeedFollowUiUpdateDescription, kOsAndroid,
     FEATURE_VALUE_TYPE(feed::kFeedFollowUiUpdate)},
    {"feed-sports-card", flag_descriptions::kFeedSportsCardName,
     flag_descriptions::kFeedSportsCardDescription, kOsAndroid,
     FEATURE_VALUE_TYPE(feed::kFeedSportsCard)},
    {"refresh-feed-on-start", flag_descriptions::kRefreshFeedOnRestartName,
     flag_descriptions::kRefreshFeedOnRestartDescription, kOsAndroid,
     FEATURE_VALUE_TYPE(feed::kRefreshFeedOnRestart)},
#endif  // BUILDFLAG(IS_ANDROID)
#if BUILDFLAG(IS_LINUX) || BUILDFLAG(IS_CHROMEOS) || BUILDFLAG(IS_MAC) || \
    BUILDFLAG(IS_WIN) || BUILDFLAG(IS_FUCHSIA)
    {"following-feed-sidepanel", flag_descriptions::kFollowingFeedSidepanelName,
     flag_descriptions::kFollowingFeedSidepanelDescription, kOsDesktop,
     FEATURE_VALUE_TYPE(feed::kWebUiFeed)},
#endif  // BUILDFLAG(IS_LINUX) || BUILDFLAG(IS_CHROMEOS) || BUILDFLAG(IS_MAC) ||
        // BUILDFLAG(IS_WIN) || BUILDFLAG(IS_FUCHSIA)
    {"enable-force-dark", flag_descriptions::kAutoWebContentsDarkModeName,
     flag_descriptions::kAutoWebContentsDarkModeDescription, kOsAll,
#if BUILDFLAG(IS_CHROMEOS_ASH)
     // TODO(https://crbug.com/1011696): Investigate crash reports and
     // re-enable variations for ChromeOS.
     FEATURE_VALUE_TYPE(blink::features::kForceWebContentsDarkMode)},
#else
     FEATURE_WITH_PARAMS_VALUE_TYPE(blink::features::kForceWebContentsDarkMode,
                                    kForceDarkVariations,
                                    "ForceDarkVariations")},
#endif  // BUILDFLAG(IS_CHROMEOS_ASH)
#if BUILDFLAG(IS_ANDROID)
    {"enable-accessibility-include-long-click-action",
     flag_descriptions::kAccessibilityIncludeLongClickActionName,
     flag_descriptions::kAccessibilityIncludeLongClickActionDescription,
     kOsAndroid,
     FEATURE_VALUE_TYPE(features::kAccessibilityIncludeLongClickAction)},
    {"enable-accessibility-page-zoom",
     flag_descriptions::kAccessibilityPageZoomName,
     flag_descriptions::kAccessibilityPageZoomDescription, kOsAndroid,
     FEATURE_VALUE_TYPE(features::kAccessibilityPageZoom)},
    {"enable-accessibility-performance-filtering",
     flag_descriptions::kAccessibilityPerformanceFilteringName,
     flag_descriptions::kAccessibilityPerformanceFilteringDescription,
     kOsAndroid,
     FEATURE_VALUE_TYPE(features::kAccessibilityPerformanceFiltering)},
    {"enable-accessibility-snapshot-stress-tests",
     flag_descriptions::kAccessibilitySnapshotStressTestsName,
     flag_descriptions::kAccessibilitySnapshotStressTestsDescription,
     kOsAndroid,
     FEATURE_VALUE_TYPE(features::kAccessibilitySnapshotStressTests)},
    {"enable-smart-zoom", flag_descriptions::kSmartZoomName,
     flag_descriptions::kSmartZoomDescription, kOsAndroid,
     FEATURE_VALUE_TYPE(features::kSmartZoom)},
#endif  // BUILDFLAG(IS_ANDROID)
    {"enable-experimental-accessibility-language-detection",
     flag_descriptions::kExperimentalAccessibilityLanguageDetectionName,
     flag_descriptions::kExperimentalAccessibilityLanguageDetectionDescription,
     kOsAll,
     SINGLE_VALUE_TYPE(
         ::switches::kEnableExperimentalAccessibilityLanguageDetection)},
    {"enable-experimental-accessibility-language-detection-dynamic",
     flag_descriptions::kExperimentalAccessibilityLanguageDetectionDynamicName,
     flag_descriptions::
         kExperimentalAccessibilityLanguageDetectionDynamicDescription,
     kOsAll,
     SINGLE_VALUE_TYPE(
         ::switches::kEnableExperimentalAccessibilityLanguageDetectionDynamic)},
    {"enable-aria-element-reflection",
     flag_descriptions::kAriaElementReflectionName,
     flag_descriptions::kAriaElementReflectionDescription, kOsAll,
     FEATURE_VALUE_TYPE(features::kEnableAriaElementReflection)},
#if BUILDFLAG(IS_CHROMEOS_ASH)
    {"enable-cros-autocorrect-params-tuning",
     flag_descriptions::kAutocorrectParamsTuningName,
     flag_descriptions::kAutocorrectParamsTuningDescription, kOsCrOS,
     FEATURE_VALUE_TYPE(ash::features::kAutocorrectParamsTuning)},
    {"enable-cros-autocorrect-toggle",
     flag_descriptions::kAutocorrectToggleName,
     flag_descriptions::kAutocorrectToggleDescription, kOsCrOS,
     FEATURE_VALUE_TYPE(ash::features::kAutocorrectToggle)},
    {"enable-cros-autocorrect-by-default",
     flag_descriptions::kAutocorrectByDefaultName,
     flag_descriptions::kAutocorrectByDefaultDescription, kOsCrOS,
     FEATURE_VALUE_TYPE(ash::features::kAutocorrectByDefault)},
    {"enable-cros-autocorrect-use-replace-surrounding-text",
     flag_descriptions::kAutocorrectUseReplaceSurroundingTextName,
     flag_descriptions::kAutocorrectUseReplaceSurroundingTextDescription,
     kOsCrOS,
     FEATURE_VALUE_TYPE(ash::features::kAutocorrectUseReplaceSurroundingText)},
    {"enable-cros-avatars-cloud-migration",
     flag_descriptions::kAvatarsCloudMigrationName,
     flag_descriptions::kAvatarsCloudMigrationDescription, kOsCrOS,
     FEATURE_VALUE_TYPE(ash::features::kAvatarsCloudMigration)},
    {"enable-cros-diacritics-on-physical-keyboard-longpress",
     flag_descriptions::kDiacriticsOnPhysicalKeyboardLongpressName,
     flag_descriptions::kDiacriticsOnPhysicalKeyboardLongpressDescription,
     kOsCrOS,
     FEATURE_VALUE_TYPE(ash::features::kDiacriticsOnPhysicalKeyboardLongpress)},
    {"enable-cros-diacritics-use-replace-surrounding-text",
     flag_descriptions::kDiacriticsUseReplaceSurroundingTextName,
     flag_descriptions::kDiacriticsUseReplaceSurroundingTextDescription,
     kOsCrOS,
     FEATURE_VALUE_TYPE(ash::features::kDiacriticsUseReplaceSurroundingText)},
    {"enable-cros-first-party-vietnamese-input",
     flag_descriptions::kFirstPartyVietnameseInputName,
     flag_descriptions::kFirstPartyVietnameseInputDescription, kOsCrOS,
     FEATURE_VALUE_TYPE(ash::features::kFirstPartyVietnameseInput)},
    {"enable-cros-hindi-inscript-layout",
     flag_descriptions::kHindiInscriptLayoutName,
     flag_descriptions::kHindiInscriptLayoutDescription, kOsCrOS,
     FEATURE_VALUE_TYPE(ash::features::kHindiInscriptLayout)},
    {"enable-cros-ime-assist-emoji-enhanced",
     flag_descriptions::kImeAssistEmojiEnhancedName,
     flag_descriptions::kImeAssistEmojiEnhancedDescription, kOsCrOS,
     FEATURE_VALUE_TYPE(ash::features::kAssistEmojiEnhanced)},
    {"enable-cros-ime-assist-multi-word",
     flag_descriptions::kImeAssistMultiWordName,
     flag_descriptions::kImeAssistMultiWordDescription, kOsCrOS,
     FEATURE_VALUE_TYPE(ash::features::kAssistMultiWord)},
    {"enable-cros-ime-assist-multi-word-expanded",
     flag_descriptions::kImeAssistMultiWordExpandedName,
     flag_descriptions::kImeAssistMultiWordExpandedDescription, kOsCrOS,
     FEATURE_VALUE_TYPE(ash::features::kAssistMultiWordExpanded)},
    {"enable-cros-ime-fst-decoder-params-update",
     flag_descriptions::kImeFstDecoderParamsUpdateName,
     flag_descriptions::kImeFstDecoderParamsUpdateDescription, kOsCrOS,
     FEATURE_VALUE_TYPE(ash::features::kImeFstDecoderParamsUpdate)},
    {"enable-cros-virtual-keyboard-new-header",
     flag_descriptions::kVirtualKeyboardNewHeaderName,
     flag_descriptions::kVirtualKeyboardNewHeaderDescription, kOsCrOS,
     FEATURE_VALUE_TYPE(ash::features::kVirtualKeyboardNewHeader)},
    {"enable-cros-ime-system-emoji-picker-clipboard",
     flag_descriptions::kImeSystemEmojiPickerClipboardName,
     flag_descriptions::kImeSystemEmojiPickerClipboardDescription, kOsCrOS,
     FEATURE_VALUE_TYPE(ash::features::kImeSystemEmojiPickerClipboard)},
    {"enable-cros-ime-system-emoji-picker-extension",
     flag_descriptions::kImeSystemEmojiPickerExtensionName,
     flag_descriptions::kImeSystemEmojiPickerExtensionDescription, kOsCrOS,
     FEATURE_VALUE_TYPE(ash::features::kImeSystemEmojiPickerExtension)},
    {"enable-cros-ime-system-emoji-picker-gif-support",
     flag_descriptions::kImeSystemEmojiPickerGIFSupportName,
     flag_descriptions::kImeSystemEmojiPickerGIFSupportDescription, kOsCrOS,
     FEATURE_VALUE_TYPE(ash::features::kImeSystemEmojiPickerGIFSupport)},
    {"enable-cros-ime-system-emoji-picker-jelly-support",
     flag_descriptions::kImeSystemEmojiPickerJellySupportName,
     flag_descriptions::kImeSystemEmojiPickerJellySupportDescription, kOsCrOS,
     FEATURE_VALUE_TYPE(ash::features::kImeSystemEmojiPickerJellySupport)},
    {"enable-cros-ime-system-emoji-picker-search-extension",
     flag_descriptions::kImeSystemEmojiPickerSearchExtensionName,
     flag_descriptions::kImeSystemEmojiPickerSearchExtensionDescription,
     kOsCrOS,
     FEATURE_VALUE_TYPE(ash::features::kImeSystemEmojiPickerSearchExtension)},
    {"enable-cros-ime-system-emoji-picker-variant-grouping",
     flag_descriptions::kImeSystemEmojiPickerVariantGroupingName,
     flag_descriptions::kImeSystemEmojiPickerVariantGroupingDescription,
     kOsCrOS,
     FEATURE_VALUE_TYPE(ash::features::kImeSystemEmojiPickerVariantGrouping)},
    {"enable-cros-ime-stylus-handwriting",
     flag_descriptions::kImeStylusHandwritingName,
     flag_descriptions::kImeStylusHandwritingDescription, kOsCrOS,
     FEATURE_VALUE_TYPE(ash::features::kImeStylusHandwriting)},
    {"enable-cros-ime-tray-hide-voice-button",
     flag_descriptions::kImeTrayHideVoiceButtonName,
     flag_descriptions::kImeTrayHideVoiceButtonDescription, kOsCrOS,
     FEATURE_VALUE_TYPE(ash::features::kImeTrayHideVoiceButton)},
    {"enable-cros-ime-us-english-model-update",
     flag_descriptions::kImeUsEnglishModelUpdateName,
     flag_descriptions::kImeUsEnglishModelUpdateDescription, kOsCrOS,
     FEATURE_VALUE_TYPE(ash::features::kImeUsEnglishModelUpdate)},
    {"enable-cros-ime-korean-mode-switch-debug",
     flag_descriptions::kImeKoreanModeSwitchDebugName,
     flag_descriptions::kImeKoreanModeSwitchDebugDescription, kOsCrOS,
     FEATURE_VALUE_TYPE(ash::features::kImeKoreanModeSwitchDebug)},
    {"enable-cros-ime-korean-only-mode-switch-on-right-alt",
     flag_descriptions::kImeKoreanOnlyModeSwitchOnRightAltName,
     flag_descriptions::kImeKoreanOnlyModeSwitchOnRightAltDescription, kOsCrOS,
     FEATURE_VALUE_TYPE(ash::features::kImeKoreanOnlyModeSwitchOnRightAlt)},
    {"enable-cros-japanese-os-settings",
     flag_descriptions::kJapaneseOSSettingsName,
     flag_descriptions::kJapaneseOSSettingsDescription, kOsCrOS,
     FEATURE_VALUE_TYPE(ash::features::kJapaneseOSSettings)},
    {"enable-cros-multilingual-typing",
     flag_descriptions::kMultilingualTypingName,
     flag_descriptions::kMultilingualTypingDescription, kOsCrOS,
     FEATURE_VALUE_TYPE(ash::features::kMultilingualTyping)},
    {"enable-cros-on-device-grammar-check",
     flag_descriptions::kCrosOnDeviceGrammarCheckName,
     flag_descriptions::kCrosOnDeviceGrammarCheckDescription, kOsCrOS,
     FEATURE_VALUE_TYPE(ash::features::kOnDeviceGrammarCheck)},
    {"enable-cros-system-japanese-physical-typing",
     flag_descriptions::kSystemJapanesePhysicalTypingName,
     flag_descriptions::kSystemJapanesePhysicalTypingDescription, kOsCrOS,
     FEATURE_VALUE_TYPE(ash::features::kSystemJapanesePhysicalTyping)},
    {"enable-cros-virtual-keyboard-round-corners",
     flag_descriptions::kVirtualKeyboardRoundCornersName,
     flag_descriptions::kVirtualKeyboardRoundCornersDescription, kOsCrOS,
     FEATURE_VALUE_TYPE(ash::features::kVirtualKeyboardRoundCorners)},
    {"enable-cros-vm-memory-management-service",
     flag_descriptions::kVmMemoryManagementServiceName,
     flag_descriptions::kVmMemoryManagementServiceDescription, kOsCrOS,
     PLATFORM_FEATURE_NAME_TYPE("CrOSLateBootVmMemoryManagementService")},
    {"enable-experimental-accessibility-dictation-context-checking",
     flag_descriptions::kExperimentalAccessibilityDictationContextCheckingName,
     flag_descriptions::
         kExperimentalAccessibilityDictationContextCheckingDescription,
     kOsCrOS,
     FEATURE_VALUE_TYPE(
         features::kExperimentalAccessibilityDictationContextChecking)},
    {"enable-experimental-accessibility-google-tts-language-packs",
     flag_descriptions::kExperimentalAccessibilityGoogleTtsLanguagePacksName,
     flag_descriptions::
         kExperimentalAccessibilityGoogleTtsLanguagePacksDescription,
     kOsCrOS,
     FEATURE_VALUE_TYPE(
         features::kExperimentalAccessibilityGoogleTtsLanguagePacks)},
    {"enable-experimental-accessibility-google-tts-high-quality-voices",
     flag_descriptions::
         kExperimentalAccessibilityGoogleTtsHighQualityVoicesName,
     flag_descriptions::
         kExperimentalAccessibilityGoogleTtsHighQualityVoicesDescription,
     kOsCrOS,
     FEATURE_VALUE_TYPE(
         features::kExperimentalAccessibilityGoogleTtsHighQualityVoices)},
    {"enable-experimental-accessibility-manifest-v3",
     flag_descriptions::kExperimentalAccessibilityManifestV3Name,
     flag_descriptions::kExperimentalAccessibilityManifestV3Description,
     kOsCrOS,
     SINGLE_VALUE_TYPE(::switches::kEnableExperimentalAccessibilityManifestV3)},
    {"enable-experimental-accessibility-switch-access-text",
     flag_descriptions::kExperimentalAccessibilitySwitchAccessTextName,
     flag_descriptions::kExperimentalAccessibilitySwitchAccessTextDescription,
     kOsCrOS,
     SINGLE_VALUE_TYPE(
         ::switches::kEnableExperimentalAccessibilitySwitchAccessText)},
    {"expose-out-of-process-video-decoding-to-lacros",
     flag_descriptions::kExposeOutOfProcessVideoDecodingToLacrosName,
     flag_descriptions::kExposeOutOfProcessVideoDecodingToLacrosDescription,
     kOsCrOS,
     FEATURE_VALUE_TYPE(media::kExposeOutOfProcessVideoDecodingToLacros)},
    {"enable-system-proxy-for-system-services",
     flag_descriptions::kSystemProxyForSystemServicesName,
     flag_descriptions::kSystemProxyForSystemServicesDescription, kOsCrOS,
     FEATURE_VALUE_TYPE(ash::features::kSystemProxyForSystemServices)},
    {"enable-federated-service", flag_descriptions::kFederatedServiceName,
     flag_descriptions::kFederatedServiceDescription, kOsCrOS,
     FEATURE_VALUE_TYPE(ash::features::kFederatedService)},
    {"screencast-v2", flag_descriptions::kScreencastV2Name,
     flag_descriptions::kScreencastV2Description, kOsCrOS,
     FEATURE_VALUE_TYPE(ash::features::kProjectorV2)},
#endif  // BUILDFLAG(IS_CHROMEOS_ASH)
    {"enable-cros-touch-text-editing-redesign",
     flag_descriptions::kTouchTextEditingRedesignName,
     flag_descriptions::kTouchTextEditingRedesignDescription, kOsAll,
     FEATURE_VALUE_TYPE(features::kTouchTextEditingRedesign)},
#if BUILDFLAG(IS_CHROMEOS)
    {"quickoffice-force-file-download",
     flag_descriptions::kQuickOfficeForceFileDownloadName,
     flag_descriptions::kQuickOfficeForceFileDownloadDescription,
     kOsCrOS | kOsLacros,
     FEATURE_VALUE_TYPE(features::kQuickOfficeForceFileDownload)},
#endif  // BUILDFLAG(IS_CHROMEOS)
#if BUILDFLAG(IS_MAC)
    {"enable-retry-capture-device-enumeration-on-crash",
     flag_descriptions::kRetryGetVideoCaptureDeviceInfosName,
     flag_descriptions::kRetryGetVideoCaptureDeviceInfosDescription, kOsMac,
     FEATURE_VALUE_TYPE(features::kRetryGetVideoCaptureDeviceInfos)},
#endif  // BUILDFLAG(IS_MAC)
#if BUILDFLAG(IS_MAC)
    {"enable-immersive-fullscreen-toolbar",
     flag_descriptions::kImmersiveFullscreenName,
     flag_descriptions::kImmersiveFullscreenDescription, kOsMac,
     FEATURE_VALUE_TYPE(features::kImmersiveFullscreen)},
#endif  // BUILDFLAG(IS_MAC)
    {"enable-web-payments-experimental-features",
     flag_descriptions::kWebPaymentsExperimentalFeaturesName,
     flag_descriptions::kWebPaymentsExperimentalFeaturesDescription, kOsAll,
     FEATURE_VALUE_TYPE(payments::features::kWebPaymentsExperimentalFeatures)},
    {"add-identity-in-can-make-payment",
     flag_descriptions::kAddIdentityInCanMakePaymentEventName,
     flag_descriptions::kAddIdentityInCanMakePaymentEventDescription, kOsAll,
     FEATURE_VALUE_TYPE(blink::features::kAddIdentityInCanMakePaymentEvent)},
    {"enable-debug-for-store-billing",
     flag_descriptions::kAppStoreBillingDebugName,
     flag_descriptions::kAppStoreBillingDebugDescription, kOsAll,
     FEATURE_VALUE_TYPE(payments::features::kAppStoreBillingDebug)},
    {"enable-debug-for-secure-payment-confirmation",
     flag_descriptions::kSecurePaymentConfirmationDebugName,
     flag_descriptions::kSecurePaymentConfirmationDebugDescription, kOsAll,
     FEATURE_VALUE_TYPE(features::kSecurePaymentConfirmationDebug)},
    {"mutation-events", flag_descriptions::kMutationEventsName,
     flag_descriptions::kMutationEventsDescription, kOsAll,
     FEATURE_VALUE_TYPE(blink::features::kMutationEvents)},
    {"fill-on-account-select", flag_descriptions::kFillOnAccountSelectName,
     flag_descriptions::kFillOnAccountSelectDescription, kOsAll,
     FEATURE_VALUE_TYPE(password_manager::features::kFillOnAccountSelect)},
#if BUILDFLAG(IS_CHROMEOS_ASH)
    {"arc-aaudio-mmap-low-latency",
     flag_descriptions::kArcAAudioMMAPLowLatencyName,
     flag_descriptions::kArcAAudioMMAPLowLatencyDescription, kOsCrOS,
     PLATFORM_FEATURE_NAME_TYPE("CrOSLateBootArcVmAAudioMMAPLowLatency")},
    {"arc-custom-tabs-experiment",
     flag_descriptions::kArcCustomTabsExperimentName,
     flag_descriptions::kArcCustomTabsExperimentDescription, kOsCrOS,
     FEATURE_VALUE_TYPE(arc::kCustomTabsExperimentFeature)},
    {"arc-documents-provider-unknown-size",
     flag_descriptions::kArcDocumentsProviderUnknownSizeName,
     flag_descriptions::kArcDocumentsProviderUnknownSizeDescription, kOsCrOS,
     FEATURE_VALUE_TYPE(arc::kDocumentsProviderUnknownSizeFeature)},
    {"arc-enable-aaudio-mmap", flag_descriptions::kArcEnableAAudioMMAPName,
     flag_descriptions::kArcEnableAAudioMMAPDescription, kOsCrOS,
     PLATFORM_FEATURE_NAME_TYPE("CrOSLateBootArcVmAAudioMMAP")},
    {kArcEnableVirtioBlkForDataInternalName,
     flag_descriptions::kArcEnableVirtioBlkForDataName,
     flag_descriptions::kArcEnableVirtioBlkForDataDesc, kOsCrOS,
     FEATURE_VALUE_TYPE(arc::kEnableVirtioBlkForData)},
    {"arc-external-storage-access",
     flag_descriptions::kArcExternalStorageAccessName,
     flag_descriptions::kArcExternalStorageAccessDescription, kOsCrOS,
     FEATURE_VALUE_TYPE(arc::kExternalStorageAccess)},
    {"arc-file-picker-experiment",
     flag_descriptions::kArcFilePickerExperimentName,
     flag_descriptions::kArcFilePickerExperimentDescription, kOsCrOS,
     FEATURE_VALUE_TYPE(arc::kFilePickerExperimentFeature)},
    {"arc-instant-response-window-open",
     flag_descriptions::kArcInstantResponseWindowOpenName,
     flag_descriptions::kArcInstantResponseWindowOpenDescription, kOsCrOS,
     FEATURE_VALUE_TYPE(arc::kInstantResponseWindowOpen)},
    {"arc-keyboard-shortcut-helper-integration",
     flag_descriptions::kArcKeyboardShortcutHelperIntegrationName,
     flag_descriptions::kArcKeyboardShortcutHelperIntegrationDescription,
     kOsCrOS,
     FEATURE_VALUE_TYPE(arc::kKeyboardShortcutHelperIntegrationFeature)},
    {"arc-native-bridge-toggle", flag_descriptions::kArcNativeBridgeToggleName,
     flag_descriptions::kArcNativeBridgeToggleDescription, kOsCrOS,
     FEATURE_VALUE_TYPE(arc::kNativeBridgeToggleFeature)},
    {"arc-per-app-language", flag_descriptions::kArcPerAppLanguageName,
     flag_descriptions::kArcPerAppLanguageDescription, kOsCrOS,
     FEATURE_VALUE_TYPE(arc::kPerAppLanguage)},
    {"arc-resize-compat", flag_descriptions::kArcResizeCompatName,
     flag_descriptions::kArcResizeCompatDescription, kOsCrOS,
     FEATURE_VALUE_TYPE(arc::kResizeCompat)},
    {"arc-rounded-window-compat",
     flag_descriptions::kArcRoundedWindowCompatName,
     flag_descriptions::kArcRoundedWindowCompatDescription, kOsCrOS,
     FEATURE_WITH_PARAMS_VALUE_TYPE(arc::kRoundedWindowCompat,
                                    kArcRoundedWindowCompatVariation,
                                    "ArcRoundedWindowCompat")},
    {"arc-rt-vcpu-dual-core", flag_descriptions::kArcRtVcpuDualCoreName,
     flag_descriptions::kArcRtVcpuDualCoreDesc, kOsCrOS,
     FEATURE_VALUE_TYPE(arc::kRtVcpuDualCore)},
    {"arc-rt-vcpu-quad-core", flag_descriptions::kArcRtVcpuQuadCoreName,
     flag_descriptions::kArcRtVcpuQuadCoreDesc, kOsCrOS,
     FEATURE_VALUE_TYPE(arc::kRtVcpuQuadCore)},
    {"arc-touchscreen-emulation",
     flag_descriptions::kArcTouchscreenEmulationName,
     flag_descriptions::kArcTouchscreenEmulationDesc, kOsCrOS,
     FEATURE_VALUE_TYPE(arc::kTouchscreenEmulation)},
    {"arc-switch-to-keymint-daemon",
     flag_descriptions::kArcSwitchToKeyMintDaemonName,
     flag_descriptions::kArcSwitchToKeyMintDaemonDesc, kOsCrOS,
     PLATFORM_FEATURE_NAME_TYPE("CrOSLateBootArcSwitchToKeyMintDaemon")},
    {"arc-switch-to-keymint-on-t",
     flag_descriptions::kArcSwitchToKeyMintOnTName,
     flag_descriptions::kArcSwitchToKeyMintOnTDesc, kOsCrOS,
     FEATURE_VALUE_TYPE(arc::kSwitchToKeyMintOnT)},
    {"arc-switch-to-keymint-on-t-override",
     flag_descriptions::kArcSwitchToKeyMintOnTOverrideName,
     flag_descriptions::kArcSwitchToKeyMintOnTOverrideDesc, kOsCrOS,
     FEATURE_VALUE_TYPE(arc::kSwitchToKeyMintOnTOverride)},
    {"arc-sync-install-priority",
     flag_descriptions::kArcSyncInstallPriorityName,
     flag_descriptions::kArcSyncInstallPriorityDescription, kOsCrOS,
     FEATURE_VALUE_TYPE(arc::kSyncInstallPriority)},
    {"arc-vmm-swap-keyboard-shortcut",
     flag_descriptions::kArcVmmSwapKBShortcutName,
     flag_descriptions::kArcVmmSwapKBShortcutDesc, kOsCrOS,
     FEATURE_VALUE_TYPE(arc::kVmmSwapKeyboardShortcut)},
    {"arc-xdg-mode", flag_descriptions::kArcXdgModeName,
     flag_descriptions::kArcXdgModeDesc, kOsCrOS,
     FEATURE_VALUE_TYPE(arc::kXdgMode)},
#endif  // BUILDFLAG(IS_CHROMEOS_ASH)
    {"enable-generic-sensor-extra-classes",
     flag_descriptions::kEnableGenericSensorExtraClassesName,
     flag_descriptions::kEnableGenericSensorExtraClassesDescription, kOsAll,
     FEATURE_VALUE_TYPE(features::kGenericSensorExtraClasses)},

#if BUILDFLAG(IS_CHROMEOS_ASH)
    {ui_devtools::switches::kEnableUiDevTools,
     flag_descriptions::kUiDevToolsName,
     flag_descriptions::kUiDevToolsDescription, kOsCrOS,
     SINGLE_VALUE_TYPE(ui_devtools::switches::kEnableUiDevTools)},
#endif  // BUILDFLAG(IS_CHROMEOS_ASH)

    {"enable-autofill-virtual-cards-on-touch-to-fill",
     flag_descriptions::kAutofillVirtualCardsOnTouchToFillAndroidName,
     flag_descriptions::kAutofillVirtualCardsOnTouchToFillAndroidDescription,
     kOsAll,
     FEATURE_VALUE_TYPE(
         autofill::features::kAutofillVirtualCardsOnTouchToFillAndroid)},

#if BUILDFLAG(IS_ANDROID)
    {"enable-autofill-touch-to-fill-for-credit-cards",
     flag_descriptions::kAutofillTouchToFillForCreditCardsAndroidName,
     flag_descriptions::kAutofillTouchToFillForCreditCardsAndroidDescription,
     kOsAndroid,
     FEATURE_VALUE_TYPE(
         autofill::features::kAutofillTouchToFillForCreditCardsAndroid)},
    {"enable-autofill-virtual-view-structure",
     flag_descriptions::kAutofillVirtualViewStructureAndroidName,
     flag_descriptions::kAutofillVirtualViewStructureAndroidDescription,
     kOsAndroid,
     FEATURE_VALUE_TYPE(
         autofill::features::kAutofillVirtualViewStructureAndroid)},
#endif

#if BUILDFLAG(IS_CHROMEOS_ASH)
    {"enable-touchscreen-calibration",
     flag_descriptions::kTouchscreenCalibrationName,
     flag_descriptions::kTouchscreenCalibrationDescription, kOsCrOS,
     SINGLE_VALUE_TYPE(ash::switches::kEnableTouchCalibrationSetting)},
#endif  // BUILDFLAG(IS_CHROMEOS_ASH)
#if BUILDFLAG(IS_CHROMEOS_ASH)
    {"prefer-constant-frame-rate",
     flag_descriptions::kPreferConstantFrameRateName,
     flag_descriptions::kPreferConstantFrameRateDescription, kOsCrOS,
     FEATURE_VALUE_TYPE(ash::features::kPreferConstantFrameRate)},
    {"force-control-face-ae", flag_descriptions::kForceControlFaceAeName,
     flag_descriptions::kForceControlFaceAeDescription, kOsCrOS,
     MULTI_VALUE_TYPE(kForceControlFaceAeChoices)},
    {"auto-framing-override", flag_descriptions::kAutoFramingOverrideName,
     flag_descriptions::kAutoFramingOverrideDescription, kOsCrOS,
     MULTI_VALUE_TYPE(kAutoFramingOverrideChoices)},
    {"camera-app-autoqr-detection",
     flag_descriptions::kCameraAppAutoQRDetectionName,
     flag_descriptions::kCameraAppAutoQRDetectionDescription, kOsCrOS,
     FEATURE_VALUE_TYPE(ash::features::kCameraAppAutoQRDetection)},
    {"crostini-gpu-support", flag_descriptions::kCrostiniGpuSupportName,
     flag_descriptions::kCrostiniGpuSupportDescription, kOsCrOS,
     FEATURE_VALUE_TYPE(ash::features::kCrostiniGpuSupport)},
    {"disable-camera-frame-rotation-at-source",
     flag_descriptions::kDisableCameraFrameRotationAtSourceName,
     flag_descriptions::kDisableCameraFrameRotationAtSourceDescription, kOsCrOS,
     FEATURE_VALUE_TYPE(media::features::kDisableCameraFrameRotationAtSource)},
    {"file-transfer-enterprise-connector",
     flag_descriptions::kFileTransferEnterpriseConnectorName,
     flag_descriptions::kFileTransferEnterpriseConnectorDescription, kOsCrOS,
     FEATURE_VALUE_TYPE(features::kFileTransferEnterpriseConnector)},
    {"file-transfer-enterprise-connector-ui",
     flag_descriptions::kFileTransferEnterpriseConnectorUIName,
     flag_descriptions::kFileTransferEnterpriseConnectorUIDescription, kOsCrOS,
     FEATURE_VALUE_TYPE(features::kFileTransferEnterpriseConnectorUI)},
    {"files-app-experimental", flag_descriptions::kFilesAppExperimentalName,
     flag_descriptions::kFilesAppExperimentalDescription, kOsCrOS,
     FEATURE_VALUE_TYPE(ash::features::kFilesAppExperimental)},
    {"files-conflict-dialog", flag_descriptions::kFilesConflictDialogName,
     flag_descriptions::kFilesConflictDialogDescription, kOsCrOS,
     FEATURE_VALUE_TYPE(ash::features::kFilesConflictDialog)},
    {"files-google-drive-settings-page",
     flag_descriptions::kFilesGoogleDriveSettingsPageName,
     flag_descriptions::kFilesGoogleDriveSettingsPageDescription, kOsCrOS,
     FEATURE_VALUE_TYPE(ash::features::kFilesGoogleDriveSettingsPage)},
    {"files-local-image-search", flag_descriptions::kFilesLocalImageSearchName,
     flag_descriptions::kFilesLocalImageSearchDescription, kOsCrOS,
     FEATURE_VALUE_TYPE(ash::features::kFilesLocalImageSearch)},
    {"files-new-directory-tree", flag_descriptions::kFilesNewDirectoryTreeName,
     flag_descriptions::kFilesNewDirectoryTreeDescription, kOsCrOS,
     FEATURE_VALUE_TYPE(ash::features::kFilesNewDirectoryTree)},
    {"files-single-partition-format",
     flag_descriptions::kFilesSinglePartitionFormatName,
     flag_descriptions::kFilesSinglePartitionFormatDescription, kOsCrOS,
     FEATURE_VALUE_TYPE(ash::features::kFilesSinglePartitionFormat)},
    {"files-trash-drive", flag_descriptions::kFilesTrashDriveName,
     flag_descriptions::kFilesTrashDriveDescription, kOsCrOS,
     FEATURE_VALUE_TYPE(ash::features::kFilesTrashDrive)},
    {"force-resync-drive", flag_descriptions::kForceReSyncDriveName,
     flag_descriptions::kForceReSyncDriveDescription, kOsCrOS,
     FEATURE_VALUE_TYPE(ash::features::kForceReSyncDrive)},
    {"force-spectre-v2-mitigation",
     flag_descriptions::kForceSpectreVariant2MitigationName,
     flag_descriptions::kForceSpectreVariant2MitigationDescription, kOsCrOS,
     FEATURE_VALUE_TYPE(
         sandbox::policy::features::kForceSpectreVariant2Mitigation)},
    {"fsps-in-recents", flag_descriptions::kFSPsInRecentsName,
     flag_descriptions::kFSPsInRecentsDescription, kOsCrOS,
     FEATURE_VALUE_TYPE(ash::features::kFSPsInRecents)},
    {"fuse-box-debug", flag_descriptions::kFuseBoxDebugName,
     flag_descriptions::kFuseBoxDebugDescription, kOsCrOS,
     FEATURE_VALUE_TYPE(ash::features::kFuseBoxDebug)},
    {"spectre-v2-mitigation", flag_descriptions::kSpectreVariant2MitigationName,
     flag_descriptions::kSpectreVariant2MitigationDescription, kOsCrOS,
     FEATURE_VALUE_TYPE(sandbox::policy::features::kSpectreVariant2Mitigation)},
    {"upload-office-to-cloud", flag_descriptions::kUploadOfficeToCloudName,
     flag_descriptions::kUploadOfficeToCloudName, kOsCrOS,
     FEATURE_VALUE_TYPE(chromeos::features::kUploadOfficeToCloud)},
    {"eap-gtc-wifi-authentication",
     flag_descriptions::kEapGtcWifiAuthenticationName,
     flag_descriptions::kEapGtcWifiAuthenticationDescription, kOsCrOS,
     FEATURE_VALUE_TYPE(ash::features::kEapGtcWifiAuthentication)},
    {"eche-swa", flag_descriptions::kEcheSWAName,
     flag_descriptions::kEcheSWADescription, kOsCrOS,
     FEATURE_VALUE_TYPE(ash::features::kEcheSWA)},
    {"eche-launcher", flag_descriptions::kEcheLauncherName,
     flag_descriptions::kEcheLauncherDescription, kOsCrOS,
     FEATURE_VALUE_TYPE(ash::features::kEcheLauncher)},
    {"eche-launcher-app-icon-in-more-apps-button",
     flag_descriptions::kEcheLauncherIconsInMoreAppsButtonName,
     flag_descriptions::kEcheLauncherIconsInMoreAppsButtonDescription, kOsCrOS,
     FEATURE_VALUE_TYPE(ash::features::kEcheLauncherIconsInMoreAppsButton)},
    {"eche-launcher-list-view", flag_descriptions::kEcheLauncherListViewName,
     flag_descriptions::kEcheLauncherListViewDescription, kOsCrOS,
     FEATURE_VALUE_TYPE(ash::features::kEcheLauncherListView)},
    {"eche-network-connection-state",
     flag_descriptions::kEcheNetworkConnectionStateName,
     flag_descriptions::kEcheNetworkConnectionStateDescription, kOsCrOS,
     FEATURE_VALUE_TYPE(ash::features::kEcheNetworkConnectionState)},
    {"eche-swa-check-android-network-info",
     flag_descriptions::kEcheSWACheckAndroidNetworkInfoName,
     flag_descriptions::kEcheSWACheckAndroidNetworkInfoDescription, kOsCrOS,
     FEATURE_VALUE_TYPE(ash::features::kEcheSWACheckAndroidNetworkInfo)},
    {"eche-swa-process-android-accessibility-tree",
     flag_descriptions::kEcheSWAProcessAndroidAccessibilityTreeName,
     flag_descriptions::kEcheSWAProcessAndroidAccessibilityTreeDescription,
     kOsCrOS,
     FEATURE_VALUE_TYPE(
         ash::features::kEcheSWAProcessAndroidAccessibilityTree)},
    {"eche-swa-debug-mode", flag_descriptions::kEcheSWADebugModeName,
     flag_descriptions::kEcheSWADebugModeDescription, kOsCrOS,
     FEATURE_VALUE_TYPE(ash::features::kEcheSWADebugMode)},
    {"eche-swa-disable-stun-server",
     flag_descriptions::kEcheSWADisableStunServerName,
     flag_descriptions::kEcheSWADisableStunServerDescription, kOsCrOS,
     FEATURE_VALUE_TYPE(ash::features::kEcheSWADisableStunServer)},
    {"eche-swa-measure-latency", flag_descriptions::kEcheSWAMeasureLatencyName,
     flag_descriptions::kEcheSWAMeasureLatencyDescription, kOsCrOS,
     FEATURE_VALUE_TYPE(ash::features::kEcheSWAMeasureLatency)},
    {"eche-swa-send-start-signaling",
     flag_descriptions::kEcheSWASendStartSignalingName,
     flag_descriptions::kEcheSWASendStartSignalingDescription, kOsCrOS,
     FEATURE_VALUE_TYPE(ash::features::kEcheSWASendStartSignaling)},
    {"phone-hub-onboarding-notifier-revemp",
     flag_descriptions::kPhoneHubOnboardingNotifierRevampName,
     flag_descriptions::kPhoneHubOnboardingNotifierRevampDescription, kOsCrOS,
     FEATURE_VALUE_TYPE(ash::features::kPhoneHubOnboardingNotifierRevamp)},
    {"print-management-jelly", flag_descriptions::kPrintManagementJellyName,
     flag_descriptions::kPrintManagementJellyDescription, kOsCrOS,
     FEATURE_VALUE_TYPE(ash::features::kPrintManagementJelly)},
    {"print-management-setup-assistance",
     flag_descriptions::kPrintManagementSetupAssistanceName,
     flag_descriptions::kPrintManagementSetupAssistanceDescription, kOsCrOS,
     FEATURE_VALUE_TYPE(ash::features::kPrintManagementSetupAssistance)},
    {"print-preview-discovered-printers",
     flag_descriptions::kPrintPreviewDiscoveredPrintersName,
     flag_descriptions::kPrintPreviewDiscoveredPrintersDescription, kOsCrOS,
     FEATURE_VALUE_TYPE(ash::features::kPrintPreviewDiscoveredPrinters)},
    {"printer-settings-printer-status",
     flag_descriptions::kPrinterSettingsPrinterStatusName,
     flag_descriptions::kPrinterSettingsPrinterStatusDescription, kOsCrOS,
     FEATURE_VALUE_TYPE(ash::features::kPrinterSettingsPrinterStatus)},
    {"printer-settings-revamp", flag_descriptions::kPrinterSettingsRevampName,
     flag_descriptions::kPrinterSettingsRevampDescription, kOsCrOS,
     FEATURE_VALUE_TYPE(ash::features::kPrinterSettingsRevamp)},
    {kGlanceablesV2InternalName, flag_descriptions::kGlanceablesV2Name,
     flag_descriptions::kGlanceablesV2Description, kOsCrOS,
     FEATURE_VALUE_TYPE(ash::features::kGlanceablesV2)},
    {kGlanceablesV2CalendarViewInternalName,
     flag_descriptions::kGlanceablesV2CalendarViewName,
     flag_descriptions::kGlanceablesV2CalendarViewDescription, kOsCrOS,
     FEATURE_VALUE_TYPE(ash::features::kGlanceablesV2CalendarView)},
#endif  // BUILDFLAG(IS_CHROMEOS_ASH)

#if BUILDFLAG(IS_ANDROID)
    {"omnibox-cache-suggestion-resources",
     flag_descriptions::kOmniboxCacheSuggestionResourcesName,
     flag_descriptions::kOmniboxCacheSuggestionResourcesDescription, kOsAndroid,
     FEATURE_VALUE_TYPE(chrome::android::kOmniboxCacheSuggestionResources)},

    {"omnibox-match-toolbar-and-status-bar-color",
     flag_descriptions::kOmniboxMatchToolbarAndStatusBarColorName,
     flag_descriptions::kOmniboxMatchToolbarAndStatusBarColorDescription,
     kOsAndroid,
     FEATURE_VALUE_TYPE(omnibox::kOmniboxMatchToolbarAndStatusBarColor)},

    {"omnibox-modernize-visual-update",
     flag_descriptions::kOmniboxModernizeVisualUpdateName,
     flag_descriptions::kOmniboxModernizeVisualUpdateDescription, kOsAndroid,
     FEATURE_WITH_PARAMS_VALUE_TYPE(omnibox::kOmniboxModernizeVisualUpdate,
                                    kOmniboxModernizeVisualUpdateVariations,
                                    "OmniboxModernizeVisualUpdate")},

    {"omnibox-most-visited-tiles-horizontal-render-group",
     flag_descriptions::kOmniboxMostVisitedTilesHorizontalRenderGroupName,
     flag_descriptions::
         kOmniboxMostVisitedTilesHorizontalRenderGroupDescription,
     kOsAndroid,
     FEATURE_VALUE_TYPE(omnibox::kMostVisitedTilesHorizontalRenderGroup)},

    {"omnibox-query-tiles-in-zps-on-ntp",
     flag_descriptions::kOmniboxQueryTilesInZPSOnNTPName,
     flag_descriptions::kOmniboxQueryTilesInZPSOnNTPDesc, kOsAndroid,
     FEATURE_WITH_PARAMS_VALUE_TYPE(omnibox::kQueryTilesInZPSOnNTP,
                                    kOmniboxQueryTilesVariations,
                                    "OmniboxQueryTilesInZPSOnNTP")},

    {"omnibox-warm-recycled-view-pool",
     flag_descriptions::kOmniboxWarmRecycledViewPoolName,
     flag_descriptions::kOmniboxWarmRecycledViewPoolDescription, kOsAndroid,
     FEATURE_VALUE_TYPE(chrome::android::kOmniboxWarmRecycledViewPool)},

    {"android-app-integration", flag_descriptions::kAndroidAppIntegrationName,
     flag_descriptions::kAndroidAppIntegrationDescription, kOsAndroid,
     FEATURE_VALUE_TYPE(chrome::android::kAndroidAppIntegration)},

    {"android-app-integration-safesearch",
     flag_descriptions::kAndroidAppIntegrationSafeSearchName,
     flag_descriptions::kAndroidAppIntegrationSafeSearchDescription, kOsAndroid,
     FEATURE_VALUE_TYPE(chrome::android::kAndroidAppIntegrationSafeSearch)},

    {"auxiliary-search-donation",
     flag_descriptions::kAuxiliarySearchDonationName,
     flag_descriptions::kAuxiliarySearchDonationDescription, kOsAndroid,
     FEATURE_WITH_PARAMS_VALUE_TYPE(chrome::android::kAuxiliarySearchDonation,
                                    kAuxiliarySearchDonationVariations,
                                    "AuxiliarySearchDonation")},
#endif  // BUILDFLAG(IS_ANDROID)

    {"omnibox-local-history-zero-suggest-beyond-ntp",
     flag_descriptions::kOmniboxLocalHistoryZeroSuggestBeyondNTPName,
     flag_descriptions::kOmniboxLocalHistoryZeroSuggestBeyondNTPDescription,
     kOsAll, FEATURE_VALUE_TYPE(omnibox::kLocalHistoryZeroSuggestBeyondNTP)},

    {"omnibox-on-clobber-focus-type-on-content",
     flag_descriptions::kOmniboxOnClobberFocusTypeOnContentName,
     flag_descriptions::kOmniboxOnClobberFocusTypeOnContentDescription, kOsAll,
     FEATURE_VALUE_TYPE(omnibox::kOmniboxOnClobberFocusTypeOnContent)},

    {"omnibox-report-assisted-query-stats",
     flag_descriptions::kOmniboxReportAssistedQueryStatsName,
     flag_descriptions::kOmniboxReportAssistedQueryStatsDescription, kOsAll,
     FEATURE_VALUE_TYPE(omnibox::kReportAssistedQueryStats)},

    {"omnibox-report-searchbox-stats",
     flag_descriptions::kOmniboxReportSearchboxStatsName,
     flag_descriptions::kOmniboxReportSearchboxStatsDescription, kOsAll,
     FEATURE_VALUE_TYPE(omnibox::kReportSearchboxStats)},

    {"omnibox-zero-suggest-prefetching",
     flag_descriptions::kOmniboxZeroSuggestPrefetchingName,
     flag_descriptions::kOmniboxZeroSuggestPrefetchingDescription, kOsAll,
     FEATURE_VALUE_TYPE(omnibox::kZeroSuggestPrefetching)},

    {"omnibox-zero-suggest-prefetching-on-srp",
     flag_descriptions::kOmniboxZeroSuggestPrefetchingOnSRPName,
     flag_descriptions::kOmniboxZeroSuggestPrefetchingOnSRPDescription, kOsAll,
     FEATURE_VALUE_TYPE(omnibox::kZeroSuggestPrefetchingOnSRP)},

    {"omnibox-zero-suggest-prefetching-on-web",
     flag_descriptions::kOmniboxZeroSuggestPrefetchingOnWebName,
     flag_descriptions::kOmniboxZeroSuggestPrefetchingOnWebDescription, kOsAll,
     FEATURE_VALUE_TYPE(omnibox::kZeroSuggestPrefetchingOnWeb)},

    {"omnibox-zero-suggest-in-memory-caching",
     flag_descriptions::kOmniboxZeroSuggestInMemoryCachingName,
     flag_descriptions::kOmniboxZeroSuggestInMemoryCachingDescription, kOsAll,
     FEATURE_VALUE_TYPE(omnibox::kZeroSuggestInMemoryCaching)},

#if BUILDFLAG(IS_LINUX) || BUILDFLAG(IS_CHROMEOS) || BUILDFLAG(IS_MAC) || \
    BUILDFLAG(IS_WIN) || BUILDFLAG(IS_FUCHSIA)
    {"omnibox-actions-ui-simplification",
     flag_descriptions::kOmniboxActionsUISimplificationName,
     flag_descriptions::kOmniboxActionsUISimplificationDescription, kOsDesktop,
     FEATURE_WITH_PARAMS_VALUE_TYPE(omnibox::kOmniboxActionsUISimplification,
                                    kOmniboxActionsUISimplificationVariations,
                                    "OmniboxActionsUISimplification")},
    {"omnibox-keyword-mode-refresh",
     flag_descriptions::kOmniboxKeywordModeRefreshName,
     flag_descriptions::kOmniboxKeywordModeRefreshDescription, kOsDesktop,
     FEATURE_VALUE_TYPE(omnibox::kOmniboxKeywordModeRefresh)},

    {"omnibox-domain-suggestions",
     flag_descriptions::kOmniboxDomainSuggestionsName,
     flag_descriptions::kOmniboxDomainSuggestionsDescription, kOsDesktop,
     FEATURE_VALUE_TYPE(omnibox::kDomainSuggestions)},

    {"omnibox-drive-suggestions",
     flag_descriptions::kOmniboxDriveSuggestionsName,
     flag_descriptions::kOmniboxDriveSuggestionsDescription, kOsDesktop,
     FEATURE_WITH_PARAMS_VALUE_TYPE(omnibox::kDocumentProvider,
                                    kOmniboxDriveSuggestionsVariations,
                                    "OmniboxDocumentProvider")},
    {"omnibox-drive-suggestions-no-setting",
     flag_descriptions::kOmniboxDriveSuggestionsNoSettingName,
     flag_descriptions::kOmniboxDriveSuggestionsNoSettingDescription,
     kOsDesktop, FEATURE_VALUE_TYPE(omnibox::kDocumentProviderNoSetting)},
    {"omnibox-drive-suggestions-no-sync-requirement",
     flag_descriptions::kOmniboxDriveSuggestionsNoSyncRequirementName,
     flag_descriptions::kOmniboxDriveSuggestionsNoSyncRequirementDescription,
     kOsDesktop,
     FEATURE_VALUE_TYPE(omnibox::kDocumentProviderNoSyncRequirement)},
    {"omnibox-force-allowed-to-be-default",
     flag_descriptions::kOmniboxForceAllowedToBeDefaultName,
     flag_descriptions::kOmniboxForceAllowedToBeDefaultDescription, kOsDesktop,
     FEATURE_VALUE_TYPE(omnibox_feature_configs::ForceAllowedToBeDefault::
                            kForceAllowedToBeDefault)},
    {"omnibox-pref-based-data-collection-consent-helper",
     flag_descriptions::kOmniboxPrefBasedDataCollectionConsentHelperName,
     flag_descriptions::kOmniboxPrefBasedDataCollectionConsentHelperDescription,
     kOsAll,
     FEATURE_VALUE_TYPE(omnibox::kPrefBasedDataCollectionConsentHelper)},
    {"omnibox-shortcut-boost", flag_descriptions::kOmniboxShortcutBoostName,
     flag_descriptions::kOmniboxShortcutBoostDescription, kOsDesktop,
     FEATURE_WITH_PARAMS_VALUE_TYPE(
         omnibox_feature_configs::ShortcutBoosting::kShortcutBoost,
         kOmniboxShortcutBoostVariations,
         "OmniboxBundledExperimentV1")},
    {"omnibox-rich-autocompletion-promising",
     flag_descriptions::kOmniboxRichAutocompletionPromisingName,
     flag_descriptions::kOmniboxRichAutocompletionPromisingDescription,
     kOsDesktop,
     FEATURE_WITH_PARAMS_VALUE_TYPE(
         omnibox::kRichAutocompletion,
         kOmniboxRichAutocompletionPromisingVariations,
         "OmniboxBundledExperimentV1")},
    {"omnibox-ml-log-url-scoring-signals",
     flag_descriptions::kOmniboxMlLogUrlScoringSignalsName,
     flag_descriptions::kOmniboxMlLogUrlScoringSignalsDescription, kOsDesktop,
     FEATURE_VALUE_TYPE(omnibox::kLogUrlScoringSignals)},
    {"omnibox-ml-url-scoring", flag_descriptions::kOmniboxMlUrlScoringName,
     flag_descriptions::kOmniboxMlUrlScoringDescription, kOsDesktop,
     FEATURE_WITH_PARAMS_VALUE_TYPE(omnibox::kMlUrlScoring,
                                    kOmniboxMlUrlScoringVariations,
                                    "MlUrlScoring")},
    {"omnibox-ml-url-scoring-model",
     flag_descriptions::kOmniboxMlUrlScoringModelName,
     flag_descriptions::kOmniboxMlUrlScoringModelDescription, kOsDesktop,
     FEATURE_VALUE_TYPE(omnibox::kUrlScoringModel)},

    {"realbox-secondary-zero-suggest",
     flag_descriptions::kRealboxSecondaryZeroSuggestName,
     flag_descriptions::kRealboxSecondaryZeroSuggestDescription, kOsDesktop,
     FEATURE_WITH_PARAMS_VALUE_TYPE(omnibox::kRealboxSecondaryZeroSuggest,
                                    kRealboxSecondaryZeroSuggestVariations,
                                    "RealboxSecondaryZeroSuggest")},
#endif  // BUILDFLAG(IS_LINUX) || BUILDFLAG(IS_CHROMEOS) || BUILDFLAG(IS_MAC) ||
        // BUILDFLAG(IS_WIN) || BUILDFLAG(IS_FUCHSIA)

#if BUILDFLAG(IS_ANDROID)
    {"omnibox-actions-in-suggest",
     flag_descriptions::kOmniboxActionsInSuggestName,
     flag_descriptions::kOmniboxActionsInSuggestDescription, kOsAndroid,
     FEATURE_WITH_PARAMS_VALUE_TYPE(omnibox::kActionsInSuggest,
                                    kOmniboxActionsInSuggestVariants,
                                    "OmniboxBundledExperimentV1")},

#endif  // BUILDFLAG(IS_ANDROID)
#if BUILDFLAG(IS_WIN)
    {"omnibox-on-device-head-suggestions",
     flag_descriptions::kOmniboxOnDeviceHeadSuggestionsName,
     flag_descriptions::kOmniboxOnDeviceHeadSuggestionsDescription, kOsWin,
     FEATURE_VALUE_TYPE(omnibox::kOnDeviceHeadProviderNonIncognito)},
    {"omnibox-on-device-head-suggestions-incognito",
     flag_descriptions::kOmniboxOnDeviceHeadSuggestionsIncognitoName,
     flag_descriptions::kOmniboxOnDeviceHeadSuggestionsIncognitoDescription,
     kOsWin, FEATURE_VALUE_TYPE(omnibox::kOnDeviceHeadProviderIncognito)},
#endif  // BUILDFLAG(IS_WIN)

    {"omnibox-on-device-tail-suggestions",
     flag_descriptions::kOmniboxOnDeviceTailSuggestionsName,
     flag_descriptions::kOmniboxOnDeviceTailSuggestionsDescription, kOsAll,
     FEATURE_VALUE_TYPE(omnibox::kOnDeviceTailModel)},

#if BUILDFLAG(IS_CHROMEOS_ASH)
    {"scheduler-configuration", flag_descriptions::kSchedulerConfigurationName,
     flag_descriptions::kSchedulerConfigurationDescription, kOsCrOS,
     MULTI_VALUE_TYPE(kSchedulerConfigurationChoices)},
#endif  // BUILDFLAG(IS_CHROMEOS_ASH)

#if BUILDFLAG(IS_ANDROID)
    {"enable-command-line-on-non-rooted-devices",
     flag_descriptions::kEnableCommandLineOnNonRootedName,
     flag_descriptions::kEnableCommandLineOnNoRootedDescription, kOsAndroid,
     FEATURE_VALUE_TYPE(chrome::android::kCommandLineOnNonRooted)},
#endif  // BUILDFLAG(IS_ANDROID)

#if BUILDFLAG(IS_ANDROID)
    {"app-menu-mobile-site-option",
     flag_descriptions::kAppMenuMobileSiteOptionName,
     flag_descriptions::kAppMenuMobileSiteOptionDescription, kOsAndroid,
     FEATURE_VALUE_TYPE(chrome::android::kAppMenuMobileSiteOption)},
#endif  // BUILDFLAG(IS_ANDROID)

    {"force-color-profile", flag_descriptions::kForceColorProfileName,
     flag_descriptions::kForceColorProfileDescription, kOsAll,
     MULTI_VALUE_TYPE(kForceColorProfileChoices)},

    {"forced-colors", flag_descriptions::kForcedColorsName,
     flag_descriptions::kForcedColorsDescription, kOsAll,
     FEATURE_VALUE_TYPE(features::kForcedColors)},

#if BUILDFLAG(IS_ANDROID)
    {"dynamic-color-gamut", flag_descriptions::kDynamicColorGamutName,
     flag_descriptions::kDynamicColorGamutDescription, kOsAndroid,
     FEATURE_VALUE_TYPE(features::kDynamicColorGamut)},
#endif

    {"memlog", flag_descriptions::kMemlogName,
     flag_descriptions::kMemlogDescription, kOsAll,
     MULTI_VALUE_TYPE(kMemlogModeChoices)},

    {"memlog-sampling-rate", flag_descriptions::kMemlogSamplingRateName,
     flag_descriptions::kMemlogSamplingRateDescription, kOsAll,
     MULTI_VALUE_TYPE(kMemlogSamplingRateChoices)},

    {"memlog-stack-mode", flag_descriptions::kMemlogStackModeName,
     flag_descriptions::kMemlogStackModeDescription, kOsAll,
     MULTI_VALUE_TYPE(kMemlogStackModeChoices)},

    {"omnibox-max-zero-suggest-matches",
     flag_descriptions::kOmniboxMaxZeroSuggestMatchesName,
     flag_descriptions::kOmniboxMaxZeroSuggestMatchesDescription,
     kOsDesktop | kOsAndroid,
     FEATURE_WITH_PARAMS_VALUE_TYPE(omnibox::kMaxZeroSuggestMatches,
                                    kMaxZeroSuggestMatchesVariations,
                                    "OmniboxBundledExperimentV1")},

    {"omnibox-ui-max-autocomplete-matches",
     flag_descriptions::kOmniboxUIMaxAutocompleteMatchesName,
     flag_descriptions::kOmniboxUIMaxAutocompleteMatchesDescription,
     kOsDesktop | kOsAndroid,
     FEATURE_WITH_PARAMS_VALUE_TYPE(
         omnibox::kUIExperimentMaxAutocompleteMatches,
         kOmniboxUIMaxAutocompleteMatchesVariations,
         "OmniboxBundledExperimentV1")},

    {"omnibox-max-url-matches", flag_descriptions::kOmniboxMaxURLMatchesName,
     flag_descriptions::kOmniboxMaxURLMatchesDescription, kOsAll,
     FEATURE_WITH_PARAMS_VALUE_TYPE(omnibox::kOmniboxMaxURLMatches,
                                    kOmniboxMaxURLMatchesVariations,
                                    "OmniboxMaxURLMatchesVariations")},

    {"omnibox-dynamic-max-autocomplete",
     flag_descriptions::kOmniboxDynamicMaxAutocompleteName,
     flag_descriptions::kOmniboxDynamicMaxAutocompleteDescription, kOsAll,
     FEATURE_WITH_PARAMS_VALUE_TYPE(omnibox::kDynamicMaxAutocomplete,
                                    kOmniboxDynamicMaxAutocompleteVariations,
                                    "OmniboxBundledExperimentV1")},

    {"omnibox-square-suggest-icons",
     flag_descriptions::kOmniboxSimplifiedUiSquareSuggestIconName,
     flag_descriptions::kOmniboxSimplifiedUiSquareSuggestIconDescription,
     kOsAll,
     FEATURE_WITH_PARAMS_VALUE_TYPE(omnibox::kSquareSuggestIcons,
                                    kOmniboxSquareSuggestionIconVariations,
                                    "OmniboxBundledExperimentV1")},

    {"omnibox-uniform-suggestion-height",
     flag_descriptions::kOmniboxSimplifiedUiUniformRowHeightName,
     flag_descriptions::kOmniboxSimplifiedUiUniformRowHeightDescription, kOsAll,
     FEATURE_WITH_PARAMS_VALUE_TYPE(omnibox::kUniformRowHeight,
                                    kOmniboxSuggestionHeightVariations,
                                    "Uniform Omnibox Suggest Heights")},
    {"omnibox-cr23-action-chips",
     flag_descriptions::kOmniboxCR23ActionChipsName,
     flag_descriptions::kOmniboxCR23ActionChipsDescription, kOsDesktop,
     FEATURE_VALUE_TYPE(omnibox::kCr2023ActionChips)},

    {"omnibox-cr23-action-chips-icons",
     flag_descriptions::kOmniboxCR23ActionChipsIconsName,
     flag_descriptions::kOmniboxCR23ActionChipsIconsDescription, kOsDesktop,
     FEATURE_VALUE_TYPE(omnibox::kCr2023ActionChipsIcons)},

    {"omnibox-cr23-expanded-state-height",
     flag_descriptions::kOmniboxCR23ExpandedStateHeightName,
     flag_descriptions::kOmniboxCR23ExpandedStateHeightDescription, kOsDesktop,
     FEATURE_VALUE_TYPE(omnibox::kExpandedStateHeight)},

    {"omnibox-cr23-expanded-state-shape",
     flag_descriptions::kOmniboxCR23ExpandedStateShapeName,
     flag_descriptions::kOmniboxCR23ExpandedStateShapeDescription, kOsDesktop,
     FEATURE_VALUE_TYPE(omnibox::kExpandedStateShape)},

    {"omnibox-cr23-expanded-state-suggest-icons",
     flag_descriptions::kOmniboxCR23ExpandedStateSuggestIconsName,
     flag_descriptions::kOmniboxCR23ExpandedStateSuggestIconsDescription,
     kOsDesktop, FEATURE_VALUE_TYPE(omnibox::kExpandedStateSuggestIcons)},

    {"omnibox-cr23-steady-state-icons",
     flag_descriptions::kOmniboxCR23SteadyStateIconsName,
     flag_descriptions::kOmniboxCR23SteadyStateIconsDescription, kOsAll,
     FEATURE_VALUE_TYPE(omnibox::kOmniboxCR23SteadyStateIcons)},

    {"omnibox-cr23-expanded-state-colors",
     flag_descriptions::kOmniboxCR23ExpandedStateColorsName,
     flag_descriptions::kOmniboxCR23ExpandedStateColorsDescription, kOsDesktop,
     FEATURE_VALUE_TYPE(omnibox::kExpandedStateColors)},

    {"omnibox-cr23-expanded-state-layout",
     flag_descriptions::kOmniboxCR23ExpandedStateLayoutName,
     flag_descriptions::kOmniboxCR23ExpandedStateLayoutDescription, kOsDesktop,
     FEATURE_VALUE_TYPE(omnibox::kExpandedLayout)},

    {"omnibox-cr23-suggestion-hover-fill-shape",
     flag_descriptions::kOmniboxCR23SuggestionHoverFillShapeName,
     flag_descriptions::kOmniboxCR23SuggestionHoverFillShapeDescription,
     kOsDesktop, FEATURE_VALUE_TYPE(omnibox::kSuggestionHoverFillShape)},

    {"omnibox-gm3-steady-state-background-color",
     flag_descriptions::kOmniboxGM3SteadyStateBackgroundColorName,
     flag_descriptions::kOmniboxGM3SteadyStateBackgroundColorDescription,
     kOsAll, FEATURE_VALUE_TYPE(omnibox::kOmniboxSteadyStateBackgroundColor)},

    {"omnibox-gm3-steady-state-height",
     flag_descriptions::kOmniboxGM3SteadyStateHeightName,
     flag_descriptions::kOmniboxGM3SteadyStateHeightDescription, kOsAll,
     FEATURE_VALUE_TYPE(omnibox::kOmniboxSteadyStateHeight)},

    {"omnibox-gm3-steady-state-text-color",
     flag_descriptions::kOmniboxGM3SteadyStateTextColorName,
     flag_descriptions::kOmniboxGM3SteadyStateTextColorDescription, kOsAll,
     FEATURE_VALUE_TYPE(omnibox::kOmniboxSteadyStateTextColor)},

    {"omnibox-gm3-steady-state-text-style",
     flag_descriptions::kOmniboxGM3SteadyStateTextStyleName,
     flag_descriptions::kOmniboxGM3SteadyStateTextStyleDescription, kOsAll,
     FEATURE_WITH_PARAMS_VALUE_TYPE(omnibox::kOmniboxSteadyStateTextStyle,
                                    kOmniboxFontSizeVariations,
                                    "OmniboxCR2023m113")},

    {"omnibox-grouping-framework-zps",
     flag_descriptions::kOmniboxGroupingFrameworkZPSName,
     flag_descriptions::kOmniboxGroupingFrameworkDescription, kOsAll,
     FEATURE_VALUE_TYPE(omnibox::kGroupingFrameworkForZPS)},

    {"omnibox-grouping-framework-non-zps",
     flag_descriptions::kOmniboxGroupingFrameworkNonZPSName,
     flag_descriptions::kOmniboxGroupingFrameworkDescription, kOsAll,
     FEATURE_VALUE_TYPE(omnibox::kGroupingFrameworkForNonZPS)},

    {"omnibox-company-entity-icon-adjustment",
     flag_descriptions::kOmniboxCompanyEntityIconAdjustmentName,
     flag_descriptions::kOmniboxCompanyEntityIconAdjustmentDescription, kOsAll,
     FEATURE_WITH_PARAMS_VALUE_TYPE(
         omnibox::kCompanyEntityIconAdjustment,
         kOmniboxCompanyEntityIconAdjustmentVariations,
         "OmniboxCompanyEntityIconAdjustment")},

    {"omnibox-calc-provider", flag_descriptions::kOmniboxCalcProviderName,
     flag_descriptions::kOmniboxCalcProviderDescription, kOsAll,
     FEATURE_VALUE_TYPE(omnibox_feature_configs::CalcProvider::kCalcProvider)},

    {"optimization-guide-debug-logs",
     flag_descriptions::kOptimizationGuideDebugLogsName,
     flag_descriptions::kOptimizationGuideDebugLogsDescription, kOsAll,
     SINGLE_VALUE_TYPE(optimization_guide::switches::kDebugLoggingEnabled)},

    {"optimization-guide-model-execution",
     flag_descriptions::kOptimizationGuideModelExecutionName,
     flag_descriptions::kOptimizationGuideModelExecutionDescription, kOsDesktop,
     FEATURE_VALUE_TYPE(
         optimization_guide::features::kOptimizationGuideModelExecution)},

    {"optimization-guide-on-device-model",
     flag_descriptions::kOptimizationGuideOnDeviceModelName,
     flag_descriptions::kOptimizationGuideOnDeviceModelDescription, kOsDesktop,
     FEATURE_VALUE_TYPE(
         optimization_guide::features::kOptimizationGuideOnDeviceModel)},

    {"organic-repeatable-queries",
     flag_descriptions::kOrganicRepeatableQueriesName,
     flag_descriptions::kOrganicRepeatableQueriesDescription,
     kOsDesktop | kOsAndroid,
     FEATURE_WITH_PARAMS_VALUE_TYPE(history::kOrganicRepeatableQueries,
                                    kOrganicRepeatableQueriesVariations,
                                    "OrganicRepeatableQueries")},

    {"history-journeys", flag_descriptions::kJourneysName,
     flag_descriptions::kJourneysDescription, kOsDesktop | kOsAndroid,
     FEATURE_WITH_PARAMS_VALUE_TYPE(history_clusters::internal::kJourneys,
                                    kJourneysVariations,
                                    "HistoryJourneys")},

    {"rename-history-journeys", flag_descriptions::kRenameJourneysName,
     flag_descriptions::kRenameJourneysDescription, kOsDesktop | kOsAndroid,
     FEATURE_VALUE_TYPE(history_clusters::kRenameJourneys)},

    {"history-journeys-labels", flag_descriptions::kJourneysLabelsName,
     flag_descriptions::kJourneysLabelsDescription, kOsDesktop | kOsAndroid,
     FEATURE_WITH_PARAMS_VALUE_TYPE(history_clusters::internal::kJourneysLabels,
                                    kJourneysLabelsVariations,
                                    "HistoryJourneysLabels")},

    {"history-journeys-show-all-clusters",
     flag_descriptions::kJourneysShowAllClustersName,
     flag_descriptions::kJourneysShowAllClustersDescription,
     kOsDesktop | kOsAndroid,
     SINGLE_VALUE_TYPE(history_clusters::switches::
                           kShouldShowAllClustersOnProminentUiSurfaces)},

    {"history-journeys-include-synced-visits",
     flag_descriptions::kJourneysIncludeSyncedVisitsName,
     flag_descriptions::kJourneysIncludeSyncedVisitsDescription,
     kOsDesktop | kOsAndroid,
     FEATURE_VALUE_TYPE(
         history_clusters::internal::kJourneysIncludeSyncedVisits)},

    {"history-journeys-zero-state-filtering",
     flag_descriptions::kJourneysZeroStateFilteringName,
     flag_descriptions::kJourneysZeroStateFilteringDescription,
     kOsDesktop | kOsAndroid,
     FEATURE_VALUE_TYPE(
         history_clusters::internal::kJourneysZeroStateFiltering)},

    {"extract-related-searches-from-prefetched-zps-response",
     flag_descriptions::kExtractRelatedSearchesFromPrefetchedZPSResponseName,
     flag_descriptions::
         kExtractRelatedSearchesFromPrefetchedZPSResponseDescription,
     kOsDesktop | kOsAndroid,
     FEATURE_VALUE_TYPE(optimization_guide::features::
                            kExtractRelatedSearchesFromPrefetchedZPSResponse)},

    {"page-image-service-optimization-guide-salient-images",
     flag_descriptions::kPageImageServiceOptimizationGuideSalientImagesName,
     flag_descriptions::
         kPageImageServiceOptimizationGuideSalientImagesDescription,
     kOsDesktop | kOsAndroid,
     FEATURE_WITH_PARAMS_VALUE_TYPE(
         page_image_service::kImageServiceOptimizationGuideSalientImages,
         kImageServiceOptimizationGuideSalientImagesVariations,
         "PageImageService")},

    {"page-image-service-suggest-powered-images",
     flag_descriptions::kPageImageServiceSuggestPoweredImagesName,
     flag_descriptions::kPageImageServiceSuggestPoweredImagesDescription,
     kOsDesktop,
     FEATURE_VALUE_TYPE(page_image_service::kImageServiceSuggestPoweredImages)},

    {"page-content-annotations", flag_descriptions::kPageContentAnnotationsName,
     flag_descriptions::kPageContentAnnotationsDescription,
     kOsDesktop | kOsAndroid,
     FEATURE_WITH_PARAMS_VALUE_TYPE(
         optimization_guide::features::kPageContentAnnotations,
         kPageContentAnnotationsVariations,
         "PageContentAnnotations")},

    {"page-content-annotations-persist-salient-image-metadata",
     flag_descriptions::kPageContentAnnotationsPersistSalientImageMetadataName,
     flag_descriptions::
         kPageContentAnnotationsPersistSalientImageMetadataDescription,
     kOsDesktop,
     FEATURE_VALUE_TYPE(
         optimization_guide::features::
             kPageContentAnnotationsPersistSalientImageMetadata)},

    {"page-content-annotations-remote-page-metadata",
     flag_descriptions::kPageContentAnnotationsRemotePageMetadataName,
     flag_descriptions::kPageContentAnnotationsRemotePageMetadataDescription,
     kOsDesktop,
     FEATURE_VALUE_TYPE(optimization_guide::features::kRemotePageMetadata)},

    {"page-visibility-page-content-annotations",
     flag_descriptions::kPageVisibilityPageContentAnnotationsName,
     flag_descriptions::kPageVisibilityPageContentAnnotationsDescription,
     kOsDesktop | kOsAndroid,
     FEATURE_VALUE_TYPE(
         optimization_guide::features::kPageVisibilityPageContentAnnotations)},

#if BUILDFLAG(IS_CHROMEOS_ASH)
    {"enable-remove-stale-policy-pinned-apps-from-shelf",
     flag_descriptions::kEnableRemoveStalePolicyPinnedAppsFromShelfName,
     flag_descriptions::kEnableRemoveStalePolicyPinnedAppsFromShelfDescription,
     kOsCrOS,
     FEATURE_VALUE_TYPE(ash::features::kRemoveStalePolicyPinnedAppsFromShelf)},
    {"handwriting-gesture-editing",
     flag_descriptions::kHandwritingGestureEditingName,
     flag_descriptions::kHandwritingGestureEditingDescription, kOsCrOS,
     FEATURE_VALUE_TYPE(ash::features::kHandwritingGestureEditing)},
    {"handwriting-legacy-recognition",
     flag_descriptions::kHandwritingLegacyRecognitionName,
     flag_descriptions::kHandwritingLegacyRecognitionDescription, kOsCrOS,
     FEATURE_VALUE_TYPE(ash::features::kHandwritingLegacyRecognition)},
    {"handwriting-library-dlc", flag_descriptions::kHandwritingLibraryDlcName,
     flag_descriptions::kHandwritingLibraryDlcDescription, kOsCrOS,
     FEATURE_VALUE_TYPE(ash::features::kHandwritingLibraryDlc)},
    {"language-packs-in-settings",
     flag_descriptions::kLanguagePacksInSettingsName,
     flag_descriptions::kLanguagePacksInSettingsDescription, kOsCrOS,
     FEATURE_VALUE_TYPE(ash::features::kLanguagePacksInSettings)},
#endif  // BUILDFLAG(IS_CHROMEOS_ASH)

    {"block-insecure-private-network-requests",
     flag_descriptions::kBlockInsecurePrivateNetworkRequestsName,
     flag_descriptions::kBlockInsecurePrivateNetworkRequestsDescription, kOsAll,
     FEATURE_VALUE_TYPE(features::kBlockInsecurePrivateNetworkRequests)},

    {"private-network-access-respect-preflight-results",
     flag_descriptions::kPrivateNetworkAccessRespectPreflightResultsName,
     flag_descriptions::kPrivateNetworkAccessRespectPreflightResultsDescription,
     kOsAll,
     FEATURE_VALUE_TYPE(
         features::kPrivateNetworkAccessRespectPreflightResults)},

    {"private-network-access-preflight-short-timeout",
     flag_descriptions::kPrivateNetworkAccessPreflightShortTimeoutName,
     flag_descriptions::kPrivateNetworkAccessPreflightShortTimeoutDescription,
     kOsAll,
     FEATURE_VALUE_TYPE(
         network::features::kPrivateNetworkAccessPreflightShortTimeout)},

    {"private-network-access-permission-prompt",
     flag_descriptions::kPrivateNetworkAccessPermissionPromptName,
     flag_descriptions::kPrivateNetworkAccessPermissionPromptDescription,
     kOsDesktop,
     FEATURE_VALUE_TYPE(
         network::features::kPrivateNetworkAccessPermissionPrompt)},

    {"main-thread-compositing-priority",
     flag_descriptions::kMainThreadCompositingPriorityName,
     flag_descriptions::kMainThreadCompositingPriorityDescription, kOsAll,
     FEATURE_VALUE_TYPE(features::kMainThreadCompositingPriority)},

    {"mbi-mode", flag_descriptions::kMBIModeName,
     flag_descriptions::kMBIModeDescription, kOsAll,
     FEATURE_WITH_PARAMS_VALUE_TYPE(features::kMBIMode,
                                    kMBIModeVariations,
                                    "MBIMode")},

#if BUILDFLAG(IS_CHROMEOS_ASH)
    {"double-tap-to-zoom-in-tablet-mode",
     flag_descriptions::kDoubleTapToZoomInTabletModeName,
     flag_descriptions::kDoubleTapToZoomInTabletModeDescription, kOsCrOS,
     FEATURE_VALUE_TYPE(features::kDoubleTapToZoomInTabletMode)},

    {"quick-settings-pwa-notifications",
     flag_descriptions::kQuickSettingsPWANotificationsName,
     flag_descriptions::kQuickSettingsPWANotificationsDescription, kOsCrOS,
     FEATURE_VALUE_TYPE(features::kQuickSettingsPWANotifications)},
#endif  // BUILDFLAG(IS_CHROMEOS_ASH)

    {flag_descriptions::kTabGroupsSaveId, flag_descriptions::kTabGroupsSaveName,
     flag_descriptions::kTabGroupsSaveDescription, kOsDesktop,
     FEATURE_VALUE_TYPE(features::kTabGroupsSave)},

    {flag_descriptions::kScrollableTabStripFlagId,
     flag_descriptions::kScrollableTabStripName,
     flag_descriptions::kScrollableTabStripDescription, kOsDesktop,
     FEATURE_WITH_PARAMS_VALUE_TYPE(features::kScrollableTabStrip,
                                    kTabScrollingVariations,
                                    "TabScrolling")},

    {flag_descriptions::kTabScrollingButtonPositionFlagId,
     flag_descriptions::kTabScrollingButtonPositionName,
     flag_descriptions::kTabScrollingButtonPositionDescription, kOsDesktop,
     FEATURE_WITH_PARAMS_VALUE_TYPE(features::kTabScrollingButtonPosition,
                                    kTabScrollingButtonPositionVariations,
                                    "TabScrollingButtonPosition")},

    {flag_descriptions::kScrollableTabStripWithDraggingFlagId,
     flag_descriptions::kScrollableTabStripWithDraggingName,
     flag_descriptions::kScrollableTabStripWithDraggingDescription, kOsDesktop,
     FEATURE_WITH_PARAMS_VALUE_TYPE(features::kScrollableTabStripWithDragging,
                                    kTabScrollingWithDraggingVariations,
                                    "TabScrollingWithDragging")},

    {flag_descriptions::kScrollableTabStripOverflowFlagId,
     flag_descriptions::kScrollableTabStripOverflowName,
     flag_descriptions::kScrollableTabStripOverflowDescription, kOsDesktop,
     FEATURE_WITH_PARAMS_VALUE_TYPE(features::kScrollableTabStripOverflow,
                                    kScrollableTabStripOverflowVariations,
                                    "ScrollableTabStripOverflow")},

    {"split-tabstrip", flag_descriptions::kSplitTabStripName,
     flag_descriptions::kSplitTabStripDescription, kOsDesktop,
     FEATURE_VALUE_TYPE(features::kSplitTabStrip)},

    {flag_descriptions::kSidePanelJourneysFlagId,
     flag_descriptions::kSidePanelJourneysName,
     flag_descriptions::kSidePanelJourneysDescription, kOsDesktop,
     FEATURE_WITH_PARAMS_VALUE_TYPE(
         history_clusters::kSidePanelJourneys,
         kSidePanelJourneysOpensFromOmniboxVariations,
         "SidePanelJourneys")},

    {flag_descriptions::kSidePanelJourneysQuerylessFlagId,
     flag_descriptions::kSidePanelJourneysQuerylessName,
     flag_descriptions::kSidePanelJourneysQuerylessDescription, kOsDesktop,
     FEATURE_VALUE_TYPE(features::kSidePanelJourneysQueryless)},

#if !BUILDFLAG(IS_ANDROID)
    {flag_descriptions::kSidePanelPinningFlagId,
     flag_descriptions::kSidePanelPinningName,
     flag_descriptions::kSidePanelPinningDescription, kOsDesktop,
     FEATURE_VALUE_TYPE(features::kSidePanelPinning)},
#endif  // !BUILDFLAG(IS_ANDROID)

#if BUILDFLAG(IS_ANDROID)
    {"enable-reader-mode-in-cct", flag_descriptions::kReaderModeInCCTName,
     flag_descriptions::kReaderModeInCCTDescription, kOsAndroid,
     FEATURE_VALUE_TYPE(chrome::android::kReaderModeInCCT)},
#endif  // BUILDFLAG(IS_ANDROID)

    {"shopping-collection",
     commerce::flag_descriptions::kShoppingCollectionName,
     commerce::flag_descriptions::kShoppingCollectionDescription,
     kOsAndroid | kOsDesktop,
     FEATURE_VALUE_TYPE(commerce::kShoppingCollection)},

    {"shopping-list", commerce::flag_descriptions::kShoppingListName,
     commerce::flag_descriptions::kShoppingListDescription,
     kOsAndroid | kOsDesktop, FEATURE_VALUE_TYPE(commerce::kShoppingList)},

    {"shopping-list-track-by-default",
     commerce::flag_descriptions::kShoppingListTrackByDefaultName,
     commerce::flag_descriptions::kShoppingListTrackByDefaultDescription,
     kOsAndroid | kOsDesktop,
     FEATURE_VALUE_TYPE(commerce::kShoppingListTrackByDefault)},

    {"local-pdp-detection",
     commerce::flag_descriptions::kCommerceLocalPDPDetectionName,
     commerce::flag_descriptions::kCommerceLocalPDPDetectionDescription,
     kOsAndroid | kOsDesktop,
     FEATURE_VALUE_TYPE(commerce::kCommerceLocalPDPDetection)},

    {"parcel-tracking-test-data",
     commerce::flag_descriptions::kParcelTrackingTestDataName,
     commerce::flag_descriptions::kParcelTrackingTestDataDescription,
     kOsAndroid | kOsDesktop,
     FEATURE_WITH_PARAMS_VALUE_TYPE(commerce::kParcelTrackingTestData,
                                    kParcelTrackingTestDataVariations,
                                    "ParcelTrackingTestData")},

#if BUILDFLAG(IS_ANDROID)
    {"price-change-module", flag_descriptions::kPriceChangeModuleName,
     flag_descriptions::kPriceChangeModuleDescription, kOsAndroid,
     FEATURE_VALUE_TYPE(chrome::android::kPriceChangeModule)},
#endif  // BUILDFLAG(IS_ANDROID)

#if !BUILDFLAG(IS_ANDROID)
    {"price-tracking-icon-colors",
     commerce::flag_descriptions::kPriceTrackingIconColorsDescription,
     commerce::flag_descriptions::kPriceTrackingIconColorsDescription,
     kOsDesktop, FEATURE_VALUE_TYPE(commerce::kPriceTrackingIconColors)},

    {"enable-retail-coupons", flag_descriptions::kRetailCouponsName,
     flag_descriptions::kRetailCouponsDescription, kOsDesktop,
     FEATURE_VALUE_TYPE(commerce::kRetailCoupons)},

    {"ntp-alpha-background-collections",
     flag_descriptions::kNtpAlphaBackgroundCollectionsName,
     flag_descriptions::kNtpAlphaBackgroundCollectionsDescription, kOsDesktop,
     FEATURE_VALUE_TYPE(ntp_features::kNtpAlphaBackgroundCollections)},

    {"ntp-background-image-error-detection",
     flag_descriptions::kNtpBackgroundImageErrorDetectionName,
     flag_descriptions::kNtpBackgroundImageErrorDetectionDescription,
     kOsDesktop,
     FEATURE_VALUE_TYPE(ntp_features::kNtpBackgroundImageErrorDetection)},

    {"ntp-cache-one-google-bar", flag_descriptions::kNtpCacheOneGoogleBarName,
     flag_descriptions::kNtpCacheOneGoogleBarDescription, kOsDesktop,
     FEATURE_VALUE_TYPE(ntp_features::kCacheOneGoogleBar)},

    {"ntp-chrome-cart-journeys-module-coexist",
     flag_descriptions::kNtpChromeCartHistoryClusterCoexistName,
     flag_descriptions::kNtpChromeCartHistoryClusterCoexistDescription,
     kOsDesktop,
     FEATURE_VALUE_TYPE(ntp_features::kNtpChromeCartHistoryClusterCoexist)},

    {"ntp-chrome-cart-in-journeys-module",
     flag_descriptions::kNtpChromeCartInHistoryClustersModuleName,
     flag_descriptions::kNtpChromeCartInHistoryClustersModuleDescription,
     kOsDesktop,
     FEATURE_WITH_PARAMS_VALUE_TYPE(
         ntp_features::kNtpChromeCartInHistoryClusterModule,
         kNtpChromeCartInHistoryClustersModuleVariations,
         "DesktopNtpModules")},

    {"ntp-discounts-in-journeys-module",
     flag_descriptions::kNtpDiscountsInHistoryClustersModuleName,
     flag_descriptions::kNtpDiscountsInHistoryClustersModuleDescription,
     kOsDesktop,
     FEATURE_VALUE_TYPE(ntp_features::kNtpHistoryClustersModuleDiscounts)},

    {"ntp-chrome-cart-module", flag_descriptions::kNtpChromeCartModuleName,
     flag_descriptions::kNtpChromeCartModuleDescription, kOsDesktop,
     FEATURE_WITH_PARAMS_VALUE_TYPE(ntp_features::kNtpChromeCartModule,
                                    kNtpChromeCartModuleVariations,
                                    "DesktopNtpModules")},

    {"chrome-cart-dom-based-heuristics",
     commerce::flag_descriptions::kChromeCartDomBasedHeuristicsName,
     commerce::flag_descriptions::kChromeCartDomBasedHeuristicsDescription,
     kOsDesktop, FEATURE_VALUE_TYPE(commerce::kChromeCartDomBasedHeuristics)},

    {"ntp-comprehensive-theme-realbox",
     flag_descriptions::kNtpComprehensiveThemeRealboxName,
     flag_descriptions::kNtpComprehensiveThemeRealboxDescription, kOsDesktop,
     FEATURE_VALUE_TYPE(ntp_features::kNtpComprehensiveThemeRealbox)},

    {"ntp-drive-module", flag_descriptions::kNtpDriveModuleName,
     flag_descriptions::kNtpDriveModuleDescription, kOsDesktop,
     FEATURE_WITH_PARAMS_VALUE_TYPE(ntp_features::kNtpDriveModule,
                                    kNtpDriveModuleVariations,
                                    "DesktopNtpModules")},

    {"ntp-drive-module-segmentation",
     flag_descriptions::kNtpDriveModuleSegmentationName,
     flag_descriptions::kNtpDriveModuleSegmentationDescription, kOsDesktop,
     FEATURE_VALUE_TYPE(ntp_features::kNtpDriveModuleSegmentation)},

    {"ntp-drive-module-show-six-files",
     flag_descriptions::kNtpDriveModuleShowSixFilesName,
     flag_descriptions::kNtpDriveModuleShowSixFilesDescription, kOsDesktop,
     FEATURE_VALUE_TYPE(ntp_features::kNtpDriveModuleShowSixFiles)},

#if !defined(OFFICIAL_BUILD)
    {"ntp-dummy-modules", flag_descriptions::kNtpDummyModulesName,
     flag_descriptions::kNtpDummyModulesDescription, kOsDesktop,
     FEATURE_VALUE_TYPE(ntp_features::kNtpDummyModules)},
#endif

    {"ntp-journeys-module", flag_descriptions::kNtpHistoryClustersModuleName,
     flag_descriptions::kNtpHistoryClustersModuleDescription, kOsDesktop,
     FEATURE_WITH_PARAMS_VALUE_TYPE(ntp_features::kNtpHistoryClustersModule,
                                    kNtpHistoryClustersModuleVariations,
                                    "DesktopNtpModules")},

    {"ntp-journeys-module-suggestion-chip-header",
     flag_descriptions::kNtpHistoryClustersModuleSuggestionChipHeaderName,
     flag_descriptions::
         kNtpHistoryClustersModuleSuggestionChipHeaderDescription,
     kOsDesktop,
     FEATURE_VALUE_TYPE(
         ntp_features::kNtpHistoryClustersModuleSuggestionChipHeader)},

    {"ntp-journeys-module-model-ranking",
     flag_descriptions::kNtpHistoryClustersModuleUseModelRankingName,
     flag_descriptions::kNtpHistoryClustersModuleUseModelRankingDescription,
     kOsDesktop,
     FEATURE_VALUE_TYPE(
         ntp_features::kNtpHistoryClustersModuleUseModelRanking)},

    {"ntp-journeys-module-text-only",
     flag_descriptions::kNtpHistoryClustersModuleTextOnlyName,
     flag_descriptions::kNtpHistoryClustersModuleTextOnlyDescription,
     kOsDesktop,
     FEATURE_VALUE_TYPE(ntp_features::kNtpHistoryClustersModuleTextOnly)},

    {"ntp-modules-header-icon", flag_descriptions::kNtpModulesHeaderIconName,
     flag_descriptions::kNtpModulesHeaderIconDescription, kOsDesktop,
     FEATURE_VALUE_TYPE(ntp_features::kNtpModulesHeaderIcon)},

    {"ntp-wide-modules", flag_descriptions::kNtpWideModulesName,
     flag_descriptions::kNtpWideModulesDescription, kOsDesktop,
     FEATURE_VALUE_TYPE(ntp_features::kNtpWideModules)},

    {"ntp-reduced-logo-space", flag_descriptions::kNtpReducedLogoSpaceName,
     flag_descriptions::kNtpReducedLogoSpaceDescription, kOsDesktop,
     FEATURE_VALUE_TYPE(ntp_features::kNtpReducedLogoSpace)},

    {"ntp-single-row-shortcuts", flag_descriptions::kNtpSingleRowShortcutsName,
     flag_descriptions::kNtpSingleRowShortcutsDescription, kOsDesktop,
     FEATURE_VALUE_TYPE(ntp_features::kNtpSingleRowShortcuts)},

    {"ntp-middle-slot-promo-dismissal",
     flag_descriptions::kNtpMiddleSlotPromoDismissalName,
     flag_descriptions::kNtpMiddleSlotPromoDismissalDescription, kOsDesktop,
     FEATURE_WITH_PARAMS_VALUE_TYPE(ntp_features::kNtpMiddleSlotPromoDismissal,
                                    kNtpMiddleSlotPromoDismissalVariations,
                                    "DesktopNtpModules")},

    {"ntp-modules-drag-and-drop", flag_descriptions::kNtpModulesDragAndDropName,
     flag_descriptions::kNtpModulesDragAndDropDescription, kOsDesktop,
     FEATURE_VALUE_TYPE(ntp_features::kNtpModulesDragAndDrop)},

    {"ntp-modules-first-run-experience",
     flag_descriptions::kNtpModulesFirstRunExperienceName,
     flag_descriptions::kNtpModulesFirstRunExperienceDescription, kOsDesktop,
     FEATURE_VALUE_TYPE(ntp_features::kNtpModulesFirstRunExperience)},

    {"ntp-modules-redesigned", flag_descriptions::kNtpModulesRedesignedName,
     flag_descriptions::kNtpModulesRedesignedDescription, kOsDesktop,
     FEATURE_VALUE_TYPE(ntp_features::kNtpModulesRedesigned)},

    {"ntp-photos-module", flag_descriptions::kNtpPhotosModuleName,
     flag_descriptions::kNtpPhotosModuleDescription, kOsDesktop,
     FEATURE_WITH_PARAMS_VALUE_TYPE(ntp_features::kNtpPhotosModule,
                                    kNtpPhotosModuleVariations,
                                    "DesktopNtpModules")},

    {"ntp-photos-opt-in-art-work",
     flag_descriptions::kNtpPhotosModuleOptInArtWorkName,
     flag_descriptions::kNtpPhotosModuleOptInArtWorkDescription, kOsDesktop,
     FEATURE_WITH_PARAMS_VALUE_TYPE(
         ntp_features::kNtpPhotosModuleCustomizedOptInArtWork,
         kNtpPhotosModuleOptInArtWorkVariations,
         "DesktopNtpModules")},

    {"ntp-photos-opt-in-title",
     flag_descriptions::kNtpPhotosModuleOptInTitleName,
     flag_descriptions::kNtpPhotosModuleOptInTitleDescription, kOsDesktop,
     FEATURE_WITH_PARAMS_VALUE_TYPE(
         ntp_features::kNtpPhotosModuleCustomizedOptInTitle,
         kNtpPhotosModuleOptInTitleVariations,
         "DesktopNtpModules")},

    {"ntp-photos-soft-opt-out",
     flag_descriptions::kNtpPhotosModuleSoftOptOutName,
     flag_descriptions::kNtpPhotosModuleSoftOptOutDescription, kOsDesktop,
     FEATURE_VALUE_TYPE(ntp_features::kNtpPhotosModuleSoftOptOut)},

    {"ntp-recipe-tasks-module", flag_descriptions::kNtpRecipeTasksModuleName,
     flag_descriptions::kNtpRecipeTasksModuleDescription, kOsDesktop,
     FEATURE_WITH_PARAMS_VALUE_TYPE(ntp_features::kNtpRecipeTasksModule,
                                    kNtpRecipeTasksModuleVariations,
                                    "DesktopNtpModules")},

    {"ntp-realbox-is-tall", flag_descriptions::kNtpRealboxIsTallName,
     flag_descriptions::kNtpRealboxIsTallDescription, kOsDesktop,
     FEATURE_VALUE_TYPE(ntp_features::kRealboxIsTall)},

    {"ntp-realbox-cr23-all", flag_descriptions::kNtpRealboxCr23AllName,
     flag_descriptions::kNtpRealboxCr23AllDescription, kOsDesktop,
     FEATURE_VALUE_TYPE(ntp_features::kRealboxCr23All)},

    {"ntp-realbox-cr23-consistent-row-height",
     flag_descriptions::kNtpRealboxCr23ConsistentRowHeightName,
     flag_descriptions::kNtpRealboxCr23ConsistentRowHeightDescription,
     kOsDesktop,
     FEATURE_VALUE_TYPE(ntp_features::kRealboxCr23ConsistentRowHeight)},

    {"ntp-realbox-cr23-expanded-state-icons",
     flag_descriptions::kNtpRealboxCr23ExpandedStateIconsName,
     flag_descriptions::kNtpRealboxCr23ExpandedStateIconsDescription,
     kOsDesktop,
     FEATURE_VALUE_TYPE(ntp_features::kRealboxCr23ExpandedStateIcons)},

    {"ntp-realbox-cr23-expanded-state-layout",
     flag_descriptions::kNtpRealboxCr23ExpandedStateLayoutName,
     flag_descriptions::kNtpRealboxCr23ExpandedStateLayoutDescription,
     kOsDesktop,
     FEATURE_VALUE_TYPE(ntp_features::kRealboxCr23ExpandedStateLayout)},

    {"ntp-realbox-cr23-theming", flag_descriptions::kNtpRealboxCr23ThemingName,
     flag_descriptions::kNtpRealboxCr23ThemingDescription, kOsDesktop,
     FEATURE_VALUE_TYPE(ntp_features::kRealboxCr23Theming)},

    {"ntp-realbox-match-searchbox-theme",
     flag_descriptions::kNtpRealboxMatchSearchboxThemeName,
     flag_descriptions::kNtpRealboxMatchSearchboxThemeDescription, kOsDesktop,
     FEATURE_VALUE_TYPE(ntp_features::kRealboxMatchSearchboxTheme)},

    {"ntp-realbox-pedals", flag_descriptions::kNtpRealboxPedalsName,
     flag_descriptions::kNtpRealboxPedalsDescription, kOsDesktop,
     FEATURE_VALUE_TYPE(omnibox::kNtpRealboxPedals)},

    {"ntp-realbox-use-google-g-icon",
     flag_descriptions::kNtpRealboxUseGoogleGIconName,
     flag_descriptions::kNtpRealboxUseGoogleGIconDescription, kOsDesktop,
     FEATURE_VALUE_TYPE(ntp_features::kRealboxUseGoogleGIcon)},

    {"ntp-realbox-width-behavior",
     flag_descriptions::kNtpRealboxWidthBehaviorName,
     flag_descriptions::kNtpRealboxWidthBehaviorDescription, kOsDesktop,
     FEATURE_WITH_PARAMS_VALUE_TYPE(ntp_features::kRealboxWidthBehavior,
                                    kNtpRealboxWidthBehaviorVariations,
                                    "NtpRealboxWidthBehavior")},

    {"ntp-safe-browsing-module", flag_descriptions::kNtpSafeBrowsingModuleName,
     flag_descriptions::kNtpSafeBrowsingModuleDescription, kOsDesktop,
     FEATURE_WITH_PARAMS_VALUE_TYPE(ntp_features::kNtpSafeBrowsingModule,
                                    kNtpSafeBrowsingModuleVariations,
                                    "DesktopNtpModules")},

    {"ntp-desktop-lens", flag_descriptions::kNtpDesktopLensName,
     flag_descriptions::kNtpDesktopLensDescription, kOsDesktop,
     FEATURE_VALUE_TYPE(ntp_features::kNtpRealboxLensSearch)},

    {"ntp-tab-resumption-module",
     flag_descriptions::kNtpTabResumptionModuleName,
     flag_descriptions::kNtpTabResumptionModuleDescription, kOsDesktop,
     FEATURE_WITH_PARAMS_VALUE_TYPE(ntp_features::kNtpTabResumptionModule,
                                    kNtpTabResumptionModuleVariations,
                                    "NtpTabResumptionModules")},

    {"price-insights", commerce::flag_descriptions::kPriceInsightsName,
     commerce::flag_descriptions::kPriceInsightsDescription, kOsDesktop,
     FEATURE_VALUE_TYPE(commerce::kPriceInsights)},

    {"shopping-page-types", commerce::flag_descriptions::kShoppingPageTypesName,
     commerce::flag_descriptions::kShoppingPageTypesDescription, kOsDesktop,
     FEATURE_VALUE_TYPE(commerce::kShoppingPageTypes)},

    {"show-discount-on-navigation",
     commerce::flag_descriptions::kShowDiscountOnNavigationName,
     commerce::flag_descriptions::kShowDiscountOnNavigationDescription,
     kOsDesktop, FEATURE_VALUE_TYPE(commerce::kShowDiscountOnNavigation)},

#endif  // !BUILDFLAG(IS_ANDROID)

#if BUILDFLAG(CHROME_WIDE_ECHO_CANCELLATION)
    {"chrome-wide-echo-cancellation",
     flag_descriptions::kChromeWideEchoCancellationName,
     flag_descriptions::kChromeWideEchoCancellationDescription,
     kOsMac | kOsWin | kOsLinux,
     FEATURE_VALUE_TYPE(media::kChromeWideEchoCancellation)},
#endif  // BUILDFLAG(CHROME_WIDE_ECHO_CANCELLATION)

#if BUILDFLAG(DCHECK_IS_CONFIGURABLE)
    {"dcheck-is-fatal", flag_descriptions::kDcheckIsFatalName,
     flag_descriptions::kDcheckIsFatalDescription, kOsWin,
     FEATURE_VALUE_TYPE(base::kDCheckIsFatalFeature)},
#endif  // BUILDFLAG(DCHECK_IS_CONFIGURABLE)

    {"enable-pixel-canvas-recording",
     flag_descriptions::kEnablePixelCanvasRecordingName,
     flag_descriptions::kEnablePixelCanvasRecordingDescription, kOsDesktop,
     FEATURE_VALUE_TYPE(features::kEnablePixelCanvasRecording)},

    {"enable-parallel-downloading", flag_descriptions::kParallelDownloadingName,
     flag_descriptions::kParallelDownloadingDescription, kOsAll,
     FEATURE_VALUE_TYPE(download::features::kParallelDownloading)},

#if BUILDFLAG(IS_WIN) || BUILDFLAG(IS_LINUX)
    {"enable-async-dns", flag_descriptions::kAsyncDnsName,
     flag_descriptions::kAsyncDnsDescription, kOsWin | kOsLinux,
     FEATURE_VALUE_TYPE(features::kAsyncDns)},
#endif  // BUILDFLAG(IS_WIN) || BUILDFLAG(IS_LINUX)

    {"downloads-migrate-to-jobs-api",
     flag_descriptions::kDownloadsMigrateToJobsAPIName,
     flag_descriptions::kDownloadsMigrateToJobsAPIDescription, kOsAndroid,
     FEATURE_VALUE_TYPE(download::features::kDownloadsMigrateToJobsAPI)},

    {"download-notification-service-unified-api",
     flag_descriptions::kDownloadNotificationServiceUnifiedAPIName,
     flag_descriptions::kDownloadNotificationServiceUnifiedAPIDescription,
     kOsAndroid,
     FEATURE_VALUE_TYPE(
         download::features::kDownloadNotificationServiceUnifiedAPI)},

    {"download-range", flag_descriptions::kDownloadRangeName,
     flag_descriptions::kDownloadRangeDescription, kOsAll,
     FEATURE_VALUE_TYPE(download::features::kDownloadRange)},

    {"tab-hover-card-images", flag_descriptions::kTabHoverCardImagesName,
     flag_descriptions::kTabHoverCardImagesDescription, kOsDesktop,
     FEATURE_VALUE_TYPE(features::kTabHoverCardImages)},

    {"enable-network-logging-to-file",
     flag_descriptions::kEnableNetworkLoggingToFileName,
     flag_descriptions::kEnableNetworkLoggingToFileDescription, kOsAll,
     SINGLE_VALUE_TYPE(network::switches::kLogNetLog)},

#if !BUILDFLAG(IS_ANDROID)
    {"sync-webauthn-credentials",
     flag_descriptions::kSyncWebauthnCredentialsName,
     flag_descriptions::kSyncWebauthnCredentialsDescription, kOsDesktop,
     FEATURE_VALUE_TYPE(syncer::kSyncWebauthnCredentials)},

    {"web-authentication-new-passkey-ui",
     flag_descriptions::kWebAuthenticationNewPasskeyUIName,
     flag_descriptions::kWebAuthenticationNewPasskeyUIDescription, kOsDesktop,
     FEATURE_VALUE_TYPE(device::kWebAuthnNewPasskeyUI)},

    {"web-authentication-permit-enterprise-attestation",
     flag_descriptions::kWebAuthenticationPermitEnterpriseAttestationName,
     flag_descriptions::
         kWebAuthenticationPermitEnterpriseAttestationDescription,
     kOsAll,
     ORIGIN_LIST_VALUE_TYPE(
         webauthn::switches::kPermitEnterpriseAttestationOriginList,
         "")},
#endif

    {"legacy-tech-report-enable-cookie-issue-reports",
     flag_descriptions::kLegacyTechReportEnableCookieIssueReportsName,
     flag_descriptions::kLegacyTechReportEnableCookieIssueReportsDescription,
     kOsAll,
     FEATURE_VALUE_TYPE(features::kLegacyTechReportEnableCookieIssueReports)},

    {"legacy-tech-report-top-level-url",
     flag_descriptions::kLegacyTechReportTopLevelUrlName,
     flag_descriptions::kLegacyTechReportTopLevelUrlDescription, kOsAll,
     FEATURE_VALUE_TYPE(features::kLegacyTechReportTopLevelUrl)},

    {
        "zero-copy-tab-capture",
        flag_descriptions::kEnableZeroCopyTabCaptureName,
        flag_descriptions::kEnableZeroCopyTabCaptureDescription,
        kOsMac | kOsWin | kOsCrOS,
        FEATURE_VALUE_TYPE(blink::features::kZeroCopyTabCapture),
    },

#if BUILDFLAG(ENABLE_PDF)
    {"accessible-pdf-form", flag_descriptions::kAccessiblePDFFormName,
     flag_descriptions::kAccessiblePDFFormDescription, kOsDesktop,
     FEATURE_VALUE_TYPE(chrome_pdf::features::kAccessiblePDFForm)},

    {"pdf-oopif", flag_descriptions::kPdfOopifName,
     flag_descriptions::kPdfOopifDescription, kOsDesktop,
     FEATURE_VALUE_TYPE(chrome_pdf::features::kPdfOopif)},

    {"pdf-portfolio", flag_descriptions::kPdfPortfolioName,
     flag_descriptions::kPdfPortfolioDescription, kOsDesktop,
     FEATURE_VALUE_TYPE(chrome_pdf::features::kPdfPortfolio)},

    {"pdf-use-skia-renderer", flag_descriptions::kPdfUseSkiaRendererName,
     flag_descriptions::kPdfUseSkiaRendererDescription, kOsDesktop,
     FEATURE_VALUE_TYPE(chrome_pdf::features::kPdfUseSkiaRenderer)},
#endif  // BUILDFLAG(ENABLE_PDF)

#if BUILDFLAG(ENABLE_PRINTING)
#if BUILDFLAG(IS_CHROMEOS)
    {"enable-borderless-printing",
     flag_descriptions::kEnableBorderlessPrintingName,
     flag_descriptions::kEnableBorderlessPrintingDescription,
     kOsCrOS | kOsLacros,
     FEATURE_VALUE_TYPE(printing::features::kEnableBorderlessPrinting)},
#endif  // BUILDFLAG(IS_CHROMEOS)

#if BUILDFLAG(IS_MAC)
    {"cups-ipp-printing-backend",
     flag_descriptions::kCupsIppPrintingBackendName,
     flag_descriptions::kCupsIppPrintingBackendDescription, kOsMac,
     FEATURE_VALUE_TYPE(printing::features::kCupsIppPrintingBackend)},
#endif  // BUILDFLAG(IS_MAC)

#if BUILDFLAG(IS_WIN)
    {"print-with-postscript-type42-fonts",
     flag_descriptions::kPrintWithPostScriptType42FontsName,
     flag_descriptions::kPrintWithPostScriptType42FontsDescription, kOsWin,
     FEATURE_VALUE_TYPE(printing::features::kPrintWithPostScriptType42Fonts)},

    {"print-with-reduced-rasterization",
     flag_descriptions::kPrintWithReducedRasterizationName,
     flag_descriptions::kPrintWithReducedRasterizationDescription, kOsWin,
     FEATURE_VALUE_TYPE(printing::features::kPrintWithReducedRasterization)},

    {"read-printer-capabilities-with-xps",
     flag_descriptions::kReadPrinterCapabilitiesWithXpsName,
     flag_descriptions::kReadPrinterCapabilitiesWithXpsDescription, kOsWin,
     FEATURE_VALUE_TYPE(printing::features::kReadPrinterCapabilitiesWithXps)},

    {"use-xps-for-printing", flag_descriptions::kUseXpsForPrintingName,
     flag_descriptions::kUseXpsForPrintingDescription, kOsWin,
     FEATURE_VALUE_TYPE(printing::features::kUseXpsForPrinting)},

    {"use-xps-for-printing-from-pdf",
     flag_descriptions::kUseXpsForPrintingFromPdfName,
     flag_descriptions::kUseXpsForPrintingFromPdfDescription, kOsWin,
     FEATURE_VALUE_TYPE(printing::features::kUseXpsForPrintingFromPdf)},
#endif  // BUILDFLAG(IS_WIN)
#endif  // BUILDFLAG(ENABLE_PRINTING)

#if BUILDFLAG(IS_WIN)
    {"enable-windows-gaming-input-data-fetcher",
     flag_descriptions::kEnableWindowsGamingInputDataFetcherName,
     flag_descriptions::kEnableWindowsGamingInputDataFetcherDescription, kOsWin,
     FEATURE_VALUE_TYPE(features::kEnableWindowsGamingInputDataFetcher)},

    {"windows11-mica-titlebar", flag_descriptions::kWindows11MicaTitlebarName,
     flag_descriptions::kWindows11MicaTitlebarDescription, kOsWin,
     FEATURE_VALUE_TYPE(kWindows11MicaTitlebar)},
#endif

#if BUILDFLAG(IS_ANDROID)
    {"enable-new-tab-search-engine-url-android",
     flag_descriptions::kNewTabSearchEngineUrlAndroidName,
     flag_descriptions::kNewTabSearchEngineUrlAndroidDescription, kOsAndroid,
     FEATURE_WITH_PARAMS_VALUE_TYPE(
         chrome::android::kNewTabSearchEngineUrlAndroid,
         kNewTabSearchEngineUrlAndroidVariations,
         "NewTabSearchEngineUrl")},

    {"enable-start-surface", flag_descriptions::kStartSurfaceAndroidName,
     flag_descriptions::kStartSurfaceAndroidDescription, kOsAndroid,
     FEATURE_WITH_PARAMS_VALUE_TYPE(chrome::android::kStartSurfaceAndroid,
                                    kStartSurfaceAndroidVariations,
                                    "ChromeStart")},

    {"enable-feed-position-on-ntp", flag_descriptions::kFeedPositionAndroidName,
     flag_descriptions::kFeedPositionAndroidDescription, kOsAndroid,
     FEATURE_WITH_PARAMS_VALUE_TYPE(chrome::android::kFeedPositionAndroid,
                                    kFeedPositionAndroidVariations,
                                    "FeedPositionAndroid")},

    {"enable-instant-start", flag_descriptions::kInstantStartName,
     flag_descriptions::kInstantStartDescription, kOsAndroid,
     FEATURE_VALUE_TYPE(chrome::android::kInstantStart)},

    {"enable-start-surface-disabled-feed-improvement",
     flag_descriptions::kStartSurfaceDisabledFeedImprovementName,
     flag_descriptions::kStartSurfaceDisabledFeedImprovementDescription,
     kOsAndroid,
     FEATURE_VALUE_TYPE(chrome::android::kStartSurfaceDisabledFeedImprovement)},

    {"enable-start-surface-refactor",
     flag_descriptions::kStartSurfaceRefactorName,
     flag_descriptions::kStartSurfaceRefactorDescription, kOsAndroid,
     FEATURE_VALUE_TYPE(chrome::android::kStartSurfaceRefactor)},

    {"enable-start-surface-on-tablet",
     flag_descriptions::kStartSurfaceOnTabletName,
     flag_descriptions::kStartSurfaceOnTabletDescription, kOsAndroid,
     FEATURE_VALUE_TYPE(chrome::android::kStartSurfaceOnTablet)},

    {"enable-start-surface-spare-tab",
     flag_descriptions::kStartSurfaceSpareTabName,
     flag_descriptions::kStartSurfaceSpareTabDescription, kOsAndroid,
     FEATURE_VALUE_TYPE(chrome::android::kStartSurfaceSpareTab)},

    {"enable-start-surface-with-accessibility",
     flag_descriptions::kStartSurfaceWithAccessibilityName,
     flag_descriptions::kStartSurfaceWithAccessibilityDescription, kOsAndroid,
     FEATURE_VALUE_TYPE(chrome::android::kStartSurfaceWithAccessibility)},

    {"enable-surface-polish", flag_descriptions::kSurfacePolishName,
     flag_descriptions::kSurfacePolishDescription, kOsAndroid,
     FEATURE_WITH_PARAMS_VALUE_TYPE(chrome::android::kSurfacePolish,
                                    kSurfacePolishVariations,
                                    "SurfacePolish")},

    {"enable-show-ntp-at-startup",
     flag_descriptions::kShowNtpAtStartupAndroidName,
     flag_descriptions::kShowNtpAtStartupAndroidDescription, kOsAndroid,
     FEATURE_VALUE_TYPE(chrome::android::kShowNtpAtStartupAndroid)},

    {"enable-show-scrollable-mvt-on-ntp",
     flag_descriptions::kShowScrollableMVTOnNTPAndroidName,
     flag_descriptions::kShowScrollableMVTOnNTPAndroidDescription, kOsAndroid,
     FEATURE_VALUE_TYPE(chrome::android::kShowScrollableMVTOnNTPAndroid)},

    {"enable-search-resumption-module",
     flag_descriptions::kSearchResumptionModuleAndroidName,
     flag_descriptions::kSearchResumptionModuleAndroidDescription, kOsAndroid,
     FEATURE_WITH_PARAMS_VALUE_TYPE(
         chrome::android::kSearchResumptionModuleAndroid,
         kSearchResumptionModuleAndroidVariations,
         "kSearchResumptionModuleAndroid")},

    {"enable-archive-tab-service", flag_descriptions::kArchiveTabServiceName,
     flag_descriptions::kArchiveTabServiceDescription, kOsAndroid,
     FEATURE_VALUE_TYPE(chrome::android::kArchiveTabService)},

    {"enable-critical-persisted-tab-data",
     flag_descriptions::kCriticalPersistedTabDataName,
     flag_descriptions::kCriticalPersistedTabDataDescription, kOsAndroid,
     FEATURE_WITH_PARAMS_VALUE_TYPE(chrome::android::kCriticalPersistedTabData,
                                    kCriticalPersistedTabDataVariations,
                                    "CriticalPersistedTabData")},
    {"enable-tabstate-flatbuffer", flag_descriptions::kTabStateFlatBufferName,
     flag_descriptions::kTabStateFlatBufferDescription, kOsAndroid,
     FEATURE_VALUE_TYPE(chrome::android::kTabStateFlatBuffer)},

    {"suppress-toolbar-captures",
     flag_descriptions::kSuppressToolbarCapturesName,
     flag_descriptions::kSuppressToolbarCapturesDescription, kOsAndroid,
     FEATURE_VALUE_TYPE(chrome::android::kSuppressToolbarCaptures)},

    {"enable-commerce-price-tracking",
     commerce::flag_descriptions::kCommercePriceTrackingName,
     commerce::flag_descriptions::kCommercePriceTrackingDescription, kOsAndroid,
     FEATURE_WITH_PARAMS_VALUE_TYPE(
         commerce::kCommercePriceTracking,
         commerce::kCommercePriceTrackingAndroidVariations,
         "CommercePriceTracking")},

    {"enable-start-surface-return-time",
     flag_descriptions::kStartSurfaceReturnTimeName,
     flag_descriptions::kStartSurfaceReturnTimeDescription, kOsAndroid,
     FEATURE_WITH_PARAMS_VALUE_TYPE(chrome::android::kStartSurfaceReturnTime,
                                    kStartSurfaceReturnTimeVariations,
                                    "StartSurfaceReturnTime")},
    {"account-reauthentication-recent-time-window",
     flag_descriptions::kAccountReauthenticationRecentTimeWindowName,
     flag_descriptions::kAccountReauthenticationRecentTimeWindowDescription,
     kOsAndroid,
     FEATURE_WITH_PARAMS_VALUE_TYPE(
         chrome::android::kAccountReauthenticationRecentTimeWindow,
         kAccountReauthenticationRecentTimeWindowVariations,
         "AccountReauthenticationRecentTimeWindow")},

    {"tab-drag-drop", flag_descriptions::kTabDragDropName,
     flag_descriptions::kTabDragDropDescription, kOsAndroid,
     FEATURE_VALUE_TYPE(chrome::android::kTabDragDropAndroid)},

    {"tab-link-drag-drop", flag_descriptions::kTabAndLinkDragDropName,
     flag_descriptions::kTabAndLinkDragDropDescription, kOsAndroid,
     FEATURE_VALUE_TYPE(chrome::android::kTabAndLinkDragDropAndroid)},

    {"enable-tab-engagement-reporting",
     flag_descriptions::kTabEngagementReportingName,
     flag_descriptions::kTabEngagementReportingDescription, kOsAndroid,
     FEATURE_VALUE_TYPE(chrome::android::kTabEngagementReportingAndroid)},

    {"enable-tablet-toolbar-reordering",
     flag_descriptions::kTabletToolbarReorderingAndroidName,
     flag_descriptions::kTabletToolbarReorderingAndroidDescription, kOsAndroid,
     FEATURE_VALUE_TYPE(chrome::android::kTabletToolbarReordering)},

    {"enable-tab-strip-startup-refactoring",
     flag_descriptions::kTabStripStartupRefactoringName,
     flag_descriptions::kTabStripStartupRefactoringDescription, kOsAndroid,
     FEATURE_VALUE_TYPE(chrome::android::kTabStripStartupRefactoring)},

    {"enable-baseline-gm3-surface-colors",
     flag_descriptions::kBaselineGM3SurfaceColorsName,
     flag_descriptions::kBaselineGM3SurfaceColorsDescription, kOsAndroid,
     FEATURE_VALUE_TYPE(chrome::android::kBaselineGM3SurfaceColors)},

    {"enable-delay-temp-strip-removal",
     flag_descriptions::kDelayTempStripRemovalName,
     flag_descriptions::kDelayTempStripRemovalDescription, kOsAndroid,
     FEATURE_VALUE_TYPE(chrome::android::kDelayTempStripRemoval)},
#endif  // BUILDFLAG(IS_ANDROID)

    {"unsafely-treat-insecure-origin-as-secure",
     flag_descriptions::kTreatInsecureOriginAsSecureName,
     flag_descriptions::kTreatInsecureOriginAsSecureDescription, kOsAll,
     ORIGIN_LIST_VALUE_TYPE(
         network::switches::kUnsafelyTreatInsecureOriginAsSecure,
         "")},

    {"disable-process-reuse", flag_descriptions::kDisableProcessReuse,
     flag_descriptions::kDisableProcessReuseDescription, kOsDesktop,
     FEATURE_VALUE_TYPE(features::kDisableProcessReuse)},

#if !BUILDFLAG(IS_ANDROID)
    {"enable-accessibility-live-caption",
     flag_descriptions::kEnableAccessibilityLiveCaptionName,
     flag_descriptions::kEnableAccessibilityLiveCaptionDescription, kOsDesktop,
     FEATURE_VALUE_TYPE(media::kLiveCaption)},

    {"read-anything", flag_descriptions::kReadAnythingName,
     flag_descriptions::kReadAnythingDescription, kOsDesktop,
     FEATURE_VALUE_TYPE(features::kReadAnything)},

    {"read-anything-read-aloud", flag_descriptions::kReadAnythingReadAloudName,
     flag_descriptions::kReadAnythingReadAloudDescription, kOsDesktop,
     FEATURE_VALUE_TYPE(features::kReadAnythingReadAloud)},

    {"read-anything-with-screen2x",
     flag_descriptions::kReadAnythingWithScreen2xName,
     flag_descriptions::kReadAnythingWithScreen2xDescription, kOsDesktop,
     FEATURE_VALUE_TYPE(features::kReadAnythingWithScreen2x)},

    {"read-anything-with-algorithm",
     flag_descriptions::kReadAnythingWithAlgorithmName,
     flag_descriptions::kReadAnythingWithAlgorithmDescription, kOsDesktop,
     FEATURE_VALUE_TYPE(features::kReadAnythingWithAlgorithm)},

    {"read-anything-webui-toolbar",
     flag_descriptions::kReadAnythingWebUIToolbarName,
     flag_descriptions::kReadAnythingWebUIToolbarDescription, kOsDesktop,
     FEATURE_VALUE_TYPE(features::kReadAnythingWebUIToolbar)},

    {"read-anything-omnibox-icon",
     flag_descriptions::kReadAnythingOmniboxIconName,
     flag_descriptions::kReadAnythingOmniboxIconDescription, kOsDesktop,
     FEATURE_VALUE_TYPE(features::kReadAnythingOmniboxIcon)},

    {"support-tool", flag_descriptions::kSupportTool,
     flag_descriptions::kSupportToolDescription, kOsDesktop,
     FEATURE_VALUE_TYPE(features::kSupportTool)},

    {"support-tool-screenshot", flag_descriptions::kSupportToolScreenshot,
     flag_descriptions::kSupportToolScreenshotDescription, kOsDesktop,
     FEATURE_VALUE_TYPE(features::kSupportToolScreenshot)},
#endif  // !BUILDFLAG(IS_ANDROID)

    {"enable-auto-disable-accessibility",
     flag_descriptions::kEnableAutoDisableAccessibilityName,
     flag_descriptions::kEnableAutoDisableAccessibilityDescription, kOsAll,
     FEATURE_VALUE_TYPE(features::kAutoDisableAccessibility)},

#if BUILDFLAG(IS_ANDROID)
    {"enable-auto-disable-accessibility-v2",
     flag_descriptions::kEnableAutoDisableAccessibilityV2Name,
     flag_descriptions::kEnableAutoDisableAccessibilityV2Description,
     kOsAndroid, FEATURE_VALUE_TYPE(features::kAutoDisableAccessibilityV2)},
#endif

#if BUILDFLAG(IS_ANDROID)
    {"cct-incognito-available-to-third-party",
     flag_descriptions::kCCTIncognitoAvailableToThirdPartyName,
     flag_descriptions::kCCTIncognitoAvailableToThirdPartyDescription,
     kOsAndroid,
     FEATURE_VALUE_TYPE(chrome::android::kCCTIncognitoAvailableToThirdParty)},
#endif

#if BUILDFLAG(IS_ANDROID)
    {"cct-minimized", flag_descriptions::kCCTMinimizedName,
     flag_descriptions::kCCTMinimizedDescription, kOsAndroid,
     FEATURE_WITH_PARAMS_VALUE_TYPE(chrome::android::kCCTMinimized,
                                    kCCTMinimizedIconVariations,
                                    "CCTMinimizedIconVariations")},
#endif

#if BUILDFLAG(IS_ANDROID)
    {"cct-page-insights-hub", flag_descriptions::kCCTPageInsightsHubName,
     flag_descriptions::kCCTPageInsightsHubDescription, kOsAndroid,
     FEATURE_WITH_PARAMS_VALUE_TYPE(chrome::android::kCCTPageInsightsHub,
                                    kCCTPageInsightsHubVariations,
                                    "CCTPageInsightsHubVariations")},
    {"cct-prevent-touches", flag_descriptions::kCCTPreventTouchesName,
     flag_descriptions::kCCTPreventTouchesDescription, kOsAndroid,
     FEATURE_VALUE_TYPE(chrome::android::kCCTPreventTouches)},
    {"cct-resizable-for-third-parties",
     flag_descriptions::kCCTResizableForThirdPartiesName,
     flag_descriptions::kCCTResizableForThirdPartiesDescription, kOsAndroid,
     FEATURE_WITH_PARAMS_VALUE_TYPE(
         chrome::android::kCCTResizableForThirdParties,
         kCCTResizableThirdPartiesDefaultPolicyVariations,
         "CCTResizableThirdPartiesDefaultPolicy")},
    {"cct-resizable-side-sheet", flag_descriptions::kCCTResizableSideSheetName,
     flag_descriptions::kCCTResizableSideSheetDescription, kOsAndroid,
     FEATURE_VALUE_TYPE(chrome::android::kCCTResizableSideSheet)},
    {"cct-resizable-side-sheet-for-third-parties",
     flag_descriptions::kCCTResizableSideSheetForThirdPartiesName,
     flag_descriptions::kCCTResizableSideSheetForThirdPartiesDescription,
     kOsAndroid,
     FEATURE_VALUE_TYPE(
         chrome::android::kCCTResizableSideSheetForThirdParties)},
#endif

#if BUILDFLAG(IS_ANDROID)
    {"cct-real-time-engagement-signals",
     flag_descriptions::kCCTRealTimeEngagementSignalsName,
     flag_descriptions::kCCTRealTimeEngagementSignalsDescription, kOsAndroid,
     FEATURE_WITH_PARAMS_VALUE_TYPE(
         chrome::android::kCCTRealTimeEngagementSignals,
         kCCTRealTimeEngagementSignalsVariations,
         "CCTRealTimeEngagementSignals")},
    {"cct-real-time-engagement-signals-alternative-impl",
     flag_descriptions::kCCTRealTimeEngagementSignalsAlternativeImplName,
     flag_descriptions::kCCTRealTimeEngagementSignalsAlternativeImplDescription,
     kOsAndroid,
     FEATURE_WITH_PARAMS_VALUE_TYPE(
         chrome::android::kCCTRealTimeEngagementSignalsAlternativeImpl,
         kCCTRealTimeEngagementSignalsAlternativeImplVariations,
         "CCTRealTimeEngagementSignalsAlternativeImpl")},
#endif

#if BUILDFLAG(IS_ANDROID)
    {"cct-brand-transparency", flag_descriptions::kCCTBrandTransparencyName,
     flag_descriptions::kCCTBrandTransparencyDescription, kOsAndroid,
     FEATURE_WITH_PARAMS_VALUE_TYPE(chrome::android::kCCTBrandTransparency,
                                    kCctBrandTransparencyVariations,
                                    "CCTBrandTransparency")},
#endif

#if BUILDFLAG(IS_CHROMEOS)
    {"allow-dsp-based-aec", flag_descriptions::kCrOSDspBasedAecAllowedName,
     flag_descriptions::kCrOSDspBasedAecAllowedDescription, kOsCrOS | kOsLacros,
     FEATURE_VALUE_TYPE(media::kCrOSDspBasedAecAllowed)},
    {"allow-dsp-based-ns", flag_descriptions::kCrOSDspBasedNsAllowedName,
     flag_descriptions::kCrOSDspBasedNsAllowedDescription, kOsCrOS | kOsLacros,
     FEATURE_VALUE_TYPE(media::kCrOSDspBasedNsAllowed)},
    {"allow-dsp-based-agc", flag_descriptions::kCrOSDspBasedAgcAllowedName,
     flag_descriptions::kCrOSDspBasedAgcAllowedDescription, kOsCrOS | kOsLacros,
     FEATURE_VALUE_TYPE(media::kCrOSDspBasedAgcAllowed)},
    {"enforce-system-aec", flag_descriptions::kCrOSEnforceSystemAecName,
     flag_descriptions::kCrOSEnforceSystemAecDescription, kOsCrOS | kOsLacros,
     FEATURE_VALUE_TYPE(media::kCrOSEnforceSystemAec)},
    {"enforce-system-aec-agc", flag_descriptions::kCrOSEnforceSystemAecAgcName,
     flag_descriptions::kCrOSEnforceSystemAecAgcDescription,
     kOsCrOS | kOsLacros, FEATURE_VALUE_TYPE(media::kCrOSEnforceSystemAecAgc)},
    {"enforce-system-aec-ns-agc",
     flag_descriptions::kCrOSEnforceSystemAecNsAgcName,
     flag_descriptions::kCrOSEnforceSystemAecNsAgcDescription,
     kOsCrOS | kOsLacros,
     FEATURE_VALUE_TYPE(media::kCrOSEnforceSystemAecNsAgc)},
    {"enforce-system-aec-ns", flag_descriptions::kCrOSEnforceSystemAecNsName,
     flag_descriptions::kCrOSEnforceSystemAecNsDescription, kOsCrOS | kOsLacros,
     FEATURE_VALUE_TYPE(media::kCrOSEnforceSystemAecNs)},
    {"system-voice-isolation-option",
     flag_descriptions::kCrOSSystemVoiceIsolationOptionName,
     flag_descriptions::kCrOSSystemVoiceIsolationOptionDescription,
     kOsCrOS | kOsLacros,
     FEATURE_VALUE_TYPE(media::kCrOSSystemVoiceIsolationOption)},
    {"ignore-ui-gains", flag_descriptions::kIgnoreUiGainsName,
     flag_descriptions::kIgnoreUiGainsDescription, kOsCrOS | kOsLacros,
     FEATURE_VALUE_TYPE(media::kIgnoreUiGains)},
    {"show-force-respect-ui-gains-toggle",
     flag_descriptions::kShowForceRespectUiGainsToggleName,
     flag_descriptions::kShowForceRespectUiGainsToggleDescription,
     kOsCrOS | kOsLacros,
     FEATURE_VALUE_TYPE(media::kShowForceRespectUiGainsToggle)},
    {"audio-flexible-loopback-for-system-loopback",
     flag_descriptions::kAudioFlexibleLoopbackForSystemLoopbackName,
     flag_descriptions::kAudioFlexibleLoopbackForSystemLoopbackDescription,
     kOsCrOS | kOsLacros,
     FEATURE_VALUE_TYPE(media::kAudioFlexibleLoopbackForSystemLoopback)},
#endif

    {"enable-css-selector-fragment-anchor",
     flag_descriptions::kEnableCssSelectorFragmentAnchorName,
     flag_descriptions::kEnableCssSelectorFragmentAnchorDescription, kOsAll,
     FEATURE_VALUE_TYPE(blink::features::kCssSelectorFragmentAnchor)},

    {"drop-input-events-before-first-paint",
     flag_descriptions::kDropInputEventsBeforeFirstPaintName,
     flag_descriptions::kDropInputEventsBeforeFirstPaintDescription, kOsAll,
     FEATURE_VALUE_TYPE(blink::features::kDropInputEventsBeforeFirstPaint)},

    // Should only be available if kResamplingScrollEvents is on, and using
    // linear resampling.
    {"enable-resampling-scroll-events-experimental-prediction",
     flag_descriptions::kEnableResamplingScrollEventsExperimentalPredictionName,
     flag_descriptions::
         kEnableResamplingScrollEventsExperimentalPredictionDescription,
     kOsAll,
     FEATURE_WITH_PARAMS_VALUE_TYPE(
         ::features::kResamplingScrollEventsExperimentalPrediction,
         kResamplingScrollEventsExperimentalPredictionVariations,
         "ResamplingScrollEventsExperimentalLatency")},

#if BUILDFLAG(IS_WIN)
    {"calculate-native-win-occlusion",
     flag_descriptions::kCalculateNativeWinOcclusionName,
     flag_descriptions::kCalculateNativeWinOcclusionDescription, kOsWin,
     FEATURE_VALUE_TYPE(features::kCalculateNativeWinOcclusion)},
#endif  // BUILDFLAG(IS_WIN)

#if !BUILDFLAG(IS_ANDROID)
    {"happiness-tracking-surveys-for-desktop-demo",
     flag_descriptions::kHappinessTrackingSurveysForDesktopDemoName,
     flag_descriptions::kHappinessTrackingSurveysForDesktopDemoDescription,
     kOsDesktop,
     FEATURE_VALUE_TYPE(features::kHappinessTrackingSurveysForDesktopDemo)},
#endif  // !BUILDFLAG(IS_ANDROID)

#if !BUILDFLAG(IS_ANDROID)
    {"hats-webui", flag_descriptions::kHatsWebUIName,
     flag_descriptions::kHatsWebUIDescription, kOsDesktop,
     FEATURE_VALUE_TYPE(features::kHaTSWebUI)},
#endif  // !BUILDFLAG(IS_ANDROID)

#if BUILDFLAG(IS_ANDROID)
    {"android-hats-refactor", flag_descriptions::kAndroidHatsRefactorName,
     flag_descriptions::kAndroidHatsRefactorDescription, kOsAndroid,
     FEATURE_VALUE_TYPE(chrome::android::kAndroidHatsRefactor)},
#endif  // BUILDFLAG(IS_ANDROID)

#if BUILDFLAG(IS_WIN)
    {"use-angle", flag_descriptions::kUseAngleName,
     flag_descriptions::kUseAngleDescriptionWindows, kOsWin,
     MULTI_VALUE_TYPE(kUseAngleChoicesWindows)},
#elif BUILDFLAG(IS_MAC)
    {"use-angle", flag_descriptions::kUseAngleName,
     flag_descriptions::kUseAngleDescriptionMac, kOsMac,
     MULTI_VALUE_TYPE(kUseAngleChoicesMac)},
#endif

#if BUILDFLAG(IS_CHROMEOS_ASH)
    {"enable-assistant-dsp", flag_descriptions::kEnableGoogleAssistantDspName,
     flag_descriptions::kEnableGoogleAssistantDspDescription, kOsCrOS,
     FEATURE_VALUE_TYPE(ash::assistant::features::kEnableDspHotword)},
#endif  // BUILDFLAG(IS_CHROMEOS_ASH)

#if BUILDFLAG(IS_CHROMEOS)
    {"disable-quick-answers-v2-translation",
     flag_descriptions::kDisableQuickAnswersV2TranslationName,
     flag_descriptions::kDisableQuickAnswersV2TranslationDescription,
     kOsCrOS | kOsLacros,
     FEATURE_VALUE_TYPE(chromeos::features::kDisableQuickAnswersV2Translation)},
    {"quick-answers-rich-card", flag_descriptions::kQuickAnswersRichCardName,
     flag_descriptions::kQuickAnswersRichCardDescription, kOsCrOS | kOsLacros,
     FEATURE_VALUE_TYPE(chromeos::features::kQuickAnswersRichCard)},
#endif  // BUILDFLAG(IS_CHROMEOS)

#if BUILDFLAG(IS_ANDROID)
    {"instance-switcher", flag_descriptions::kInstanceSwitcherName,
     flag_descriptions::kInstanceSwitcherDescription, kOsAndroid,
     FEATURE_VALUE_TYPE(chrome::android::kInstanceSwitcher)},
#endif  // BUILDFLAG(IS_ANDROID)

    {"enable-gamepad-button-axis-events",
     flag_descriptions::kEnableGamepadButtonAxisEventsName,
     flag_descriptions::kEnableGamepadButtonAxisEventsDescription, kOsAll,
     FEATURE_VALUE_TYPE(features::kEnableGamepadButtonAxisEvents)},

    {"enable-gamepad-multitouch",
     flag_descriptions::kEnableGamepadMultitouchName,
     flag_descriptions::kEnableGamepadMultitouchDescription, kOsWin,
     FEATURE_VALUE_TYPE(features::kEnableGamepadMultitouch)},

    {"restrict-gamepad-access", flag_descriptions::kRestrictGamepadAccessName,
     flag_descriptions::kRestrictGamepadAccessDescription, kOsAll,
     FEATURE_VALUE_TYPE(features::kRestrictGamepadAccess)},

    {"enable-gamepad-trigger-rumble",
     flag_descriptions::kEnableGamepadTriggerRumbleName,
     flag_descriptions::kEnableGamepadTriggerRumbleDescription, kOsAll,
     FEATURE_VALUE_TYPE(blink::features::kWGIGamepadTriggerRumble)},

#if !BUILDFLAG(IS_ANDROID)
    {"sharing-desktop-screenshots",
     flag_descriptions::kSharingDesktopScreenshotsName,
     flag_descriptions::kSharingDesktopScreenshotsDescription, kOsDesktop,
     FEATURE_VALUE_TYPE(sharing_hub::kDesktopScreenshots)},
#endif

#if BUILDFLAG(IS_CHROMEOS_ASH)
    {"cros-labs-overview-desk-navigation",
     flag_descriptions::kOverviewDeskNavigationName,
     flag_descriptions::kOverviewDeskNavigationDescription, kOsCrOS,
     FEATURE_VALUE_TYPE(ash::features::kOverviewDeskNavigation)},
#endif  // BUILDFLAG(IS_CHROMEOS_ASH)

#if BUILDFLAG(IS_CHROMEOS_ASH)
    {"enable-assistant-stereo-input",
     flag_descriptions::kEnableGoogleAssistantStereoInputName,
     flag_descriptions::kEnableGoogleAssistantStereoInputDescription, kOsCrOS,
     FEATURE_VALUE_TYPE(ash::assistant::features::kEnableStereoAudioInput)},
#endif  // BUILDFLAG(IS_CHROMEOS_ASH)

    {"enable-gpu-service-logging",
     flag_descriptions::kEnableGpuServiceLoggingName,
     flag_descriptions::kEnableGpuServiceLoggingDescription, kOsAll,
     SINGLE_VALUE_TYPE(switches::kEnableGPUServiceLogging)},

#if !BUILDFLAG(IS_ANDROID)
    {"hardware-media-key-handling",
     flag_descriptions::kHardwareMediaKeyHandling,
     flag_descriptions::kHardwareMediaKeyHandlingDescription, kOsDesktop,
     FEATURE_VALUE_TYPE(media::kHardwareMediaKeyHandling)},
#endif

#if BUILDFLAG(IS_CHROMEOS_ASH)
    {"app-management-app-details",
     flag_descriptions::kAppManagementAppDetailsName,
     flag_descriptions::kAppManagementAppDetailsDescription, kOsCrOS,
     FEATURE_VALUE_TYPE(features::kAppManagementAppDetails)},

    {"arc-window-predictor", flag_descriptions::kArcWindowPredictorName,
     flag_descriptions::kArcWindowPredictorDescription, kOsCrOS,
     FEATURE_VALUE_TYPE(full_restore::features::kArcWindowPredictor)},

    {"use-fake-device-for-media-stream",
     flag_descriptions::kUseFakeDeviceForMediaStreamName,
     flag_descriptions::kUseFakeDeviceForMediaStreamDescription, kOsCrOS,
     FEATURE_VALUE_TYPE(media::kUseFakeDeviceForMediaStream)},
#endif  // BUILDFLAG(IS_CHROMEOS_ASH)

#if BUILDFLAG(IS_CHROMEOS_ASH)
    {"arc-input-overlay-beta", flag_descriptions::kArcInputOverlayNameBeta,
     flag_descriptions::kArcInputOverlayDescriptionBeta, kOsCrOS,
     FEATURE_VALUE_TYPE(ash::features::kArcInputOverlayBeta)},

    {"arc-input-overlay-alpha-v2",
     flag_descriptions::kArcInputOverlayNameAlphaV2,
     flag_descriptions::kArcInputOverlayDescriptionAlphaV2, kOsCrOS,
     FEATURE_VALUE_TYPE(ash::features::kArcInputOverlayAlphaV2)},
#endif  // BUILDFLAG(IS_CHROMEOS_ASH)

#if BUILDFLAG(IS_CHROMEOS) && BUILDFLAG(USE_CHROMEOS_MEDIA_ACCELERATION)
#if !BUILDFLAG(USE_VAAPI)
    {"chromeos-direct-video-decoder",
     flag_descriptions::kChromeOSDirectVideoDecoderName,
     flag_descriptions::kChromeOSDirectVideoDecoderDescription,
     kOsCrOS | kOsLacros,
     FEATURE_VALUE_TYPE(media::kUseChromeOSDirectVideoDecoder)},
#endif  // !BUILDFLAG(USE_VAAPI)

    {"enable-vbr-encode-acceleration",
     flag_descriptions::kChromeOSHWVBREncodingName,
     flag_descriptions::kChromeOSHWVBREncodingDescription, kOsCrOS | kOsLacros,
     FEATURE_VALUE_TYPE(media::kChromeOSHWVBREncoding)},
#if defined(ARCH_CPU_ARM_FAMILY)
    {"prefer-gl-image-processor",
     flag_descriptions::kPreferGLImageProcessorName,
     flag_descriptions::kPreferGLImageProcessorDescription, kOsCrOS | kOsLacros,
     FEATURE_VALUE_TYPE(media::kPreferGLImageProcessor)},
    {"prefer-software-mt21", flag_descriptions::kPreferSoftwareMT21Name,
     flag_descriptions::kPreferSoftwareMT21Description, kOsCrOS | kOsLacros,
     FEATURE_VALUE_TYPE(media::kPreferSoftwareMT21)},
#endif  // defined(ARCH_CPU_ARM_FAMILY)
#endif  // BUILDFLAG(IS_CHROMEOS) && BUILDFLAG(USE_CHROMEOS_MEDIA_ACCELERATION)

#if BUILDFLAG(IS_ANDROID)
    {"force-startup-signin-promo",
     flag_descriptions::kForceStartupSigninPromoName,
     flag_descriptions::kForceStartupSigninPromoDescription, kOsAndroid,
     FEATURE_VALUE_TYPE(switches::kForceStartupSigninPromo)},

    {"tangible-sync", flag_descriptions::kTangibleSyncName,
     flag_descriptions::kTangibleSyncDescription, kOsAndroid,
     FEATURE_WITH_PARAMS_VALUE_TYPE(switches::kTangibleSync,
                                    kTangibleSyncVariations,
                                    "TangibleSyncVariations")},
#endif  // BUILDFLAG(IS_ANDROID)

    {"gainmap-hdr-images", flag_descriptions::kGainmapHdrImagesName,
     flag_descriptions::kGainmapHdrImagesDescription, kOsAll,
     FEATURE_VALUE_TYPE(blink::features::kGainmapHdrImages)},

    {"avif-gainmap-hdr-images", flag_descriptions::kAvifGainmapHdrImagesName,
     flag_descriptions::kAvifGainmapHdrImagesDescription, kOsAll,
     FEATURE_VALUE_TYPE(blink::features::kAvifGainmapHdrImages)},

    {"autofill-use-improved-label-disambiguation",
     flag_descriptions::kAutofillUseImprovedLabelDisambiguationName,
     flag_descriptions::kAutofillUseImprovedLabelDisambiguationDescription,
     kOsAll,
     FEATURE_VALUE_TYPE(
         autofill::features::kAutofillUseImprovedLabelDisambiguation)},

    {"file-handling-icons", flag_descriptions::kFileHandlingIconsName,
     flag_descriptions::kFileHandlingIconsDescription, kOsDesktop,
     FEATURE_VALUE_TYPE(blink::features::kFileHandlingIcons)},

    {"file-system-access-locking-scheme",
     flag_descriptions::kFileSystemAccessLockingSchemeName,
     flag_descriptions::kFileSystemAccessLockingSchemeDescription, kOsAll,
     FEATURE_VALUE_TYPE(blink::features::kFileSystemAccessLockingScheme)},

    {"file-system-access-persistent-permission",
     flag_descriptions::kFileSystemAccessPersistentPermissionName,
     flag_descriptions::kFileSystemAccessPersistentPermissionDescription,
     kOsDesktop,
     FEATURE_VALUE_TYPE(features::kFileSystemAccessPersistentPermissions)},

    {"file-system-observer", flag_descriptions::kFileSystemObserverName,
     flag_descriptions::kFileSystemObserverDescription, kOsAll,
     FEATURE_VALUE_TYPE(blink::features::kFileSystemObserver)},

    {"strict-origin-isolation", flag_descriptions::kStrictOriginIsolationName,
     flag_descriptions::kStrictOriginIsolationDescription, kOsAll,
     FEATURE_VALUE_TYPE(features::kStrictOriginIsolation)},

#if BUILDFLAG(IS_ANDROID)
    {"enable-logging-js-console-messages",
     flag_descriptions::kLogJsConsoleMessagesName,
     flag_descriptions::kLogJsConsoleMessagesDescription, kOsAndroid,
     FEATURE_VALUE_TYPE(features::kLogJsConsoleMessages)},
#endif  // BUILDFLAG(IS_ANDROID)

#if BUILDFLAG(IS_CHROMEOS_ASH)
    {"allow-cross-device-feature-suite",
     flag_descriptions::kAllowCrossDeviceFeatureSuiteName,
     flag_descriptions::kAllowCrossDeviceFeatureSuiteDescription, kOsCrOS,
     FEATURE_VALUE_TYPE(ash::features::kAllowCrossDeviceFeatureSuite)},

    {"link-cross-device-internals",
     flag_descriptions::kLinkCrossDeviceInternalsName,
     flag_descriptions::kLinkCrossDeviceInternalsDescription, kOsCrOS,
     FEATURE_VALUE_TYPE(ash::features::kLinkCrossDeviceInternals)},

    {"allow-repeated-updates", flag_descriptions::kAllowRepeatedUpdatesName,
     flag_descriptions::kAllowRepeatedUpdatesDescription, kOsCrOS,
     FEATURE_VALUE_TYPE(ash::features::kAllowRepeatedUpdates)},

    {"allow-scroll-settings", flag_descriptions::kAllowScrollSettingsName,
     flag_descriptions::kAllowScrollSettingsDescription, kOsCrOS,
     FEATURE_VALUE_TYPE(ash::features::kAllowScrollSettings)},

    {"enable-fast-ink-for-software-cursor",
     flag_descriptions::kEnableFastInkForSoftwareCursorName,
     flag_descriptions::kEnableFastInkForSoftwareCursorDescription, kOsCrOS,
     FEATURE_VALUE_TYPE(ash::features::kEnableFastInkForSoftwareCursor)},

    {"enable-neural-palm-adaptive-hold",
     flag_descriptions::kEnableNeuralPalmAdaptiveHoldName,
     flag_descriptions::kEnableNeuralPalmAdaptiveHoldDescription, kOsCrOS,
     FEATURE_VALUE_TYPE(ui::kEnableNeuralPalmAdaptiveHold)},

    {"enable-neural-stylus-palm-rejection",
     flag_descriptions::kEnableNeuralStylusPalmRejectionName,
     flag_descriptions::kEnableNeuralStylusPalmRejectionDescription, kOsCrOS,
     FEATURE_VALUE_TYPE(ui::kEnableNeuralPalmDetectionFilter)},

    {"enable-edge-detection", flag_descriptions::kEnableEdgeDetectionName,
     flag_descriptions::kEnableEdgeDetectionDescription, kOsCrOS,
     FEATURE_VALUE_TYPE(ui::kEnableEdgeDetection)},

    {"fast-pair", flag_descriptions::kFastPairName,
     flag_descriptions::kFastPairDescription, kOsCrOS,
     FEATURE_VALUE_TYPE(ash::features::kFastPair)},

    {"fast-pair-devices-bluetooth-settings",
     flag_descriptions::kFastPairDevicesBluetoothSettingsName,
     flag_descriptions::kFastPairDevicesBluetoothSettingsDescription, kOsCrOS,
     FEATURE_VALUE_TYPE(ash::features::kFastPairDevicesBluetoothSettings)},

    {"fast-pair-handshake-long-term-refactor",
     flag_descriptions::kFastPairHandshakeLongTermRefactorName,
     flag_descriptions::kFastPairHandshakeLongTermRefactorDescription, kOsCrOS,
     FEATURE_VALUE_TYPE(ash::features::kFastPairHandshakeLongTermRefactor)},

    {"fast-pair-hid", flag_descriptions::kFastPairHIDName,
     flag_descriptions::kFastPairHIDDescription, kOsCrOS,
     FEATURE_VALUE_TYPE(ash::features::kFastPairHID)},

    {"fast-pair-low-power", flag_descriptions::kFastPairLowPowerName,
     flag_descriptions::kFastPairLowPowerDescription, kOsCrOS,
     FEATURE_VALUE_TYPE(ash::features::kFastPairLowPower)},

    {"fast-pair-pwa-companion", flag_descriptions::kFastPairPwaCompanionName,
     flag_descriptions::kFastPairPwaCompanionDescription, kOsCrOS,
     FEATURE_VALUE_TYPE(ash::features::kFastPairPwaCompanion)},

    {"fast-pair-saved-devices", flag_descriptions::kFastPairSavedDevicesName,
     flag_descriptions::kFastPairSavedDevicesDescription, kOsCrOS,
     FEATURE_VALUE_TYPE(ash::features::kFastPairSavedDevices)},

    {"fast-pair-software-scanning",
     flag_descriptions::kFastPairSoftwareScanningName,
     flag_descriptions::kFastPairSoftwareScanningDescription, kOsCrOS,
     FEATURE_VALUE_TYPE(ash::features::kFastPairSoftwareScanning)},

    {"nearby-presence", flag_descriptions::kNearbyPresenceName,
     flag_descriptions::kNearbyPresenceDescription, kOsCrOS,
     FEATURE_VALUE_TYPE(ash::features::kNearbyPresence)},

    {"pcie-billboard-notification",
     flag_descriptions::kPcieBillboardNotificationName,
     flag_descriptions::kPcieBillboardNotificationDescription, kOsCrOS,
     FEATURE_VALUE_TYPE(ash::features::kPcieBillboardNotification)},

    {"use-search-click-for-right-click",
     flag_descriptions::kUseSearchClickForRightClickName,
     flag_descriptions::kUseSearchClickForRightClickDescription, kOsCrOS,
     FEATURE_VALUE_TYPE(ash::features::kUseSearchClickForRightClick)},

    {"show-metered-toggle", flag_descriptions::kMeteredShowToggleName,
     flag_descriptions::kMeteredShowToggleDescription, kOsCrOS,
     FEATURE_VALUE_TYPE(features::kMeteredShowToggle)},

    {"display-alignment-assistance",
     flag_descriptions::kDisplayAlignmentAssistanceName,
     flag_descriptions::kDisplayAlignmentAssistanceDescription, kOsCrOS,
     FEATURE_VALUE_TYPE(ash::features::kDisplayAlignAssist)},

    {"dropdown-panel", flag_descriptions::kDropdownPanel,
     flag_descriptions::kDropdownPanelDescription, kOsCrOS,
     FEATURE_VALUE_TYPE(ash::features::kDropdownPanel)},

    {"enable-experimental-rgb-keyboard-patterns",
     flag_descriptions::kExperimentalRgbKeyboardPatternsName,
     flag_descriptions::kExperimentalRgbKeyboardPatternsDescription, kOsCrOS,
     FEATURE_VALUE_TYPE(ash::features::kExperimentalRgbKeyboardPatterns)},

    {"enable-get-debugd-logs-in-parallel",
     flag_descriptions::kEnableGetDebugdLogsInParallelName,
     flag_descriptions::kEnableGetDebugdLogsInParallelDescription, kOsCrOS,
     FEATURE_VALUE_TYPE(ash::features::kEnableGetDebugdLogsInParallel)},

    {"enable-hostname-setting", flag_descriptions::kEnableHostnameSettingName,
     flag_descriptions::kEnableHostnameSettingDescription, kOsCrOS,
     FEATURE_VALUE_TYPE(ash::features::kEnableHostnameSetting)},

    {"enable-oauth-ipp", flag_descriptions::kEnableOAuthIppName,
     flag_descriptions::kEnableOAuthIppDescription, kOsCrOS,
     FEATURE_VALUE_TYPE(ash::features::kEnableOAuthIpp)},

    {"enable-shortcut-customization-app",
     flag_descriptions::kEnableShortcutCustomizationAppName,
     flag_descriptions::kEnableShortcutCustomizationAppDescription, kOsCrOS,
     FEATURE_VALUE_TYPE(features::kShortcutCustomizationApp)},

    {"enable-shortcut-customization",
     flag_descriptions::kEnableShortcutCustomizationName,
     flag_descriptions::kEnableShortcutCustomizationDescription, kOsCrOS,
     FEATURE_VALUE_TYPE(features::kShortcutCustomization)},

    {"enable-search-customizable-shortcuts-in-launcher",
     flag_descriptions::kEnableSearchCustomizableShortcutsInLauncherName,
     flag_descriptions::kEnableSearchCustomizableShortcutsInLauncherDescription,
     kOsCrOS,
     FEATURE_VALUE_TYPE(ash::features::kSearchCustomizableShortcutsInLauncher)},

    {"enable-input-device-settings-logging",
     flag_descriptions::kEnableInputDeviceSettingsLoggingName,
     flag_descriptions::kEnableInputDeviceSettingsLoggingDescription, kOsCrOS,
     FEATURE_VALUE_TYPE(ash::features::kEnableInputDeviceSettingsLogging)},

    {"enable-input-device-settings-split",
     flag_descriptions::kEnableInputDeviceSettingsSplitName,
     flag_descriptions::kEnableInputDeviceSettingsSplitDescription, kOsCrOS,
     FEATURE_VALUE_TYPE(ash::features::kInputDeviceSettingsSplit)},

    {"enable-peripheral-customization",
     flag_descriptions::kEnablePeripheralCustomizationName,
     flag_descriptions::kEnablePeripheralCustomizationDescription, kOsCrOS,
     FEATURE_VALUE_TYPE(ash::features::kPeripheralCustomization)},

    {"enable-peripheral-notification",
     flag_descriptions::kEnablePeripheralNotificationName,
     flag_descriptions::kEnablePeripheralNotificationDescription, kOsCrOS,
     FEATURE_VALUE_TYPE(ash::features::kPeripheralNotification)},

    {"enable-accessibility-accelerators-notifications-timeout",
     flag_descriptions::kAccessibilityAcceleratorNotificationsTimeoutName,
     flag_descriptions::
         kAccessibilityAcceleratorNotificationsTimeoutDescription,
     kOsCrOS,
     FEATURE_VALUE_TYPE(
         features::kAccessibilityAcceleratorNotificationsTimeout)},

    {"enable-accessibility-service",
     flag_descriptions::kAccessibilityServiceName,
     flag_descriptions::kAccessibilityServiceDescription, kOsCrOS,
     FEATURE_VALUE_TYPE(features::kAccessibilityService)},

    {"enable-accessibility-select-to-speak-hover-text-improvements",
     flag_descriptions::kAccessibilitySelectToSpeakHoverTextImprovementsName,
     flag_descriptions::
         kAccessibilitySelectToSpeakHoverTextImprovementsDescription,
     kOsCrOS,
     FEATURE_VALUE_TYPE(
         features::kAccessibilitySelectToSpeakHoverTextImprovements)},

    {"enable-accessibility-dictation-keyboard-improvements",
     flag_descriptions::kAccessibilityDictationKeyboardImprovementsName,
     flag_descriptions::kAccessibilityDictationKeyboardImprovementsDescription,
     kOsCrOS,
     FEATURE_VALUE_TYPE(features::kAccessibilityDictationKeyboardImprovements)},

    {"enable-accessibility-extra-large-cursor",
     flag_descriptions::kAccessibilityExtraLargeCursorName,
     flag_descriptions::kAccessibilityExtraLargeCursorDescription, kOsCrOS,
     FEATURE_VALUE_TYPE(features::kAccessibilityExtraLargeCursor)},

    {"enable-accessibility-facegaze",
     flag_descriptions::kAccessibilityFaceGazeName,
     flag_descriptions::kAccessibilityFaceGazeDescription, kOsCrOS,
     FEATURE_VALUE_TYPE(features::kAccessibilityFaceGaze)},

    {"enable-only-show-new-shortcut-app",
     flag_descriptions::kOnlyShowNewShortcutsAppName,
     flag_descriptions::kOnlyShowNewShortcutsAppDescription, kOsCrOS,
     FEATURE_VALUE_TYPE(ash::features::kOnlyShowNewShortcutsApp)},

    {"enable-pip-double-tap-to-resize",
     flag_descriptions::kPipDoubleTapToResizeName,
     flag_descriptions::kPipDoubleTapToResizeDescription, kOsCrOS,
     FEATURE_VALUE_TYPE(ash::features::kPipDoubleTapToResize)},

    {"enable-pip-pinch-to-resize", flag_descriptions::kPipPinchToResizeName,
     flag_descriptions::kPipPinchToResizeDescription, kOsCrOS,
     FEATURE_VALUE_TYPE(ash::features::kPipPinchToResize)},

    {"enable-pip-tilt", flag_descriptions::kPipTiltName,
     flag_descriptions::kPipTiltDescription, kOsCrOS,
     FEATURE_VALUE_TYPE(ash::features::kPipTilt)},

    {"enable-pip-tuck", flag_descriptions::kPipTuckName,
     flag_descriptions::kPipTuckDescription, kOsCrOS,
     FEATURE_VALUE_TYPE(ash::features::kPipTuck)},

#endif  // BUILDFLAG(IS_CHROMEOS_ASH)

    {"enable-fenced-frames", flag_descriptions::kEnableFencedFramesName,
     flag_descriptions::kEnableFencedFramesDescription, kOsAll,
     FEATURE_VALUE_TYPE(blink::features::kFencedFrames)},

    {"enable-fenced-frames-cross-origin-automatic-beacons",
     flag_descriptions::kEnableFencedFramesCrossOriginAutomaticBeaconsName,
     flag_descriptions::
         kEnableFencedFramesCrossOriginAutomaticBeaconsDescription,
     kOsAll,
     FEATURE_VALUE_TYPE(
         blink::features::kFencedFramesCrossOriginAutomaticBeacons)},

    {"enable-fenced-frames-developer-mode",
     flag_descriptions::kEnableFencedFramesDeveloperModeName,
     flag_descriptions::kEnableFencedFramesDeveloperModeDescription, kOsAll,
     FEATURE_VALUE_TYPE(blink::features::kFencedFramesDefaultMode)},

    {"enable-fenced-frames-reporting-attestations-changes",
     flag_descriptions::kEnableFencedFramesReportingAttestationsChangeName,
     flag_descriptions::
         kEnableFencedFramesReportingAttestationsChangeDescription,
     kOsAll,
     FEATURE_VALUE_TYPE(
         blink::features::kFencedFramesReportingAttestationsChanges)},

    {"storage-access-api", flag_descriptions::kStorageAccessAPIName,
     flag_descriptions::kStorageAccessAPIDescription, kOsAll,
     FEATURE_WITH_PARAMS_VALUE_TYPE(blink::features::kStorageAccessAPI,
                                    kStorageAccessAPIVariations,
                                    "kStorageAccessAPI")},

    {"enable-unsafe-webgpu", flag_descriptions::kUnsafeWebGPUName,
     flag_descriptions::kUnsafeWebGPUDescription, kOsAll,
     SINGLE_VALUE_TYPE(switches::kEnableUnsafeWebGPU)},

    {"enable-webgpu-developer-features",
     flag_descriptions::kWebGpuDeveloperFeaturesName,
     flag_descriptions::kWebGpuDeveloperFeaturesDescription, kOsAll,
     SINGLE_VALUE_TYPE(switches::kEnableWebGPUDeveloperFeatures)},

#if BUILDFLAG(IS_ANDROID)
    {"autofill-use-mobile-label-disambiguation",
     flag_descriptions::kAutofillUseMobileLabelDisambiguationName,
     flag_descriptions::kAutofillUseMobileLabelDisambiguationDescription,
     kOsAndroid,
     FEATURE_WITH_PARAMS_VALUE_TYPE(
         autofill::features::kAutofillUseMobileLabelDisambiguation,
         kAutofillUseMobileLabelDisambiguationVariations,
         "AutofillUseMobileLabelDisambiguation")},
#endif  // BUILDFLAG(IS_ANDROID)

#if BUILDFLAG(IS_CHROMEOS_ASH)
    {"gesture-properties-dbus-service",
     flag_descriptions::kEnableGesturePropertiesDBusServiceName,
     flag_descriptions::kEnableGesturePropertiesDBusServiceDescription, kOsCrOS,
     FEATURE_VALUE_TYPE(ash::features::kGesturePropertiesDBusService)},
#endif  // BUILDFLAG(IS_CHROMEOS_ASH)

    {"ev-details-in-page-info", flag_descriptions::kEvDetailsInPageInfoName,
     flag_descriptions::kEvDetailsInPageInfoDescription, kOsDesktop,
     FEATURE_VALUE_TYPE(features::kEvDetailsInPageInfo)},

#if BUILDFLAG(IS_CHROMEOS)
    {"global-media-controls-cros-updated-ui",
     flag_descriptions::kGlobalMediaControlsCrOSUpdatedUIName,
     flag_descriptions::kGlobalMediaControlsCrOSUpdatedUIDescription, kOsCrOS,
     FEATURE_VALUE_TYPE(media::kGlobalMediaControlsCrOSUpdatedUI)},
#endif  // BUILDFLAG(IS_CHROMEOS)

    {"enable-cooperative-scheduling",
     flag_descriptions::kCooperativeSchedulingName,
     flag_descriptions::kCooperativeSchedulingDescription, kOsAll,
     FEATURE_VALUE_TYPE(features::kCooperativeScheduling)},

#if BUILDFLAG(IS_CHROMEOS_ASH)
    {"enable-privacy-indicators", flag_descriptions::kPrivacyIndicatorsName,
     flag_descriptions::kPrivacyIndicatorsDescription, kOsCrOS,
     FEATURE_VALUE_TYPE(ash::features::kPrivacyIndicators)},
#endif  // BUILDFLAG(IS_CHROMEOS_ASH)

#if BUILDFLAG(ENABLE_SUPERVISED_USERS)
    {"enable-proto-api-for-classify-url",
     flag_descriptions::kEnableProtoApiForClassifyUrlName,
     flag_descriptions::kEnableProtoApiForClassifyUrlDescription,
     kOsAndroid | (kOsDesktop & ~kOsFuchsia),
     FEATURE_VALUE_TYPE(supervised_user::kEnableProtoApiForClassifyUrl)},
#endif  // BUILDFLAG(ENABLE_SUPERVISED_USERS)

#if BUILDFLAG(IS_LINUX) || BUILDFLAG(IS_CHROMEOS)
    {"enable-network-service-sandbox",
     flag_descriptions::kEnableNetworkServiceSandboxName,
     flag_descriptions::kEnableNetworkServiceSandboxDescription,
     kOsLinux | kOsLacros | kOsCrOS,
     FEATURE_VALUE_TYPE(sandbox::policy::features::kNetworkServiceSandbox)},
#endif  // BUILDFLAG(IS_LINUX) || BUILDFLAG(IS_CHROMEOS)

#if BUILDFLAG(ALLOW_OOP_VIDEO_DECODER)
    {"use-out-of-process-video-decoding",
     flag_descriptions::kUseOutOfProcessVideoDecodingName,
     flag_descriptions::kUseOutOfProcessVideoDecodingDescription,
     kOsLinux | kOsCrOS | kOsLacros,
     FEATURE_VALUE_TYPE(media::kUseOutOfProcessVideoDecoding)},
#endif  // BUILDFLAG(ALLOW_OOP_VIDEO_DECODER)

#if BUILDFLAG(ENABLE_SUPERVISED_USERS)

    {"enable-family-link-website-filtering",
     flag_descriptions::kFilterWebsitesForSupervisedUsersOnDesktopName,
     flag_descriptions::kFilterWebsitesForSupervisedUsersOnDesktopDescription,
     kOsLinux | kOsMac | kOsWin,
     FEATURE_VALUE_TYPE(
         supervised_user::kFilterWebsitesForSupervisedUsersOnDesktopAndIOS)},

#if BUILDFLAG(IS_MAC) || BUILDFLAG(IS_LINUX) || BUILDFLAG(IS_WIN)
    {"enable-family-link-extensions-permissions",
     flag_descriptions::
         kEnableExtensionsPermissionsForSupervisedUsersOnDesktopName,
     flag_descriptions::
         kEnableExtensionsPermissionsForSupervisedUsersOnDesktopDescription,
     kOsLinux | kOsMac | kOsWin,
     FEATURE_VALUE_TYPE(
         supervised_user::
             kEnableExtensionsPermissionsForSupervisedUsersOnDesktop)},
#endif

    {"enable-family-link-prefs-controls",
     flag_descriptions::kSupervisedPrefsControlledBySupervisedStoreName,
     flag_descriptions::kSupervisedPrefsControlledBySupervisedStoreDescription,
     kOsLinux | kOsMac | kOsWin,
     FEATURE_VALUE_TYPE(
         supervised_user::kSupervisedPrefsControlledBySupervisedStore)},

    {"enable-family-link-managed-ui",
     flag_descriptions::kEnableManagedByParentUiName,
     flag_descriptions::kEnableManagedByParentUiDescription,
     kOsLinux | kOsMac | kOsWin,
     FEATURE_VALUE_TYPE(supervised_user::kEnableManagedByParentUi)},

    {"enable-supervised-users-remain-signed-in",
     flag_descriptions::kClearingCookiesKeepsSupervisedUsersSignedInName,
     flag_descriptions::kClearingCookiesKeepsSupervisedUsersSignedInDescription,
     kOsLinux | kOsMac | kOsWin,
     FEATURE_VALUE_TYPE(
         supervised_user::kClearingCookiesKeepsSupervisedUsersSignedIn)},
#endif  // BUILDFLAG(ENABLE_SUPERVISED_USERS)

    {"notification-scheduler", flag_descriptions::kNotificationSchedulerName,
     flag_descriptions::kNotificationSchedulerDescription, kOsAndroid,
     FEATURE_VALUE_TYPE(notifications::features::kNotificationScheduleService)},

    {"notification-scheduler-debug-options",
     flag_descriptions::kNotificationSchedulerDebugOptionName,
     flag_descriptions::kNotificationSchedulerDebugOptionDescription,
     kOsAndroid, MULTI_VALUE_TYPE(kNotificationSchedulerChoices)},

#if BUILDFLAG(IS_ANDROID)

    {"debug-chime-notification",
     flag_descriptions::kChimeAlwaysShowNotificationName,
     flag_descriptions::kChimeAlwaysShowNotificationDescription, kOsAndroid,
     SINGLE_VALUE_TYPE(notifications::switches::kDebugChimeNotification)},

    {"use-chime-android-sdk", flag_descriptions::kChimeAndroidSdkName,
     flag_descriptions::kChimeAndroidSdkDescription, kOsAndroid,
     FEATURE_VALUE_TYPE(notifications::features::kUseChimeAndroidSdk)},

#endif  // BUILDFLAG(IS_ANDROID)

    {"use-client-gmb-interface", flag_descriptions::kUseClientGmbInterfaceName,
     flag_descriptions::kUseClientGmbInterfaceDescription, kOsAll,
     FEATURE_VALUE_TYPE(features::kUseClientGmbInterface)},

#if BUILDFLAG(IS_CHROMEOS_ASH)
    {"contextual-nudges", flag_descriptions::kContextualNudgesName,
     flag_descriptions::kContextualNudgesDescription, kOsCrOS,
     FEATURE_VALUE_TYPE(ash::features::kContextualNudges)},

    {"scalable-iph-debug", flag_descriptions::kScalableIphDebugName,
     flag_descriptions::kScalableIphDebugDescription, kOsCrOS,
     FEATURE_VALUE_TYPE(ash::features::kScalableIphDebug)},
#endif  // BUILDFLAG(IS_CHROMEOS_ASH)

#if BUILDFLAG(IS_CHROMEOS_ASH)
    {"settings-app-notification-settings",
     flag_descriptions::kSettingsAppNotificationSettingsName,
     flag_descriptions::kSettingsAppNotificationSettingsDescription, kOsCrOS,
     FEATURE_VALUE_TYPE(ash::features::kSettingsAppNotificationSettings)},
#endif  // BUILDFLAG(IS_CHROMEOS_ASH)

    {"use-sha1-server-handshakes", flag_descriptions::kSHA1ServerSignatureName,
     flag_descriptions::kSHA1ServerSignatureDescription, kOsAll,
     FEATURE_VALUE_TYPE(net::features::kSHA1ServerSignature)},

    {"encrypted-client-hello", flag_descriptions::kEncryptedClientHelloName,
     flag_descriptions::kEncryptedClientHelloDescription, kOsAll,
     FEATURE_VALUE_TYPE(net::features::kEncryptedClientHello)},

    {"use-dns-https-svcb-alpn", flag_descriptions::kUseDnsHttpsSvcbAlpnName,
     flag_descriptions::kUseDnsHttpsSvcbAlpnDescription,
     kOsLinux | kOsMac | kOsWin | kOsCrOS | kOsAndroid,
     FEATURE_VALUE_TYPE(net::features::kUseDnsHttpsSvcbAlpn)},

#if !BUILDFLAG(IS_ANDROID)
    {"hidpi-capture", flag_descriptions::kWebContentsCaptureHiDpiName,
     flag_descriptions::kWebContentsCaptureHiDpiDescription, kOsDesktop,
     FEATURE_VALUE_TYPE(media::kWebContentsCaptureHiDpi)},
#endif  // !BUILDFLAG(IS_ANDROID)

#if BUILDFLAG(IS_ANDROID)
    {"web-otp-backend", flag_descriptions::kWebOtpBackendName,
     flag_descriptions::kWebOtpBackendDescription, kOsAndroid,
     MULTI_VALUE_TYPE(kWebOtpBackendChoices)},

    {"darken-websites-checkbox-in-themes-setting",
     flag_descriptions::kDarkenWebsitesCheckboxInThemesSettingName,
     flag_descriptions::kDarkenWebsitesCheckboxInThemesSettingDescription,
     kOsAndroid,
     FEATURE_VALUE_TYPE(
         content_settings::kDarkenWebsitesCheckboxInThemesSetting)},

    {"lens-camera-assisted-search",
     flag_descriptions::kLensCameraAssistedSearchName,
     flag_descriptions::kLensCameraAssistedSearchDescription, kOsAndroid,
     FEATURE_WITH_PARAMS_VALUE_TYPE(chrome::android::kLensCameraAssistedSearch,
                                    kLensCameraAssistedSearchVariations,
                                    "LensCameraAssistedSearch")},
#endif  // BUILDFLAG(IS_ANDROID)

    {"back-forward-cache", flag_descriptions::kBackForwardCacheName,
     flag_descriptions::kBackForwardCacheDescription, kOsAll,
     FEATURE_WITH_PARAMS_VALUE_TYPE(features::kBackForwardCache,
                                    kBackForwardCacheVariations,
                                    "BackForwardCache")},

    {"windows-scrolling-personality",
     flag_descriptions::kWindowsScrollingPersonalityName,
     flag_descriptions::kWindowsScrollingPersonalityDescription, kOsAll,
     FEATURE_VALUE_TYPE(features::kWindowsScrollingPersonality)},

#if BUILDFLAG(IS_WIN) || BUILDFLAG(IS_ANDROID)
    {"elastic-overscroll", flag_descriptions::kElasticOverscrollName,
     flag_descriptions::kElasticOverscrollDescription, kOsWin | kOsAndroid,
     FEATURE_VALUE_TYPE(features::kElasticOverscroll)},
#endif

#if !BUILDFLAG(IS_ANDROID)
    {"element-capture", flag_descriptions::kElementCaptureName,
     flag_descriptions::kElementCaptureDescription, kOsDesktop,
     FEATURE_VALUE_TYPE(blink::features::kElementCapture)},
#endif

    {"device-posture", flag_descriptions::kDevicePostureName,
     flag_descriptions::kDevicePostureDescription, kOsAll,
     FEATURE_VALUE_TYPE(features::kDevicePosture)},

    {"viewport-segments", flag_descriptions::kViewportSegmentsName,
     flag_descriptions::kViewportSegmentsDescription, kOsAll,
     FEATURE_VALUE_TYPE(features::kViewportSegments)},

#if BUILDFLAG(IS_CHROMEOS_ASH)
    {"device-force-scheduled-reboot",
     flag_descriptions::kDeviceForceScheduledRebootName,
     flag_descriptions::kDeviceForceScheduledRebootDescription, kOsCrOS,
     FEATURE_VALUE_TYPE(ash::features::kDeviceForceScheduledReboot)},
    {"enable-assistant-aec", flag_descriptions::kEnableGoogleAssistantAecName,
     flag_descriptions::kEnableGoogleAssistantAecDescription, kOsCrOS,
     FEATURE_VALUE_TYPE(ash::assistant::features::kAssistantAudioEraser)},
#endif

#if BUILDFLAG(IS_WIN)
    {"enable-winrt-geolocation-implementation",
     flag_descriptions::kWinrtGeolocationImplementationName,
     flag_descriptions::kWinrtGeolocationImplementationDescription, kOsWin,
     FEATURE_VALUE_TYPE(features::kWinrtGeolocationImplementation)},
#endif

#if BUILDFLAG(IS_MAC)
    {"enable-core-location-backend",
     flag_descriptions::kMacCoreLocationBackendName,
     flag_descriptions::kMacCoreLocationBackendDescription, kOsMac,
     FEATURE_VALUE_TYPE(features::kMacCoreLocationBackend)},
#endif

#if !BUILDFLAG(IS_ANDROID)
    {"mute-notification-snooze-action",
     flag_descriptions::kMuteNotificationSnoozeActionName,
     flag_descriptions::kMuteNotificationSnoozeActionDescription, kOsDesktop,
     FEATURE_VALUE_TYPE(features::kMuteNotificationSnoozeAction)},
#endif  // !BUILDFLAG(IS_ANDROID)

#if BUILDFLAG(IS_MAC)
    {"enable-new-mac-notification-api",
     flag_descriptions::kNewMacNotificationAPIName,
     flag_descriptions::kNewMacNotificationAPIDescription, kOsMac,
     FEATURE_VALUE_TYPE(features::kNewMacNotificationAPI)},
#endif

#if BUILDFLAG(IS_CHROMEOS_ASH)
    {"exo-consumed-by-ime-by-flag",
     flag_descriptions::kExoConsumedByImeByFlagName,
     flag_descriptions::kExoConsumedByImeByFlagDescription, kOsCrOS,
     FEATURE_VALUE_TYPE(ash::features::kExoConsumedByImeByFlag)},
    {"exo-gamepad-vibration", flag_descriptions::kExoGamepadVibrationName,
     flag_descriptions::kExoGamepadVibrationDescription, kOsCrOS,
     FEATURE_VALUE_TYPE(ash::features::kGamepadVibration)},
    {"exo-ordinal-motion", flag_descriptions::kExoOrdinalMotionName,
     flag_descriptions::kExoOrdinalMotionDescription, kOsCrOS,
     FEATURE_VALUE_TYPE(ash::features::kExoOrdinalMotion)},
    {"exo-surrounding-text-offset",
     flag_descriptions::kExoSurroundingTextOffsetName,
     flag_descriptions::kExoSurroundingTextOffsetDescription, kOsCrOS,
     FEATURE_VALUE_TYPE(ash::features::kExoSurroundingTextOffset)},
#endif  // BUILDFLAG(IS_CHROMEOS_ASH)

#if BUILDFLAG(IS_MAC)
    {"screentime", flag_descriptions::kScreenTimeName,
     flag_descriptions::kScreenTimeDescription, kOsMac,
     FEATURE_VALUE_TYPE(screentime::kScreenTime)},
#endif

    {"enable-heavy-ad-intervention",
     flag_descriptions::kHeavyAdInterventionName,
     flag_descriptions::kHeavyAdInterventionDescription, kOsAll,
     FEATURE_VALUE_TYPE(heavy_ad_intervention::features::kHeavyAdIntervention)},

    {"heavy-ad-privacy-mitigations",
     flag_descriptions::kHeavyAdPrivacyMitigationsName,
     flag_descriptions::kHeavyAdPrivacyMitigationsDescription, kOsAll,
     FEATURE_VALUE_TYPE(
         heavy_ad_intervention::features::kHeavyAdPrivacyMitigations)},

#if BUILDFLAG(IS_CHROMEOS_ASH)
    {"crostini-container-install",
     flag_descriptions::kCrostiniContainerInstallName,
     flag_descriptions::kCrostiniContainerInstallDescription, kOsCrOS,
     MULTI_VALUE_TYPE(kCrostiniContainerChoices)},
#endif  // BUILDFLAG(IS_CHROMEOS_ASH)

#if BUILDFLAG(IS_CHROMEOS_ASH)
    {"os-settings-app-notifications-page",
     flag_descriptions::kOsSettingsAppNotificationsPageName,
     flag_descriptions::kOsSettingsAppNotificationsPageDescription, kOsCrOS,
     FEATURE_VALUE_TYPE(ash::features::kOsSettingsAppNotificationsPage)},
    {"help-app-app-detail-page", flag_descriptions::kHelpAppAppDetailPageName,
     flag_descriptions::kHelpAppAppDetailPageDescription, kOsCrOS,
     FEATURE_VALUE_TYPE(ash::features::kHelpAppAppDetailPage)},
    {"help-app-apps-list", flag_descriptions::kHelpAppAppsListName,
     flag_descriptions::kHelpAppAppsListDescription, kOsCrOS,
     FEATURE_VALUE_TYPE(ash::features::kHelpAppAppsList)},
    {"help-app-auto-trigger-install-dialog",
     flag_descriptions::kHelpAppAutoTriggerInstallDialogName,
     flag_descriptions::kHelpAppAutoTriggerInstallDialogDescription, kOsCrOS,
     FEATURE_VALUE_TYPE(ash::features::kHelpAppAutoTriggerInstallDialog)},
    {"help-app-cros-components", flag_descriptions::kHelpAppCrosComponentsName,
     flag_descriptions::kHelpAppCrosComponentsDescription, kOsCrOS,
     FEATURE_VALUE_TYPE(ash::features::kHelpAppCrosComponents)},
    {"help-app-home-page-app-articles",
     flag_descriptions::kHelpAppHomePageAppArticlesName,
     flag_descriptions::kHelpAppHomePageAppArticlesDescription, kOsCrOS,
     FEATURE_VALUE_TYPE(ash::features::kHelpAppHomePageAppArticles)},
    {"help-app-launcher-search", flag_descriptions::kHelpAppLauncherSearchName,
     flag_descriptions::kHelpAppLauncherSearchDescription, kOsCrOS,
     FEATURE_VALUE_TYPE(ash::features::kHelpAppLauncherSearch)},
    {"media-app-pdf-a11y-ocr", flag_descriptions::kMediaAppPdfA11yOcrName,
     flag_descriptions::kMediaAppPdfA11yOcrDescription, kOsCrOS,
     FEATURE_VALUE_TYPE(ash::features::kMediaAppPdfA11yOcr)},
    {"release-notes-notification-all-channels",
     flag_descriptions::kReleaseNotesNotificationAllChannelsName,
     flag_descriptions::kReleaseNotesNotificationAllChannelsDescription,
     kOsCrOS,
     FEATURE_VALUE_TYPE(ash::features::kReleaseNotesNotificationAllChannels)},
    {"use-android-staging-smds", flag_descriptions::kUseAndroidStagingSmdsName,
     flag_descriptions::kUseAndroidStagingSmdsDescription, kOsCrOS,
     FEATURE_VALUE_TYPE(ash::features::kUseAndroidStagingSmds)},
    {"use-stork-smds-server-address",
     flag_descriptions::kUseStorkSmdsServerAddressName,
     flag_descriptions::kUseStorkSmdsServerAddressDescription, kOsCrOS,
     FEATURE_VALUE_TYPE(ash::features::kUseStorkSmdsServerAddress)},
    {"use-wallpaper-staging-url",
     flag_descriptions::kUseWallpaperStagingUrlName,
     flag_descriptions::kUseWallpaperStagingUrlDescription, kOsCrOS,
     FEATURE_VALUE_TYPE(ash::features::kUseWallpaperStagingUrl)},
#endif  // BUILDFLAG(IS_CHROMEOS_ASH)

#if BUILDFLAG(IS_ANDROID)
    {"autofill-enable-manual-fallback-for-virtual-cards",
     flag_descriptions::kAutofillEnableManualFallbackForVirtualCardsName,
     flag_descriptions::kAutofillEnableManualFallbackForVirtualCardsDescription,
     kOsAndroid,
     FEATURE_VALUE_TYPE(
         autofill::features::kAutofillEnableManualFallbackForVirtualCards)},
#endif  // BUILDFLAG(IS_ANDROID)

    {"autofill-enable-card-product-name",
     flag_descriptions::kAutofillEnableCardProductNameName,
     flag_descriptions::kAutofillEnableCardProductNameDescription, kOsAll,
     FEATURE_VALUE_TYPE(autofill::features::kAutofillEnableCardProductName)},

#if BUILDFLAG(ENABLE_PAINT_PREVIEW) && BUILDFLAG(IS_ANDROID)
    {"paint-preview-demo", flag_descriptions::kPaintPreviewDemoName,
     flag_descriptions::kPaintPreviewDemoDescription, kOsAndroid,
     FEATURE_VALUE_TYPE(paint_preview::kPaintPreviewDemo)},
#endif  // BUILDFLAG(ENABLE_PAINT_PREVIEW) && BUILDFLAG(IS_ANDROID)

#if BUILDFLAG(IS_CHROMEOS_ASH)
    {"panel-self-refresh-2", flag_descriptions::kPanelSelfRefresh2Name,
     flag_descriptions::kPanelSelfRefresh2Description, kOsCrOS,
     FEATURE_VALUE_TYPE(display::features::kPanelSelfRefresh2)},
#endif  // BUILDFLAG(IS_CHROMEOS_ASH)

    {"fullscreen-popup-windows", flag_descriptions::kFullscreenPopupWindowsName,
     flag_descriptions::kFullscreenPopupWindowsDescription,
     kOsDesktop | kOsAndroid,
     FEATURE_VALUE_TYPE(blink::features::kFullscreenPopupWindows)},

#if BUILDFLAG(IS_WIN)
    {"run-video-capture-service-in-browser",
     flag_descriptions::kRunVideoCaptureServiceInBrowserProcessName,
     flag_descriptions::kRunVideoCaptureServiceInBrowserProcessDescription,
     kOsWin,
     FEATURE_VALUE_TYPE(features::kRunVideoCaptureServiceInBrowserProcess)},
#endif  // BUILDFLAG(IS_WIN)
    {"disruptive-notification-permission-revocation",
     flag_descriptions::kDisruptiveNotificationPermissionRevocationName,
     flag_descriptions::kDisruptiveNotificationPermissionRevocationDescription,
     kOsAll,
     FEATURE_VALUE_TYPE(features::kDisruptiveNotificationPermissionRevocation)},
    {"double-buffer-compositing",
     flag_descriptions::kDoubleBufferCompositingName,
     flag_descriptions::kDoubleBufferCompositingDescription, kOsCrOS,
     SINGLE_VALUE_TYPE(switches::kDoubleBufferCompositing)},

#if !BUILDFLAG(IS_ANDROID)
    {"page-info-hide-site-settings",
     flag_descriptions::kPageInfoHideSiteSettingsName,
     flag_descriptions::kPageInfoHideSiteSettingsDescription, kOsDesktop,
     FEATURE_VALUE_TYPE(page_info::kPageInfoHideSiteSettings)},
#endif  // !BUILDFLAG(IS_ANDROID)

#if !BUILDFLAG(IS_ANDROID)
    {"page-info-history-desktop",
     flag_descriptions::kPageInfoHistoryDesktopName,
     flag_descriptions::kPageInfoHistoryDesktopDescription, kOsDesktop,
     FEATURE_VALUE_TYPE(page_info::kPageInfoHistoryDesktop)},
#endif  // !BUILDFLAG(IS_ANDROID)

#if !BUILDFLAG(IS_ANDROID)
    {"page-info-about-this-page-persistent-side-panel-entry",
     flag_descriptions::kPageInfoAboutThisPagePersistentEntryName,
     flag_descriptions::kPageInfoAboutThisPagePersistentEntryDescription,
     kOsDesktop,
     FEATURE_VALUE_TYPE(page_info::kAboutThisSitePersistentSidePanelEntry)},
#endif

    {"user-bypass-ui", flag_descriptions::kUserBypassUIName,
     flag_descriptions::kUserBypassUIDescription, kOsDesktop | kOsAndroid,
     FEATURE_VALUE_TYPE(content_settings::features::kUserBypassUI)},

    {"preloading-on-performance-page",
     flag_descriptions::kPreloadingOnPerformancePageName,
     flag_descriptions::kPreloadingOnPerformancePageDescription, kOsDesktop,
     FEATURE_WITH_PARAMS_VALUE_TYPE(
         features::kPerformanceSettingsPreloadingSubpage,
         kPerformanceSettingsPreloadingSubpageVariations,
         "PerformanceSettingsPreloadingSubpage")},

    {"tracking-protection-3pcd", flag_descriptions::kTrackingProtection3pcdName,
     flag_descriptions::kTrackingProtection3pcdDescription,
     kOsDesktop | kOsAndroid,
     FEATURE_VALUE_TYPE(content_settings::features::kTrackingProtection3pcd)},

    {"tracking-protection-onboarding-rollback-flow",
     flag_descriptions::kTrackingProtectionOnboardingRollbackName,
     flag_descriptions::kTrackingProtectionOnboardingRollbackDescription,
     kOsDesktop | kOsAndroid,
     FEATURE_VALUE_TYPE(
         privacy_sandbox::kTrackingProtectionOnboardingRollback)},

#if BUILDFLAG(IS_CHROMEOS_ASH)
    {kClipboardHistoryLongpressInternalName,
     flag_descriptions::kClipboardHistoryLongpressName,
     flag_descriptions::kClipboardHistoryLongpressDescription, kOsCrOS,
     FEATURE_VALUE_TYPE(ash::features::kClipboardHistoryLongpress)},
    {kClipboardHistoryRefreshInternalName,
     flag_descriptions::kClipboardHistoryRefreshName,
     flag_descriptions::kClipboardHistoryRefreshDescription, kOsCrOS,
     FEATURE_VALUE_TYPE(chromeos::features::kClipboardHistoryRefresh)},
    {kClipboardHistoryUrlTitlesInternalName,
     flag_descriptions::kClipboardHistoryUrlTitlesName,
     flag_descriptions::kClipboardHistoryUrlTitlesDescription, kOsCrOS,
     FEATURE_VALUE_TYPE(ash::features::kClipboardHistoryUrlTitles)},
    {"clipboard-history-web-contents-paste",
     flag_descriptions::kClipboardHistoryWebContentsPasteName,
     flag_descriptions::kClipboardHistoryWebContentsPasteDescription, kOsCrOS,
     FEATURE_VALUE_TYPE(ash::features::kClipboardHistoryWebContentsPaste)},
#endif  // BUILDFLAG(IS_CHROMEOS_ASH)

#if BUILDFLAG(IS_WIN)
    {"enable-media-foundation-video-capture",
     flag_descriptions::kEnableMediaFoundationVideoCaptureName,
     flag_descriptions::kEnableMediaFoundationVideoCaptureDescription, kOsWin,
     FEATURE_VALUE_TYPE(media::kMediaFoundationVideoCapture)},
#endif  // BUILDFLAG(IS_WIN)
    {"shared-highlighting-manager",
     flag_descriptions::kSharedHighlightingManagerName,
     flag_descriptions::kSharedHighlightingManagerDescription, kOsAll,
     FEATURE_VALUE_TYPE(shared_highlighting::kSharedHighlightingManager)},

#if BUILDFLAG(IS_CHROMEOS_ASH)
    {"reset-shortcut-customizations",
     flag_descriptions::kResetShortcutCustomizationsName,
     flag_descriptions::kResetShortcutCustomizationsDescription, kOsCrOS,
     FEATURE_VALUE_TYPE(ash::features::kResetShortcutCustomizations)},
    {"shimless-rma-os-update", flag_descriptions::kShimlessRMAOsUpdateName,
     flag_descriptions::kShimlessRMAOsUpdateDescription, kOsCrOS,
     FEATURE_VALUE_TYPE(ash::features::kShimlessRMAOsUpdate)},
    {"shimless-rma-compliance-check",
     flag_descriptions::kShimlessRMAComplianceCheckName,
     flag_descriptions::kShimlessRMAComplianceCheckDescription, kOsCrOS,
     FEATURE_VALUE_TYPE(ash::features::kShimlessRMAComplianceCheck)},
    {"shimless-rma-sku-description",
     flag_descriptions::kShimlessRMASkuDescriptionName,
     flag_descriptions::kShimlessRMASkuDescriptionDescription, kOsCrOS,
     FEATURE_VALUE_TYPE(ash::features::kShimlessRMASkuDescription)},
    {"shortcut-customization-jelly",
     flag_descriptions::kShortcutCustomizationJellyName,
     flag_descriptions::kShortcutCustomizationJellyDescription, kOsCrOS,
     FEATURE_VALUE_TYPE(ash::features::kShortcutCustomizationJelly)},
    {"nearby-sharing-self-share",
     flag_descriptions::kNearbySharingSelfShareName,
     flag_descriptions::kNearbySharingSelfShareDescription, kOsCrOS,
     FEATURE_VALUE_TYPE(features::kNearbySharingSelfShare)},
#endif  // BUILDFLAG(IS_CHROMEOS_ASH)

#if BUILDFLAG(IS_CHROMEOS_ASH)
    {"enable-palm-suppression", flag_descriptions::kEnablePalmSuppressionName,
     flag_descriptions::kEnablePalmSuppressionDescription, kOsCrOS,
     FEATURE_VALUE_TYPE(ui::kEnablePalmSuppression)},
#endif  // BUILDFLAG(IS_CHROMEOS_ASH)

    {"enable-experimental-cookie-features",
     flag_descriptions::kEnableExperimentalCookieFeaturesName,
     flag_descriptions::kEnableExperimentalCookieFeaturesDescription, kOsAll,
     MULTI_VALUE_TYPE(kEnableExperimentalCookieFeaturesChoices)},

    {"canvas-2d-layers", flag_descriptions::kCanvas2DLayersName,
     flag_descriptions::kCanvas2DLayersDescription, kOsAll,
     SINGLE_VALUE_TYPE(switches::kEnableCanvas2DLayers)},

    {"enable-machine-learning-model-loader-web-platform-api",
     flag_descriptions::kEnableMachineLearningModelLoaderWebPlatformApiName,
     flag_descriptions::
         kEnableMachineLearningModelLoaderWebPlatformApiDescription,
     kOsAll,
     FEATURE_VALUE_TYPE(
         features::kEnableMachineLearningModelLoaderWebPlatformApi)},

    {"one-time-permission", flag_descriptions::kOneTimePermissionName,
     flag_descriptions::kOneTimePermissionDescription, kOsDesktop,
     FEATURE_VALUE_TYPE(permissions::features::kOneTimePermission)},

    {"improved-semantics-activity-indicators",
     flag_descriptions::kImprovedSemanticsActivityIndicatorsName,
     flag_descriptions::kImprovedSemanticsActivityIndicatorsDescription,
     kOsDesktop,
     FEATURE_VALUE_TYPE(
         content_settings::features::kImprovedSemanticsActivityIndicators)},

    {"attribution-reporting-debug-mode",
     flag_descriptions::kAttributionReportingDebugModeName,
     flag_descriptions::kAttributionReportingDebugModeDescription, kOsAll,
     SINGLE_VALUE_TYPE(switches::kAttributionReportingDebugMode)},

    {"private-aggregation-developer-mode",
     flag_descriptions::kPrivateAggregationDeveloperModeName,
     flag_descriptions::kPrivateAggregationDeveloperModeDescription, kOsAll,
     SINGLE_VALUE_TYPE(switches::kPrivateAggregationDeveloperMode)},

#if BUILDFLAG(IS_CHROMEOS_ASH)
    {"productivity-launcher", flag_descriptions::kProductivityLauncherName,
     flag_descriptions::kProductivityLauncherDescription, kOsCrOS,
     FEATURE_WITH_PARAMS_VALUE_TYPE(ash::features::kProductivityLauncher,
                                    kProductivityLauncherVariations,
                                    "ProductivityLauncher")},
    {"launcher-item-suggest", flag_descriptions::kLauncherItemSuggestName,
     flag_descriptions::kLauncherItemSuggestDescription, kOsCrOS,
     FEATURE_WITH_PARAMS_VALUE_TYPE(ash::kLauncherItemSuggest,
                                    kLauncherItemSuggestVariations,
                                    "LauncherItemSuggest")},
    {"eol-incentive", flag_descriptions::kEolIncentiveName,
     flag_descriptions::kEolIncentiveDescription, kOsCrOS,
     FEATURE_WITH_PARAMS_VALUE_TYPE(ash::features::kEolIncentive,
                                    kEolIncentiveVariations,
                                    "EolIncentive")},
    {"productivity-launcher-image-search",
     flag_descriptions::kProductivityLauncherImageSearchName,
     flag_descriptions::kProductivityLauncherImageSearchDescription, kOsCrOS,
     FEATURE_VALUE_TYPE(ash::features::kProductivityLauncherImageSearch)},
    {"scanning-app-jelly", flag_descriptions::kScanningAppJellyName,
     flag_descriptions::kScanningAppJellyDescription, kOsCrOS,
     FEATURE_VALUE_TYPE(ash::features::kScanningAppJelly)},
    {"shelf-auto-hide-separation",
     flag_descriptions::kShelfAutoHideSeparationName,
     flag_descriptions::kShelfAutoHideSeparationDescription, kOsCrOS,
     FEATURE_VALUE_TYPE(ash::features::kShelfAutoHideSeparation)},
    {"shelf-stacked-hotseat", flag_descriptions::kShelfStackedHotseatName,
     flag_descriptions::kShelfStackedHotseatDescription, kOsCrOS,
     FEATURE_VALUE_TYPE(ash::features::kShelfStackedHotseat)},
    {"launcher-game-search", flag_descriptions::kLauncherGameSearchName,
     flag_descriptions::kLauncherGameSearchDescription, kOsCrOS,
     FEATURE_VALUE_TYPE(search_features::kLauncherGameSearch)},
    {"launcher-fuzzy-match-across-providers",
     flag_descriptions::kLauncherFuzzyMatchAcrossProvidersName,
     flag_descriptions::kLauncherFuzzyMatchAcrossProvidersDescription, kOsCrOS,
     FEATURE_VALUE_TYPE(search_features::kLauncherFuzzyMatchAcrossProviders)},
    {"launcher-keyword-extraction-scoring",
     flag_descriptions::kLauncherKeywordExtractionScoring,
     flag_descriptions::kLauncherKeywordExtractionScoringDescription, kOsCrOS,
     FEATURE_VALUE_TYPE(search_features::kLauncherKeywordExtractionScoring)},
    {"launcher-fuzzy-match-for-omnibox",
     flag_descriptions::kLauncherFuzzyMatchForOmniboxName,
     flag_descriptions::kLauncherFuzzyMatchForOmniboxDescription, kOsCrOS,
     FEATURE_VALUE_TYPE(search_features::kLauncherFuzzyMatchForOmnibox)},
    {"launcher-search-control", flag_descriptions::kLauncherSearchControlName,
     flag_descriptions::kLauncherSearchControlDescription, kOsCrOS,
     FEATURE_VALUE_TYPE(ash::features::kLauncherSearchControl)},
    {"launcher-nudge-session-reset",
     flag_descriptions::kLauncherNudgeSessionResetName,
     flag_descriptions::kLauncherNudgeSessionResetDescription, kOsCrOS,
     FEATURE_VALUE_TYPE(ash::features::kLauncherNudgeSessionReset)},
    {"launcher-system-info-answer-cards",
     flag_descriptions::kLauncherSystemInfoAnswerCardsName,
     flag_descriptions::kLauncherSystemInfoAnswerCardsDescription, kOsCrOS,
     FEATURE_VALUE_TYPE(search_features::kLauncherSystemInfoAnswerCards)},
    {"text-in-shelf", flag_descriptions::kTextInShelfName,
     flag_descriptions::kTextInShelfDescription, kOsCrOS,
     FEATURE_VALUE_TYPE(ash::features::kHomeButtonWithText)},
    {"launcher-local-image-search",
     flag_descriptions::kLauncherLocalImageSearchName,
     flag_descriptions::kLauncherLocalImageSearchDescription, kOsCrOS,
     FEATURE_VALUE_TYPE(search_features::kLauncherImageSearch)},
    {"launcher-local-image-search-ocr",
     flag_descriptions::kLauncherLocalImageSearchOcrName,
     flag_descriptions::kLauncherLocalImageSearchOcrDescription, kOsCrOS,
     FEATURE_VALUE_TYPE(search_features::kLauncherImageSearchOcr)},
    {"launcher-local-image-search-ica",
     flag_descriptions::kLauncherLocalImageSearchIcaName,
     flag_descriptions::kLauncherLocalImageSearchIcaDescription, kOsCrOS,
     FEATURE_VALUE_TYPE(search_features::kLauncherImageSearchIca)},
    {"quick-app-access-test-ui", flag_descriptions::kQuickAppAccessTestUIName,
     flag_descriptions::kQuickAppAccessTestUIDescription, kOsCrOS,
     FEATURE_VALUE_TYPE(ash::features::kQuickAppAccessTestUI)},

#endif  // BUILDFLAG(IS_CHROMEOS_ASH)

#if BUILDFLAG(IS_CHROMEOS_ASH)
    {"mac-address-randomization",
     flag_descriptions::kMacAddressRandomizationName,
     flag_descriptions::kMacAddressRandomizationDescription, kOsCrOS,
     FEATURE_VALUE_TYPE(ash::features::kMacAddressRandomization)},

    {"oobe-jelly", flag_descriptions::kOobeJellyName,
     flag_descriptions::kOobeJellyDescription, kOsCrOS,
     FEATURE_VALUE_TYPE(ash::features::kOobeJelly)},
    {"oobe-jelly-modal", flag_descriptions::kOobeJellyModalName,
     flag_descriptions::kOobeJellyModalDescription, kOsCrOS,
     FEATURE_VALUE_TYPE(ash::features::kOobeJellyModal)},

    {"smds-support", flag_descriptions::kSmdsSupportName,
     flag_descriptions::kSmdsSupportDescription, kOsCrOS,
     FEATURE_VALUE_TYPE(ash::features::kSmdsSupport)},

    {"smds-support-euicc-upload",
     flag_descriptions::kSmdsSupportEuiccUploadName,
     flag_descriptions::kSmdsSupportEuiccUploadDescription, kOsCrOS,
     FEATURE_VALUE_TYPE(ash::features::kSmdsSupportEuiccUpload)},

    {"tethering-experimental-functionality",
     flag_descriptions::kTetheringExperimentalFunctionalityName,
     flag_descriptions::kTetheringExperimentalFunctionalityDescription, kOsCrOS,
     FEATURE_VALUE_TYPE(ash::features::kTetheringExperimentalFunctionality)},

    {"dynamic-search-update-animation",
     flag_descriptions::kDynamicSearchUpdateAnimationName,
     flag_descriptions::kDynamicSearchUpdateAnimationDescription, kOsCrOS,
     FEATURE_WITH_PARAMS_VALUE_TYPE(
         app_list_features::kDynamicSearchUpdateAnimation,
         kDynamicSearchUpdateAnimationVariations,
         "LauncherDynamicAnimations")},
#endif  // BUILDFLAG(IS_CHROMEOS_ASH)

#if BUILDFLAG(IS_CHROMEOS_ASH)
    {"frame-throttle-fps", flag_descriptions::kFrameThrottleFpsName,
     flag_descriptions::kFrameThrottleFpsDescription, kOsCrOS,
     MULTI_VALUE_TYPE(kFrameThrottleFpsChoices)},
#endif  // BUILDFLAG(IS_CHROMEOS_ASH)

#if BUILDFLAG(IS_ANDROID)
    {"incognito-reauthentication-for-android",
     flag_descriptions::kIncognitoReauthenticationForAndroidName,
     flag_descriptions::kIncognitoReauthenticationForAndroidDescription,
     kOsAndroid,
     FEATURE_VALUE_TYPE(chrome::android::kIncognitoReauthenticationForAndroid)},
#endif

#if BUILDFLAG(IS_ANDROID)
    {"enable-surface-control", flag_descriptions::kAndroidSurfaceControlName,
     flag_descriptions::kAndroidSurfaceControlDescription, kOsAndroid,
     FEATURE_VALUE_TYPE(features::kAndroidSurfaceControl)},

    {"enable-image-reader", flag_descriptions::kAImageReaderName,
     flag_descriptions::kAImageReaderDescription, kOsAndroid,
     FEATURE_VALUE_TYPE(features::kAImageReader)},
#endif  // BUILDFLAG(IS_ANDROID)

#if BUILDFLAG(IS_ANDROID)
    {"smart-suggestion-for-large-downloads",
     flag_descriptions::kSmartSuggestionForLargeDownloadsName,
     flag_descriptions::kSmartSuggestionForLargeDownloadsDescription,
     kOsAndroid,
     FEATURE_VALUE_TYPE(download::features::kSmartSuggestionForLargeDownloads)},
#endif  // BUILDFLAG(IS_ANDROID)

#if BUILDFLAG(ENABLE_JXL_DECODER)
    {"enable-jxl", flag_descriptions::kEnableJXLName,
     flag_descriptions::kEnableJXLDescription, kOsAll,
     FEATURE_VALUE_TYPE(blink::features::kJXL)},
#endif  // BUILDFLAG(ENABLE_JXL_DECODER)

#if BUILDFLAG(IS_ANDROID)
    {"messages-for-android-ads-blocked",
     flag_descriptions::kMessagesForAndroidAdsBlockedName,
     flag_descriptions::kMessagesForAndroidAdsBlockedDescription, kOsAndroid,
     FEATURE_VALUE_TYPE(messages::kMessagesForAndroidAdsBlocked)},
    {"messages-for-android-infrastructure",
     flag_descriptions::kMessagesForAndroidInfrastructureName,
     flag_descriptions::kMessagesForAndroidInfrastructureDescription,
     kOsAndroid,
     FEATURE_VALUE_TYPE(messages::kMessagesForAndroidInfrastructure)},
    {"messages-for-android-offer-notification",
     flag_descriptions::kMessagesForAndroidOfferNotificationName,
     flag_descriptions::kMessagesForAndroidOfferNotificationDescription,
     kOsAndroid,
     FEATURE_VALUE_TYPE(messages::kMessagesForAndroidOfferNotification)},
    {"messages-for-android-permission-update",
     flag_descriptions::kMessagesForAndroidPermissionUpdateName,
     flag_descriptions::kMessagesForAndroidPermissionUpdateDescription,
     kOsAndroid,
     FEATURE_VALUE_TYPE(messages::kMessagesForAndroidPermissionUpdate)},
    {"messages-for-android-popup-blocked",
     flag_descriptions::kMessagesForAndroidPopupBlockedName,
     flag_descriptions::kMessagesForAndroidPopupBlockedDescription, kOsAndroid,
     FEATURE_VALUE_TYPE(messages::kMessagesForAndroidPopupBlocked)},
    {"messages-for-android-pwa-install",
     flag_descriptions::kMessagesForAndroidPWAInstallName,
     flag_descriptions::kMessagesForAndroidPWAInstallDescription, kOsAndroid,
     FEATURE_VALUE_TYPE(webapps::features::kInstallableAmbientBadgeMessage)},
    {"messages-for-android-save-card",
     flag_descriptions::kMessagesForAndroidSaveCardName,
     flag_descriptions::kMessagesForAndroidSaveCardDescription, kOsAndroid,
     FEATURE_VALUE_TYPE(messages::kMessagesForAndroidSaveCard)},
    {"messages-for-android-stacking-animation",
     flag_descriptions::kMessagesForAndroidStackingAnimationName,
     flag_descriptions::kMessagesForAndroidStackingAnimationDescription,
     kOsAndroid,
     FEATURE_VALUE_TYPE(messages::kMessagesForAndroidStackingAnimation)},
#endif

#if BUILDFLAG(IS_WIN) || BUILDFLAG(IS_MAC) || BUILDFLAG(IS_LINUX) || \
    BUILDFLAG(IS_FUCHSIA)
    {"quick-commands", flag_descriptions::kQuickCommandsName,
     flag_descriptions::kQuickCommandsDescription, kOsDesktop,
     FEATURE_VALUE_TYPE(features::kQuickCommands)},
#endif

#if BUILDFLAG(IS_ANDROID)
    {"quick-delete-for-android", flag_descriptions::kQuickDeleteForAndroidName,
     flag_descriptions::kQuickDeleteForAndroidDescription, kOsAndroid,
     FEATURE_VALUE_TYPE(chrome::android::kQuickDeleteForAndroid)},
#endif  // BUILDFLAG(IS_ANDROID)

    {"pwa-update-dialog-for-icon",
     flag_descriptions::kPwaUpdateDialogForAppIconName,
     flag_descriptions::kPwaUpdateDialogForAppIconDescription, kOsAll,
     FEATURE_VALUE_TYPE(features::kPwaUpdateDialogForIcon)},

    {"responsive-toolbar", flag_descriptions::kResponsiveToolbarName,
     flag_descriptions::kResponsiveToolbarDescription, kOsDesktop,
     FEATURE_VALUE_TYPE(features::kResponsiveToolbar)},

#if BUILDFLAG(ENABLE_OOP_PRINTING)
    {"enable-oop-print-drivers", flag_descriptions::kEnableOopPrintDriversName,
     flag_descriptions::kEnableOopPrintDriversDescription, kOsDesktop,
     FEATURE_VALUE_TYPE(printing::features::kEnableOopPrintDrivers)},
#endif

#if BUILDFLAG(IS_ANDROID)
    {"media-picker-adoption", flag_descriptions::kMediaPickerAdoptionStudyName,
     flag_descriptions::kMediaPickerAdoptionStudyDescription, kOsAndroid,
     FEATURE_WITH_PARAMS_VALUE_TYPE(
         photo_picker::features::kAndroidMediaPickerAdoption,
         kPhotoPickerAdoptionStudyFeatureVariations,
         "MediaPickerAdoption")},
#endif  // BUILDFLAG(IS_ANDROID)

    {"privacy-sandbox-ads-apis",
     flag_descriptions::kPrivacySandboxAdsAPIsOverrideName,
     flag_descriptions::kPrivacySandboxAdsAPIsOverrideDescription, kOsAll,
     SINGLE_VALUE_TYPE(switches::kEnablePrivacySandboxAdsApis)},

#if BUILDFLAG(IS_ANDROID)
    {"privacy-sandbox-ads-notice-cct",
     flag_descriptions::kPrivacySandboxAdsNoticeCCTName,
     flag_descriptions::kPrivacySandboxAdsNoticeCCTDescription, kOsAndroid,
     FEATURE_VALUE_TYPE(privacy_sandbox::kPrivacySandboxAdsNoticeCCT)},
#endif  // BUILDFLAG(IS_ANDROID)

    {"privacy-sandbox-proactive-topics-blocking",
     flag_descriptions::kPrivacySandboxProactiveTopicsBlockingName,
     flag_descriptions::kPrivacySandboxProactiveTopicsBlockingDescription,
     kOsAll,
     FEATURE_VALUE_TYPE(
         privacy_sandbox::kPrivacySandboxProactiveTopicsBlocking)},

#if BUILDFLAG(IS_ANDROID)
    {"site-data-improvements", flag_descriptions::kSiteDataImprovementsName,
     flag_descriptions::kSiteDataImprovementsDescription, kOsAndroid,
     FEATURE_VALUE_TYPE(browser_ui::kSiteDataImprovements)},
#endif  // BUILDFLAG(IS_ANDROID)

    {"animated-image-resume", flag_descriptions::kAnimatedImageResumeName,
     flag_descriptions::kAnimatedImageResumeDescription, kOsAll,
     FEATURE_VALUE_TYPE(features::kAnimatedImageResume)},

    {"enable-friendlier-safe-browsing-settings-enhanced-protection",
     flag_descriptions::
         kEnableFriendlierSafeBrowsingSettingsEnhancedProtectionName,
     flag_descriptions::
         kEnableFriendlierSafeBrowsingSettingsEnhancedProtectionDescription,
     kOsAll,
     FEATURE_VALUE_TYPE(
         safe_browsing::kFriendlierSafeBrowsingSettingsEnhancedProtection)},

    {"enable-friendlier-safe-browsing-settings-standard-protection",
     flag_descriptions::
         kEnableFriendlierSafeBrowsingSettingsStandardProtectionName,
     flag_descriptions::
         kEnableFriendlierSafeBrowsingSettingsStandardProtectionDescription,
     kOsAll,
     FEATURE_VALUE_TYPE(
         safe_browsing::kFriendlierSafeBrowsingSettingsStandardProtection)},

    {"enable-red-interstitial-facelift",
     flag_descriptions::kEnableRedInterstitialFaceliftName,
     flag_descriptions::kEnableRedInterstitialFaceliftDescription, kOsAll,
     FEATURE_VALUE_TYPE(safe_browsing::kRedInterstitialFacelift)},

    {"enable-suspicious-site-detection-rt-lookups",
     flag_descriptions::kEnableSuspiciousSiteDetectionRTLookupsName,
     flag_descriptions::kEnableSuspiciousSiteDetectionRTLookupsDescription,
     kOsDesktop | kOsAndroid,
     FEATURE_VALUE_TYPE(safe_browsing::kSuspiciousSiteDetectionRTLookups)},

    {"enable-tailored-security-retry-for-sync-users",
     flag_descriptions::kTailoredSecurityRetryForSyncUsersName,
     flag_descriptions::kTailoredSecurityRetryForSyncUsersDescription, kOsAll,
     FEATURE_VALUE_TYPE(safe_browsing::kTailoredSecurityRetryForSyncUsers)},

    {"enable-tailored-security-updated-messages",
     flag_descriptions::kEnableTailoredSecurityUpdatedMessagesName,
     flag_descriptions::kEnableTailoredSecurityUpdatedMessagesDescription,
     kOsAll,
     FEATURE_VALUE_TYPE(safe_browsing::kTailoredSecurityUpdatedMessages)},

#if !BUILDFLAG(IS_ANDROID)
    {"sct-auditing", flag_descriptions::kSCTAuditingName,
     flag_descriptions::kSCTAuditingDescription, kOsDesktop,
     FEATURE_WITH_PARAMS_VALUE_TYPE(features::kSCTAuditing,
                                    kSCTAuditingVariations,
                                    "SCTAuditingVariations")},
#endif  // !BUILDFLAG(IS_ANDROID)

#if BUILDFLAG(IS_ANDROID)
    {"incognito-screenshot", flag_descriptions::kIncognitoScreenshotName,
     flag_descriptions::kIncognitoScreenshotDescription, kOsAndroid,
     FEATURE_VALUE_TYPE(chrome::android::kIncognitoScreenshot)},

    {"incognito-downloads-warning",
     flag_descriptions::kIncognitoDownloadsWarningName,
     flag_descriptions::kIncognitoDownloadsWarningDescription, kOsAndroid,
     FEATURE_VALUE_TYPE(features::kIncognitoDownloadsWarning)},
#endif

    {"incognito-ntp-revamp", flag_descriptions::kIncognitoNtpRevampName,
     flag_descriptions::kIncognitoNtpRevampDescription, kOsAll,
     FEATURE_VALUE_TYPE(features::kIncognitoNtpRevamp)},

    {"insecure-download-warnings",
     flag_descriptions::kInsecureDownloadWarningsName,
     flag_descriptions::kInsecureDownloadWarningsDescription, kOsAll,
     FEATURE_VALUE_TYPE(features::kInsecureDownloadWarnings)},

    {"show-performance-metrics-hud",
     flag_descriptions::kShowPerformanceMetricsHudName,
     flag_descriptions::kShowPerformanceMetricsHudDescription, kOsAll,
     FEATURE_VALUE_TYPE(features::kHudDisplayForPerformanceMetrics)},

#if BUILDFLAG(IS_CHROMEOS_ASH)
    {"disable-buffer-bw-compression",
     flag_descriptions::kDisableBufferBWCompressionName,
     flag_descriptions::kDisableBufferBWCompressionDescription, kOsCrOS,
     SINGLE_VALUE_TYPE(switches::kDisableBufferBWCompression)},
#endif  // BUILDFLAG(IS_CHROMEOS_ASH)

    {"enable-speculation-rules-prerendering-target-hint",
     flag_descriptions::kSpeculationRulesPrerenderingTargetHintName,
     flag_descriptions::kSpeculationRulesPrerenderingTargetHintDescription,
     kOsAll, FEATURE_VALUE_TYPE(blink::features::kPrerender2InNewTab)},

    {"search-suggestion-for-prerender2",
     flag_descriptions::kSupportSearchSuggestionForPrerender2Name,
     flag_descriptions::kSupportSearchSuggestionForPrerender2Description,
     kOsAll,
     FEATURE_VALUE_TYPE(features::kSupportSearchSuggestionForPrerender2)},

    {"omnibox-search-prefetch",
     flag_descriptions::kEnableOmniboxSearchPrefetchName,
     flag_descriptions::kEnableOmniboxSearchPrefetchDescription, kOsAll,
     FEATURE_WITH_PARAMS_VALUE_TYPE(kSearchPrefetchServicePrefetching,
                                    kSearchPrefetchServicePrefetchingVariations,
                                    "SearchSuggestionPrefetch")},
    {"omnibox-search-client-prefetch",
     flag_descriptions::kEnableOmniboxClientSearchPrefetchName,
     flag_descriptions::kEnableOmniboxClientSearchPrefetchDescription, kOsAll,
     FEATURE_VALUE_TYPE(kSearchNavigationPrefetch)},

    {"chrome-labs", flag_descriptions::kChromeLabsName,
     flag_descriptions::kChromeLabsDescription, kOsDesktop,
     FEATURE_WITH_PARAMS_VALUE_TYPE(features::kChromeLabs,
                                    kChromeLabsVariations,
                                    "ChromeLabs")},

    {flag_descriptions::kChromeRefresh2023Id,
     flag_descriptions::kChromeRefresh2023Name,
     flag_descriptions::kChromeRefresh2023Description, kOsDesktop,
     FEATURE_WITH_PARAMS_VALUE_TYPE(features::kChromeRefresh2023,
                                    kChromeRefresh2023Variations,
                                    "ChromeRefresh2023")},

    {flag_descriptions::kChromeWebuiRefresh2023Id,
     flag_descriptions::kChromeWebuiRefresh2023Name,
     flag_descriptions::kChromeWebuiRefresh2023Description, kOsDesktop,
     FEATURE_VALUE_TYPE(features::kChromeWebuiRefresh2023)},

    {"chrome-refresh-2023-ntb", flag_descriptions::kChromeRefresh2023NTBName,
     flag_descriptions::kChromeRefresh2023NTBDescription, kOsDesktop,
     FEATURE_WITH_PARAMS_VALUE_TYPE(features::kChromeRefresh2023NTB,
                                    kChromeRefresh2023NTBVariations,
                                    "ChromeRefresh2023NTBVariations")},

    {"chrome-refresh-2023-top-chrome-font",
     flag_descriptions::kChromeRefresh2023TopChromeFontName,
     flag_descriptions::kChromeRefresh2023TopChromeFontDescription, kOsDesktop,
     FEATURE_VALUE_TYPE(features::kChromeRefresh2023TopChromeFont)},

#if BUILDFLAG(IS_ANDROID)
    {"autofill-enable-offers-in-clank-keyboard-accessory",
     flag_descriptions::kAutofillEnableOffersInClankKeyboardAccessoryName,
     flag_descriptions::
         kAutofillEnableOffersInClankKeyboardAccessoryDescription,
     kOsAndroid,
     FEATURE_VALUE_TYPE(
         autofill::features::kAutofillEnableOffersInClankKeyboardAccessory)},
#endif

#if BUILDFLAG(ENABLE_PDF)

#if !BUILDFLAG(IS_ANDROID)
    {"pdf-ocr", flag_descriptions::kPdfOcrName,
     flag_descriptions::kPdfOcrDescription,
     kOsMac | kOsWin | kOsLinux | kOsCrOS | kOsLacros,
     FEATURE_VALUE_TYPE(features::kPdfOcr)},
#endif  // !BUILDFLAG(IS_ANDROID)

    {"pdf-xfa-forms", flag_descriptions::kPdfXfaFormsName,
     flag_descriptions::kPdfXfaFormsDescription, kOsDesktop,
     FEATURE_VALUE_TYPE(chrome_pdf::features::kPdfXfaSupport)},
#endif  // BUILDFLAG(ENABLE_PDF)

#if BUILDFLAG(IS_CHROMEOS_ASH)
    {"backlight-ocr", flag_descriptions::kBacklightOcrName,
     flag_descriptions::kBacklightOcrDescription, kOsCrOS | kOsLacros,
     FEATURE_VALUE_TYPE(features::kBacklightOcr)},
#endif  // BUILDFLAG(IS_CHROMEOS_ASH)

#if BUILDFLAG(IS_ANDROID)
    {"send-tab-to-self-v2", flag_descriptions::kSendTabToSelfV2Name,
     flag_descriptions::kSendTabToSelfV2Description, kOsAndroid,
     FEATURE_VALUE_TYPE(send_tab_to_self::kSendTabToSelfV2)},
#endif  // BUILDFLAG(IS_ANDROID)

    {"enable-managed-configuration-web-api",
     flag_descriptions::kEnableManagedConfigurationWebApiName,
     flag_descriptions::kEnableManagedConfigurationWebApiDescription, kOsCrOS,
     FEATURE_VALUE_TYPE(blink::features::kManagedConfiguration)},

    {"enable-system-entropy",
     flag_descriptions::kEnableSystemEntropyOnPerformanceNavigationTimingName,
     flag_descriptions::
         kEnableSystemEntropyOnPerformanceNavigationTimingDescription,
     kOsAll,
     FEATURE_VALUE_TYPE(blink::features::kPerformanceNavigateSystemEntropy)},

    {"clear-cross-site-cross-browsing-context-group-window-name",
     flag_descriptions::kClearCrossSiteCrossBrowsingContextGroupWindowNameName,
     flag_descriptions::
         kClearCrossSiteCrossBrowsingContextGroupWindowNameDescription,
     kOsAll,
     FEATURE_VALUE_TYPE(
         features::kClearCrossSiteCrossBrowsingContextGroupWindowName)},

#if BUILDFLAG(IS_CHROMEOS)
    {kTaskManagerEndProcessDisabledForExtensionInternalName,
     flag_descriptions::kTaskManagerEndProcessDisabledForExtensionName,
     flag_descriptions::kTaskManagerEndProcessDisabledForExtensionDescription,
     kOsCrOS | kOsLacros,
     FEATURE_VALUE_TYPE(features::kDesktopPWAsRunOnOsLogin)},
#endif  // BUILDFLAG(IS_CHROMEOS)

#if BUILDFLAG(IS_CHROMEOS_ASH)
    {kWallpaperFastRefreshInternalName,
     flag_descriptions::kWallpaperFastRefreshName,
     flag_descriptions::kWallpaperFastRefreshDescription, kOsCrOS,
     FEATURE_VALUE_TYPE(ash::features::kWallpaperFastRefresh)},
    {kWallpaperGooglePhotosSharedAlbumsInternalName,
     flag_descriptions::kWallpaperGooglePhotosSharedAlbumsName,
     flag_descriptions::kWallpaperGooglePhotosSharedAlbumsDescription, kOsCrOS,
     FEATURE_VALUE_TYPE(ash::features::kWallpaperGooglePhotosSharedAlbums)},
    {kWallpaperPerDeskName, flag_descriptions::kWallpaperPerDeskName,
     flag_descriptions::kWallpaperPerDeskDescription, kOsCrOS,
     FEATURE_VALUE_TYPE(ash::features::kWallpaperPerDesk)},
#endif  // BUILDFLAG(IS_CHROMEOS_ASH)

#if BUILDFLAG(IS_CHROMEOS) || BUILDFLAG(IS_LINUX)
    {"enable-get-all-screens-media", flag_descriptions::kGetAllScreensMediaName,
     flag_descriptions::kGetAllScreensMediaDescription,
     kOsCrOS | kOsLacros | kOsLinux,
     FEATURE_VALUE_TYPE(blink::features::kGetAllScreensMedia)},
#endif  // BUILDFLAG(IS_CHROMEOS) || BUILDFLAG(IS_LINUX)

#if BUILDFLAG(IS_CHROMEOS)
    {"enable-run-on-os-login", flag_descriptions::kRunOnOsLoginName,
     flag_descriptions::kRunOnOsLoginDescription, kOsCrOS | kOsLacros,
     FEATURE_VALUE_TYPE(features::kDesktopPWAsRunOnOsLogin)},
    {"enable-prevent-close", flag_descriptions::kPreventCloseName,
     flag_descriptions::kPreventCloseDescription, kOsCrOS | kOsLacros,
     FEATURE_VALUE_TYPE(features::kDesktopPWAsPreventClose)},

    {"enable-cloud-identifiers",
     flag_descriptions::kFileSystemAccessGetCloudIdentifiersName,
     flag_descriptions::kFileSystemAccessGetCloudIdentifiersDescription,
     kOsCrOS | kOsLacros,
     FEATURE_VALUE_TYPE(blink::features::kFileSystemAccessGetCloudIdentifiers)},

    {"lacros-color-management", flag_descriptions::kLacrosColorManagementName,
     flag_descriptions::kLacrosColorManagementDescription, kOsLacros,
     FEATURE_VALUE_TYPE(features::kLacrosColorManagement)},

    {"preinstalled-web-app-window-experiment",
     flag_descriptions::kPreinstalledWebAppWindowExperimentName,
     flag_descriptions::kPreinstalledWebAppWindowExperimentDescription,
     kOsCrOS | kOsLacros,
     FEATURE_WITH_PARAMS_VALUE_TYPE(
         features::kPreinstalledWebAppWindowExperiment,
         kPreinstalledWebAppWindowExperimentVariations,
         "PreinstalledWebAppWindowExperimentVariations")},
#endif  // BUILDFLAG(IS_CHROMEOS)

    {"enable-global-vaapi-lock", flag_descriptions::kGlobalVaapiLockName,
     flag_descriptions::kGlobalVaapiLockDescription,
     kOsCrOS | kOsLinux | kOsLacros,
     FEATURE_VALUE_TYPE(media::kGlobalVaapiLock)},

#if BUILDFLAG(IS_WIN) ||                                      \
    (BUILDFLAG(IS_LINUX) || BUILDFLAG(IS_CHROMEOS_LACROS)) || \
    BUILDFLAG(IS_MAC) || BUILDFLAG(IS_FUCHSIA)
    {
        "ui-debug-tools",
        flag_descriptions::kUIDebugToolsName,
        flag_descriptions::kUIDebugToolsDescription,
        kOsWin | kOsLinux | kOsLacros | kOsMac | kOsFuchsia,
        FEATURE_VALUE_TYPE(features::kUIDebugTools),
    },

    {"sync-poll-immediately-on-every-startup",
     flag_descriptions::kSyncPollImmediatelyOnEveryStartupName,
     flag_descriptions::kSyncPollImmediatelyOnEveryStartupDescription,
     kOsDesktop,
     FEATURE_VALUE_TYPE(syncer::kSyncPollImmediatelyOnEveryStartup)},
#endif
    {"http-cache-partitioning",
     flag_descriptions::kSplitCacheByNetworkIsolationKeyName,
     flag_descriptions::kSplitCacheByNetworkIsolationKeyDescription,
     kOsWin | kOsLinux | kOsLacros | kOsMac | kOsCrOS | kOsAndroid | kOsFuchsia,
     FEATURE_VALUE_TYPE(net::features::kSplitCacheByNetworkIsolationKey)},

#if BUILDFLAG(IS_ANDROID)
    {"content-languages-in-language-picker",
     flag_descriptions::kContentLanguagesInLanguagePickerName,
     flag_descriptions::kContentLanguagesInLanguagePickerDescription,
     kOsAndroid,
     FEATURE_WITH_PARAMS_VALUE_TYPE(language::kContentLanguagesInLanguagePicker,
                                    kContentLanguagesInLanguaePickerVariations,
                                    "ContentLanguagesInLanguagePicker")},
#endif

#if !BUILDFLAG(IS_ANDROID)
    {"filling-across-grouped-sites",
     flag_descriptions::kFillingAcrossGroupedSitesName,
     flag_descriptions::kFillingAcrossGroupedSitesDescription, kOsDesktop,
     FEATURE_VALUE_TYPE(
         password_manager::features::kFillingAcrossGroupedSites)},
#endif  // !BUILDFLAG(IS_ANDROID)

    {"draw-predicted-ink-point", flag_descriptions::kDrawPredictedPointsName,
     flag_descriptions::kDrawPredictedPointsDescription, kOsAll,
     FEATURE_WITH_PARAMS_VALUE_TYPE(features::kDrawPredictedInkPoint,
                                    kDrawPredictedPointVariations,
                                    "DrawPredictedInkPoint")},

#if BUILDFLAG(IS_ANDROID)
    {"optimization-guide-personalized-fetching",
     flag_descriptions::kOptimizationGuidePersonalizedFetchingName,
     flag_descriptions::kOptimizationGuidePersonalizedFetchingDescription,
     kOsAndroid,
     FEATURE_WITH_PARAMS_VALUE_TYPE(
         optimization_guide::features::kOptimizationGuidePersonalizedFetching,
         kOptimizationGuidePersonalizedFetchingAllowPageInsightsVariations,
         "OptimizationGuidePersonalizedFetchingAllowPageInsights")},
    {"optimization-guide-push-notifications",
     flag_descriptions::kOptimizationGuidePushNotificationName,
     flag_descriptions::kOptimizationGuidePushNotificationDescription,
     kOsAndroid,
     FEATURE_VALUE_TYPE(optimization_guide::features::kPushNotifications)},
#endif

    {"fedcm-add-account", flag_descriptions::kFedCmAddAccountName,
     flag_descriptions::kFedCmAddAccountDescription, kOsAll,
     FEATURE_VALUE_TYPE(features::kFedCmAddAccount)},

    {"fedcm-authz", flag_descriptions::kFedCmAuthzName,
     flag_descriptions::kFedCmAuthzDescription, kOsDesktop,
     FEATURE_VALUE_TYPE(features::kFedCmAuthz)},

    {"fedcm-auto-selected-flag", flag_descriptions::kFedCmAutoSelectedFlagName,
     flag_descriptions::kFedCmAutoSelectedFlagDescription, kOsAll,
     FEATURE_VALUE_TYPE(features::kFedCmAutoSelectedFlag)},

    {"fedcm-disconnect", flag_descriptions::kFedCmDisconnectName,
     flag_descriptions::kFedCmDisconnectDescription, kOsAll,
     FEATURE_VALUE_TYPE(features::kFedCmDisconnect)},

    {"fedcm-domain-hint", flag_descriptions::kFedCmDomainHintName,
     flag_descriptions::kFedCmDomainHintDescription, kOsAll,
     FEATURE_VALUE_TYPE(features::kFedCmDomainHint)},

    {"fedcm-error", flag_descriptions::kFedCmErrorName,
     flag_descriptions::kFedCmErrorDescription, kOsAll,
     FEATURE_VALUE_TYPE(features::kFedCmError)},

    {"fedcm-exempt-idp-with-third-party-cookies",
     flag_descriptions::kFedCmExemptIdpWithThirdPartyCookiesName,
     flag_descriptions::kFedCmExemptIdpWithThirdPartyCookiesDescription, kOsAll,
     FEATURE_VALUE_TYPE(features::kFedCmExemptIdpWithThirdPartyCookies)},

    {"fedcm-idp-registration", flag_descriptions::kFedCmIdPRegistrationName,
     flag_descriptions::kFedCmIdPRegistrationDescription, kOsDesktop,
     FEATURE_VALUE_TYPE(features::kFedCmIdPRegistration)},

    {"fedcm-idp-signin-status-api",
     flag_descriptions::kFedCmIdpSigninStatusName,
     flag_descriptions::kFedCmIdpSigninStatusDescription, kOsAll,
     FEATURE_VALUE_TYPE(features::kFedCmIdpSigninStatusEnabled)},

    {"fedcm-metrics-endpoint", flag_descriptions::kFedCmMetricsEndpointName,
     flag_descriptions::kFedCmMetricsEndpointDescription, kOsAll,
     FEATURE_VALUE_TYPE(features::kFedCmMetricsEndpoint)},

    {"fedcm-multi-idp", flag_descriptions::kFedCmMultiIdpName,
     flag_descriptions::kFedCmMultiIdpDescription, kOsDesktop,
     FEATURE_VALUE_TYPE(features::kFedCmMultipleIdentityProviders)},

    {"fedcm-selective-disclosure",
     flag_descriptions::kFedCmSelectiveDisclosureName,
     flag_descriptions::kFedCmSelectiveDisclosureDescription, kOsAll,
     FEATURE_VALUE_TYPE(features::kFedCmSelectiveDisclosure)},

    {"fedcm-without-third-party-cookies",
     flag_descriptions::kFedCmWithoutThirdPartyCookiesName,
     flag_descriptions::kFedCmWithoutThirdPartyCookiesDescription, kOsAll,
     FEATURE_VALUE_TYPE(features::kFedCmWithoutThirdPartyCookies)},

    {"fedcm-without-well-known-enforcement",
     flag_descriptions::kFedCmWithoutWellKnownEnforcementName,
     flag_descriptions::kFedCmWithoutWellKnownEnforcementDescription, kOsAll,
     FEATURE_VALUE_TYPE(features::kFedCmWithoutWellKnownEnforcement)},

    {"web-identity-digital-credentials",
     flag_descriptions::kWebIdentityDigitalCredentialsName,
     flag_descriptions::kWebIdentityDigitalCredentialsDescription, kOsAndroid,
     FEATURE_VALUE_TYPE(features::kWebIdentityDigitalCredentials)},

#if BUILDFLAG(IS_CHROMEOS_ASH)
    {"bluetooth-sessionized-metrics",
     flag_descriptions::kBluetoothSessionizedMetricsName,
     flag_descriptions::kBluetoothSessionizedMetricsDescription, kOsCrOS,
     FEATURE_VALUE_TYPE(metrics::structured::kBluetoothSessionizedMetrics)},
#endif

    {"autofill-highlight-only-changed-value-in-preview-mode",
     flag_descriptions::kAutofillHighlightOnlyChangedValuesInPreviewModeName,
     flag_descriptions::
         kAutofillHighlightOnlyChangedValuesInPreviewModeDescription,
     kOsDesktop,
     FEATURE_VALUE_TYPE(
         autofill::features::kAutofillHighlightOnlyChangedValuesInPreviewMode)},

    {"sanitizer-api", flag_descriptions::kSanitizerApiName,
     flag_descriptions::kSanitizerApiDescription, kOsAll,
     FEATURE_VALUE_TYPE(blink::features::kSanitizerAPI)},

#if BUILDFLAG(IS_CHROMEOS_ASH)
    {"enable-input-event-logging",
     flag_descriptions::kEnableInputEventLoggingName,
     flag_descriptions::kEnableInputEventLoggingDescription, kOsCrOS,
     FEATURE_VALUE_TYPE(ui::kEnableInputEventLogging)},
#endif

    {"autofill-enable-sticky-manual-fallback-for-cards",
     flag_descriptions::kAutofillEnableStickyManualFallbackForCardsName,
     flag_descriptions::kAutofillEnableStickyManualFallbackForCardsDescription,
     kOsAndroid,
     FEATURE_VALUE_TYPE(
         autofill::features::kAutofillEnableStickyManualFallbackForCards)},

    {"autofill-prevent-overriding-prefilled-values",
     flag_descriptions::kAutofillPreventOverridingPrefilledValuesName,
     flag_descriptions::kAutofillPreventOverridingPrefilledValuesDescription,
     kOsDesktop | kOsAndroid,
     FEATURE_VALUE_TYPE(
         autofill::features::kAutofillPreventOverridingPrefilledValues)},

    {"autofill-show-autocomplete-delete-button",
     flag_descriptions::kAutofillShowAutocompleteDeleteButtonName,
     flag_descriptions::kAutofillShowAutocompleteDeleteButtonDescription,
     kOsDesktop,
     FEATURE_VALUE_TYPE(
         autofill::features::kAutofillShowAutocompleteDeleteButton)},

    {flag_descriptions::kEnableLensStandaloneFlagId,
     flag_descriptions::kEnableLensStandaloneName,
     flag_descriptions::kEnableLensStandaloneDescription, kOsDesktop,
     FEATURE_VALUE_TYPE(lens::features::kLensStandalone)},

#if BUILDFLAG(ENABLE_LENS_DESKTOP_GOOGLE_BRANDED_FEATURES)
    {"csc-companion-enable-page-content",
     flag_descriptions::kCscCompanionEnablePageContentName,
     flag_descriptions::kCscCompanionEnablePageContentDescription, kOsDesktop,
     FEATURE_VALUE_TYPE(companion::features::kCompanionEnablePageContent)},

    {"csc-force-companion-pinned-state",
     flag_descriptions::kCscForceCompanionPinnedStateName,
     flag_descriptions::kCscForceCompanionPinnedStateDescription, kOsDesktop,
     MULTI_VALUE_TYPE(kForceCompanionPinnedStateChoices)},

    {"csc-side-panel-companion", flag_descriptions::kCscSidePanelCompanionName,
     flag_descriptions::kCscSidePanelCompanionDescription, kOsDesktop,
     FEATURE_WITH_PARAMS_VALUE_TYPE(
         companion::features::internal::kSidePanelCompanion,
         kSidePanelCompanionVariations,
         "CSC")},
#endif  // BUILDFLAG(ENABLE_LENS_DESKTOP_GOOGLE_BRANDED_FEATURES)

    {"enable-lens-image-format-optimizations",
     flag_descriptions::kLensImageFormatOptimizationsName,
     flag_descriptions::kLensImageFormatOptimizationsDescription, kOsDesktop,
     FEATURE_WITH_PARAMS_VALUE_TYPE(
         lens::features::kLensImageFormatOptimizations,
         kLensImageFormatVariations,
         "LensImageFormatOptimizations")},

    {"enable-lens-image-translate", flag_descriptions::kLensImageTranslateName,
     flag_descriptions::kLensImageTranslateDescription, kOsDesktop,
     FEATURE_VALUE_TYPE(lens::features::kEnableImageTranslate)},

    {"enable-lens-ping", flag_descriptions::kEnableLensPingName,
     flag_descriptions::kEnableLensPingDescription, kOsDesktop,
     FEATURE_WITH_PARAMS_VALUE_TYPE(lens::features::kEnableLensPing,
                                    kLensPingVariations,
                                    "EnableLensPing")},

#if BUILDFLAG(IS_ANDROID)
    {"biometric-reauth-password-filling",
     flag_descriptions::kBiometricReauthForPasswordFillingName,
     flag_descriptions::kBiometricReauthForPasswordFillingDescription,
     kOsAndroid,
     FEATURE_VALUE_TYPE(password_manager::features::kBiometricTouchToFill)},
#endif

#if !BUILDFLAG(IS_ANDROID) && BUILDFLAG(GOOGLE_CHROME_BRANDING)
    {"ios-promo-password-bubble",
     flag_descriptions::kIOSPromoPasswordBubbleName,
     flag_descriptions::kIOSPromoPasswordBubbleDecription, kOsDesktop,
     FEATURE_WITH_PARAMS_VALUE_TYPE(promos_features::kIOSPromoPasswordBubble,
                                    kIOSPromoPasswordBubbleVariations,
                                    "IOSPromoPasswordBubble")},
#endif

#if BUILDFLAG(IS_CHROMEOS_ASH)
    {"enable-keyboard-backlight-toggle",
     flag_descriptions::kEnableKeyboardBacklightToggleName,
     flag_descriptions::kEnableKeyboardBacklightToggleDescription, kOsCrOS,
     FEATURE_VALUE_TYPE(ash::features::kEnableKeyboardBacklightToggle)},
#endif  // BUILDFLAG(IS_CHROMEOS_ASH)

    {"align-wakeups", flag_descriptions::kAlignWakeUpsName,
     flag_descriptions::kAlignWakeUpsDescription, kOsAll,
     FEATURE_VALUE_TYPE(base::kAlignWakeUps)},

    {"enable-throttle-display-none-and-visibility-hidden-cross-origin-iframes",
     flag_descriptions::
         kThrottleDisplayNoneAndVisibilityHiddenCrossOriginIframesName,
     flag_descriptions::
         kThrottleDisplayNoneAndVisibilityHiddenCrossOriginIframesDescription,
     kOsDesktop | kOsAndroid,
     FEATURE_VALUE_TYPE(
         blink::features::
             kThrottleDisplayNoneAndVisibilityHiddenCrossOriginIframes)},

    {"enable-new-base-url-inheritance-behavior",
     flag_descriptions::kNewBaseUrlInheritanceBehaviorName,
     flag_descriptions::kNewBaseUrlInheritanceBehaviorDescription,
     kOsDesktop | kOsAndroid,
     FEATURE_VALUE_TYPE(blink::features::kNewBaseUrlInheritanceBehavior)},

#if BUILDFLAG(ENABLE_VALIDATING_COMMAND_DECODER)
    {"use-passthrough-command-decoder",
     flag_descriptions::kUsePassthroughCommandDecoderName,
     flag_descriptions::kUsePassthroughCommandDecoderDescription, kOsAll,
     FEATURE_VALUE_TYPE(features::kDefaultPassthroughCommandDecoder)},
#endif  // BUILDFLAG(ENABLE_VALIDATING_COMMAND_DECODER)

#if BUILDFLAG(IS_CHROMEOS_ASH)
    {"focus-follows-cursor", flag_descriptions::kFocusFollowsCursorName,
     flag_descriptions::kFocusFollowsCursorDescription, kOsCrOS,
     FEATURE_VALUE_TYPE(::features::kFocusFollowsCursor)},
#endif  // BUILDFLAG(IS_CHROMEOS_ASH)

#if !BUILDFLAG(IS_ANDROID)
    {"password-generation-experiment",
     flag_descriptions::kPasswordGenerationExperimentName,
     flag_descriptions::kPasswordGenerationExperimentDescription, kOsDesktop,
     FEATURE_WITH_PARAMS_VALUE_TYPE(
         password_manager::features::kPasswordGenerationExperiment,
         kPasswordGenerationExperimentVariations,
         "PasswordGenerationExperiment")},
#endif

#if BUILDFLAG(IS_CHROMEOS)
    {"local-printer-observing", flag_descriptions::kLocalPrinterObservingName,
     flag_descriptions::kLocalPrinterObservingDescription, kOsCrOS | kOsLacros,
     FEATURE_VALUE_TYPE(features::kLocalPrinterObserving)},
    {"print-preview-setup-assistance",
     flag_descriptions::kPrintPreviewSetupAssistanceName,
     flag_descriptions::kPrintPreviewSetupAssistanceDescription,
     kOsCrOS | kOsLacros,
     FEATURE_VALUE_TYPE(features::kPrintPreviewSetupAssistance)},
#endif

#if !BUILDFLAG(IS_ANDROID)
    {"cbd-timeframe-required", flag_descriptions::kCbdTimeframeRequiredName,
     flag_descriptions::kCbdTimeframeRequiredDescription, kOsDesktop,
     FEATURE_VALUE_TYPE(features::kCbdTimeframeRequired)},
#endif

#if BUILDFLAG(IS_ANDROID)
    {"privacy-guide-android", flag_descriptions::kPrivacyGuideAndroidName,
     flag_descriptions::kPrivacyGuideAndroidDescription, kOsAndroid,
     FEATURE_VALUE_TYPE(features::kPrivacyGuideAndroid)},
#endif

#if BUILDFLAG(IS_ANDROID)
    {"privacy-guide-android-3", flag_descriptions::kPrivacyGuideAndroid3Name,
     flag_descriptions::kPrivacyGuideAndroid3Description, kOsAndroid,
     FEATURE_VALUE_TYPE(features::kPrivacyGuideAndroid3)},
#endif

    {"privacy-guide-3", flag_descriptions::kPrivacyGuide3Name,
     flag_descriptions::kPrivacyGuide3Description, kOsDesktop,
     FEATURE_VALUE_TYPE(features::kPrivacyGuide3)},

    {"privacy-guide-preload", flag_descriptions::kPrivacyGuidePreloadName,
     flag_descriptions::kPrivacyGuidePreloadDescription, kOsDesktop,
     FEATURE_VALUE_TYPE(features::kPrivacyGuidePreload)},

#if BUILDFLAG(IS_ANDROID)
    {"privacy-guide-preload-android",
     flag_descriptions::kPrivacyGuidePreloadAndroidName,
     flag_descriptions::kPrivacyGuidePreloadAndroidDescription, kOsAndroid,
     FEATURE_VALUE_TYPE(features::kPrivacyGuidePreloadAndroid)},
#endif

    {"tab-search-fuzzy-search", flag_descriptions::kTabSearchFuzzySearchName,
     flag_descriptions::kTabSearchFuzzySearchDescription, kOsDesktop,
     FEATURE_WITH_PARAMS_VALUE_TYPE(features::kTabSearchFuzzySearch,
                                    kTabSearchSearchThresholdVariations,
                                    "TabSearchFuzzySearch")},

#if BUILDFLAG(IS_CHROMEOS_ASH)
    {"enable-phone-hub-call-notification",
     flag_descriptions::kPhoneHubCallNotificationName,
     flag_descriptions::kPhoneHubCallNotificationDescription, kOsCrOS,
     FEATURE_VALUE_TYPE(ash::features::kPhoneHubCallNotification)},
#endif  // BUILDFLAG(IS_CHROMEOS_ASH)

    {"test-third-party-cookie-phaseout",
     flag_descriptions::kTestThirdPartyCookiePhaseoutName,
     flag_descriptions::kTestThirdPartyCookiePhaseoutDescription, kOsAll,
     SINGLE_VALUE_TYPE(network::switches::kTestThirdPartyCookiePhaseout)},

    {"third-party-storage-partitioning",
     flag_descriptions::kThirdPartyStoragePartitioningName,
     flag_descriptions::kThirdPartyStoragePartitioningDescription, kOsAll,
     FEATURE_VALUE_TYPE(net::features::kThirdPartyStoragePartitioning)},

    {"tpc-phase-out-facilitated-testing",
     flag_descriptions::kTPCPhaseOutFacilitatedTestingName,
     flag_descriptions::kTPCPhaseOutFacilitatedTestingDescription, kOsAll,
     FEATURE_WITH_PARAMS_VALUE_TYPE(
         features::kCookieDeprecationFacilitatedTesting,
         kTPCPhaseOutFacilitatedTestingVariations,
         "TPCPhaseOutFacilitatedTesting")},

    {"tpcd-heuristics-grants", flag_descriptions::kTpcdHeuristicsGrantsName,
     flag_descriptions::kTpcdHeuristicsGrantsDescription, kOsAll,
     FEATURE_WITH_PARAMS_VALUE_TYPE(
         content_settings::features::kTpcdHeuristicsGrants,
         kTpcdHeuristicsGrantsVariations,
         "TpcdHeuristicsGrants")},

    {"tpcd-metadata-grants", flag_descriptions::kTpcdMetadataGrantsName,
     flag_descriptions::kTpcdMetadataGrantsDescription, kOsAll,
     FEATURE_VALUE_TYPE(net::features::kTpcdMetadataGrants)},

    {"third-party-cookie-deprecation-trial",
     flag_descriptions::kTpcdSupportSettingsName,
     flag_descriptions::kTpcdSupportSettingsDescription, kOsAll,
     FEATURE_VALUE_TYPE(net::features::kTpcdSupportSettings)},

    {"bounce-tracking-mitigations", flag_descriptions::kDIPSName,
     flag_descriptions::kDIPSDescription, kOsAll,
     FEATURE_VALUE_TYPE(features::kDIPS)},

#if BUILDFLAG(IS_CHROMEOS_ASH)
    {kBorealisBigGlInternalName, flag_descriptions::kBorealisBigGlName,
     flag_descriptions::kBorealisBigGlDescription, kOsCrOS,
     FEATURE_VALUE_TYPE(ash::features::kBorealisBigGl)},
    {kBorealisDGPUInternalName, flag_descriptions::kBorealisDGPUName,
     flag_descriptions::kBorealisDGPUDescription, kOsCrOS,
     FEATURE_VALUE_TYPE(ash::features::kBorealisDGPU)},
    {kBorealisForceBetaClientInternalName,
     flag_descriptions::kBorealisForceBetaClientName,
     flag_descriptions::kBorealisForceBetaClientDescription, kOsCrOS,
     FEATURE_VALUE_TYPE(ash::features::kBorealisForceBetaClient)},
    {kBorealisForceDoubleScaleInternalName,
     flag_descriptions::kBorealisForceDoubleScaleName,
     flag_descriptions::kBorealisForceDoubleScaleDescription, kOsCrOS,
     FEATURE_VALUE_TYPE(ash::features::kBorealisForceDoubleScale)},
    {kBorealisLinuxModeInternalName, flag_descriptions::kBorealisLinuxModeName,
     flag_descriptions::kBorealisLinuxModeDescription, kOsCrOS,
     FEATURE_VALUE_TYPE(ash::features::kBorealisLinuxMode)},
    {kBorealisPermittedInternalName, flag_descriptions::kBorealisPermittedName,
     flag_descriptions::kBorealisPermittedDescription, kOsCrOS,
     FEATURE_VALUE_TYPE(ash::features::kBorealisPermitted)},
    {kBorealisProvisionInternalName, flag_descriptions::kBorealisProvisionName,
     flag_descriptions::kBorealisProvisionDescription, kOsCrOS,
     FEATURE_VALUE_TYPE(ash::features::kBorealisProvision)},
    {kBorealisScaleClientByDPIInternalName,
     flag_descriptions::kBorealisScaleClientByDPIName,
     flag_descriptions::kBorealisScaleClientByDPIDescription, kOsCrOS,
     FEATURE_VALUE_TYPE(ash::features::kBorealisScaleClientByDPI)},
    {kBorealisWebUIInstallerInternalName,
     flag_descriptions::kBorealisWebUIInstallerName,
     flag_descriptions::kBorealisWebUIInstallerDescription, kOsCrOS,
     FEATURE_VALUE_TYPE(ash::features::kBorealisWebUIInstaller)},
#endif  // BUILDFLAG(IS_CHROMEOS_ASH)

    {"https-first-mode-v2-for-engaged-sites",
     flag_descriptions::kHttpsFirstModeV2ForEngagedSitesName,
     flag_descriptions::kHttpsFirstModeV2ForEngagedSitesDescription,
     kOsDesktop | kOsAndroid,
     FEATURE_VALUE_TYPE(features::kHttpsFirstModeV2ForEngagedSites)},

    {"https-upgrades", flag_descriptions::kHttpsUpgradesName,
     flag_descriptions::kHttpsUpgradesDescription, kOsDesktop | kOsAndroid,
     FEATURE_VALUE_TYPE(features::kHttpsUpgrades)},

    {"https-first-mode-incognito",
     flag_descriptions::kHttpsFirstModeIncognitoName,
     flag_descriptions::kHttpsFirstModeIncognitoDescription,
     kOsDesktop | kOsAndroid,
     FEATURE_VALUE_TYPE(features::kHttpsFirstModeIncognito)},

    {"https-first-mode-for-typically-secure-users",
     flag_descriptions::kHttpsFirstModeForTypicallySecureUsersName,
     flag_descriptions::kHttpsFirstModeForTypicallySecureUsersDescription,
     kOsDesktop | kOsAndroid,
     FEATURE_VALUE_TYPE(features::kHttpsFirstModeV2ForTypicallySecureUsers)},

#if BUILDFLAG(IS_ANDROID)
    {"omnibox-2023-refresh-connection-security-indicators",
     flag_descriptions::kOmnibox2023RefreshConnectionSecurityIndicatorsName,
     flag_descriptions::
         kOmnibox2023RefreshConnectionSecurityIndicatorsDescription,
     kOsAndroid,
     FEATURE_VALUE_TYPE(omnibox::kUpdatedConnectionSecurityIndicators)},
#endif  // BUILDFLAG(IS_ANDROID)

    {"enable-drdc", flag_descriptions::kEnableDrDcName,
     flag_descriptions::kEnableDrDcDescription, kOsAll,
     FEATURE_VALUE_TYPE(features::kEnableDrDc)},

    {"force-gpu-main-thread-to-normal-priority-drdc",
     flag_descriptions::kForceGpuMainThreadToNormalPriorityDrDcName,
     flag_descriptions::kForceGpuMainThreadToNormalPriorityDrDcDescription,
     kOsAll,
     FEATURE_VALUE_TYPE(features::kForceGpuMainThreadToNormalPriorityDrDc)},

#if BUILDFLAG(IS_CHROMEOS_ASH)
    {"traffic-counters", flag_descriptions::kTrafficCountersEnabledName,
     flag_descriptions::kTrafficCountersEnabledDescription, kOsCrOS,
     FEATURE_VALUE_TYPE(ash::features::kTrafficCountersEnabled)},
#endif  // BUILDFLAG(IS_CHROMEOS_ASH)

#if BUILDFLAG(ENABLE_EXTENSIONS)
    {"extensions-menu-access-control",
     flag_descriptions::kExtensionsMenuAccessControlName,
     flag_descriptions::kExtensionsMenuAccessControlDescription, kOsDesktop,
     FEATURE_VALUE_TYPE(extensions_features::kExtensionsMenuAccessControl)},

    {"iph-extensions-menu-feature",
     flag_descriptions::kIPHExtensionsMenuFeatureName,
     flag_descriptions::kIPHExtensionsMenuFeatureDescription, kOsDesktop,
     FEATURE_VALUE_TYPE(feature_engagement::kIPHExtensionsMenuFeature)},

    {"iph-extensions-request-access-button-feature",
     flag_descriptions::kIPHExtensionsRequestAccessButtonFeatureName,
     flag_descriptions::kIPHExtensionsRequestAccessButtonFeatureDescription,
     kOsDesktop,
     FEATURE_VALUE_TYPE(
         feature_engagement::kIPHExtensionsRequestAccessButtonFeature)},

#if BUILDFLAG(IS_CHROMEOS)
    {"extension-web-file-handlers",
     flag_descriptions::kExtensionWebFileHandlersName,
     flag_descriptions::kExtensionWebFileHandlersDescription, kOsCrOS,
     FEATURE_VALUE_TYPE(extensions_features::kExtensionWebFileHandlers)},
#endif  // IS_CHROMEOS
#if BUILDFLAG(IS_WIN)
    {"launch-windows-native-hosts-directly",
     flag_descriptions::kLaunchWindowsNativeHostsDirectlyName,
     flag_descriptions::kLaunchWindowsNativeHostsDirectlyDescription, kOsWin,
     FEATURE_VALUE_TYPE(
         extensions_features::kLaunchWindowsNativeHostsDirectly)},
#endif  // IS_WIN
#endif  // ENABLE_EXTENSIONS

    {"canvas-oop-rasterization", flag_descriptions::kCanvasOopRasterizationName,
     flag_descriptions::kCanvasOopRasterizationDescription, kOsAll,
     FEATURE_VALUE_TYPE(features::kCanvasOopRasterization)},

#if !BUILDFLAG(IS_ANDROID)
    {"captured-surface-control", flag_descriptions::kCapturedSurfaceControlName,
     flag_descriptions::kCapturedSurfaceControlDescription, kOsDesktop,
     FEATURE_VALUE_TYPE(blink::features::kCapturedSurfaceControl)},
#endif

    {"skia-graphite", flag_descriptions::kSkiaGraphiteName,
     flag_descriptions::kSkiaGraphiteDescription, kOsAll,
     FEATURE_VALUE_TYPE(features::kSkiaGraphite)},

#if BUILDFLAG(IS_ANDROID)
    {"bookmarks-improved-save-flow",
     flag_descriptions::kBookmarksImprovedSaveFlowName,
     flag_descriptions::kBookmarksImprovedSaveFlowDescription, kOsAndroid,
     FEATURE_VALUE_TYPE(chrome::android::kBookmarksImprovedSaveFlow)},

    {"bookmarks-refresh", flag_descriptions::kBookmarksRefreshName,
     flag_descriptions::kBookmarksRefreshDescription, kOsAndroid,
     FEATURE_WITH_PARAMS_VALUE_TYPE(chrome::android::kBookmarksRefresh,
                                    kBookmarksRefreshVariations,
                                    "Collections")},
#endif

    {"enable-tab-audio-muting", flag_descriptions::kTabAudioMutingName,
     flag_descriptions::kTabAudioMutingDescription, kOsDesktop,
     FEATURE_VALUE_TYPE(media::kEnableTabMuting)},

#if defined(TOOLKIT_VIEWS)
    {"side-search", flag_descriptions::kSideSearchName,
     flag_descriptions::kSideSearchDescription, kOsDesktop,
     FEATURE_VALUE_TYPE(features::kSideSearch)},

    {"search-web-in-side-panel", flag_descriptions::kSearchWebInSidePanelName,
     flag_descriptions::kSearchWebInSidePanelDescription, kOsDesktop,
     FEATURE_VALUE_TYPE(features::kSearchWebInSidePanel)},
#endif  // defined(TOOLKIT_VIEWS)

#if !BUILDFLAG(IS_ANDROID)
    {flag_descriptions::kCustomizeChromeSidePanelId,
     flag_descriptions::kCustomizeChromeSidePanelName,
     flag_descriptions::KCustomizeChromeSidePanelDescription, kOsDesktop,
     FEATURE_VALUE_TYPE(features::kCustomizeChromeSidePanel)},

    {"customize-chrome-color-extraction",
     flag_descriptions::kCustomizeChromeColorExtractionName,
     flag_descriptions::kCustomizeChromeColorExtractionDescription, kOsDesktop,
     FEATURE_VALUE_TYPE(ntp_features::kCustomizeChromeColorExtraction)},

    {"customize-chrome-side-panel-extensions-card",
     flag_descriptions::kCustomizeChromeSidePanelExtensionsCardName,
     flag_descriptions::kCustomizeChromeSidePanelExtensionsCardDescription,
     kOsDesktop,
     FEATURE_VALUE_TYPE(ntp_features::kCustomizeChromeSidePanelExtensionsCard)},

    {"customize-chrome-wallpaper-search",
     flag_descriptions::kCustomizeChromeWallpaperSearchName,
     flag_descriptions::kCustomizeChromeWallpaperSearchDescription, kOsDesktop,
     FEATURE_VALUE_TYPE(ntp_features::kCustomizeChromeWallpaperSearch)},

    {"customize-chrome-wallpaper-search-inspiration-card",
     flag_descriptions::kCustomizeChromeWallpaperSearchInspirationCardName,
     flag_descriptions::
         kCustomizeChromeWallpaperSearchInspirationCardDescription,
     kOsDesktop,
     FEATURE_VALUE_TYPE(
         ntp_features::kCustomizeChromeWallpaperSearchInspirationCard)},

    {"wallpaper-search-settings-visibility",
     flag_descriptions::kWallpaperSearchSettingsVisibilityName,
     flag_descriptions::kWallpaperSearchSettingsVisibilityDescription,
     kOsDesktop,
     FEATURE_VALUE_TYPE(optimization_guide::features::internal::
                            kWallpaperSearchSettingsVisibility)},
#endif

#if BUILDFLAG(IS_CHROMEOS_ASH)
    {"enable-component-updater-test-request",
     flag_descriptions::kComponentUpdaterTestRequestName,
     flag_descriptions::kComponentUpdaterTestRequestDescription, kOsCrOS,
     SINGLE_VALUE_TYPE_AND_VALUE(switches::kComponentUpdater,
                                 component_updater::kSwitchTestRequestParam)},
#endif

    {"enable-raw-draw", flag_descriptions::kEnableRawDrawName,
     flag_descriptions::kEnableRawDrawDescription, kOsAll,
     FEATURE_VALUE_TYPE(features::kRawDraw)},

#if BUILDFLAG(IS_CHROMEOS_LACROS)
    {"enable-delegated-compositing",
     flag_descriptions::kEnableDelegatedCompositingName,
     flag_descriptions::kEnableDelegatedCompositingDescription, kOsAll,
     FEATURE_VALUE_TYPE(features::kDelegatedCompositing)},
#endif

#if BUILDFLAG(IS_WIN) || BUILDFLAG(IS_LINUX) || BUILDFLAG(IS_MAC) || \
    BUILDFLAG(IS_CHROMEOS)
    {"document-picture-in-picture-api",
     flag_descriptions::kDocumentPictureInPictureApiName,
     flag_descriptions::kDocumentPictureInPictureApiDescription,
     kOsMac | kOsWin | kOsLinux | kOsCrOS | kOsLacros,
     FEATURE_VALUE_TYPE(blink::features::kDocumentPictureInPictureAPI)},

    {"media-session-enter-picture-in-picture",
     flag_descriptions::kMediaSessionEnterPictureInPictureName,
     flag_descriptions::kMediaSessionEnterPictureInPictureDescription,
     kOsMac | kOsWin | kOsLinux | kOsCrOS | kOsLacros,
     FEATURE_VALUE_TYPE(blink::features::kMediaSessionEnterPictureInPicture)},
#endif  // BUILDFLAG(IS_WIN) || BUILDFLAG(IS_LINUX) || BUILDFLAG(IS_MAC) ||
        // BUILDFLAG(IS_CHROMEOS)

    {"web-midi", flag_descriptions::kWebMidiName,
     flag_descriptions::kWebMidiDescription, kOsAll,
     FEATURE_VALUE_TYPE(features::kWebMidi)},

#if BUILDFLAG(IS_CHROMEOS_ASH)
    {"dns-over-https-with-identifiers-reuse-old-policy",
     flag_descriptions::kDnsOverHttpsWithIdentifiersReuseOldPolicyName,
     flag_descriptions::kDnsOverHttpsWithIdentifiersReuseOldPolicyDescription,
     kOsCrOS,
     FEATURE_VALUE_TYPE(
         ash::features::kDnsOverHttpsWithIdentifiersReuseOldPolicy)},
#endif

    {"enable-commerce-developer", flag_descriptions::kCommerceDeveloperName,
     flag_descriptions::kCommerceDeveloperDescription, kOsAll,
     FEATURE_VALUE_TYPE(commerce::kCommerceDeveloper)},

#if BUILDFLAG(IS_CHROMEOS_ASH)
    {"enable-libinput-to-handle-touchpad",
     flag_descriptions::kEnableLibinputToHandleTouchpadName,
     flag_descriptions::kEnableLibinputToHandleTouchpadDescription, kOsCrOS,
     FEATURE_VALUE_TYPE(ui::kLibinputHandleTouchpad)},
#endif  // BUILDFLAG(IS_CHROMEOS_ASH)

#if BUILDFLAG(IS_CHROMEOS_ASH)
    {"enable-desks-templates", flag_descriptions::kDesksTemplatesName,
     flag_descriptions::kDesksTemplatesDescription, kOsCrOS,
     FEATURE_VALUE_TYPE(ash::features::kDesksTemplates)},
#endif

    {"large-favicon-from-google",
     flag_descriptions::kLargeFaviconFromGoogleName,
     flag_descriptions::kLargeFaviconFromGoogleDescription, kOsAndroid,
     FEATURE_WITH_PARAMS_VALUE_TYPE(features::kLargeFaviconFromGoogle,
                                    kLargeFaviconFromGoogleVariations,
                                    "LargeFaviconFromGoogle")},

#if BUILDFLAG(IS_CHROMEOS_ASH)
    {"video-conference", flag_descriptions::kVideoConferenceName,
     flag_descriptions::kVideoConferenceDescription, kOsCrOS,
     FEATURE_VALUE_TYPE(ash::features::kVideoConference)},

    {"vc-background-replace", flag_descriptions::kVcBackgroundReplaceName,
     flag_descriptions::kVcBackgroundReplaceDescription, kOsCrOS,
     FEATURE_VALUE_TYPE(ash::features::kVcBackgroundReplace)},

    {"vc-segmentation-model", flag_descriptions::kVcSegmentationModelName,
     flag_descriptions::kVcSegmentationModelDescription, kOsCrOS,
     FEATURE_WITH_PARAMS_VALUE_TYPE(ash::features::kVcSegmentationModel,
                                    kVcSegmentationModelVariations,
                                    "VCSegmentationModel")},
    {"vc-light-intensity", flag_descriptions::kVcLightIntensityName,
     flag_descriptions::kVcLightIntensityDescription, kOsCrOS,
     FEATURE_WITH_PARAMS_VALUE_TYPE(ash::features::kVcLightIntensity,
                                    kVcLightIntensityVariations,
                                    "VCLightIntensity")},
    {"vc-web-api", flag_descriptions::kVcWebApiName,
     flag_descriptions::kVcWebApiDescription, kOsCrOS,
     FEATURE_VALUE_TYPE(ash::features::kVcWebApi)},

#endif

#if BUILDFLAG(IS_ANDROID)
    {"touch-drag-and-context-menu",
     flag_descriptions::kTouchDragAndContextMenuName,
     flag_descriptions::kTouchDragAndContextMenuDescription, kOsAndroid,
     FEATURE_VALUE_TYPE(features::kTouchDragAndContextMenu)},
    {"animated-image-drag-shadow",
     flag_descriptions::kAnimatedImageDragShadowName,
     flag_descriptions::kAnimatedImageDragShadowDescription, kOsAndroid,
     FEATURE_VALUE_TYPE(chrome::android::kAnimatedImageDragShadow)},
    {"drag-drop-into-omnibox", flag_descriptions::kDragDropIntoOmniboxName,
     flag_descriptions::kDragDropIntoOmniboxDescription, kOsAndroid,
     FEATURE_VALUE_TYPE(chrome::android::kDragDropIntoOmnibox)},
#endif  // BUILDFLAG(IS_ANDROID)

    {"autofill-enable-update-virtual-card-enrollment",
     flag_descriptions::kAutofillEnableUpdateVirtualCardEnrollmentName,
     flag_descriptions::kAutofillEnableUpdateVirtualCardEnrollmentDescription,
     kOsAll,
     FEATURE_VALUE_TYPE(
         autofill::features::kAutofillEnableUpdateVirtualCardEnrollment)},

    {"autofill-move-legal-terms-and-icon-for-new-card-enrollment",
     flag_descriptions::kAutofillMoveLegalTermsAndIconForNewCardEnrollmentName,
     flag_descriptions::
         kAutofillMoveLegalTermsAndIconForNewCardEnrollmentDescription,
     kOsDesktop,
     FEATURE_VALUE_TYPE(
         autofill::features::
             kAutofillMoveLegalTermsAndIconForNewCardEnrollment)},

#if BUILDFLAG(IS_CHROMEOS_ASH)
    {"lacros-shared-components-dir",
     flag_descriptions::kLacrosSharedComponentsDirName,
     flag_descriptions::kLacrosSharedComponentsDirDescription, kOsCrOS,
     FEATURE_VALUE_TYPE(features::kLacrosSharedComponentsDir)},
#endif

#if BUILDFLAG(IS_CHROMEOS_LACROS)
    {"lacros-merge-icu-data-file",
     flag_descriptions::kLacrosMergeIcuDataFileName,
     flag_descriptions::kLacrosMergeIcuDataFileDescription, kOsCrOS | kOsLacros,
     FEATURE_VALUE_TYPE(base::i18n::kLacrosMergeIcuDataFile)},
#endif

#if !BUILDFLAG(IS_ANDROID)
    // TODO(https://crbug.com/1443349): Add Windows once library supports it.
    {"layout-extraction", flag_descriptions::kLayoutExtractionName,
     flag_descriptions::kLayoutExtractionDescription,
     kOsMac | kOsLinux | kOsCrOS | kOsLacros,
     FEATURE_VALUE_TYPE(features::kLayoutExtraction)},
#endif

    {"origin-agent-cluster-default",
     flag_descriptions::kOriginAgentClusterDefaultName,
     flag_descriptions::kOriginAgentClusterDefaultDescription, kOsAll,
     FEATURE_VALUE_TYPE(blink::features::kOriginAgentClusterDefaultEnabled)},

    {"origin-keyed-processes-by-default",
     flag_descriptions::kOriginKeyedProcessesByDefaultName,
     flag_descriptions::kOriginKeyedProcessesByDefaultDescription, kOsAll,
     FEATURE_VALUE_TYPE(features::kOriginKeyedProcessesByDefault)},

#if !BUILDFLAG(IS_ANDROID)
    {"enable-code-based-rbd", flag_descriptions::kCodeBasedRBDName,
     flag_descriptions::kCodeBasedRBDDescription, kOsDesktop,
     FEATURE_WITH_PARAMS_VALUE_TYPE(commerce::kCodeBasedRBD,
                                    kCodeBasedRBDVariations,
                                    "CodeBasedRBD")},

    {"enable-discount-consent-v2", flag_descriptions::kDiscountConsentV2Name,
     flag_descriptions::kDiscountConsentV2Description, kOsDesktop,
     FEATURE_WITH_PARAMS_VALUE_TYPE(commerce::kDiscountConsentV2,
                                    kDiscountConsentV2Variations,
                                    "DiscountConsentV2")},

    {"enable-merchant-wide-promotions",
     flag_descriptions::kMerchantWidePromotionsName,
     flag_descriptions::kMerchantWidePromotionsDescription, kOsDesktop,
     FEATURE_WITH_PARAMS_VALUE_TYPE(commerce::kMerchantWidePromotion,
                                    kMerchantWidePromotionVariations,
                                    "MerchantWidePromotion")},
#endif

    {"edit-context", flag_descriptions::kEditContextName,
     flag_descriptions::kEditContextDescription, kOsAll,
     FEATURE_VALUE_TYPE(blink::features::kEditContext)},
#if BUILDFLAG(IS_CHROMEOS_ASH)
    {"enable-fake-keyboard-heuristic",
     flag_descriptions::kEnableFakeKeyboardHeuristicName,
     flag_descriptions::kEnableFakeKeyboardHeuristicDescription, kOsCrOS,
     FEATURE_VALUE_TYPE(ui::kEnableFakeKeyboardHeuristic)},
#endif  // BUILDFLAG(IS_CHROMEOS_ASH)
#if !BUILDFLAG(IS_ANDROID)
    {"enable-isolated-sandboxed-iframes",
     flag_descriptions::kIsolatedSandboxedIframesName,
     flag_descriptions::kIsolatedSandboxedIframesDescription, kOsDesktop,
     FEATURE_WITH_PARAMS_VALUE_TYPE(
         blink::features::kIsolateSandboxedIframes,
         kIsolateSandboxedIframesGroupingVariations,
         "IsolateSandboxedIframes" /* trial name */)},
#endif

    {"download-warning-improvements",
     flag_descriptions::kDownloadWarningImprovementsName,
     flag_descriptions::kDownloadWarningImprovementsDescription,
     kOsLinux | kOsLacros | kOsMac | kOsWin | kOsCrOS,
     MULTI_VALUE_TYPE(kDownloadWarningImprovementsChoices)},

    {"unthrottled-nested-timeout",
     flag_descriptions::kUnthrottledNestedTimeoutName,
     flag_descriptions::kUnthrottledNestedTimeoutDescription, kOsAll,
     FEATURE_WITH_PARAMS_VALUE_TYPE(
         blink::features::kMaxUnthrottledTimeoutNestingLevel,
         kUnthrottledNestedTimeout_Variations,
         "NestingLevel")},

    {"reduce-accept-language", flag_descriptions::kReduceAcceptLanguageName,
     flag_descriptions::kReduceAcceptLanguageDescription, kOsAll,
     FEATURE_VALUE_TYPE(network::features::kReduceAcceptLanguage)},

    {"reduce-transfer-size-updated-ipc",
     flag_descriptions::kReduceTransferSizeUpdatedIPCName,
     flag_descriptions::kReduceTransferSizeUpdatedIPCDescription, kOsAll,
     FEATURE_VALUE_TYPE(network::features::kReduceTransferSizeUpdatedIPC)},

#if BUILDFLAG(IS_CHROMEOS_ASH)
    {"enable-variable-refresh-rate",
     flag_descriptions::kEnableVariableRefreshRateName,
     flag_descriptions::kEnableVariableRefreshRateDescription, kOsCrOS,
     FEATURE_VALUE_TYPE(features::kEnableVariableRefreshRate)},

    {"enable-variable-refresh-rate-always-on",
     flag_descriptions::kEnableVariableRefreshRateAlwaysOnName,
     flag_descriptions::kEnableVariableRefreshRateAlwaysOnDescription, kOsCrOS,
     FEATURE_VALUE_TYPE(features::kEnableVariableRefreshRateAlwaysOn)},

    {"enable-projector-app-debug", flag_descriptions::kProjectorAppDebugName,
     flag_descriptions::kProjectorAppDebugDescription, kOsCrOS,
     FEATURE_VALUE_TYPE(ash::features::kProjectorAppDebug)},

    {kProjectorServerSideSpeechRecognition,
     flag_descriptions::kProjectorServerSideSpeechRecognitionName,
     flag_descriptions::kProjectorServerSideSpeechRecognitionDescription,
     kOsCrOS,
     FEATURE_VALUE_TYPE(ash::features::kInternalServerSideSpeechRecognition)},
#endif  // BUILDFLAG(IS_CHROMEOS_ASH)

    {"web-sql-access", flag_descriptions::kWebSQLAccessName,
     flag_descriptions::kWebSQLAccessDescription, kOsAll,
     FEATURE_VALUE_TYPE(blink::features::kWebSQLAccess)},

    {"omit-cors-client-cert", flag_descriptions::kOmitCorsClientCertName,
     flag_descriptions::kOmitCorsClientCertDescription, kOsAll,
     FEATURE_VALUE_TYPE(network::features::kOmitCorsClientCert)},

    {"use-idna2008-non-transitional",
     flag_descriptions::kUseIDNA2008NonTransitionalName,
     flag_descriptions::kUseIDNA2008NonTransitionalDescription, kOsAll,
     FEATURE_VALUE_TYPE(url::kUseIDNA2008NonTransitional)},

#if BUILDFLAG(IS_CHROMEOS)
    {"sync-chromeos-explicit-passphrase-sharing",
     flag_descriptions::kSyncChromeOSExplicitPassphraseSharingName,
     flag_descriptions::kSyncChromeOSExplicitPassphraseSharingDescription,
     kOsCrOS | kOsLacros,
     FEATURE_VALUE_TYPE(syncer::kSyncChromeOSExplicitPassphraseSharing)},
#endif

#if BUILDFLAG(IS_CHROMEOS_ASH)
    {"always-enable-hdcp", flag_descriptions::kAlwaysEnableHdcpName,
     flag_descriptions::kAlwaysEnableHdcpDescription, kOsCrOS,
     MULTI_VALUE_TYPE(kAlwaysEnableHdcpChoices)},
    {"throttle-ambient-animations",
     flag_descriptions::kAmbientModeThrottleAnimationName,
     flag_descriptions::kAmbientModeThrottleAnimationDescription, kOsCrOS,
     FEATURE_VALUE_TYPE(ash::features::kAmbientModeThrottleAnimation)},
    {"enable-touchpads-in-diagnostics-app",
     flag_descriptions::kEnableTouchpadsInDiagnosticsAppName,
     flag_descriptions::kEnableTouchpadsInDiagnosticsAppDescription, kOsCrOS,
     FEATURE_VALUE_TYPE(ash::features::kEnableTouchpadsInDiagnosticsApp)},
    {"enable-touchscreens-in-diagnostics-app",
     flag_descriptions::kEnableTouchscreensInDiagnosticsAppName,
     flag_descriptions::kEnableTouchscreensInDiagnosticsAppDescription, kOsCrOS,
     FEATURE_VALUE_TYPE(ash::features::kEnableTouchscreensInDiagnosticsApp)},
    {"enable-external-keyboards-in-diagnostics-app",
     flag_descriptions::kEnableExternalKeyboardsInDiagnosticsAppName,
     flag_descriptions::kEnableExternalKeyboardsInDiagnosticsAppDescription,
     kOsCrOS,
     FEATURE_VALUE_TYPE(ash::features::kEnableExternalKeyboardsInDiagnostics)},
    {"enable-power-sounds", flag_descriptions::kSystemSoundsName,
     flag_descriptions::kSystemSoundsDescription, kOsCrOS,
     FEATURE_VALUE_TYPE(ash::features::kSystemSounds)},
#endif  // BUILDFLAG(IS_CHROMEOS_ASH)

    {"autofill-enable-virtual-card-metadata",
     flag_descriptions::kAutofillEnableVirtualCardMetadataName,
     flag_descriptions::kAutofillEnableVirtualCardMetadataDescription, kOsAll,
     FEATURE_VALUE_TYPE(
         autofill::features::kAutofillEnableVirtualCardMetadata)},

#if BUILDFLAG(IS_ANDROID)
    {"password-suggestion-bottom-sheet-v2",
     flag_descriptions::kPasswordSuggestionBottomSheetV2Name,
     flag_descriptions::kPasswordSuggestionBottomSheetV2Description, kOsAndroid,
     FEATURE_VALUE_TYPE(
         password_manager::features::kPasswordSuggestionBottomSheetV2)},

    {"pwa-restore-ui", flag_descriptions::kPwaRestoreUiName,
     flag_descriptions::kPwaRestoreUiDescription, kOsAndroid,
     FEATURE_VALUE_TYPE(chrome::android::kPwaRestoreUi)},
#endif  // BUILDFLAG(IS_ANDROID)
    {"autofill-enable-ranking-formula-address-profiles",
     flag_descriptions::kAutofillEnableRankingFormulaAddressProfilesName,
     flag_descriptions::kAutofillEnableRankingFormulaAddressProfilesDescription,
     kOsAll,
     FEATURE_VALUE_TYPE(
         autofill::features::kAutofillEnableRankingFormulaAddressProfiles)},

    {"autofill-enable-ranking-formula-credit-cards",
     flag_descriptions::kAutofillEnableRankingFormulaCreditCardsName,
     flag_descriptions::kAutofillEnableRankingFormulaCreditCardsDescription,
     kOsAll,
     FEATURE_VALUE_TYPE(
         autofill::features::kAutofillEnableRankingFormulaCreditCards)},

    {"safe-browsing-hash-prefix",
     flag_descriptions::kSafeBrowsingHashPrefixRealTimeLookupsName,
     flag_descriptions::kSafeBrowsingHashPrefixRealTimeLookupsDescription,
     kOsAll, FEATURE_VALUE_TYPE(safe_browsing::kHashPrefixRealTimeLookups)},
#if BUILDFLAG(IS_ANDROID)
    {"safe-browsing-new-gms-core-api-for-browse-url-database-check",
     flag_descriptions::kSafeBrowsingNewGmsApiForBrowseUrlDatabaseCheckName,
     flag_descriptions::
         kSafeBrowsingNewGmsApiForBrowseUrlDatabaseCheckDescription,
     kOsAndroid,
     FEATURE_VALUE_TYPE(
         safe_browsing::kSafeBrowsingNewGmsApiForBrowseUrlDatabaseCheck)},
#endif  // BUILDFLAG(IS_ANDROID)

#if !BUILDFLAG(IS_ANDROID)
    {"safety-check-notification-permissions",
     flag_descriptions::kSafetyCheckNotificationPermissionsName,
     flag_descriptions::kSafetyCheckNotificationPermissionsDescription,
     kOsDesktop,
     FEATURE_VALUE_TYPE(features::kSafetyCheckNotificationPermissions)},

    {"notification-interaction-history",
     flag_descriptions::kNotificationInteractionHistoryName,
     flag_descriptions::kNotificationInteractionHistoryDescription, kOsDesktop,
     FEATURE_VALUE_TYPE(
         permissions::features::kNotificationInteractionHistory)},

    {"safety-check-unused-site-permissions",
     flag_descriptions::kSafetyCheckUnusedSitePermissionsName,
     flag_descriptions::kSafetyCheckUnusedSitePermissionsDescription,
     kOsDesktop,
     FEATURE_WITH_PARAMS_VALUE_TYPE(
         content_settings::features::kSafetyCheckUnusedSitePermissions,
         kSafetyCheckUnusedSitePermissionsVariations,
         "SafetyCheckUnusedSitePermissions")},

    {"safety-hub", flag_descriptions::kSafetyHubName,
     flag_descriptions::kSafetyHubDescription, kOsDesktop,
     FEATURE_VALUE_TYPE(features::kSafetyHub)},
#endif  // !BUILDFLAG(IS_ANDROID)

#if BUILDFLAG(IS_ANDROID)
    {"enable-commerce-hint-android",
     flag_descriptions::kCommerceHintAndroidName,
     flag_descriptions::kCommerceHintAndroidDescription, kOsAndroid,
     FEATURE_VALUE_TYPE(commerce::kCommerceHintAndroid)},
#endif

#if BUILDFLAG(IS_WIN) || BUILDFLAG(IS_LINUX)
    {"enable-web-bluetooth-confirm-pairing-support",
     flag_descriptions::kWebBluetoothConfirmPairingSupportName,
     flag_descriptions::kWebBluetoothConfirmPairingSupportDescription,
     kOsDesktop,
     FEATURE_VALUE_TYPE(device::features::kWebBluetoothConfirmPairingSupport)},
#endif  // BUILDFLAG(IS_WIN) || BUILDFLAG(IS_LINUX)

    {"quick-intensive-throttling-after-loading",
     flag_descriptions::kQuickIntensiveWakeUpThrottlingAfterLoadingName,
     flag_descriptions::kQuickIntensiveWakeUpThrottlingAfterLoadingDescription,
     kOsAll,
     FEATURE_VALUE_TYPE(
         blink::features::kQuickIntensiveWakeUpThrottlingAfterLoading)},

#if BUILDFLAG(IS_MAC)
    {"swap-backquote-keys-in-iso-keyboard",
     flag_descriptions::kSwapBackquoteKeysInISOKeyboardName,
     flag_descriptions::kSwapBackquoteKeysInISOKeyboardDescription, kOsMac,
     FEATURE_VALUE_TYPE(features::kSwapBackquoteKeysInISOKeyboard)},

    {"system-color-chooser", flag_descriptions::kSystemColorChooserName,
     flag_descriptions::kSystemColorChooserDescription, kOsMac,
     FEATURE_VALUE_TYPE(blink::features::kSystemColorChooser)},
#endif  // BUILDFLAG(IS_MAC)

    {"ignore-sync-encryption-keys-long-missing",
     flag_descriptions::kIgnoreSyncEncryptionKeysLongMissingName,
     flag_descriptions::kIgnoreSyncEncryptionKeysLongMissingDescription, kOsAll,
     FEATURE_VALUE_TYPE(syncer::kIgnoreSyncEncryptionKeysLongMissing)},

#if BUILDFLAG(IS_ANDROID)
    {"autofill-enable-fido-progress-dialog",
     flag_descriptions::kAutofillEnableFIDOProgressDialogName,
     flag_descriptions::kAutofillEnableFIDOProgressDialogDescription,
     kOsAndroid,
     FEATURE_VALUE_TYPE(autofill::features::kAutofillEnableFIDOProgressDialog)},
#endif  // BUILDFLAG(IS_ANDROID)

    {"enable-perfetto-system-tracing",
     flag_descriptions::kEnablePerfettoSystemTracingName,
     flag_descriptions::kEnablePerfettoSystemTracingDescription, kOsAndroid,
     FEATURE_VALUE_TYPE(features::kEnablePerfettoSystemTracing)},

#if BUILDFLAG(IS_ANDROID)
    {"enable-android-gamepad-vibration",
     flag_descriptions::kEnableAndroidGamepadVibrationName,
     flag_descriptions::kEnableAndroidGamepadVibrationDescription, kOsAndroid,
     FEATURE_VALUE_TYPE(features::kEnableAndroidGamepadVibration)},
#endif  // BUILDFLAG(IS_ANDROID)

#if BUILDFLAG(IS_ANDROID)
    {"request-desktop-site-additions",
     flag_descriptions::kRequestDesktopSiteAdditionsName,
     flag_descriptions::kRequestDesktopSiteAdditionsDescription, kOsAndroid,
     FEATURE_VALUE_TYPE(features::kRequestDesktopSiteAdditions)},
#endif  // BUILDFLAG(IS_ANDROID)

#if BUILDFLAG(IS_ANDROID)
    {"request-desktop-site-defaults",
     flag_descriptions::kRequestDesktopSiteDefaultsName,
     flag_descriptions::kRequestDesktopSiteDefaultsDescription, kOsAndroid,
     FEATURE_VALUE_TYPE(chrome::android::kRequestDesktopSiteDefaults)},
#endif  // BUILDFLAG(IS_ANDROID)

#if BUILDFLAG(IS_ANDROID)
    {"request-desktop-site-defaults-downgrade",
     flag_descriptions::kRequestDesktopSiteDefaultsDowngradeName,
     flag_descriptions::kRequestDesktopSiteDefaultsDowngradeDescription,
     kOsAndroid,
     FEATURE_VALUE_TYPE(chrome::android::kRequestDesktopSiteDefaultsDowngrade)},
#endif  // BUILDFLAG(IS_ANDROID)

#if BUILDFLAG(IS_ANDROID)
    {"request-desktop-site-defaults-logging",
     flag_descriptions::kRequestDesktopSiteDefaultsLoggingName,
     flag_descriptions::kRequestDesktopSiteDefaultsLoggingDescription,
     kOsAndroid,
     FEATURE_VALUE_TYPE(chrome::android::kRequestDesktopSiteDefaultsLogging)},
#endif  // BUILDFLAG(IS_ANDROID)

#if BUILDFLAG(IS_ANDROID)
    {"request-desktop-site-window-setting",
     flag_descriptions::kRequestDesktopSiteWindowSettingName,
     flag_descriptions::kRequestDesktopSiteWindowSettingDescription, kOsAndroid,
     FEATURE_VALUE_TYPE(features::kRequestDesktopSiteWindowSetting)},
#endif  // BUILDFLAG(IS_ANDROID)

#if BUILDFLAG(IS_ANDROID)
    {"request-desktop-site-zoom",
     flag_descriptions::kRequestDesktopSiteZoomName,
     flag_descriptions::kRequestDesktopSiteZoomDescription, kOsAndroid,
     FEATURE_VALUE_TYPE(features::kRequestDesktopSiteZoom)},
#endif  // BUILDFLAG(IS_ANDROID)

#if !BUILDFLAG(IS_ANDROID)
    {"enable-web-hid-on-extension-service-worker",
     flag_descriptions::kEnableWebHidOnExtensionServiceWorkerName,
     flag_descriptions::kEnableWebHidOnExtensionServiceWorkerDescription,
     kOsDesktop,
     FEATURE_VALUE_TYPE(features::kEnableWebHidOnExtensionServiceWorker)},
#endif

    {"autofill-enable-remade-downstream-metrics",
     flag_descriptions::kAutofillEnableRemadeDownstreamMetricsName,
     flag_descriptions::kAutofillEnableRemadeDownstreamMetricsDescription,
     kOsAll,
     FEATURE_VALUE_TYPE(
         autofill::features::kAutofillEnableRemadeDownstreamMetrics)},

#if BUILDFLAG(IS_CHROMEOS_ASH)
    {"enable-seamless-refresh-rate-switching",
     flag_descriptions::kEnableSeamlessRefreshRateSwitchingName,
     flag_descriptions::kEnableSeamlessRefreshRateSwitchingDescription, kOsCrOS,
     FEATURE_VALUE_TYPE(ash::features::kSeamlessRefreshRateSwitching)},
#endif  // BUILDFLAG(IS_CHROMEOS_ASH)

    {"click-to-call", flag_descriptions::kClickToCallName,
     flag_descriptions::kClickToCallDescription, kOsAll,
     FEATURE_VALUE_TYPE(kClickToCall)},

    {"clipboard-unsanitized-content",
     flag_descriptions::kClipboardUnsanitizedContentName,
     flag_descriptions::kClipboardUnsanitizedContentDescription, kOsAll,
     FEATURE_VALUE_TYPE(blink::features::kClipboardUnsanitizedContent)},

    {"clipboard-well-formed-html-sanitization-write",
     flag_descriptions::kClipboardWellFormedHtmlSanitizationWriteName,
     flag_descriptions::kClipboardWellFormedHtmlSanitizationWriteDescription,
     kOsAll,
     FEATURE_VALUE_TYPE(
         blink::features::kClipboardWellFormedHtmlSanitizationWrite)},

    {"clipboard-maximum-age", flag_descriptions::kClipboardMaximumAgeName,
     flag_descriptions::kClipboardMaximumAgeDescription, kOsAndroid,
     FEATURE_WITH_PARAMS_VALUE_TYPE(kClipboardMaximumAge,
                                    kClipboardMaximumAgeVariations,
                                    "ClipboardMaximumAge")},

#if BUILDFLAG(IS_CHROMEOS_ASH)
    {"enable-media-dynamic-cgroup", flag_descriptions::kMediaDynamicCgroupName,
     flag_descriptions::kMediaDynamicCgroupDescription, kOsCrOS,
     PLATFORM_FEATURE_NAME_TYPE("CrOSLateBootMediaDynamicCgroup")},
#endif  // BUILDFLAG(IS_CHROMEOS_ASH)

    {"autofill-parse-vcn-card-on-file-standalone-cvc-fields",
     flag_descriptions::kAutofillParseVcnCardOnFileStandaloneCvcFieldsName,
     flag_descriptions::
         kAutofillParseVcnCardOnFileStandaloneCvcFieldsDescription,
     kOsAll,
     FEATURE_VALUE_TYPE(
         autofill::features::kAutofillParseVcnCardOnFileStandaloneCvcFields)},

    {"background-resource-fetch",
     flag_descriptions::kBackgroundResourceFetchName,
     flag_descriptions::kBackgroundResourceFetchDescription, kOsAll,
     FEATURE_VALUE_TYPE(blink::features::kBackgroundResourceFetch)},

    {"skip-service-worker-fetch-handler",
     flag_descriptions::kSkipServiceWorkerFetchHandlerName,
     flag_descriptions::kSkipServiceWorkerFetchHandlerDescription, kOsAll,
     FEATURE_WITH_PARAMS_VALUE_TYPE(
         features::kServiceWorkerSkipIgnorableFetchHandler,
         kServiceWorkerSkipIgnorableFetchHandlerVariations,
         "ServiceWorkerSkipIgnorableFetchHandler")},

#if BUILDFLAG(IS_ANDROID)
    {"external-navigation-debug-logs",
     flag_descriptions::kExternalNavigationDebugLogsName,
     flag_descriptions::kExternalNavigationDebugLogsDescription, kOsAndroid,
     FEATURE_VALUE_TYPE(external_intents::kExternalNavigationDebugLogs)},
#endif

#if !BUILDFLAG(IS_ANDROID)
    {"memory-saver-multi-state-mode",
     flag_descriptions::kHighEfficiencyMultistateModeAvailableName,
     flag_descriptions::kHighEfficiencyMultistateModeAvailableDescription,
     kOsDesktop,
     FEATURE_WITH_PARAMS_VALUE_TYPE(
         performance_manager::features::kHighEfficiencyMultistateMode,
         kHighEfficiencyMultistateModeVariations,
         "HighEfficiencyMultistateMode")},

    {"memory-saver-discarded-tab-treatment",
     flag_descriptions::kHighEfficiencyDiscardedTabTreatmentName,
     flag_descriptions::kHighEfficiencyDiscardedTabTreatmentDescription,
     kOsDesktop,
     FEATURE_WITH_PARAMS_VALUE_TYPE(
         performance_manager::features::kDiscardedTabTreatment,
         kDiscardedTabTreatmentVariations,
         "DiscardedTabTreatment")},

    {"memory-saver-memory-usage-in-hovercards",
     flag_descriptions::kHighEfficiencyMemoryUsageInHovercardsName,
     flag_descriptions::kHighEfficiencyMemoryUsageInHovercardsDescription,
     kOsDesktop,
     FEATURE_WITH_PARAMS_VALUE_TYPE(
         performance_manager::features::kMemoryUsageInHovercards,
         kMemoryUsageInHovercardsVariations,
         "MemoryUsageInHovercards")},

    {"memory-saver-discard-exceptions-improvements",
     flag_descriptions::kHighEfficiencyDiscardExceptionsImprovementsName,
     flag_descriptions::kHighEfficiencyDiscardExceptionsImprovementsDescription,
     kOsDesktop,
     FEATURE_VALUE_TYPE(
         performance_manager::features::kDiscardExceptionsImprovements)},

    {"memory-saver-savings-reporting-improvements",
     flag_descriptions::kHighEfficiencySavingsReportingImprovementsName,
     flag_descriptions::kHighEfficiencySavingsReportingImprovementsDescription,
     kOsDesktop,
     FEATURE_WITH_PARAMS_VALUE_TYPE(
         performance_manager::features::kMemorySavingsReportingImprovements,
         kHighEfficiencyMemorySavingsReportingVariations,
         "MemorySavingsReportingImprovements")},

    {"probabilistic-memory-saver",
     flag_descriptions::kProbabilisticMemorySaverName,
     flag_descriptions::kProbabilisticMemorySaverDescription, kOsDesktop,
     FEATURE_WITH_PARAMS_VALUE_TYPE(
         performance_manager::features::kProbabilisticProactiveDiscarding,
         kProbabilisticMemorySaverMode,
         "ProbabilisticProactiveDiscarding")},

    {"modal-memory-saver", flag_descriptions::kModalMemorySaverName,
     flag_descriptions::kModalMemorySaverDescription, kOsDesktop,
     FEATURE_WITH_PARAMS_VALUE_TYPE(
         performance_manager::features::kModalMemorySaver,
         kModalMemorySaverMode,
         "ModalMemorySaverMode")},
#endif

    {"webui-omnibox-popup", flag_descriptions::kWebUIOmniboxPopupName,
     flag_descriptions::kWebUIOmniboxPopupDescription, kOsDesktop,
     FEATURE_VALUE_TYPE(omnibox::kWebUIOmniboxPopup)},

#if BUILDFLAG(IS_CHROMEOS_ASH)
    {"arc-nearby-share-fuse-box", flag_descriptions::kArcNearbyShareFuseBoxName,
     flag_descriptions::kArcNearbyShareFuseBoxDescription, kOsCrOS,
     FEATURE_VALUE_TYPE(arc::kEnableArcNearbyShareFuseBox)},
#endif

#if BUILDFLAG(IS_CHROMEOS_ASH)
    {"arc-vm-memory-size", flag_descriptions::kArcVmMemorySizeName,
     flag_descriptions::kArcVmMemorySizeDesc, kOsCrOS,
     FEATURE_WITH_PARAMS_VALUE_TYPE(arc::kVmMemorySize,
                                    kArcVmMemorySizeVariations,
                                    "VmMemorySize")},
#endif  // BUILDFLAG(IS_CHROMEOS_ASH)

    {"autofill-enable-card-art-image",
     flag_descriptions::kAutofillEnableCardArtImageName,
     flag_descriptions::kAutofillEnableCardArtImageDescription, kOsAll,
     FEATURE_VALUE_TYPE(autofill::features::kAutofillEnableCardArtImage)},

#if BUILDFLAG(IS_ANDROID)
    {"restore-tabs-on-fre", flag_descriptions::kRestoreTabsOnFREName,
     flag_descriptions::kRestoreTabsOnFREDescription, kOsAndroid,
     FEATURE_WITH_PARAMS_VALUE_TYPE(chrome::android::kRestoreTabsOnFRE,
                                    kRestoreTabsOnFREVariations,
                                    "RestoreTabsOnFRE")},

    {"restore-synced-placeholder-tabs",
     flag_descriptions::kRestoreSyncedPlaceholderTabsName,
     flag_descriptions::kRestoreSyncedPlaceholderTabsDescription, kOsAndroid,
     FEATURE_VALUE_TYPE(syncer::kRestoreSyncedPlaceholderTabs)},

    {"context-menu-popup-for-all-screen-sizes",
     flag_descriptions::kContextMenuPopupForAllScreenSizesName,
     flag_descriptions::kContextMenuPopupForAllScreenSizesDescription,
     kOsAndroid,
     FEATURE_VALUE_TYPE(chrome::android::kContextMenuPopupForAllScreenSizes)},
#endif
    {"use-dmsaa-for-tiles", flag_descriptions::kUseDMSAAForTilesName,
     flag_descriptions::kUseDMSAAForTilesDescription, kOsAll,
     FEATURE_VALUE_TYPE(::features::kUseDMSAAForTiles)},

#if BUILDFLAG(IS_ANDROID)
    {"use-dmsaa-for-tiles-android-gl",
     flag_descriptions::kUseDMSAAForTilesAndroidGLName,
     flag_descriptions::kUseDMSAAForTilesAndroidGLDescription, kOsAndroid,
     FEATURE_VALUE_TYPE(::features::kUseDMSAAForTilesAndroidGL)},
#endif
    {"enable-web-usb-on-extension-service-worker",
     flag_descriptions::kEnableWebUsbOnExtensionServiceWorkerName,
     flag_descriptions::kEnableWebUsbOnExtensionServiceWorkerDescription,
     kOsAndroid | kOsDesktop,
     FEATURE_VALUE_TYPE(features::kEnableWebUsbOnExtensionServiceWorker)},

#if BUILDFLAG(IS_CHROMEOS_ASH)
    {"enable-holding-space-predictability",
     flag_descriptions::kHoldingSpacePredictabilityName,
     flag_descriptions::kHoldingSpacePredictabilityDescription, kOsCrOS,
     FEATURE_VALUE_TYPE(ash::features::kHoldingSpacePredictability)},
    {"enable-holding-space-refresh",
     flag_descriptions::kHoldingSpaceRefreshName,
     flag_descriptions::kHoldingSpaceRefreshDescription, kOsCrOS,
     FEATURE_VALUE_TYPE(ash::features::kHoldingSpaceRefresh)},
    {"enable-holding-space-suggestions",
     flag_descriptions::kHoldingSpaceSuggestionsName,
     flag_descriptions::kHoldingSpaceSuggestionsDescription, kOsCrOS,
     FEATURE_VALUE_TYPE(ash::features::kHoldingSpaceSuggestions)},
    {"enable-welcome-tour", flag_descriptions::kWelcomeTourName,
     flag_descriptions::kWelcomeTourDescription, kOsCrOS,
     FEATURE_VALUE_TYPE(ash::features::kWelcomeTour)},
    {"enable-welcome-tour-force-user-eligibility",
     flag_descriptions::kWelcomeTourForceUserEligibilityName,
     flag_descriptions::kWelcomeTourForceUserEligibilityDescription, kOsCrOS,
     FEATURE_VALUE_TYPE(ash::features::kWelcomeTourForceUserEligibility)},
#endif

    {"use-nat64-for-ipv4-literal",
     flag_descriptions::kUseNAT64ForIPv4LiteralName,
     flag_descriptions::kUseNAT64ForIPv4LiteralDescription, kOsAll,
     FEATURE_VALUE_TYPE(net::features::kUseNAT64ForIPv4Literal)},

#if BUILDFLAG(IS_ANDROID)
    {"enable-passwords-account-storage",
     flag_descriptions::kEnablePasswordsAccountStorageName,
     flag_descriptions::kEnablePasswordsAccountStorageDescription, kOsAndroid,
     FEATURE_VALUE_TYPE(
         password_manager::features::kEnablePasswordsAccountStorage)},
#endif  // BUILDFLAG(IS_CHROMEOS_ASH)

#if BUILDFLAG(IS_WIN)
    {"cloud-ap-auth-attach-as-header",
     flag_descriptions::kCloudApAuthAttachAsHeaderName,
     flag_descriptions::kCloudApAuthAttachAsHeaderDescription, kOsWin,
     FEATURE_VALUE_TYPE(enterprise_auth::kCloudApAuthAttachAsHeader)},
#endif  // BUILDFLAG(IS_WIN)

#if BUILDFLAG(IS_CHROMEOS_ASH)
    {"enable-per-desk-z-order", flag_descriptions::kEnablePerDeskZOrderName,
     flag_descriptions::kEnablePerDeskZOrderDescription, kOsCrOS,
     FEATURE_VALUE_TYPE(ash::features::kEnablePerDeskZOrder)},
#endif  // BUILDFLAG(IS_CHROMEOS_ASH)

#if !BUILDFLAG(IS_ANDROID)
    {"simplified-bookmark-save-flow",
     power_bookmarks::flag_descriptions::kSimplifiedBookmarkSaveFlowName,
     power_bookmarks::flag_descriptions::kSimplifiedBookmarkSaveFlowName,
     kOsDesktop,
     FEATURE_VALUE_TYPE(power_bookmarks::kSimplifiedBookmarkSaveFlow)},
#endif

#if BUILDFLAG(IS_CHROMEOS_ASH)
    {"gallery-app-pdf-edit-notification",
     flag_descriptions::kGalleryAppPdfEditNotificationName,
     flag_descriptions::kGalleryAppPdfEditNotificationDescription, kOsCrOS,
     FEATURE_WITH_PARAMS_VALUE_TYPE(
         ash::features::kGalleryAppPdfEditNotification,
         kGalleryAppPdfEditNotificationVariations,
         "GalleryAppPdfEditNotification")},
#endif

#if BUILDFLAG(IS_CHROMEOS_ASH)
    {"google-one-offer-files-banner",
     flag_descriptions::kGoogleOneOfferFilesBannerName,
     flag_descriptions::kGoogleOneOfferFilesBannerDescription, kOsCrOS,
     FEATURE_VALUE_TYPE(ash::features::kGoogleOneOfferFilesBanner)},
#endif

    {"sync-autofill-wallet-credential-data",
     flag_descriptions::kSyncAutofillWalletCredentialDataName,
     flag_descriptions::kSyncAutofillWalletCredentialDataDescription, kOsAll,
     FEATURE_VALUE_TYPE(syncer::kSyncAutofillWalletCredentialData)},

    {"sync-autofill-wallet-usage-data",
     flag_descriptions::kSyncAutofillWalletUsageDataName,
     flag_descriptions::kSyncAutofillWalletUsageDataDescription, kOsAll,
     FEATURE_VALUE_TYPE(syncer::kSyncAutofillWalletUsageData)},

#if !BUILDFLAG(IS_ANDROID)
    {"ui-enable-shared-image-cache-for-gpu",
     flag_descriptions::kUIEnableSharedImageCacheForGpuName,
     flag_descriptions::kUIEnableSharedImageCacheForGpuDescription, kOsDesktop,
     FEATURE_VALUE_TYPE(::features::kUIEnableSharedImageCacheForGpu)},
#endif

#if !BUILDFLAG(IS_ANDROID)
    {"devtools-tab-target", flag_descriptions::kDevToolsTabTargetLiteralName,
     flag_descriptions::kDevToolsTabTargetLiteralDescription, kOsDesktop,
     FEATURE_VALUE_TYPE(::features::kDevToolsTabTarget)},
#endif

#if BUILDFLAG(IS_CHROMEOS_ASH)
    {"cros-labs-window-cycle-shortcut",
     flag_descriptions::kSameAppWindowCycleName,
     flag_descriptions::kSameAppWindowCycleDescription, kOsCrOS,
     FEATURE_VALUE_TYPE(ash::features::kSameAppWindowCycle)},
#endif

#if BUILDFLAG(IS_CHROMEOS_ASH)
    {"promise-icons", flag_descriptions::kPromiseIconsName,
     flag_descriptions::kPromiseIconsDescription, kOsCrOS,
     FEATURE_VALUE_TYPE(ash::features::kPromiseIcons)},
#endif

#if BUILDFLAG(IS_CHROMEOS_ASH)
    {"printing-ppd-channel", flag_descriptions::kPrintingPpdChannelName,
     flag_descriptions::kPrintingPpdChannelDescription, kOsCrOS,
     MULTI_VALUE_TYPE(kPrintingPpdChannelChoices)},
#endif

    {"storage-buckets", flag_descriptions::kStorageBucketsName,
     flag_descriptions::kStorageBucketsDescription, kOsAll,
     FEATURE_VALUE_TYPE(blink::features::kStorageBuckets)},

    {"service-worker-bypass-fetch-handler",
     flag_descriptions::kServiceWorkerBypassFetchHandlerName,
     flag_descriptions::kServiceWorkerBypassFetchHandlerDescription, kOsAll,
     FEATURE_WITH_PARAMS_VALUE_TYPE(features::kServiceWorkerBypassFetchHandler,
                                    kServiceWorkerBypassFetchHandlerVariations,
                                    "ServiceWorkerBypassFetchHandler")},

    {"service-worker-bypass-fetch-handler-for-main-resource",
     flag_descriptions::kServiceWorkerBypassFetchHandlerForMainResourceName,
     flag_descriptions::
         kServiceWorkerBypassFetchHandlerForMainResourceDescription,
     kOsAll, FEATURE_VALUE_TYPE(features::kServiceWorkerBypassFetchHandler)},

    {"service-worker-static-router",
     flag_descriptions::kServiceWorkerStaticRouterName,
     flag_descriptions::kServiceWorkerStaticRouterDescription, kOsAll,
     FEATURE_VALUE_TYPE(features::kServiceWorkerStaticRouter)},

#if BUILDFLAG(IS_CHROMEOS_ASH)
    {"app-deduplication-service-fondue",
     flag_descriptions::kAppDeduplicationServiceFondueName,
     flag_descriptions::kAppDeduplicationServiceFondueDescription, kOsCrOS,
     FEATURE_VALUE_TYPE(features::kAppDeduplicationServiceFondue)},
#endif

    {"autofill-suggest-server-card-instead-of-local-card",
     flag_descriptions::kAutofillSuggestServerCardInsteadOfLocalCardName,
     flag_descriptions::kAutofillSuggestServerCardInsteadOfLocalCardDescription,
     kOsAll,
     FEATURE_VALUE_TYPE(
         autofill::features::kAutofillSuggestServerCardInsteadOfLocalCard)},

#if BUILDFLAG(IS_ANDROID)
    {"android-improved-bookmarks",
     flag_descriptions::kAndroidImprovedBookmarksName,
     flag_descriptions::kAndroidImprovedBookmarksDescription, kOsAndroid,
     FEATURE_VALUE_TYPE(chrome::android::kAndroidImprovedBookmarks)},
#endif  // BUILDFLAG(IS_ANDROID)

#if BUILDFLAG(IS_CHROMEOS_ASH)
    {"arc-idle-manager", flag_descriptions::kArcIdleManagerName,
     flag_descriptions::kArcIdleManagerDescription, kOsCrOS,
     FEATURE_VALUE_TYPE(arc::kEnableArcIdleManager)},
#endif  // BUILDFLAG(IS_CHROMEOS_ASH)

#if BUILDFLAG(ENABLE_EXTENSIONS)
    {"enable-webview-tag-mparch-behavior",
     flag_descriptions::kWebViewTagMPArchBehaviorName,
     flag_descriptions::kWebViewTagMPArchBehaviorDescription, kOsDesktop,
     FEATURE_VALUE_TYPE(extensions_features::kWebviewTagMPArchBehavior)},
#endif  // BUILDFLAG(ENABLE_EXTENSIONS)
        //
#if BUILDFLAG(IS_ANDROID)
    {"surface-control-magnifier",
     flag_descriptions::kSurfaceControlMagnifierName,
     flag_descriptions::kSurfaceControlMagnifierDescription, kOsAndroid,
     FEATURE_VALUE_TYPE(features::kAndroidSurfaceControlMagnifier)},
#endif  // BUILDFLAG(IS_ANDROID)

    {"use-multi-plane-format-for-hardware-video",
     flag_descriptions::kUseMultiPlaneFormatForHardwareVideoName,
     flag_descriptions::kUseMultiPlaneFormatForHardwareVideoDescription, kOsAll,
     FEATURE_VALUE_TYPE(media::kUseMultiPlaneFormatForHardwareVideo)},

    {"use-multi-plane-format-for-software-video",
     flag_descriptions::kUseMultiPlaneFormatForSoftwareVideoName,
     flag_descriptions::kUseMultiPlaneFormatForSoftwareVideoDescription, kOsAll,
     FEATURE_VALUE_TYPE(media::kUseMultiPlaneFormatForSoftwareVideo)},

#if BUILDFLAG(IS_CHROMEOS_ASH)
    {"enable-notification-image-drag",
     flag_descriptions::kEnableNotificationImageDragName,
     flag_descriptions::kEnableNotificationImageDragDescription, kOsCrOS,
     FEATURE_VALUE_TYPE(ash::features::kNotificationImageDrag)},
#endif

#if BUILDFLAG(IS_CHROMEOS_ASH)
    {"enable-notifier-collision",
     flag_descriptions::kEnableNotifierCollisionName,
     flag_descriptions::kEnableNotifierCollisionDescription, kOsCrOS,
     FEATURE_VALUE_TYPE(ash::features::kNotifierCollision)},
#endif

#if !BUILDFLAG(IS_ANDROID)
    {"autofill-enable-new-save-card-bubble-ui",
     flag_descriptions::kAutofillEnableNewSaveCardBubbleUiName,
     flag_descriptions::kAutofillEnableNewSaveCardBubbleUiDescription,
     kOsDesktop,
     FEATURE_VALUE_TYPE(
         autofill::features::kAutofillEnableNewSaveCardBubbleUi)},
#endif  // !BUILDFLAG(IS_ANDROID)

    {"autofill-enable-new-card-art-and-network-images",
     flag_descriptions::kAutofillEnableNewCardArtAndNetworkImagesName,
     flag_descriptions::kAutofillEnableNewCardArtAndNetworkImagesDescription,
     kOsAll,
     FEATURE_VALUE_TYPE(
         autofill::features::kAutofillEnableNewCardArtAndNetworkImages)},

    {"autofill-enable-card-art-server-side-stretching",
     flag_descriptions::kAutofillEnableCardArtServerSideStretchingName,
     flag_descriptions::kAutofillEnableCardArtServerSideStretchingDescription,
     kOsAll,
     FEATURE_VALUE_TYPE(
         autofill::features::kAutofillEnableCardArtServerSideStretching)},

    {"power-bookmark-backend", flag_descriptions::kPowerBookmarkBackendName,
     flag_descriptions::kPowerBookmarkBackendDescription, kOsAll,
     FEATURE_VALUE_TYPE(power_bookmarks::kPowerBookmarkBackend)},

#if !BUILDFLAG(IS_ANDROID)
    {"user-notes-side-panel", flag_descriptions::kUserNotesSidePanelName,
     flag_descriptions::kUserNotesSidePanelDescription, kOsDesktop,
     FEATURE_VALUE_TYPE(user_notes::kUserNotes)},
#endif

    {"autofill-enable-merchant-opt-out-client-side-url-filtering",
     flag_descriptions::kAutofillEnableMerchantOptOutClientSideUrlFilteringName,
     flag_descriptions::
         kAutofillEnableMerchantOptOutClientSideUrlFilteringDescription,
     kOsAll,
     FEATURE_VALUE_TYPE(
         autofill::features::
             kAutofillEnableMerchantOptOutClientSideUrlFiltering)},

#if BUILDFLAG(IS_CHROMEOS_ASH)
    {"enable-eol-notification-reset-dismissed-prefs",
     flag_descriptions::kEolResetDismissedPrefsName,
     flag_descriptions::kEolResetDismissedPrefsDescription, kOsCrOS,
     SINGLE_VALUE_TYPE(ash::switches::kEolResetDismissedPrefs)},
#endif

#if BUILDFLAG(IS_ANDROID)
    {"password-generation-bottom-sheet",
     flag_descriptions::kPasswordGenerationBottomSheetName,
     flag_descriptions::kPasswordGenerationBottomSheetDescription, kOsAndroid,
     FEATURE_VALUE_TYPE(
         password_manager::features::kPasswordGenerationBottomSheet)},
#endif  // BUILDFLAG(IS_ANDROID)

    {"enable-preferences-account-storage",
     flag_descriptions::kEnablePreferencesAccountStorageName,
     flag_descriptions::kEnablePreferencesAccountStorageDescription, kOsAll,
     FEATURE_VALUE_TYPE(syncer::kEnablePreferencesAccountStorage)},

#if BUILDFLAG(IS_CHROMEOS_ASH)
    {"app-launch-automation", flag_descriptions::kAppLaunchAutomationName,
     flag_descriptions::kAppLaunchAutomationDescription, kOsCrOS,
     FEATURE_VALUE_TYPE(ash::features::kAppLaunchAutomation)},
#endif

#if BUILDFLAG(IS_CHROMEOS_LACROS)
    {"experimental-web-app-storage-partition-isolation",
     flag_descriptions::kExperimentalWebAppStoragePartitionIsolationName,
     flag_descriptions::kExperimentalWebAppStoragePartitionIsolationDescription,
     kOsLacros,
     FEATURE_VALUE_TYPE(
         chromeos::features::kExperimentalWebAppStoragePartitionIsolation)},

    {"blink-extension", flag_descriptions::kBlinkExtensionName,
     flag_descriptions::kBlinkExtensionDescription, kOsLacros,
     FEATURE_VALUE_TYPE(chromeos::features::kBlinkExtension)},
    {"blink-extension-diagnostics",
     flag_descriptions::kBlinkExtensionDiagnosticsName,
     flag_descriptions::kBlinkExtensionDiagnosticsDescription, kOsLacros,
     FEATURE_VALUE_TYPE(chromeos::features::kBlinkExtensionDiagnostics)},
#endif  // BUILDFLAG(IS_CHROMEOS_LACROS)

#if BUILDFLAG(IS_CHROMEOS_ASH)
    {"enable-desk-button", flag_descriptions::kDeskButtonName,
     flag_descriptions::kDeskButtonDescription, kOsCrOS,
     FEATURE_VALUE_TYPE(ash::features::kDeskButton)},
    {"cros-focus-mode", flag_descriptions::kFocusModeName,
     flag_descriptions::kFocusModeDescription, kOsCrOS,
     FEATURE_VALUE_TYPE(ash::features::kFocusMode)},
    {"render-arc-notifications-by-chrome",
     flag_descriptions::kRenderArcNotificationsByChromeName,
     flag_descriptions::kRenderArcNotificationsByChromeDescription, kOsCrOS,
     FEATURE_VALUE_TYPE(ash::features::kRenderArcNotificationsByChrome)},
#endif

#if !BUILDFLAG(IS_ANDROID) && BUILDFLAG(GOOGLE_CHROME_BRANDING)
    {"settings-enable-get-the-most-out-of-chrome",
     flag_descriptions::kSettingsEnableGetTheMostOutOfChromeName,
     flag_descriptions::kSettingsEnableGetTheMostOutOfChromeDescription,
     kOsDesktop, FEATURE_VALUE_TYPE(::features::kGetTheMostOutOfChrome)},
#endif

    {"enable-file-backed-blob-factory",
     flag_descriptions::kEnableFileBackedBlobFactoryName,
     flag_descriptions::kEnableFileBackedBlobFactoryDescription, kOsAll,
     FEATURE_VALUE_TYPE(blink::features::kEnableFileBackedBlobFactory)},

#if BUILDFLAG(IS_MAC) || BUILDFLAG(IS_WIN) || BUILDFLAG(IS_ANDROID)
    {"autofill-enable-payments-mandatory-reauth",
     flag_descriptions::kAutofillEnablePaymentsMandatoryReauthName,
     flag_descriptions::kAutofillEnablePaymentsMandatoryReauthDescription,
     kOsMac | kOsWin | kOsAndroid,
     FEATURE_VALUE_TYPE(
         autofill::features::kAutofillEnablePaymentsMandatoryReauth)},
#endif  // BUILDFLAG(IS_MAC) || BUILDFLAG(IS_WIN) || BUILDFLAG(IS_ANDROID)

    {"autofill-enable-email-otp-for-vcn-yellow-path",
     flag_descriptions::kAutofillEnableEmailOtpForVcnYellowPathName,
     flag_descriptions::kAutofillEnableEmailOtpForVcnYellowPathDescription,
     kOsAll,
     FEATURE_VALUE_TYPE(
         autofill::features::kAutofillEnableEmailOtpForVcnYellowPath)},

    {"enable-compression-dictionary-transport",
     flag_descriptions::kCompressionDictionaryTransportName,
     flag_descriptions::kCompressionDictionaryTransportDescription, kOsAll,
     FEATURE_VALUE_TYPE(network::features::kCompressionDictionaryTransport)},

    {"enable-compression-dictionary-transport-backend",
     flag_descriptions::kCompressionDictionaryTransportBackendName,
     flag_descriptions::kCompressionDictionaryTransportBackendDescription,
     kOsAll,
     FEATURE_VALUE_TYPE(
         network::features::kCompressionDictionaryTransportBackend)},

    {"enable-compression-dictionary-transport-allow-http1",
     flag_descriptions::kCompressionDictionaryTransportOverHttp1Name,
     flag_descriptions::kCompressionDictionaryTransportOverHttp1Description,
     kOsAll,
     FEATURE_VALUE_TYPE(
         network::features::kCompressionDictionaryTransportOverHttp1)},

    {"enable-compression-dictionary-transport-require-known-root-cert",
     flag_descriptions::kCompressionDictionaryTransportRequireKnownRootCertName,
     flag_descriptions::
         kCompressionDictionaryTransportRequireKnownRootCertDescription,
     kOsAll,
     FEATURE_VALUE_TYPE(
         network::features::
             kCompressionDictionaryTransportRequireKnownRootCert)},

    {"enable-compute-pressure-rate-obfuscation-mitigation",
     flag_descriptions::kComputePressureRateObfuscationMitigationName,
     flag_descriptions::kComputePressureRateObfuscationMitigationDescription,
     kOsAll,
     FEATURE_VALUE_TYPE(
         blink::features::kComputePressureRateObfuscationMitigation)},

    {"enable-compute-pressure-break-calibration-mitigation",
     flag_descriptions::kComputePressureBreakCalibrationMitigationName,
     flag_descriptions::kComputePressureBreakCalibrationMitigationDescription,
     kOsAll,
     FEATURE_VALUE_TYPE(features::kComputePressureBreakCalibrationMitigation)},

    {"enable-zstd-content-encoding",
     flag_descriptions::kZstdContentEncodingName,
     flag_descriptions::kZstdContentEncodingDescription, kOsAll,
     FEATURE_VALUE_TYPE(net::features::kZstdContentEncoding)},

    {"enable-shared-zstd", flag_descriptions::kSharedZstdName,
     flag_descriptions::kSharedZstdDescription, kOsAll,
     FEATURE_VALUE_TYPE(network::features::kSharedZstd)},

#if BUILDFLAG(IS_CHROMEOS_ASH)
    {"arc-arc-on-demand", flag_descriptions::kArcArcOnDemandExperimentName,
     flag_descriptions::kArcArcOnDemandExperimentDescription, kOsCrOS,
     FEATURE_VALUE_TYPE(arc::kArcOnDemandFeature)},
#endif

#if BUILDFLAG(IS_ANDROID)
    {"web-authentication-android-credential-management",
     flag_descriptions::kWebAuthnAndroidCredManName,
     flag_descriptions::kWebAuthnAndroidCredManDescription, kOsAndroid,
     FEATURE_WITH_PARAMS_VALUE_TYPE(device::kWebAuthnAndroidCredMan,
                                    kWebAuthnAndroidCredManVariations,
                                    "WebAuthenticationAndroidCredMan")},
#endif  // BUILDFLAG(IS_ANDROID)

    {"permission-storage-access-api",
     flag_descriptions::kPermissionStorageAccessAPIName,
     flag_descriptions::kPermissionStorageAccessAPIDescription,
     kOsDesktop | kOsAndroid,
     FEATURE_VALUE_TYPE(permissions::features::kPermissionStorageAccessAPI)},

#if BUILDFLAG(IS_ANDROID)
    {"android-extended-keyboard-shortcuts",
     flag_descriptions::kAndroidExtendedKeyboardShortcutsName,
     flag_descriptions::kAndroidExtendedKeyboardShortcutsDescription,
     kOsAndroid,
     FEATURE_VALUE_TYPE(blink::features::kAndroidExtendedKeyboardShortcuts)},
    {"convert-trackpad-events-to-mouse",
     flag_descriptions::kConvertTrackpadEventsToMouseName,
     flag_descriptions::kConvertTrackpadEventsToMouseDescription, kOsAndroid,
     FEATURE_VALUE_TYPE(ui::kConvertTrackpadEventsToMouse)},
    {"deprecated-external-picker-function",
     flag_descriptions::kDeprecatedExternalPickerFunctionName,
     flag_descriptions::kDeprecatedExternalPickerFunctionDescription,
     kOsAndroid, FEATURE_VALUE_TYPE(ui::kDeprecatedExternalPickerFunction)},
    {"enable-cct-text-fragment-lookup-api",
     flag_descriptions::kCCTTextFragmentLookupApiEnabledName,
     flag_descriptions::kCCTTextFragmentLookupApiEnabledDescription, kOsAndroid,
     FEATURE_VALUE_TYPE(chrome::android::kCCTTextFragmentLookupApiEnabled)},

#endif  // BUILDFLAG(IS_ANDROID)

#if BUILDFLAG(IS_CHROMEOS_ASH)
    {"enable-missive-storage-config", flag_descriptions::kMissiveStorageName,
     flag_descriptions::kMissiveStorageDescription, kOsCrOS,
     PLATFORM_FEATURE_WITH_PARAMS_VALUE_TYPE(
         "CrOSLateBootMissiveStorage",
         kCrOSLateBootMissiveStorageDefaultVariations,
         "CrOSLateBootMissiveStorage")},
#endif  // BUILDFLAG(IS_CHROMEOS_ASH)

#if !BUILDFLAG(IS_ANDROID)
    {"cast-mirroring-target-playout-delay",
     flag_descriptions::kCastMirroringTargetPlayoutDelayName,
     flag_descriptions::kCastMirroringTargetPlayoutDelayDescription, kOsDesktop,
     MULTI_VALUE_TYPE(kCastMirroringTargetPlayoutDelayChoices)},
#endif  // !BUILDFLAG(IS_ANDROID)

    {"enable-policy-test-page", flag_descriptions::kEnablePolicyTestPageName,
     flag_descriptions::kEnablePolicyTestPageDescription, kOsAll,
     FEATURE_VALUE_TYPE(policy::features::kEnablePolicyTestPage)},

#if BUILDFLAG(IS_CHROMEOS_ASH)
    {"allow-devtools-in-system-ui",
     flag_descriptions::kAllowDevtoolsInSystemUIName,
     flag_descriptions::kAllowDevtoolsInSystemUIDescription, kOsCrOS,
     FEATURE_VALUE_TYPE(ash::features::kAllowDevtoolsInSystemUI)},
    {"cros-shortstand", flag_descriptions::kCrosShortstandName,
     flag_descriptions::kCrosShortstandDescription, kOsCrOS,
     FEATURE_VALUE_TYPE(chromeos::features::kCrosShortstand)},
    {"cros-web-app-shortcut-ui-update",
     flag_descriptions::kCrosWebAppShortcutUiUpdateName,
     flag_descriptions::kCrosWebAppShortcutUiUpdateDescription, kOsCrOS,
     FEATURE_VALUE_TYPE(chromeos::features::kCrosWebAppShortcutUiUpdate)},
    {"separate-web-app-shortcut-badge-icon",
     flag_descriptions::kSeparateWebAppShortcutBadgeIconName,
     flag_descriptions::kSeparateWebAppShortcutBadgeIconDescription, kOsCrOS,
     FEATURE_VALUE_TYPE(ash::features::kSeparateWebAppShortcutBadgeIcon)},
    {"enable-audio-focus-enforcement",
     flag_descriptions::kEnableAudioFocusEnforcementName,
     flag_descriptions::kEnableAudioFocusEnforcementDescription, kOsCrOS,
     FEATURE_VALUE_TYPE(media_session::features::kAudioFocusEnforcement)},
#endif  // BUILDFLAG(IS_CHROMEOS_ASH)

    {"use-gpu-scheduler-dfs", flag_descriptions::kUseGpuSchedulerDfsName,
     flag_descriptions::kUseGpuSchedulerDfsDescription, kOsAll,
     FEATURE_VALUE_TYPE(features::kUseGpuSchedulerDfs)},

#if BUILDFLAG(IS_CHROMEOS_ASH)
    {"cros-battery-saver", flag_descriptions::kCrosBatterySaverName,
     flag_descriptions::kCrosBatterySaverDescription, kOsCrOS,
     FEATURE_VALUE_TYPE(ash::features::kBatterySaver)},
#endif  // BUILDFLAG(IS_CHROMEOS_ASH)
        //
#if BUILDFLAG(IS_ANDROID)
    {"thumbnail-placeholder", flag_descriptions::kThumbnailPlaceholderName,
     flag_descriptions::kThumbnailPlaceholderDescription, kOsAndroid,
     FEATURE_VALUE_TYPE(chrome::android::kThumbnailPlaceholder)},
#endif  // BUILDFLAG(IS_ANDROID)

    {"enable-process-per-site-up-to-main-frame-threshold",
     flag_descriptions::kEnableProcessPerSiteUpToMainFrameThresholdName,
     flag_descriptions::kEnableProcessPerSiteUpToMainFrameThresholdDescription,
     kOsDesktop | kOsAndroid,
     FEATURE_VALUE_TYPE(features::kProcessPerSiteUpToMainFrameThreshold)},

#if BUILDFLAG(IS_MAC) || BUILDFLAG(IS_WIN) || BUILDFLAG(IS_LINUX)
    {"camera-mic-effects", flag_descriptions::kCameraMicEffectsName,
     flag_descriptions::kCameraMicEffectsDescription,
     static_cast<unsigned short>(kOsMac | kOsWin | kOsLinux),
     FEATURE_VALUE_TYPE(media::kCameraMicEffects)},

    {"camera-mic-preview", flag_descriptions::kCameraMicPreviewName,
     flag_descriptions::kCameraMicPreviewDescription,
     static_cast<unsigned short>(kOsMac | kOsWin | kOsLinux),
     FEATURE_VALUE_TYPE(features::kCameraMicPreview)},
#endif

#if BUILDFLAG(IS_CHROMEOS_ASH)
    {"cros-battery-saver-always-on",
     flag_descriptions::kCrosBatterySaverAlwaysOnName,
     flag_descriptions::kCrosBatterySaverAlwaysOnDescription, kOsCrOS,
     FEATURE_VALUE_TYPE(ash::features::kBatterySaverAlwaysOn)},
#endif  // BUILDFLAG(IS_CHROMEOS_ASH)

    {"render-document", flag_descriptions::kRenderDocumentName,
     flag_descriptions::kRenderDocumentDescription, kOsAll,
     FEATURE_WITH_PARAMS_VALUE_TYPE(features::kRenderDocument,
                                    kRenderDocumentVariations,
                                    "RenderDocument")},

    {"site-instance-groups-for-data-urls",
     flag_descriptions::kSiteInstanceGroupsForDataUrlsName,
     flag_descriptions::kSiteInstanceGroupsForDataUrlsDescription, kOsAll,
     FEATURE_VALUE_TYPE(features::kSiteInstanceGroupsForDataUrls)},

    {"early-document-swap-for-back-forward-transitions",
     flag_descriptions::kEarlyDocumentSwapForBackForwardTransitionsName,
     flag_descriptions::kEarlyDocumentSwapForBackForwardTransitionsDescription,
     kOsAll,
     FEATURE_VALUE_TYPE(features::kEarlyDocumentSwapForBackForwardTransitions)},

#if BUILDFLAG(ENABLE_EXTENSIONS)
    {"cws-info-fast-check", flag_descriptions::kCWSInfoFastCheckName,
     flag_descriptions::kCWSInfoFastCheckDescription, kOsDesktop,
     FEATURE_VALUE_TYPE(extensions::kCWSInfoFastCheck)},

    {"safety-check-extensions", flag_descriptions::kSafetyCheckExtensionsName,
     flag_descriptions::kSafetyCheckExtensionsDescription, kOsDesktop,
     FEATURE_VALUE_TYPE(features::kSafetyCheckExtensions)},
#endif  // BUILDFLAG(ENABLE_EXTENSIONS)

#if BUILDFLAG(IS_ANDROID)
    {"webapk-install-failure-notification",
     flag_descriptions::kWebApkInstallFailureNotificationName,
     flag_descriptions::kWebApkInstallFailureNotificationDescription,
     kOsAndroid,
     FEATURE_VALUE_TYPE(webapps::features::kWebApkInstallFailureNotification)},
    {"webapk-install-failure-retry",
     flag_descriptions::kWebApkInstallFailureRetryName,
     flag_descriptions::kWebApkInstallFailureRetryDescription, kOsAndroid,
     FEATURE_VALUE_TYPE(webapps::features::kWebApkInstallFailureRetry)},
#endif  // BUILDFLAG(IS_ANDROID)

    {"autofill-enable-cvc-storage-and-filling",
     flag_descriptions::kAutofillEnableCvcStorageAndFillingName,
     flag_descriptions::kAutofillEnableCvcStorageAndFillingDescription, kOsAll,
     FEATURE_VALUE_TYPE(
         autofill::features::kAutofillEnableCvcStorageAndFilling)},

#if BUILDFLAG(IS_CHROMEOS_ASH)
    {"drive-fs-show-cse-files", flag_descriptions::kDriveFsShowCSEFilesName,
     flag_descriptions::kDriveFsShowCSEFilesDescription, kOsCrOS,
     FEATURE_VALUE_TYPE(ash::features::kDriveFsShowCSEFiles)},
#endif  // BUILDFLAG(IS_CHROMEOS_ASH)

#if BUILDFLAG(IS_CHROMEOS_ASH)
    {"cros-labs-continuous-overview-animation",
     flag_descriptions::kContinuousOverviewScrollAnimationName,
     flag_descriptions::kContinuousOverviewScrollAnimationDescription, kOsCrOS,
     FEATURE_VALUE_TYPE(ash::features::kContinuousOverviewScrollAnimation)},
#endif  // BUILDFLAG(IS_CHROMEOS_ASH)

#if BUILDFLAG(IS_CHROMEOS_ASH)
    {"system-nudge-v2", flag_descriptions::kSystemNudgeV2Name,
     flag_descriptions::kSystemNudgeV2Description, kOsCrOS,
     FEATURE_VALUE_TYPE(ash::features::kSystemNudgeV2)},
#endif  // BUILDFLAG(IS_CHROMEOS_ASH)

#if BUILDFLAG(IS_CHROMEOS_ASH)
    {"cros-labs-window-splitting", flag_descriptions::kWindowSplittingName,
     flag_descriptions::kWindowSplittingDescription, kOsCrOS,
     FEATURE_VALUE_TYPE(ash::features::kWindowSplitting)},
#endif  // BUILDFLAG(IS_CHROMEOS_ASH)

    {"privacy-sandbox-enrollment-overrides",
     flag_descriptions::kPrivacySandboxEnrollmentOverridesName,
     flag_descriptions::kPrivacySandboxEnrollmentOverridesDescription, kOsAll,
     ORIGIN_LIST_VALUE_TYPE(privacy_sandbox::kPrivacySandboxEnrollmentOverrides,
                            "")},

#if !BUILDFLAG(IS_ANDROID)
    {"enable-smart-card-web-api", flag_descriptions::kSmartCardWebApiName,
     flag_descriptions::kSmartCardWebApiDescription, kOsDesktop,
     FEATURE_VALUE_TYPE(blink::features::kSmartCard)},
    {"create-shortcut-ignores-manifest",
     flag_descriptions::kCreateShortcutIgnoresManifestName,
     flag_descriptions::kCreateShortcutIgnoresManifestDescription, kOsDesktop,
     FEATURE_VALUE_TYPE(webapps::features::kCreateShortcutIgnoresManifest)},
#endif  // !BUILDFLAG(IS_ANDROID)

#if BUILDFLAG(IS_ANDROID)
    {"ml-mobile-pwa-prompt", flag_descriptions::kMobilePWAInstallPromptMlName,
     flag_descriptions::kMobilePWAInstallPromptMlDescription, kOsAndroid,
     FEATURE_VALUE_TYPE(webapps::features::kInstallPromptSegmentation)},
#endif

#if BUILDFLAG(IS_ANDROID)
    {"mouse-and-trackpad-dropdown-menu",
     flag_descriptions::kMouseAndTrackpadDropdownMenuName,
     flag_descriptions::kMouseAndTrackpadDropdownMenuDescription, kOsAndroid,
     FEATURE_VALUE_TYPE(features::kMouseAndTrackpadDropdownMenu)},
#endif

#if BUILDFLAG(IS_ANDROID)
    {"autofill-enable-payments-android-bottom-sheet",
     flag_descriptions::kAutofillEnablePaymentsAndroidBottomSheetName,
     flag_descriptions::kAutofillEnablePaymentsAndroidBottomSheetDescription,
     kOsAndroid,
     FEATURE_VALUE_TYPE(
         autofill::features::kAutofillEnablePaymentsAndroidBottomSheet)},
#endif

#if BUILDFLAG(IS_CHROMEOS_ASH)
    {"asynchronous-scanner-discovery",
     flag_descriptions::kAsynchronousScannerDiscoveryName,
     flag_descriptions::kAsynchronousScannerDiscoveryDescription, kOsCrOS,
     FEATURE_VALUE_TYPE(ash::features::kAsynchronousScannerDiscovery)},
#endif  // BUILDFLAG(IS_CHROMEOS_ASH)

#if BUILDFLAG(IS_ANDROID)
    {"read-aloud", flag_descriptions::kReadAloudName,
     flag_descriptions::kReadAloudDescription, kOsAndroid,
     FEATURE_VALUE_TYPE(chrome::android::kReadAloud)},
#endif

    {"hide-incognito-media-metadata",
     flag_descriptions::kHideIncognitoMediaMetadataName,
     flag_descriptions::kHideIncognitoMediaMetadataDescription, kOsAll,
     FEATURE_VALUE_TYPE(media::kHideIncognitoMediaMetadata)},

#if BUILDFLAG(IS_ANDROID)
    {"advanced-peripherals-support",
     flag_descriptions::kAdvancedPeripheralsSupportName,
     flag_descriptions::kAdvancedPeripheralsSupportDescription, kOsAndroid,
     FEATURE_VALUE_TYPE(chrome::android::kAdvancedPeripheralsSupport)},

    {"advanced-peripherals-support-tab-strip",
     flag_descriptions::kAdvancedPeripheralsSupportTabStripName,
     flag_descriptions::kAdvancedPeripheralsSupportTabStripDescription,
     kOsAndroid,
     FEATURE_VALUE_TYPE(chrome::android::kAdvancedPeripheralsSupportTabStrip)},
#endif  // BUILDFLAG(IS_ANDROID)

#if BUILDFLAG(IS_ANDROID)
    {"filling-across-affiliated-websites-android",
     flag_descriptions::kFillingAcrossAffiliatedWebsitesName,
     flag_descriptions::kFillingAcrossAffiliatedWebsitesDescription, kOsAndroid,
     FEATURE_VALUE_TYPE(
         password_manager::features::kFillingAcrossAffiliatedWebsitesAndroid)},
#endif

#if BUILDFLAG(IS_LINUX) || BUILDFLAG(IS_MAC) || BUILDFLAG(IS_WIN)
    {"third-party-profile-management",
     flag_descriptions::kThirdPartyProfileManagementName,
     flag_descriptions::kThirdPartyProfileManagementDescription,
     kOsLinux | kOsMac | kOsWin,
     FEATURE_VALUE_TYPE(
         profile_management::features::kThirdPartyProfileManagement)},

    {"uno-desktop", flag_descriptions::kUnoDesktopName,
     flag_descriptions::kUnoDesktopDescription, kOsLinux | kOsMac | kOsWin,
     FEATURE_VALUE_TYPE(switches::kUnoDesktop)},

    {"enable-user-link-capturing-pwa",
     flag_descriptions::kDesktopPWAsUserLinkCapturingName,
     flag_descriptions::kDesktopPWAsUserLinkCapturingDescription,
     kOsLinux | kOsMac | kOsWin,
     FEATURE_WITH_PARAMS_VALUE_TYPE(features::kDesktopPWAsLinkCapturing,
                                    kDesktopPWAsLinkCapturingVariations,
                                    "DesktopPWAsLinkCapturing")},
#endif  // BUILDFLAG(IS_LINUX) || BUILDFLAG(IS_MAC) || BUILDFLAG(IS_WIN)

    {"forgot-password-form-support",
     flag_descriptions::kForgotPasswordFormSupportName,
     flag_descriptions::kForgotPasswordFormSupportDescription, kOsAll,
     FEATURE_VALUE_TYPE(
         password_manager::features::kForgotPasswordFormSupport)},

    {"ip-protection-proxy-opt-out",
     flag_descriptions::kIpProtectionProxyOptOutName,
     flag_descriptions::kIpProtectionProxyOptOutDescription, kOsAll,
     MULTI_VALUE_TYPE(kIpProtectionProxyOptOutChoices)},

#if BUILDFLAG(IS_ANDROID)
    {"autofill-enable-android-n-key-for-fido-authentication",
     flag_descriptions::kAutofillEnableAndroidNKeyForFidoAuthenticationName,
     flag_descriptions::
         kAutofillEnableAndroidNKeyForFidoAuthenticationDescription,
     kOsAndroid,
     FEATURE_VALUE_TYPE(
         autofill::features::kAutofillEnableAndroidNKeyForFidoAuthentication)},
#endif  // BUILDFLAG(IS_ANDROID)

    {"protected-audience-debug-token",
     flag_descriptions::kProtectedAudiencesConsentedDebugTokenName,
     flag_descriptions::kProtectedAudiencesConsentedDebugTokenDescription,
     kOsAll,
     STRING_VALUE_TYPE(switches::kProtectedAudiencesConsentedDebugToken, "")},

    {"deprecate-unload", flag_descriptions::kDeprecateUnloadName,
     flag_descriptions::kDeprecateUnloadDescription, kOsAll | kDeprecated,
     FEATURE_VALUE_TYPE(blink::features::kDeprecateUnload)},

    {"autofill-enable-fpan-risk-based-authentication",
     flag_descriptions::kAutofillEnableFpanRiskBasedAuthenticationName,
     flag_descriptions::kAutofillEnableFpanRiskBasedAuthenticationDescription,
     kOsAll,
     FEATURE_VALUE_TYPE(
         autofill::features::kAutofillEnableFpanRiskBasedAuthentication)},

#if !BUILDFLAG(IS_ANDROID) && !BUILDFLAG(IS_CHROMEOS)
    {"cco-test1", flag_descriptions::kCcoTest1Name,
     flag_descriptions::kCcoTest1Description, kOsLinux | kOsMac | kOsWin,
     FEATURE_VALUE_TYPE(features::kCcoTest1)},
#endif

    {"draw-immediately-when-interactive",
     flag_descriptions::kDrawImmediatelyWhenInteractiveName,
     flag_descriptions::kDrawImmediatelyWhenInteractiveDescription, kOsAll,
     FEATURE_VALUE_TYPE(features::kDrawImmediatelyWhenInteractive)},

#if BUILDFLAG(IS_CHROMEOS_ASH)
    {"almanac-game-migration", flag_descriptions::kAlmanacGameMigrationName,
     flag_descriptions::kAlmanacGameMigrationDescription, kOsCrOS,
     FEATURE_VALUE_TYPE(apps::kAlmanacGameMigration)},
#endif

#if BUILDFLAG(IS_MAC)
    {"enable-mac-pwas-notification-attribution",
     flag_descriptions::kMacPWAsNotificationAttributionName,
     flag_descriptions::kMacPWAsNotificationAttributionDescription, kOsMac,
     FEATURE_VALUE_TYPE(features::kAppShimNotificationAttribution)},
#endif  // BUILDFLAG(IS_MAC)

    {"indexed-db-compress-values-with-snappy",
     flag_descriptions::kIndexedDBCompressValuesWithSnappy,
     flag_descriptions::kIndexedDBCompressValuesWithSnappyDescription, kOsAll,
     FEATURE_VALUE_TYPE(blink::features::kIndexedDBCompressValuesWithSnappy)},

    {"indexed-db-default-durability-relaxed",
     flag_descriptions::kIndexedDBDefaultDurabilityRelaxed,
     flag_descriptions::kIndexedDBDefaultDurabilityRelaxed, kOsAll,
     FEATURE_VALUE_TYPE(storage::kDefaultBucketUsesRelaxedDurability)},

    {"autofill-enable-server-iban",
     flag_descriptions::kAutofillEnableServerIbanName,
     flag_descriptions::kAutofillEnableServerIbanDescription, kOsDesktop,
     FEATURE_VALUE_TYPE(autofill::features::kAutofillEnableServerIban)},

#if !BUILDFLAG(IS_ANDROID)
    {"enable-manta-service", flag_descriptions::kEnableMantaServiceName,
     flag_descriptions::kEnableMantaServiceDescription, kOsDesktop,
     FEATURE_VALUE_TYPE(manta::features::kMantaService)},
#endif  // !BUILDFLAG(IS_ANDROID)

#if BUILDFLAG(IS_CHROMEOS_ASH)
    {"advanced-documentscan-api",
     flag_descriptions::kAdvancedDocumentScanApiName,
     flag_descriptions::kAdvancedDocumentScanApiDescription, kOsCrOS,
     FEATURE_VALUE_TYPE(ash::features::kAdvancedDocumentScanAPI)},
#endif  // BUILDFLAG(IS_CHROMEOS_ASH)

#if BUILDFLAG(ENABLE_HLS_DEMUXER)
    {"enable-builtin-hls", flag_descriptions::kEnableBuiltinHlsName,
     flag_descriptions::kEnableBuiltinHlsDescription, kOsAll,
     FEATURE_VALUE_TYPE(media::kBuiltInHlsPlayer)},
#endif

#if !BUILDFLAG(IS_CHROMEOS_ASH)
    {"profiles-reordering", flag_descriptions::kProfilesReorderingName,
     flag_descriptions::kProfilesReorderingDescription, kOsDesktop,
     FEATURE_VALUE_TYPE(kProfilesReordering)},
#endif

#if BUILDFLAG(IS_ANDROID)
    {"hide-tab-on-tab-switcher", flag_descriptions::kHideTabOnTabSwitcherName,
     flag_descriptions::kHideTabOnTabSwitcherDescription, kOsAndroid,
     FEATURE_VALUE_TYPE(chrome::android::kHideTabOnTabSwitcher)},
#endif  // BUILDFLAG(IS_ANDROID)

    {"autofill-enable-merchant-domain-in-unmask-card-request",
     flag_descriptions::kAutofillEnableMerchantDomainInUnmaskCardRequestName,
     flag_descriptions::
         kAutofillEnableMerchantDomainInUnmaskCardRequestDescription,
     kOsAll,
     FEATURE_VALUE_TYPE(
         autofill::features::kAutofillEnableMerchantDomainInUnmaskCardRequest)},

    {"autofill-update-chrome-settings-link-to-gpay-web",
     flag_descriptions::kAutofillUpdateChromeSettingsLinkToGPayWebName,
     flag_descriptions::kAutofillUpdateChromeSettingsLinkToGPayWebDescription,
     kOsAll,
     FEATURE_VALUE_TYPE(
         autofill::features::kAutofillUpdateChromeSettingsLinkToGPayWeb)},

#if !BUILDFLAG(IS_ANDROID)
    {"new-confirmation-bubble-for-generated-passwords",
     flag_descriptions::kNewConfirmationBubbleForGeneratedPasswordsName,
     flag_descriptions::kNewConfirmationBubbleForGeneratedPasswordsDescription,
     kOsDesktop,
     FEATURE_VALUE_TYPE(password_manager::features::
                            kNewConfirmationBubbleForGeneratedPasswords)},
#endif  // !BUILDFLAG(IS_ANDROID)

    {"enable-secure-payment-confirmation-extensions",
     flag_descriptions::kSecurePaymentConfirmationExtensionsName,
     flag_descriptions::kSecurePaymentConfirmationExtensionsDescription, kOsAll,
     FEATURE_VALUE_TYPE(blink::features::kSecurePaymentConfirmationExtensions)},

#if BUILDFLAG(IS_CHROMEOS_ASH)
    {"enable-runtime-counters-telemetry",
     flag_descriptions::kEnableRuntimeCountersTelemetryName,
     flag_descriptions::kEnableRuntimeCountersTelemetryDescription, kOsCrOS,
     FEATURE_VALUE_TYPE(reporting::kEnableRuntimeCountersTelemetry)},
#endif  // BUILDFLAG(IS_CHROMEOS_ASH)
#if BUILDFLAG(IS_CHROMEOS_ASH)
    {"upstream-trusted-reports-firmware",
     flag_descriptions::kUpstreamTrustedReportsFirmwareName,
     flag_descriptions::kUpstreamTrustedReportsFirmwareDescription, kOsCrOS,
     FEATURE_VALUE_TYPE(ash::features::kUpstreamTrustedReportsFirmware)},
#endif  // BUILDFLAG(IS_CHROMEOS_ASH)

#if BUILDFLAG(IS_CHROMEOS_ASH)
    {"ipp-first-setup-for-usb-printers",
     flag_descriptions::kIppFirstSetupForUsbPrintersName,
     flag_descriptions::kIppFirstSetupForUsbPrintersDescription, kOsCrOS,
     FEATURE_VALUE_TYPE(ash::features::kIppFirstSetupForUsbPrinters)},
#endif  // BUILDFLAG(IS_CHROMEOS_ASH)

#if BUILDFLAG(ENABLE_BOUND_SESSION_CREDENTIALS)
    {"enable-bound-session-credentials",
     flag_descriptions::kEnableBoundSessionCredentialsName,
     flag_descriptions::kEnableBoundSessionCredentialsDescription,
     kOsMac | kOsWin | kOsLinux,
     FEATURE_WITH_PARAMS_VALUE_TYPE(switches::kEnableBoundSessionCredentials,
                                    kEnableBoundSessionCredentialsVariations,
                                    "EnableBoundSessionCredentials")},
#endif  // BUILDFLAG(ENABLE_BOUND_SESSION_CREDENTIALS)

#if BUILDFLAG(IS_CHROMEOS_ASH)
    {"cros-soul", flag_descriptions::kCrosSoulName,
     flag_descriptions::kCrosSoulDescription, kOsCrOS,
     PLATFORM_FEATURE_NAME_TYPE("CrOSLateBootCrOSSOUL")},
#endif  // BUILDFLAG(IS_CHROMEOS_ASH)
    {"use-shared-images-for-pepper-video",
     flag_descriptions::kUseSharedImagesForPepperVideoName,
     flag_descriptions::kUseSharedImagesForPepperVideoDescription, kOsAll,
     FEATURE_VALUE_TYPE(media::kUseSharedImagesForPepperVideo)},

#if BUILDFLAG(IS_ANDROID)
    {"grid-tab-switcher-android-animations",
     flag_descriptions::kGridTabSwitcherAndroidAnimationsName,
     flag_descriptions::kGridTabSwitcherAndroidAnimationsDescription,
     kOsAndroid,
     FEATURE_VALUE_TYPE(chrome::android::kGridTabSwitcherAndroidAnimations)},
#endif  // !BUILDFLAG(IS_ANDROID)

#if BUILDFLAG(ENABLE_LIBRARY_CDMS)
    {"cdm-storage-database", flag_descriptions::kCdmStorageDatabaseName,
     flag_descriptions::kCdmStorageDatabaseDescription, kOsDesktop,
     FEATURE_VALUE_TYPE(features::kCdmStorageDatabase)},

    {"cdm-storage-database-migration",
     flag_descriptions::kCdmStorageDatabaseMigrationName,
     flag_descriptions::kCdmStorageDatabaseMigrationDescription, kOsDesktop,
     FEATURE_VALUE_TYPE(features::kCdmStorageDatabaseMigration)},
#endif  // BUILDFLAG(ENABLE_LIBRARY_CDMS)

#if BUILDFLAG(IS_ANDROID)
    {"defer-tab-switcher-layout-creation",
     flag_descriptions::kDeferTabSwitcherLayoutCreationName,
     flag_descriptions::kDeferTabSwitcherLayoutCreationDescription, kOsAndroid,
     FEATURE_VALUE_TYPE(chrome::android::kDeferTabSwitcherLayoutCreation)},
#endif  // BUILDFLAG(IS_ANDROID)

    {"observable-api", flag_descriptions::kObservableAPIName,
     flag_descriptions::kObservableAPIDescription, kOsAll,
     FEATURE_VALUE_TYPE(blink::features::kObservableAPI)},

#if BUILDFLAG(IS_ANDROID)
    {"android-hub", flag_descriptions::kAndroidHubName,
     flag_descriptions::kAndroidHubDescription, kOsAndroid,
     FEATURE_WITH_PARAMS_VALUE_TYPE(chrome::android::kAndroidHub,
                                    kAndroidHubVariations,
                                    "AndroidHub")},
#endif  // !BUILDFLAG(IS_ANDROID)

#if BUILDFLAG(IS_WIN)
    {"enable-web-app-system-media-controls-win",
     flag_descriptions::kWebAppSystemMediaControlsWinName,
     flag_descriptions::kWebAppSystemMediaControlsWinDescription, kOsWin,
     FEATURE_VALUE_TYPE(features::kWebAppSystemMediaControlsWin)},
#endif  // BUILDFLAG(IS_WIN)

#if BUILDFLAG(ENABLE_COMPOSE)
    {flag_descriptions::kComposeId, flag_descriptions::kComposeName,
     flag_descriptions::kComposeDescription,
     kOsWin | kOsLinux | kOsMac | kOsCrOS,
     FEATURE_VALUE_TYPE(compose::features::kEnableCompose)},
#endif

#if BUILDFLAG(IS_CHROMEOS)
    {"app-to-app-link-capturing", flag_descriptions::kAppToAppLinkCapturingName,
     flag_descriptions::kAppToAppLinkCapturingDescription, kOsCrOS | kOsLacros,
     FEATURE_VALUE_TYPE(apps::features::kAppToAppLinkCapturing)},

    {"app-to-app-link-capturing-workspace-apps",
     flag_descriptions::kAppToAppLinkCapturingWorkspaceAppsName,
     flag_descriptions::kAppToAppLinkCapturingWorkspaceAppsDescription,
     kOsCrOS | kOsLacros,
     FEATURE_VALUE_TYPE(apps::features::kAppToAppLinkCapturingWorkspaceApps)},
#endif  // BUILDFLAG(IS_CHROMEOS)

    {"related-website-sets-permission-grants",
     flag_descriptions::kShowRelatedWebsiteSetsPermissionGrantsName,
     flag_descriptions::kShowRelatedWebsiteSetsPermissionGrantsDescription,
     kOsDesktop | kOsAndroid,
     FEATURE_VALUE_TYPE(
         permissions::features::kShowRelatedWebsiteSetsPermissionGrants)},

    {"password-parsing-on-save-uses-predictions",
     flag_descriptions::kPasswordParsingOnSaveUsesPredictionsName,
     flag_descriptions::kPasswordParsingOnSaveUsesPredictionsDescription,
     kOsAll,
     FEATURE_VALUE_TYPE(
         password_manager::features::kUseServerPredictionsOnSaveParsing)},

#if BUILDFLAG(IS_ANDROID)
    {"upm-local-no-migration",
     flag_descriptions::
         kUnifiedPasswordManagerLocalPasswordsAndroidNoMigrationName,
     flag_descriptions::
         kUnifiedPasswordManagerLocalPasswordsAndroidNoMigrationDescription,
     kOsAndroid,
     FEATURE_VALUE_TYPE(
         password_manager::features::
             kUnifiedPasswordManagerLocalPasswordsAndroidNoMigration)},
#endif  // BUILDFLAG(IS_ANDROID)

#if BUILDFLAG(IS_CHROMEOS_ASH)
    {"app-install-service-uri", flag_descriptions::kAppInstallServiceUriName,
     flag_descriptions::kAppInstallServiceUriDescription, kOsCrOS,
     FEATURE_VALUE_TYPE(chromeos::features::kAppInstallServiceUri)},

    {"coral-feature-key", flag_descriptions::kCoralFeatureKeyName,
     flag_descriptions::kCoralFeatureKeyDescription, kOsCrOS,
     STRING_VALUE_TYPE(ash::switches::kCoralFeatureKey, "")},
#endif  // BUILDFLAG(IS_CHROMEOS_ASH)

#if BUILDFLAG(ENABLE_FFMPEG_VIDEO_DECODERS)
    {"theora-video-codec", flag_descriptions::kTheoraVideoCodecName,
     flag_descriptions::kTheoraVideoCodecDescription, kOsDesktop,
     FEATURE_VALUE_TYPE(media::kTheoraVideoCodec)},
#endif

#if BUILDFLAG(IS_MAC)
    {"video-toolbox-av1-decoding",
     flag_descriptions::kVideoToolboxAv1DecodingName,
     flag_descriptions::kVideoToolboxAv1DecodingDescription, kOsMac,
     FEATURE_VALUE_TYPE(media::kVideoToolboxAv1Decoding)},
#endif

    {"sync-session-on-visibility-changed",
     flag_descriptions::kSyncSessionOnVisibilityChangedName,
     flag_descriptions::kSyncSessionOnVisibilityChangedDescription, kOsAll,
     FEATURE_VALUE_TYPE(syncer::kSyncSessionOnVisibilityChanged)},

#if BUILDFLAG(IS_ANDROID)
    {"dynamic-top-chrome", flag_descriptions::kDynamicTopChromeName,
     flag_descriptions::kDynamicTopChromeDescription, kOsAndroid,
     FEATURE_VALUE_TYPE(chrome::android::kDynamicTopChrome)},
#endif

#if !BUILDFLAG(IS_ANDROID)
    {"password-generation-strong-label-experiment",
     flag_descriptions::kPasswordGenerationStrongLabelExperimentName,
     flag_descriptions::kPasswordGenerationStrongLabelExperimentDescription,
     kOsDesktop, FEATURE_VALUE_TYPE(blink::features::kPasswordStrongLabel)},
#endif

#if !BUILDFLAG(IS_ANDROID)
    {"trusted-vault-frequent-degraded-recoverability-polling",
     flag_descriptions::kTrustedVaultFrequentDegradedRecoverabilityPollingName,
     flag_descriptions::
         kTrustedVaultFrequentDegradedRecoverabilityPollingDescription,
     kOsDesktop,
     FEATURE_VALUE_TYPE(
         trusted_vault::kTrustedVaultFrequentDegradedRecoverabilityPolling)},
#endif

#if BUILDFLAG(IS_MAC)
    {"restart-to-gain-access-to-keychain",
     flag_descriptions::kRestartToGainAccessToKeychainName,
     flag_descriptions::kRestartToGainAccessToKeychainDescription, kOsMac,
     FEATURE_VALUE_TYPE(
         password_manager::features::kRestartToGainAccessToKeychain)},
#endif

#if BUILDFLAG(IS_ANDROID)
    {"use-gms-core-for-branding-info",
     flag_descriptions::kUseGMSCoreForBrandingInfoName,
     flag_descriptions::kUseGMSCoreForBrandingInfoDescription, kOsAndroid,
     FEATURE_VALUE_TYPE(
         password_manager::features::kUseGMSCoreForBrandingInfo)},
#endif

#if BUILDFLAG(IS_CHROMEOS)
    {"platform-keys-aes-encryption",
     flag_descriptions::kPlatformKeysAesEncryptionName,
     flag_descriptions::kPlatformKeysAesEncryptionDescription, kOsCrOS,
     FEATURE_VALUE_TYPE(features::kPlatformKeysAesEncryption)},
#endif  // BUILDFLAG(IS_CHROMEOS)

    {"autofill-enable-save-card-loading-and-confirmation",
     flag_descriptions::kAutofillEnableSaveCardLoadingAndConfirmationName,
     flag_descriptions::
         kAutofillEnableSaveCardLoadingAndConfirmationDescription,
     kOsAll,
     FEATURE_VALUE_TYPE(
         autofill::features::kAutofillEnableSaveCardLoadingAndConfirmation)},

    {"autofill-enable-vcn-enroll-loading-and-confirmation",
     flag_descriptions::kAutofillEnableVcnEnrollLoadingAndConfirmationName,
     flag_descriptions::
         kAutofillEnableVcnEnrollLoadingAndConfirmationDescription,
     kOsAll,
     FEATURE_VALUE_TYPE(
         autofill::features::kAutofillEnableVcnEnrollLoadingAndConfirmation)},

#if BUILDFLAG(IS_ANDROID)
    {"boarding-pass-detector", flag_descriptions::kBoardingPassDetectorName,
     flag_descriptions::kBoardingPassDetectorDescription, kOsAndroid,
     FEATURE_WITH_PARAMS_VALUE_TYPE(features::kBoardingPassDetector,
                                    kBoardingPassDetectorVariations,
                                    "Allowed Urls")},
#endif  // BUILDFLAG(IS_ANDROID)

    {"autofill-enable-card-benefits",
     flag_descriptions::kAutofillEnableCardBenefitsName,
     flag_descriptions::kAutofillEnableCardBenefitsDescription, kOsAll,
     FEATURE_VALUE_TYPE(autofill::features::kAutofillEnableCardBenefits)},

#if BUILDFLAG(IS_LINUX) || BUILDFLAG(IS_MAC) || BUILDFLAG(IS_WIN)
    {flag_descriptions::kAutofillContentEditablesId,
     flag_descriptions::kAutofillContentEditablesName,
     flag_descriptions::kAutofillContentEditablesDescription,
     kOsWin | kOsLinux | kOsMac,
     FEATURE_VALUE_TYPE(autofill::features::kAutofillContentEditables)},
#endif

#if BUILDFLAG(IS_CHROMEOS_ASH)
    {"cloud-gaming-device", flag_descriptions::kCloudGamingDeviceName,
     flag_descriptions::kCloudGamingDeviceDescription, kOsCrOS,
     FEATURE_VALUE_TYPE(chromeos::features::kCloudGamingDevice)},
#endif  // BUILDFLAG(IS_CHROMEOS_ASH)

    {"autofill-enable-user-avatar-in-save-card-footer",
     flag_descriptions::kAutofillEnableUserAvatarInSaveCardFooterName,
     flag_descriptions::kAutofillEnableUserAvatarInSaveCardFooterDescription,
     kOsDesktop,
     FEATURE_VALUE_TYPE(
         autofill::features::kAutofillEnableUserAvatarInSaveCardFooter)},

    {"content-settings-partitioning",
     flag_descriptions::kContentSettingsPartitioningName,
     flag_descriptions::kContentSettingsPartitioningDescription, kOsAll,
     FEATURE_VALUE_TYPE(
         content_settings::features::kContentSettingsPartitioning)},

#if BUILDFLAG(IS_ANDROID)
    {"enable-vertical-automotive-back-button-toolbar",
     flag_descriptions::kVerticalAutomotiveBackButtonToolbarName,
     flag_descriptions::kVerticalAutomotiveBackButtonToolbarDescription,
     kOsAndroid,
     FEATURE_VALUE_TYPE(chrome::android::kVerticalAutomotiveBackButtonToolbar)},
#endif  // BUILDFLAG(IS_ANDROID)

#if BUILDFLAG(IS_ANDROID)
    {"remove-upm-unenrollment", flag_descriptions::kRemoveUPMUnenrollmentName,
     flag_descriptions::kRemoveUPMUnenrollmentDescription, kOsAndroid,
     FEATURE_VALUE_TYPE(password_manager::features::kRemoveUPMUnenrollment)},
#endif

    // NOTE: Adding a new flag requires adding a corresponding entry to enum
    // "LoginCustomFlags" in tools/metrics/histograms/enums.xml. See "Flag
    // Histograms" in tools/metrics/histograms/README.md (run the
    // AboutFlagsHistogramTest unit test to verify this process).
};

class FlagsStateSingleton : public flags_ui::FlagsState::Delegate {
 public:
  FlagsStateSingleton()
      : flags_state_(
            std::make_unique<flags_ui::FlagsState>(kFeatureEntries, this)) {}
  FlagsStateSingleton(const FlagsStateSingleton&) = delete;
  FlagsStateSingleton& operator=(const FlagsStateSingleton&) = delete;
  ~FlagsStateSingleton() override = default;

  static FlagsStateSingleton* GetInstance() {
    return base::Singleton<FlagsStateSingleton>::get();
  }

  static flags_ui::FlagsState* GetFlagsState() {
    return GetInstance()->flags_state_.get();
  }

  void RebuildState(const std::vector<flags_ui::FeatureEntry>& entries) {
    flags_state_ = std::make_unique<flags_ui::FlagsState>(entries, this);
  }

  void RestoreDefaultState() {
    flags_state_ =
        std::make_unique<flags_ui::FlagsState>(kFeatureEntries, this);
  }

 private:
  // flags_ui::FlagsState::Delegate:
  bool ShouldExcludeFlag(const flags_ui::FlagsStorage* storage,
                         const FeatureEntry& entry) override {
    return flags::IsFlagExpired(storage, entry.internal_name);
  }

  std::unique_ptr<flags_ui::FlagsState> flags_state_;
};

bool ShouldSkipNonDeprecatedFeatureEntry(const FeatureEntry& entry) {
  return ~entry.supported_platforms & kDeprecated;
}

}  // namespace

#if BUILDFLAG(IS_CHROMEOS_ASH)
// This method may be invoked both synchronously or asynchronously. Based on
// whether the current user is the owner of the device, generates the
// appropriate flag storage.
void GetStorageAsync(Profile* profile,
                     GetStorageCallback callback,
                     bool current_user_is_owner) {
  // On ChromeOS the owner can set system wide flags and other users can only
  // set flags for their own session.
  if (current_user_is_owner) {
    ash::OwnerSettingsServiceAsh* service =
        ash::OwnerSettingsServiceAshFactory::GetForBrowserContext(profile);
    std::move(callback).Run(
        std::make_unique<ash::about_flags::OwnerFlagsStorage>(
            profile->GetPrefs(), service),
        flags_ui::kOwnerAccessToFlags);
  } else {
    std::move(callback).Run(std::make_unique<flags_ui::PrefServiceFlagsStorage>(
                                profile->GetPrefs()),
                            flags_ui::kGeneralAccessFlagsOnly);
  }
}
#endif

// ash-chrome uses different storage flag storage logic from other desktop
// platforms.
void GetStorage(Profile* profile, GetStorageCallback callback) {
#if BUILDFLAG(IS_CHROMEOS_ASH)
  // Bypass possible incognito profile.
  // On ChromeOS the owner can set system wide flags and other users can only
  // set flags for their own session.
  Profile* original_profile = profile->GetOriginalProfile();
  if (base::SysInfo::IsRunningOnChromeOS() &&
      ash::OwnerSettingsServiceAshFactory::GetForBrowserContext(
          original_profile)) {
    ash::OwnerSettingsServiceAsh* service =
        ash::OwnerSettingsServiceAshFactory::GetForBrowserContext(
            original_profile);
    service->IsOwnerAsync(base::BindOnce(&GetStorageAsync, original_profile,
                                         std::move(callback)));
  } else {
    GetStorageAsync(original_profile, std::move(callback),
                    /*current_user_is_owner=*/false);
  }
#else
  std::move(callback).Run(std::make_unique<flags_ui::PrefServiceFlagsStorage>(
                              g_browser_process->local_state()),
                          flags_ui::kOwnerAccessToFlags);
#endif
}

bool ShouldSkipConditionalFeatureEntry(const flags_ui::FlagsStorage* storage,
                                       const FeatureEntry& entry) {
  version_info::Channel channel = chrome::GetChannel();
#if BUILDFLAG(IS_CHROMEOS_ASH)
  // enable-projector-server-side-speech-recognition is only available if
  // the InternalServerSideSpeechRecognitionControl flag is enabled as well.
  if (!strcmp(kProjectorServerSideSpeechRecognition, entry.internal_name)) {
    return !ash::features::
        IsInternalServerSideSpeechRecognitionControlEnabled();
  }

  // enable-ui-devtools is only available on for non Stable channels.
  if (!strcmp(ui_devtools::switches::kEnableUiDevTools, entry.internal_name) &&
      channel == version_info::Channel::STABLE) {
    return true;
  }

  // Skip lacros-availability-policy always. This is a pseudo entry
  // and used to carry the policy value crossing the Chrome's lifetime.
  if (!strcmp(ash::standalone_browser::kLacrosAvailabilityPolicyInternalName,
              entry.internal_name)) {
    return true;
  }

  // Skip lacros-backward-data-migration-policy always. This is a pseudo entry
  // and used to carry the policy value crossing the Chrome's lifetime.
  if (!strcmp(crosapi::browser_util::
                  kLacrosDataBackwardMigrationModePolicyInternalName,
              entry.internal_name)) {
    return true;
  }
  // Skip lacros-selection if it is controlled by LacrosSelection policy.
  if (!strcmp(kLacrosSelectionInternalName, entry.internal_name)) {
    return crosapi::browser_util::GetCachedLacrosSelectionPolicy() !=
           crosapi::browser_util::LacrosSelectionPolicy::kUserChoice;
  }

  if (!strcmp(kPreferDcheckInternalName, entry.internal_name)) {
    return !crosapi::browser_util::IsLacrosAllowedToBeEnabled();
  }

  if (!strcmp(kLacrosStabilityInternalName, entry.internal_name)) {
    return !crosapi::browser_util::IsLacrosAllowedToBeEnabled();
  }

  if (!strcmp(kLacrosOnlyInternalName, entry.internal_name)) {
    return !crosapi::browser_util::IsLacrosOnlyFlagAllowed();
  }

  if (!strcmp(kArcEnableVirtioBlkForDataInternalName, entry.internal_name)) {
    return !arc::IsArcVmEnabled();
  }

  if (!strcmp(kEnableSuspendToDiskInternalName, entry.internal_name)) {
    // All Suspend to disk flags require that hibernate is supported.
    if (!ash::HibernateManager::IsHibernateSupported()) {
      return true;
    }

    return false;
  }

  // Only show Borealis flags on enabled devices.
  if (!strcmp(kBorealisBigGlInternalName, entry.internal_name)) {
    return !base::FeatureList::IsEnabled(features::kBorealis);
  }

  if (!strcmp(kBorealisDGPUInternalName, entry.internal_name)) {
    return !base::FeatureList::IsEnabled(features::kBorealis);
  }

  if (!strcmp(kBorealisForceBetaClientInternalName, entry.internal_name)) {
    return !base::FeatureList::IsEnabled(features::kBorealis);
  }

  if (!strcmp(kBorealisForceDoubleScaleInternalName, entry.internal_name)) {
    return !base::FeatureList::IsEnabled(features::kBorealis);
  }

  if (!strcmp(kBorealisLinuxModeInternalName, entry.internal_name)) {
    return !base::FeatureList::IsEnabled(features::kBorealis);
  }

  if (!strcmp(kBorealisPermittedInternalName, entry.internal_name)) {
    return !base::FeatureList::IsEnabled(features::kBorealis);
  }

  if (!strcmp(kBorealisProvisionInternalName, entry.internal_name)) {
    return !base::FeatureList::IsEnabled(features::kBorealis);
  }

  if (!strcmp(kBorealisScaleClientByDPIInternalName, entry.internal_name)) {
    return !base::FeatureList::IsEnabled(features::kBorealis);
  }

  if (!strcmp(kBorealisWebUIInstallerInternalName, entry.internal_name)) {
    return !base::FeatureList::IsEnabled(features::kBorealis);
  }

  // Only show wallpaper fast refresh flag if channel is one of
  // Dev/Canary/Unknown.
  if (!strcmp(kWallpaperFastRefreshInternalName, entry.internal_name)) {
    return (channel != version_info::Channel::DEV &&
            channel != version_info::Channel::CANARY &&
            channel != version_info::Channel::UNKNOWN);
  }

  // Only show clipboard history longpress flag if channel is one of
  // Beta/Dev/Canary/Unknown.
  if (!strcmp(kClipboardHistoryLongpressInternalName, entry.internal_name)) {
    return channel != version_info::Channel::BETA &&
           channel != version_info::Channel::DEV &&
           channel != version_info::Channel::CANARY &&
           channel != version_info::Channel::UNKNOWN;
  }

  // Disable and prevent users from enabling Floss on boards that were
  // explicitly built without it (b/228902194 for more info).
  if (!strcmp(kBluetoothUseFlossInternalName, entry.internal_name)) {
    return floss::features::IsFlossAvailabilityCheckNeeded() &&
           base::FeatureList::GetInstance()->IsFeatureOverriddenFromCommandLine(
               floss::features::kFlossIsAvailable.name,
               base::FeatureList::OVERRIDE_DISABLE_FEATURE);
  }

  // Only show Time of Day wallpaper flag if channel is one of
  // Dev/Canary/Unknown.
  if (!strcmp(kTimeOfDayWallpaperInternalName, entry.internal_name)) {
    return (channel != version_info::Channel::DEV &&
            channel != version_info::Channel::CANARY &&
            channel != version_info::Channel::UNKNOWN);
  }

  // Only show Time of Day Screen Saver flag if channel is one of
  // Dev/Canary/Unknown.
  if (!strcmp(kTimeOfDayScreenSaverInternalName, entry.internal_name)) {
    return (channel != version_info::Channel::DEV &&
            channel != version_info::Channel::CANARY &&
            channel != version_info::Channel::UNKNOWN);
  }

  // Only show glanceables flag if channel is one of Beta/Dev/Canary/Unknown.
  if (!strcmp(kGlanceablesV2InternalName, entry.internal_name)) {
    return channel != version_info::Channel::BETA &&
           channel != version_info::Channel::DEV &&
           channel != version_info::Channel::CANARY &&
           channel != version_info::Channel::UNKNOWN;
  }

  // Only show glanceables calendar view flag if channel is one of
  // Beta/Dev/Canary/Unknown.
  if (!strcmp(kGlanceablesV2CalendarViewInternalName, entry.internal_name)) {
    return channel != version_info::Channel::BETA &&
           channel != version_info::Channel::DEV &&
           channel != version_info::Channel::CANARY &&
           channel != version_info::Channel::UNKNOWN;
  }
#endif  // BUILDFLAG(IS_CHROMEOS_ASH)

  // enable-unsafe-webgpu is only available on Dev/Canary channels.
  if (!strcmp("enable-unsafe-webgpu", entry.internal_name) &&
      channel != version_info::Channel::DEV &&
      channel != version_info::Channel::CANARY &&
      channel != version_info::Channel::UNKNOWN) {
    return true;
  }

  if (flags::IsFlagExpired(storage, entry.internal_name)) {
    return true;
  }

  return false;
}

void ConvertFlagsToSwitches(flags_ui::FlagsStorage* flags_storage,
                            base::CommandLine* command_line,
                            flags_ui::SentinelsMode sentinels) {
  if (command_line->HasSwitch(switches::kNoExperiments)) {
    return;
  }

  FlagsStateSingleton::GetFlagsState()->ConvertFlagsToSwitches(
      flags_storage, command_line, sentinels, switches::kEnableFeatures,
      switches::kDisableFeatures);
}

std::vector<std::string> RegisterAllFeatureVariationParameters(
    flags_ui::FlagsStorage* flags_storage,
    base::FeatureList* feature_list) {
  return FlagsStateSingleton::GetFlagsState()
      ->RegisterAllFeatureVariationParameters(flags_storage, feature_list);
}

void GetFlagFeatureEntries(flags_ui::FlagsStorage* flags_storage,
                           flags_ui::FlagAccess access,
                           base::Value::List& supported_entries,
                           base::Value::List& unsupported_entries) {
  FlagsStateSingleton::GetFlagsState()->GetFlagFeatureEntries(
      flags_storage, access, supported_entries, unsupported_entries,
      base::BindRepeating(&ShouldSkipConditionalFeatureEntry,
                          // Unretained: this callback doesn't outlive this
                          // stack frame.
                          base::Unretained(flags_storage)));
}

void GetFlagFeatureEntriesForDeprecatedPage(
    flags_ui::FlagsStorage* flags_storage,
    flags_ui::FlagAccess access,
    base::Value::List& supported_entries,
    base::Value::List& unsupported_entries) {
  FlagsStateSingleton::GetFlagsState()->GetFlagFeatureEntries(
      flags_storage, access, supported_entries, unsupported_entries,
      base::BindRepeating(&ShouldSkipNonDeprecatedFeatureEntry));
}

flags_ui::FlagsState* GetCurrentFlagsState() {
  return FlagsStateSingleton::GetFlagsState();
}

bool IsRestartNeededToCommitChanges() {
  return FlagsStateSingleton::GetFlagsState()->IsRestartNeededToCommitChanges();
}

void SetFeatureEntryEnabled(flags_ui::FlagsStorage* flags_storage,
                            const std::string& internal_name,
                            bool enable) {
  FlagsStateSingleton::GetFlagsState()->SetFeatureEntryEnabled(
      flags_storage, internal_name, enable);
}

void SetOriginListFlag(const std::string& internal_name,
                       const std::string& value,
                       flags_ui::FlagsStorage* flags_storage) {
  FlagsStateSingleton::GetFlagsState()->SetOriginListFlag(internal_name, value,
                                                          flags_storage);
}

void SetStringFlag(const std::string& internal_name,
                   const std::string& value,
                   flags_ui::FlagsStorage* flags_storage) {
  FlagsStateSingleton::GetFlagsState()->SetStringFlag(internal_name, value,
                                                      flags_storage);
}

void RemoveFlagsSwitches(base::CommandLine::SwitchMap* switch_list) {
  FlagsStateSingleton::GetFlagsState()->RemoveFlagsSwitches(switch_list);
}

void ResetAllFlags(flags_ui::FlagsStorage* flags_storage) {
  FlagsStateSingleton::GetFlagsState()->ResetAllFlags(flags_storage);
}

#if BUILDFLAG(IS_CHROMEOS)
void CrosUrlFlagsRedirect() {
#if BUILDFLAG(IS_CHROMEOS_LACROS)
  lacros_url_handling::NavigateInAsh(GURL(chrome::kChromeUIFlagsURL));
#else
  // Note: This will only be called by the UI when Lacros is available.
  DCHECK(crosapi::BrowserManager::Get());
  crosapi::BrowserManager::Get()->SwitchToTab(
      GURL(chrome::kChromeUIFlagsURL),
      /*path_behavior=*/NavigateParams::RESPECT);
#endif
}
#endif

void RecordUMAStatistics(flags_ui::FlagsStorage* flags_storage,
                         const std::string& histogram_name) {
  std::set<std::string> switches;
  std::set<std::string> features;
  std::set<std::string> variation_ids;
  FlagsStateSingleton::GetFlagsState()->GetSwitchesAndFeaturesFromFlags(
      flags_storage, &switches, &features, &variation_ids);
  // Don't report variation IDs since we don't have an UMA histogram for them.
  flags_ui::ReportAboutFlagsHistogram(histogram_name, switches, features);
}

namespace testing {

std::vector<FeatureEntry>* GetEntriesForTesting() {
  static base::NoDestructor<std::vector<FeatureEntry>> entries;
  return entries.get();
}

void SetFeatureEntries(const std::vector<FeatureEntry>& entries) {
  CHECK(GetEntriesForTesting()->empty());  // IN-TEST
  for (const auto& entry : entries) {
    GetEntriesForTesting()->push_back(entry);  // IN-TEST
  }
  FlagsStateSingleton::GetInstance()->RebuildState(
      *GetEntriesForTesting());  // IN-TEST
}

ScopedFeatureEntries::ScopedFeatureEntries(
    const std::vector<flags_ui::FeatureEntry>& entries) {
  SetFeatureEntries(entries);
}

ScopedFeatureEntries::~ScopedFeatureEntries() {
  GetEntriesForTesting()->clear();  // IN-TEST
  // Restore the flag state to the production flags.
  FlagsStateSingleton::GetInstance()->RestoreDefaultState();
}

base::span<const FeatureEntry> GetFeatureEntries() {
  if (!GetEntriesForTesting()->empty()) {
    return base::span<FeatureEntry>(*GetEntriesForTesting());
  }
  return base::make_span(kFeatureEntries, std::size(kFeatureEntries));
}

}  // namespace testing

}  // namespace about_flags
