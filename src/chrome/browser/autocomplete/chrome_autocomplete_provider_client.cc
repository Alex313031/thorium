// Copyright 2023 The Chromium Authors and Alex313031
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "chrome/browser/autocomplete/chrome_autocomplete_provider_client.h"

#include <stddef.h>

#include <algorithm>

#include "base/callback_helpers.h"
#include "base/strings/utf_string_conversions.h"
#include "build/build_config.h"
#include "build/chromeos_buildflags.h"
#include "chrome/app/chrome_command_ids.h"
#include "chrome/browser/autocomplete/autocomplete_classifier_factory.h"
#include "chrome/browser/autocomplete/document_suggestions_service_factory.h"
#include "chrome/browser/autocomplete/in_memory_url_index_factory.h"
#include "chrome/browser/autocomplete/remote_suggestions_service_factory.h"
#include "chrome/browser/autocomplete/shortcuts_backend_factory.h"
#include "chrome/browser/autocomplete/zero_suggest_cache_service_factory.h"
#include "chrome/browser/bitmap_fetcher/bitmap_fetcher_service.h"
#include "chrome/browser/bitmap_fetcher/bitmap_fetcher_service_factory.h"
#include "chrome/browser/bookmarks/bookmark_model_factory.h"
#include "chrome/browser/browser_process.h"
#include "chrome/browser/chrome_notification_types.h"
#include "chrome/browser/history/history_service_factory.h"
#include "chrome/browser/history/top_sites_factory.h"
#include "chrome/browser/history_clusters/history_clusters_service_factory.h"
#include "chrome/browser/prefs/incognito_mode_prefs.h"
#include "chrome/browser/preloading/prefetch/search_prefetch/search_prefetch_service.h"
#include "chrome/browser/preloading/prefetch/search_prefetch/search_prefetch_service_factory.h"
#include "chrome/browser/profiles/profile.h"
#include "chrome/browser/profiles/profile_key.h"
#include "chrome/browser/query_tiles/tile_service_factory.h"
#include "chrome/browser/search_engines/template_url_service_factory.h"
#include "chrome/browser/signin/identity_manager_factory.h"
#include "chrome/browser/sync/sync_service_factory.h"
#include "chrome/browser/translate/chrome_translate_client.h"
#include "chrome/browser/ui/browser_command_controller.h"
#include "chrome/browser/ui/browser_finder.h"
#include "chrome/browser/ui/omnibox/omnibox_pedal_implementations.h"
#include "chrome/common/chrome_features.h"
#include "chrome/common/pref_names.h"
#include "chrome/common/url_constants.h"
#include "components/history/core/browser/history_service.h"
#include "components/history/core/browser/top_sites.h"
#include "components/history_clusters/core/features.h"
#include "components/language/core/browser/pref_names.h"
#include "components/omnibox/browser/actions/omnibox_pedal_provider.h"
#include "components/omnibox/browser/autocomplete_classifier.h"
#include "components/omnibox/browser/autocomplete_match.h"
#include "components/omnibox/browser/omnibox_field_trial.h"
#include "components/omnibox/browser/omnibox_triggered_feature_service.h"
#include "components/omnibox/browser/shortcuts_backend.h"
#include "components/omnibox/browser/tab_matcher.h"
#include "components/omnibox/common/omnibox_features.h"
#include "components/prefs/pref_service.h"
#include "components/signin/public/identity_manager/accounts_in_cookie_jar_info.h"
#include "components/signin/public/identity_manager/identity_manager.h"
#include "components/sync/driver/sync_service.h"
#include "components/translate/core/browser/translate_manager.h"
#include "components/unified_consent/url_keyed_data_collection_consent_helper.h"
#include "content/public/browser/navigation_entry.h"
#include "content/public/browser/notification_service.h"
#include "content/public/browser/service_worker_context.h"
#include "content/public/browser/storage_partition.h"
#include "content/public/browser/web_contents.h"
#include "content/public/browser/web_contents_user_data.h"
#include "extensions/buildflags/buildflags.h"
#include "net/traffic_annotation/network_traffic_annotation.h"
#include "third_party/blink/public/common/storage_key/storage_key.h"
#include "url/origin.h"

#if BUILDFLAG(ENABLE_EXTENSIONS)
#include "chrome/browser/autocomplete/keyword_extensions_delegate_impl.h"
#endif

