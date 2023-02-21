// Copyright 2020 The Chromium Authors
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "components/privacy_sandbox/privacy_sandbox_settings.h"

#include "base/feature_list.h"
#include "base/json/values_util.h"
#include "base/observer_list.h"
#include "base/ranges/algorithm.h"
#include "base/time/time.h"
#include "components/content_settings/core/browser/cookie_settings.h"
#include "components/content_settings/core/browser/host_content_settings_map.h"
#include "components/content_settings/core/common/pref_names.h"
#include "components/prefs/pref_service.h"
#include "components/prefs/scoped_user_pref_update.h"
#include "components/privacy_sandbox/privacy_sandbox_features.h"
#include "components/privacy_sandbox/privacy_sandbox_prefs.h"
#include "content/public/common/content_features.h"
#include "net/base/registry_controlled_domains/registry_controlled_domain.h"
#include "net/cookies/site_for_cookies.h"
#include "url/gurl.h"
#include "url/origin.h"

namespace privacy_sandbox {

namespace {

constexpr char kBlockedTopicsTopicKey[] = "topic";
constexpr char kBlockedTopicsBlockTimeKey[] = "blockedOn";

bool IsCookiesClearOnExitEnabled(HostContentSettingsMap* map) {
  return map->GetDefaultContentSetting(ContentSettingsType::COOKIES,
                                       /*provider_id=*/nullptr) ==
         ContentSetting::CONTENT_SETTING_SESSION_ONLY;
}

// Convert a stored FLEDGE block eTLD+1 into applicable content settings
// patterns. This ensures that if Public Suffix List membership changes, the
// stored item continues to match as when it was set. Multiple patterns are set
// to support IP address fallbacks, which do not support [*.] prefixes.
// TODO (crbug.com/1287153): This is somewhat hacky and can be removed when
// FLEDGE is controlled by a content setting directly.
std::vector<ContentSettingsPattern> FledgeBlockToContentSettingsPatterns(
    const std::string& entry) {
  return {ContentSettingsPattern::FromString("[*.]" + entry),
          ContentSettingsPattern::FromString(entry)};
}

// Returns a base::Value for storage in prefs that represents |topic| blocked
// at the current time.
base::Value CreateBlockedTopicEntry(const CanonicalTopic& topic) {
  base::Value entry(base::Value::Type::DICTIONARY);
  entry.SetKey(kBlockedTopicsTopicKey, topic.ToValue());
  entry.SetKey(kBlockedTopicsBlockTimeKey,
               base::TimeToValue(base::Time::Now()));
  return entry;
}

}  // namespace

PrivacySandboxSettings::PrivacySandboxSettings(
    std::unique_ptr<Delegate> delegate,
    HostContentSettingsMap* host_content_settings_map,
    scoped_refptr<content_settings::CookieSettings> cookie_settings,
    PrefService* pref_service)
    : delegate_(std::move(delegate)),
      host_content_settings_map_(host_content_settings_map),
      cookie_settings_(cookie_settings),
      pref_service_(pref_service) {
  DCHECK(pref_service_);
  DCHECK(host_content_settings_map_);
  DCHECK(cookie_settings_);
  // "Clear on exit" causes a cookie deletion on shutdown. But for practical
  // purposes, we're notifying the observers on startup (which should be
  // equivalent, as no cookie operations could have happened while the profile
  // was shut down).
  if (IsCookiesClearOnExitEnabled(host_content_settings_map_))
    OnCookiesCleared();

  pref_change_registrar_.Init(pref_service_);
  pref_change_registrar_.Add(
      prefs::kPrivacySandboxApisEnabledV2,
      base::BindRepeating(&PrivacySandboxSettings::OnPrivacySandboxPrefChanged,
                          base::Unretained(this)));
  pref_change_registrar_.Add(
      prefs::kPrivacySandboxFirstPartySetsEnabled,
      base::BindRepeating(
          &PrivacySandboxSettings::OnFirstPartySetsEnabledPrefChanged,
          base::Unretained(this)));
}

PrivacySandboxSettings::~PrivacySandboxSettings() = default;

bool PrivacySandboxSettings::IsTopicsAllowed() const {
  // M1 specific
  if (base::FeatureList::IsEnabled(privacy_sandbox::kPrivacySandboxSettings4)) {
    return IsM1PrivacySandboxApiEnabled(prefs::kPrivacySandboxM1TopicsEnabled);
  }

  // Topics API calculation should be prevented if the user has blocked 3PC
  // cookies, as there will be no context specific check.
  const auto cookie_controls_mode =
      static_cast<content_settings::CookieControlsMode>(
          pref_service_->GetInteger(prefs::kCookieControlsMode));
  const auto default_content_setting =
      cookie_settings_->GetDefaultCookieSetting(/*provider_id=*/nullptr);

  const bool third_party_cookies_blocked =
      default_content_setting == ContentSetting::CONTENT_SETTING_BLOCK ||
      cookie_controls_mode ==
          content_settings::CookieControlsMode::kBlockThirdParty;
  return IsPrivacySandboxEnabled() && !third_party_cookies_blocked;
}

bool PrivacySandboxSettings::IsTopicsAllowedForContext(
    const GURL& url,
    const absl::optional<url::Origin>& top_frame_origin) const {
  // M1 specific
  if (base::FeatureList::IsEnabled(privacy_sandbox::kPrivacySandboxSettings4)) {
    return IsTopicsAllowed() && IsSiteDataAllowed(url);
  }

  // If the Topics API is disabled completely, it is not available in any
  // context.
  return IsTopicsAllowed() &&
         IsPrivacySandboxEnabledForContext(url, top_frame_origin);
}

bool PrivacySandboxSettings::IsTopicAllowed(const CanonicalTopic& topic) {
  const auto& blocked_topics =
      pref_service_->GetList(prefs::kPrivacySandboxBlockedTopics);

  for (const auto& item : blocked_topics) {
    auto blocked_topic =
        CanonicalTopic::FromValue(*item.GetDict().Find(kBlockedTopicsTopicKey));
    if (!blocked_topic)
      continue;

    if (topic == *blocked_topic)
      return false;
  }
  return true;
}

void PrivacySandboxSettings::SetTopicAllowed(const CanonicalTopic& topic,
                                             bool allowed) {
  ScopedListPrefUpdate scoped_pref_update(pref_service_,
                                          prefs::kPrivacySandboxBlockedTopics);

  // Presence in the preference list indicates that a topic is blocked, as
  // there is no concept of explicitly allowed topics. Thus, allowing a topic
  // is the same as removing it, if it exists, from the blocklist. Blocking
  // a topic is the same as adding it to the blocklist, but as duplicate entries
  // are undesireable, removing any existing reference first is desireable.
  // Thus, regardless of |allowed|, removing any existing reference is the
  // first step.
  scoped_pref_update->EraseIf([&](const base::Value& value) {
    auto* blocked_topic_value = value.GetDict().Find(kBlockedTopicsTopicKey);
    auto converted_topic = CanonicalTopic::FromValue(*blocked_topic_value);
    return converted_topic && *converted_topic == topic;
  });

  // If the topic is being blocked, it can be (re)added to the blocklist. If the
  // topic was removed from the blocklist above, this is equivalent to updating
  // the modified time associated with the entry to the current time. As data
  // deletions are typically from the current time backwards, this makes it
  // more likely to be removed - a privacy improvement.
  if (!allowed)
    scoped_pref_update->Append(CreateBlockedTopicEntry(topic));
}

void PrivacySandboxSettings::ClearTopicSettings(base::Time start_time,
                                                base::Time end_time) {
  ScopedListPrefUpdate scoped_pref_update(pref_service_,
                                          prefs::kPrivacySandboxBlockedTopics);

  // Shortcut for maximum time range deletion.
  if (start_time == base::Time() && end_time == base::Time::Max()) {
    scoped_pref_update->clear();
    return;
  }

  scoped_pref_update->EraseIf([&](const base::Value& value) {
    auto blocked_time =
        base::ValueToTime(value.GetDict().Find(kBlockedTopicsBlockTimeKey));
    return start_time <= blocked_time && blocked_time <= end_time;
  });
}

base::Time PrivacySandboxSettings::TopicsDataAccessibleSince() const {
  return pref_service_->GetTime(
      prefs::kPrivacySandboxTopicsDataAccessibleSince);
}

bool PrivacySandboxSettings::IsAttributionReportingAllowed(
    const url::Origin& top_frame_origin,
    const url::Origin& reporting_origin) const {
  return IsPrivacySandboxEnabledForContext(reporting_origin.GetURL(),
                                           top_frame_origin);
}

bool PrivacySandboxSettings::MaySendAttributionReport(
    const url::Origin& source_origin,
    const url::Origin& destination_origin,
    const url::Origin& reporting_origin) const {
  // The |reporting_origin| needs to have been accessible in both source
  // and trigger contexts. These are both checked when they occur, but
  // user settings may have changed between then and when the attribution report
  // is sent.
  return IsPrivacySandboxEnabledForContext(reporting_origin.GetURL(),
                                           source_origin) &&
         IsPrivacySandboxEnabledForContext(reporting_origin.GetURL(),
                                           destination_origin);
}

void PrivacySandboxSettings::SetFledgeJoiningAllowed(
    const std::string& top_frame_etld_plus1,
    bool allowed) {
  ScopedDictPrefUpdate scoped_pref_update(
      pref_service_, prefs::kPrivacySandboxFledgeJoinBlocked);

  // Ensure that the provided etld_plus1 actually is an etld+1.
  auto effective_top_frame_etld_plus1 =
      net::registry_controlled_domains::GetDomainAndRegistry(
          top_frame_etld_plus1,
          net::registry_controlled_domains::INCLUDE_PRIVATE_REGISTRIES);

  // Hosts are also accepted as a fallback. This may occur if the private
  // registry has changed, and what the caller may be assuming is an eTLD+1 no
  // longer is. Simply ignoring non-eTLD+1's may thus result in unexpected
  // access.
  if (effective_top_frame_etld_plus1 != top_frame_etld_plus1) {
    // Add a dummy scheme and use GURL to confirm the provided string is a valid
    // host.
    const GURL url("https://" + top_frame_etld_plus1);
    effective_top_frame_etld_plus1 = url.host();
  }

  // Ignore attempts to configure an empty etld+1. This will also catch the
  // case where the eTLD+1 was not even a host, as GURL will have canonicalised
  // it to empty.
  if (effective_top_frame_etld_plus1.length() == 0) {
    NOTREACHED() << "Cannot control FLEDGE joining for empty eTLD+1";
    return;
  }

  if (allowed) {
    // Existence of the key implies blocking, so simply removing the key is
    // sufficient. If the key wasn't already present, the following is a no-op.
    scoped_pref_update->Remove(effective_top_frame_etld_plus1);
  } else {
    // Overriding the creation date for keys which already exist is acceptable.
    // Time range based deletions are typically started from the current time,
    // and so this will be more aggressively removed. This decreases the chance
    // a potentially sensitive website remains in preferences.
    scoped_pref_update->Set(effective_top_frame_etld_plus1,
                            base::TimeToValue(base::Time::Now()));
  }
}

void PrivacySandboxSettings::ClearFledgeJoiningAllowedSettings(
    base::Time start_time,
    base::Time end_time) {
  ScopedDictPrefUpdate scoped_pref_update(
      pref_service_, prefs::kPrivacySandboxFledgeJoinBlocked);
  auto& pref_data = scoped_pref_update.Get();

  // Shortcut for maximum time range deletion
  if (start_time == base::Time() && end_time == base::Time::Max()) {
    pref_data.clear();
    return;
  }

  std::vector<std::string> keys_to_remove;
  for (auto entry : pref_data) {
    absl::optional<base::Time> created_time = base::ValueToTime(entry.second);
    if (created_time.has_value() && start_time <= created_time &&
        created_time <= end_time) {
      keys_to_remove.push_back(entry.first);
    }
  }

  for (const auto& key : keys_to_remove)
    pref_data.Remove(key);
}

bool PrivacySandboxSettings::IsFledgeJoiningAllowed(
    const url::Origin& top_frame_origin) const {
  ScopedDictPrefUpdate scoped_pref_update(
      pref_service_, prefs::kPrivacySandboxFledgeJoinBlocked);
  auto& pref_data = scoped_pref_update.Get();
  for (auto entry : pref_data) {
    if (base::ranges::any_of(FledgeBlockToContentSettingsPatterns(entry.first),
                             [&](const auto& pattern) {
                               return pattern.Matches(
                                   top_frame_origin.GetURL());
                             })) {
      return false;
    }
  }
  return true;
}

bool PrivacySandboxSettings::IsFledgeAllowed(
    const url::Origin& top_frame_origin,
    const url::Origin& auction_party) const {
  if (base::FeatureList::IsEnabled(privacy_sandbox::kPrivacySandboxSettings4)) {
    return IsM1PrivacySandboxApiEnabled(
               prefs::kPrivacySandboxM1FledgeEnabled) &&
           IsSiteDataAllowed(auction_party.GetURL());
  }

  return IsPrivacySandboxEnabledForContext(auction_party.GetURL(),
                                           top_frame_origin);
}

bool PrivacySandboxSettings::IsSharedStorageAllowed(
    const url::Origin& top_frame_origin,
    const url::Origin& accessing_origin) const {
  // Ensures that Shared Storage is only allowed if both Privacy Sandbox is
  // enabled and full cookie access is enabled for this context.
  return IsPrivacySandboxEnabledForContext(accessing_origin.GetURL(),
                                           top_frame_origin);
}

bool PrivacySandboxSettings::IsSharedStorageWriteAllowed(
    const url::Origin& top_frame_origin,
    const url::Origin& accessing_origin) {
  // TODO(crbug.com/1378703): Respect appropriate M1 pref and site data settings
  // when release 4 is enabled.
  return IsSharedStorageAllowed(top_frame_origin, accessing_origin);
}
bool PrivacySandboxSettings::IsSharedStorageSelectURLAllowed(
    const url::Origin& top_frame_origin,
    const url::Origin& accessing_origin) {
  // TODO(crbug.com/1378703): Respect appropriate M1 pref and site data settings
  // when release 4 is enabled.
  return IsSharedStorageAllowed(top_frame_origin, accessing_origin);
}
bool PrivacySandboxSettings::IsSharedStorageAggregateReportingAllowed(
    const url::Origin& top_frame_origin,
    const url::Origin& accessing_origin) {
  // TODO(crbug.com/1378703): Respect appropriate M1 pref and site data settings
  // when release 4 is enabled.
  return IsSharedStorageAllowed(top_frame_origin, accessing_origin);
}

bool PrivacySandboxSettings::IsPrivateAggregationAllowed(
    const url::Origin& top_frame_origin,
    const url::Origin& reporting_origin) const {
  return IsPrivacySandboxEnabledForContext(reporting_origin.GetURL(),
                                           top_frame_origin);
}

bool PrivacySandboxSettings::IsPrivacySandboxEnabled() const {
  // If the delegate is restricting access the Privacy Sandbox is disabled.
  if (delegate_->IsPrivacySandboxRestricted())
    return false;

  if (delegate_->IsIncognitoProfile())
    return false;

  // For Measurement and Relevance APIs, we explicitly do not require the
  // underlying pref to be enabled if there is a local flag enabling the APIs to
  // allow for local testing.
  if (base::FeatureList::IsEnabled(
          privacy_sandbox::kOverridePrivacySandboxSettingsLocalTesting)) {
    return true;
  }

  return pref_service_->GetBoolean(prefs::kPrivacySandboxApisEnabledV2);
}

void PrivacySandboxSettings::SetPrivacySandboxEnabled(bool enabled) {
  pref_service_->SetBoolean(prefs::kPrivacySandboxApisEnabledV2, enabled);
}

bool PrivacySandboxSettings::IsTrustTokensAllowed() {
  return IsPrivacySandboxEnabled();
}

bool PrivacySandboxSettings::IsPrivacySandboxRestricted() const {
  return delegate_->IsPrivacySandboxRestricted();
}

void PrivacySandboxSettings::OnCookiesCleared() {
  SetTopicsDataAccessibleFromNow();
}

void PrivacySandboxSettings::OnPrivacySandboxPrefChanged() {
  for (auto& observer : observers_)
    observer.OnTrustTokenBlockingChanged(!IsTrustTokensAllowed());
}

void PrivacySandboxSettings::OnFirstPartySetsEnabledPrefChanged() {
  if (!base::FeatureList::IsEnabled(features::kFirstPartySets))
    return;

  for (auto& observer : observers_) {
    observer.OnFirstPartySetsEnabledChanged(
        pref_service_->GetBoolean(prefs::kPrivacySandboxFirstPartySetsEnabled));
  }
}

void PrivacySandboxSettings::AddObserver(Observer* observer) {
  observers_.AddObserver(observer);
}

void PrivacySandboxSettings::RemoveObserver(Observer* observer) {
  observers_.RemoveObserver(observer);
}

void PrivacySandboxSettings::SetDelegateForTesting(
    std::unique_ptr<Delegate> delegate) {
  delegate_ = std::move(delegate);
}

PrivacySandboxSettings::PrivacySandboxSettings() = default;

bool PrivacySandboxSettings::IsPrivacySandboxEnabledForContext(
    const GURL& url,
    const absl::optional<url::Origin>& top_frame_origin) const {
  if (!IsPrivacySandboxEnabled())
    return false;

  // Third party cookies must also be available for this context. An empty site
  // for cookies is provided so the context is always treated as a third party.
  return cookie_settings_->IsFullCookieAccessAllowed(
      url, net::SiteForCookies(), top_frame_origin,
      content_settings::CookieSettings::QueryReason::kPrivacySandbox);
}

void PrivacySandboxSettings::SetTopicsDataAccessibleFromNow() const {
  pref_service_->SetTime(prefs::kPrivacySandboxTopicsDataAccessibleSince,
                         base::Time::Now());

  for (auto& observer : observers_)
    observer.OnTopicsDataAccessibleSinceUpdated();
}

bool PrivacySandboxSettings::IsSiteDataAllowed(const GURL& url) const {
  // Relying on |host_content_settings_map_| instead of |cookie_settings_|
  // allows to query whether the site associated with the |url| is allowed to
  // access Site data (aka ContentSettingsType::COOKIES) from a stand-alone
  // point of view. This is not possible via |cookies_settings_|, which _also_
  // takes into account third party context.
  return host_content_settings_map_->GetContentSetting(
             url, GURL(), ContentSettingsType::COOKIES) !=
         ContentSetting::CONTENT_SETTING_BLOCK;
}

bool PrivacySandboxSettings::IsM1PrivacySandboxApiEnabled(
    const std::string& pref_name) const {
  DCHECK(pref_name == prefs::kPrivacySandboxM1TopicsEnabled ||
         pref_name == prefs::kPrivacySandboxM1FledgeEnabled ||
         pref_name == prefs::kPrivacySandboxM1AdMeasurementEnabled);

  if (delegate_->IsIncognitoProfile())
    return false;

  if (IsPrivacySandboxRestricted())
    return false;

  // For Measurement and Relevance APIs, we explicitly do not require the
  // underlying pref to be enabled if there is a local flag enabling the APIs to
  // allow for local testing.
  if (base::FeatureList::IsEnabled(
          privacy_sandbox::kOverridePrivacySandboxSettingsLocalTesting)) {
    return true;
  }

  return pref_service_->GetBoolean(pref_name);
}

}  // namespace privacy_sandbox