#if !BUILDFLAG(IS_ANDROID)
#include "chrome/browser/sharing_hub/sharing_hub_features.h"
#include "chrome/browser/ui/browser.h"
#include "chrome/browser/ui/browser_commands.h"
#include "chrome/browser/ui/browser_list.h"
#include "chrome/browser/ui/tabs/tab_strip_model.h"
#include "chrome/browser/ui/ui_features.h"
#include "chrome/browser/ui/views/side_panel/history_clusters/history_clusters_side_panel_coordinator.h"
#include "chrome/browser/upgrade_detector/upgrade_detector.h"
#endif

namespace {

#if !BUILDFLAG(IS_ANDROID)
// This list should be kept in sync with chrome/common/webui_url_constants.h.
// Only include useful sub-pages, confirmation alerts are not useful.
const char* const kChromeSettingsSubPages[] = {
    chrome::kAddressesSubPage,        chrome::kAutofillSubPage,
    chrome::kClearBrowserDataSubPage, chrome::kContentSettingsSubPage,
    chrome::kLanguageOptionsSubPage,  chrome::kPasswordManagerSubPage,
    chrome::kPaymentsSubPage,         chrome::kResetProfileSettingsSubPage,
    chrome::kSearchEnginesSubPage,    chrome::kSyncSetupSubPage,
#if !BUILDFLAG(IS_CHROMEOS_ASH)
    chrome::kImportDataSubPage,       chrome::kManageProfileSubPage,
    chrome::kPeopleSubPage,
#endif
};
#endif  // BUILDFLAG(IS_ANDROID)

}  // namespace

ChromeAutocompleteProviderClient::ChromeAutocompleteProviderClient(
    Profile* profile)
    : profile_(profile),
      scheme_classifier_(profile),
      url_consent_helper_(unified_consent::UrlKeyedDataCollectionConsentHelper::
                              NewPersonalizedDataCollectionConsentHelper(
                                  SyncServiceFactory::GetForProfile(profile_))),
      tab_matcher_(GetTemplateURLService(), profile_),
      storage_partition_(nullptr),
      omnibox_triggered_feature_service_(
          std::make_unique<OmniboxTriggeredFeatureService>()) {
  pedal_provider_ = std::make_unique<OmniboxPedalProvider>(
      *this, GetPedalImplementations(IsOffTheRecord(), false));
}

ChromeAutocompleteProviderClient::~ChromeAutocompleteProviderClient() = default;

scoped_refptr<network::SharedURLLoaderFactory>
ChromeAutocompleteProviderClient::GetURLLoaderFactory() {
  return profile_->GetDefaultStoragePartition()
      ->GetURLLoaderFactoryForBrowserProcess();
}

PrefService* ChromeAutocompleteProviderClient::GetPrefs() const {
  return profile_->GetPrefs();
}

PrefService* ChromeAutocompleteProviderClient::GetLocalState() {
  return g_browser_process->local_state();
}

std::string ChromeAutocompleteProviderClient::GetApplicationLocale() const {
  return g_browser_process->GetApplicationLocale();
}

const AutocompleteSchemeClassifier&
ChromeAutocompleteProviderClient::GetSchemeClassifier() const {
  return scheme_classifier_;
}

AutocompleteClassifier*
ChromeAutocompleteProviderClient::GetAutocompleteClassifier() {
  return AutocompleteClassifierFactory::GetForProfile(profile_);
}

history::HistoryService* ChromeAutocompleteProviderClient::GetHistoryService() {
  return HistoryServiceFactory::GetForProfile(
      profile_, ServiceAccessType::EXPLICIT_ACCESS);
}

history_clusters::HistoryClustersService*
ChromeAutocompleteProviderClient::GetHistoryClustersService() {
  return HistoryClustersServiceFactory::GetForBrowserContext(profile_);
}

scoped_refptr<history::TopSites>
ChromeAutocompleteProviderClient::GetTopSites() {
  return TopSitesFactory::GetForProfile(profile_);
}

bookmarks::BookmarkModel* ChromeAutocompleteProviderClient::GetBookmarkModel() {
  return BookmarkModelFactory::GetForBrowserContext(profile_);
}

history::URLDatabase* ChromeAutocompleteProviderClient::GetInMemoryDatabase() {
  history::HistoryService* history_service = GetHistoryService();

  // This method is called in unit test contexts where the HistoryService isn't
  // loaded.
  return history_service ? history_service->InMemoryDatabase() : nullptr;
}

InMemoryURLIndex* ChromeAutocompleteProviderClient::GetInMemoryURLIndex() {
  return InMemoryURLIndexFactory::GetForProfile(profile_);
}

TemplateURLService* ChromeAutocompleteProviderClient::GetTemplateURLService() {
  return TemplateURLServiceFactory::GetForProfile(profile_);
}

const TemplateURLService*
ChromeAutocompleteProviderClient::GetTemplateURLService() const {
  return TemplateURLServiceFactory::GetForProfile(profile_);
}

RemoteSuggestionsService*
ChromeAutocompleteProviderClient::GetRemoteSuggestionsService(
    bool create_if_necessary) const {
  return RemoteSuggestionsServiceFactory::GetForProfile(profile_,
                                                        create_if_necessary);
}

DocumentSuggestionsService*
ChromeAutocompleteProviderClient::GetDocumentSuggestionsService(
    bool create_if_necessary) const {
  return DocumentSuggestionsServiceFactory::GetForProfile(profile_,
                                                          create_if_necessary);
}

ZeroSuggestCacheService*
ChromeAutocompleteProviderClient::GetZeroSuggestCacheService() {
  return ZeroSuggestCacheServiceFactory::GetForProfile(profile_);
}

const ZeroSuggestCacheService*
ChromeAutocompleteProviderClient::GetZeroSuggestCacheService() const {
  return ZeroSuggestCacheServiceFactory::GetForProfile(profile_);
}

OmniboxPedalProvider* ChromeAutocompleteProviderClient::GetPedalProvider()
    const {
  // This may be null for systems that don't have Pedals (Android, e.g.).
  return pedal_provider_.get();
}

scoped_refptr<ShortcutsBackend>
ChromeAutocompleteProviderClient::GetShortcutsBackend() {
  return ShortcutsBackendFactory::GetForProfile(profile_);
}

scoped_refptr<ShortcutsBackend>
ChromeAutocompleteProviderClient::GetShortcutsBackendIfExists() {
  return ShortcutsBackendFactory::GetForProfileIfExists(profile_);
}

std::unique_ptr<KeywordExtensionsDelegate>
ChromeAutocompleteProviderClient::GetKeywordExtensionsDelegate(
    KeywordProvider* keyword_provider) {
#if BUILDFLAG(ENABLE_EXTENSIONS)
  return std::make_unique<KeywordExtensionsDelegateImpl>(profile_,
                                                         keyword_provider);
#else
  return nullptr;
#endif
}

std::string ChromeAutocompleteProviderClient::GetAcceptLanguages() const {
  return profile_->GetPrefs()->GetString(language::prefs::kAcceptLanguages);
}

std::string
ChromeAutocompleteProviderClient::GetEmbedderRepresentationOfAboutScheme()
    const {
  return content::kChromeUIScheme;
}

std::vector<std::u16string> ChromeAutocompleteProviderClient::GetBuiltinURLs() {
  std::vector<std::string> chrome_builtins(
      chrome::kChromeHostURLs,
      chrome::kChromeHostURLs + chrome::kNumberOfChromeHostURLs);
  std::sort(chrome_builtins.begin(), chrome_builtins.end());

  std::vector<std::u16string> builtins;

  for (auto i(chrome_builtins.begin()); i != chrome_builtins.end(); ++i)
    builtins.push_back(base::ASCIIToUTF16(*i));

#if !BUILDFLAG(IS_ANDROID)
  std::u16string settings(base::ASCIIToUTF16(chrome::kChromeUISettingsHost) +
                          u"/");
  for (size_t i = 0; i < std::size(kChromeSettingsSubPages); i++) {
    builtins.push_back(settings +
                       base::ASCIIToUTF16(kChromeSettingsSubPages[i]));
  }
#endif

  return builtins;
}

std::vector<std::u16string>
ChromeAutocompleteProviderClient::GetBuiltinsToProvideAsUserTypes() {
  std::vector<std::u16string> builtins_to_provide;
  builtins_to_provide.push_back(
      base::ASCIIToUTF16(chrome::kChromeUIFlagsURL));
  builtins_to_provide.push_back(
      base::ASCIIToUTF16(chrome::kChromeUIChromeURLsURL));
#if !BUILDFLAG(IS_ANDROID)
  builtins_to_provide.push_back(
      base::ASCIIToUTF16(chrome::kChromeUISettingsURL));
#endif
  builtins_to_provide.push_back(
      base::ASCIIToUTF16(chrome::kChromeUIVersionURL));
  return builtins_to_provide;
}

component_updater::ComponentUpdateService*
ChromeAutocompleteProviderClient::GetComponentUpdateService() {
  return g_browser_process->component_updater();
}

query_tiles::TileService*
ChromeAutocompleteProviderClient::GetQueryTileService() const {
  ProfileKey* profile_key = profile_->GetProfileKey();
  return query_tiles::TileServiceFactory::GetForKey(profile_key);
}

OmniboxTriggeredFeatureService*
ChromeAutocompleteProviderClient::GetOmniboxTriggeredFeatureService() const {
  return omnibox_triggered_feature_service_.get();
}

signin::IdentityManager* ChromeAutocompleteProviderClient::GetIdentityManager()
    const {
  return IdentityManagerFactory::GetForProfile(profile_);
}

bool ChromeAutocompleteProviderClient::IsOffTheRecord() const {
  return profile_->IsOffTheRecord();
}

bool ChromeAutocompleteProviderClient::SearchSuggestEnabled() const {
  return profile_->GetPrefs()->GetBoolean(prefs::kSearchSuggestEnabled);
}

bool ChromeAutocompleteProviderClient::AllowDeletingBrowserHistory() const {
  return profile_->GetPrefs()->GetBoolean(prefs::kAllowDeletingBrowserHistory);
}

bool ChromeAutocompleteProviderClient::IsPersonalizedUrlDataCollectionActive()
    const {
  return url_consent_helper_->IsEnabled();
}

bool ChromeAutocompleteProviderClient::IsAuthenticated() const {
  const auto* identity_manager =
      IdentityManagerFactory::GetForProfile(profile_);
  return identity_manager &&
         !identity_manager->GetAccountsInCookieJar().signed_in_accounts.empty();
}

bool ChromeAutocompleteProviderClient::IsSyncActive() const {
  syncer::SyncService* sync = SyncServiceFactory::GetForProfile(profile_);
  return sync && sync->IsSyncFeatureActive();
}

std::string ChromeAutocompleteProviderClient::ProfileUserName() const {
  return profile_->GetProfileUserName();
}

void ChromeAutocompleteProviderClient::Classify(
    const std::u16string& text,
    bool prefer_keyword,
    bool allow_exact_keyword_match,
    metrics::OmniboxEventProto::PageClassification page_classification,
    AutocompleteMatch* match,
    GURL* alternate_nav_url) {
  AutocompleteClassifier* classifier = GetAutocompleteClassifier();
  DCHECK(classifier);
  classifier->Classify(text, prefer_keyword, allow_exact_keyword_match,
                       page_classification, match, alternate_nav_url);
}

void ChromeAutocompleteProviderClient::DeleteMatchingURLsForKeywordFromHistory(
    history::KeywordID keyword_id,
    const std::u16string& term) {
  GetHistoryService()->DeleteMatchingURLsForKeyword(keyword_id, term);
}

void ChromeAutocompleteProviderClient::PrefetchImage(const GURL& url) {
  // Note: Android uses different image fetching mechanism to avoid
  // penalty of copying byte buffers from C++ to Java.
#if !BUILDFLAG(IS_ANDROID)
  BitmapFetcherService* bitmap_fetcher_service =
      BitmapFetcherServiceFactory::GetForBrowserContext(profile_);
  bitmap_fetcher_service->Prefetch(url);
#endif  // !BUILDFLAG(IS_ANDROID)
}

void ChromeAutocompleteProviderClient::StartServiceWorker(
    const GURL& destination_url) {
  if (!SearchSuggestEnabled())
    return;

  if (profile_->IsOffTheRecord())
    return;

  content::StoragePartition* partition = storage_partition_;
  if (!partition)
    partition = profile_->GetDefaultStoragePartition();
  if (!partition)
    return;

  content::ServiceWorkerContext* context = partition->GetServiceWorkerContext();
  if (!context)
    return;

  context->StartServiceWorkerForNavigationHint(
      destination_url, blink::StorageKey(url::Origin::Create(destination_url)),
      base::DoNothing());
}

const TabMatcher& ChromeAutocompleteProviderClient::GetTabMatcher() const {
  return tab_matcher_;
}

bool ChromeAutocompleteProviderClient::IsIncognitoModeAvailable() const {
  return IncognitoModePrefs::IsIncognitoAllowed(profile_);
}

bool ChromeAutocompleteProviderClient::IsSharingHubAvailable() const {
#if !BUILDFLAG(IS_ANDROID)
  return sharing_hub::SharingHubOmniboxEnabled(profile_);
#else
  return false;
#endif
}

base::WeakPtr<AutocompleteProviderClient>
ChromeAutocompleteProviderClient::GetWeakPtr() {
  return weak_ptr_factory_.GetWeakPtr();
}

bool ChromeAutocompleteProviderClient::StrippedURLsAreEqual(
    const GURL& url1,
    const GURL& url2,
    const AutocompleteInput* input) const {
  AutocompleteInput empty_input;
  if (!input)
    input = &empty_input;
  const TemplateURLService* template_url_service = GetTemplateURLService();
  return AutocompleteMatch::GURLToStrippedGURL(
             url1, *input, template_url_service, std::u16string(),
             /*keep_search_intent_params=*/false) ==
         AutocompleteMatch::GURLToStrippedGURL(
             url2, *input, template_url_service, std::u16string(),
             /*keep_search_intent_params=*/false);
}

void ChromeAutocompleteProviderClient::OpenSharingHub() {
#if !BUILDFLAG(IS_ANDROID)
  Browser* browser = BrowserList::GetInstance()->GetLastActive();
  if (browser) {
    browser->command_controller()->ExecuteCommand(IDC_SHARING_HUB);
  }
#endif  // !BUILDFLAG(IS_ANDROID)
}

void ChromeAutocompleteProviderClient::NewIncognitoWindow() {
#if !BUILDFLAG(IS_ANDROID)
  chrome::NewIncognitoWindow(profile_);
#endif  // !BUILDFLAG(IS_ANDROID)
}

void ChromeAutocompleteProviderClient::OpenIncognitoClearBrowsingDataDialog() {
#if !BUILDFLAG(IS_ANDROID)
  Browser* browser = BrowserList::GetInstance()->GetLastActive();
  if (browser) {
    chrome::ShowIncognitoClearBrowsingDataDialog(browser);
  }
#endif  // !BUILDFLAG(IS_ANDROID)
}

void ChromeAutocompleteProviderClient::CloseIncognitoWindows() {
#if !BUILDFLAG(IS_ANDROID)
  if (profile_->IsIncognitoProfile()) {
    BrowserList::CloseAllBrowsersWithIncognitoProfile(
        profile_, base::DoNothing(), base::DoNothing(), true);
  }
#endif  // !BUILDFLAG(IS_ANDROID)
}

bool ChromeAutocompleteProviderClient::OpenJourneys(const std::string& query) {
#if !BUILDFLAG(IS_ANDROID)
  if (!base::FeatureList::IsEnabled(features::kUnifiedSidePanel) ||
      !base::FeatureList::IsEnabled(history_clusters::kSidePanelJourneys) ||
      !history_clusters::kSidePanelJourneysOpensFromOmnibox.Get()) {
    return false;
  }

  Browser* browser = BrowserList::GetInstance()->GetLastActive();
  if (!browser)
    return false;

  if (auto* history_clusters_side_panel_coordinator =
          HistoryClustersSidePanelCoordinator::BrowserUserData::FromBrowser(
              browser)) {
    history_clusters_side_panel_coordinator->Show(query);
    return true;
  }

#endif  // !BUILDFLAG(IS_ANDROID)

  return false;
}

void ChromeAutocompleteProviderClient::PromptPageTranslation() {
#if !BUILDFLAG(IS_ANDROID)
  Browser* browser = BrowserList::GetInstance()->GetLastActive();
  content::WebContents* contents = nullptr;
  if (browser)
    contents = browser->tab_strip_model()->GetActiveWebContents();
  if (contents) {
    ChromeTranslateClient* translate_client =
        ChromeTranslateClient::FromWebContents(contents);
    if (translate_client) {
      DCHECK_NE(nullptr, translate_client->GetTranslateManager());
      translate_client->GetTranslateManager()->ShowTranslateUI(
          /*auto_translate=*/true, /*triggered_from_menu=*/true);
    }
  }
#endif  // !BUILDFLAG(IS_ANDROID)
}
