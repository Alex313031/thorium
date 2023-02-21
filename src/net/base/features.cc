// Copyright 2023 The Chromium Authors and Alex313031
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "net/base/features.h"

#include <vector>

#include "base/feature_list.h"
#include "build/build_config.h"

namespace net::features {

BASE_FEATURE(kAlpsForHttp2, "AlpsForHttp2", base::FEATURE_ENABLED_BY_DEFAULT);

BASE_FEATURE(kAvoidH2Reprioritization,
             "AvoidH2Reprioritization",
             base::FEATURE_DISABLED_BY_DEFAULT);

BASE_FEATURE(kCapReferrerToOriginOnCrossOrigin,
             "CapReferrerToOriginOnCrossOrigin",
             base::FEATURE_DISABLED_BY_DEFAULT);

BASE_FEATURE(kDnsTransactionDynamicTimeouts,
             "DnsTransactionDynamicTimeouts",
             base::FEATURE_DISABLED_BY_DEFAULT);

const base::FeatureParam<double> kDnsTransactionTimeoutMultiplier{
    &kDnsTransactionDynamicTimeouts, "DnsTransactionTimeoutMultiplier", 7.5};

const base::FeatureParam<base::TimeDelta> kDnsMinTransactionTimeout{
    &kDnsTransactionDynamicTimeouts, "DnsMinTransactionTimeout",
    base::Seconds(12)};

BASE_FEATURE(kUseDnsHttpsSvcb,
             "UseDnsHttpsSvcb",
             base::FEATURE_ENABLED_BY_DEFAULT);

const base::FeatureParam<bool> kUseDnsHttpsSvcbEnforceSecureResponse{
    &kUseDnsHttpsSvcb, "UseDnsHttpsSvcbEnforceSecureResponse", false};

const base::FeatureParam<base::TimeDelta> kUseDnsHttpsSvcbInsecureExtraTimeMax{
    &kUseDnsHttpsSvcb, "UseDnsHttpsSvcbInsecureExtraTimeMax",
    base::Milliseconds(50)};

const base::FeatureParam<int> kUseDnsHttpsSvcbInsecureExtraTimePercent{
    &kUseDnsHttpsSvcb, "UseDnsHttpsSvcbInsecureExtraTimePercent", 20};

const base::FeatureParam<base::TimeDelta> kUseDnsHttpsSvcbInsecureExtraTimeMin{
    &kUseDnsHttpsSvcb, "UseDnsHttpsSvcbInsecureExtraTimeMin",
    base::Milliseconds(5)};

const base::FeatureParam<base::TimeDelta> kUseDnsHttpsSvcbSecureExtraTimeMax{
    &kUseDnsHttpsSvcb, "UseDnsHttpsSvcbSecureExtraTimeMax",
    base::Milliseconds(50)};

const base::FeatureParam<int> kUseDnsHttpsSvcbSecureExtraTimePercent{
    &kUseDnsHttpsSvcb, "UseDnsHttpsSvcbSecureExtraTimePercent", 20};

const base::FeatureParam<base::TimeDelta> kUseDnsHttpsSvcbSecureExtraTimeMin{
    &kUseDnsHttpsSvcb, "UseDnsHttpsSvcbSecureExtraTimeMin",
    base::Milliseconds(5)};

BASE_FEATURE(kUseDnsHttpsSvcbAlpn,
             "UseDnsHttpsSvcbAlpn",
             base::FEATURE_DISABLED_BY_DEFAULT);

BASE_FEATURE(kEnableTLS13EarlyData,
             "EnableTLS13EarlyData",
             base::FEATURE_DISABLED_BY_DEFAULT);

BASE_FEATURE(kEncryptedClientHello,
             "EncryptedClientHello",
             base::FEATURE_DISABLED_BY_DEFAULT);

BASE_FEATURE(kNetworkQualityEstimator,
             "NetworkQualityEstimator",
             base::FEATURE_DISABLED_BY_DEFAULT);

BASE_FEATURE(kSplitCacheByIncludeCredentials,
             "SplitCacheByIncludeCredentials",
             base::FEATURE_DISABLED_BY_DEFAULT);

BASE_FEATURE(kSplitCacheByNetworkIsolationKey,
             "SplitCacheByNetworkIsolationKey",
             base::FEATURE_DISABLED_BY_DEFAULT);

BASE_FEATURE(kSplitHostCacheByNetworkIsolationKey,
             "SplitHostCacheByNetworkIsolationKey",
             base::FEATURE_DISABLED_BY_DEFAULT);

BASE_FEATURE(kPartitionConnectionsByNetworkIsolationKey,
             "PartitionConnectionsByNetworkIsolationKey",
             base::FEATURE_DISABLED_BY_DEFAULT);

BASE_FEATURE(kForceIsolationInfoFrameOriginToTopLevelFrame,
             "ForceIsolationInfoFrameOriginToTopLevelFrame",
             base::FEATURE_DISABLED_BY_DEFAULT);

BASE_FEATURE(kPartitionHttpServerPropertiesByNetworkIsolationKey,
             "PartitionHttpServerPropertiesByNetworkIsolationKey",
             base::FEATURE_DISABLED_BY_DEFAULT);

BASE_FEATURE(kPartitionSSLSessionsByNetworkIsolationKey,
             "PartitionSSLSessionsByNetworkIsolationKey",
             base::FEATURE_DISABLED_BY_DEFAULT);

BASE_FEATURE(kPartitionNelAndReportingByNetworkIsolationKey,
             "PartitionNelAndReportingByNetworkIsolationKey",
             base::FEATURE_DISABLED_BY_DEFAULT);

BASE_FEATURE(kEnableDoubleKeyNetworkAnonymizationKey,
             "EnableDoubleKeyNetworkAnonymizationKey",
             base::FEATURE_DISABLED_BY_DEFAULT);

BASE_FEATURE(kEnableCrossSiteFlagNetworkAnonymizationKey,
             "EnableCrossSiteFlagNetworkAnonymizationKey",
             base::FEATURE_DISABLED_BY_DEFAULT);

BASE_FEATURE(kTLS13KeyUpdate,
             "TLS13KeyUpdate",
             base::FEATURE_DISABLED_BY_DEFAULT);

BASE_FEATURE(kPermuteTLSExtensions,
             "PermuteTLSExtensions",
             base::FEATURE_DISABLED_BY_DEFAULT);

BASE_FEATURE(kPostQuantumCECPQ2,
             "PostQuantumCECPQ2",
             base::FEATURE_DISABLED_BY_DEFAULT);
BASE_FEATURE(kPostQuantumCECPQ2SomeDomains,
             "PostQuantumCECPQ2SomeDomains",
             base::FEATURE_DISABLED_BY_DEFAULT);
const base::FeatureParam<std::string>
    kPostQuantumCECPQ2Prefix(&kPostQuantumCECPQ2SomeDomains, "prefix", "a");

// This is feature-gated, but enabled, to act as a kill switch, in case there
// are unforeseen consequences to fully removing TLS 1.0/1.1.
//
// TODO(https://crbug.com/1376584): Remove this feature and all TLS 1.0/1.1
// support code.
BASE_FEATURE(kSSLMinVersionAtLeastTLS12,
             "SSLMinVersionAtLeastTLS12",
             base::FEATURE_ENABLED_BY_DEFAULT);

BASE_FEATURE(kNetUnusedIdleSocketTimeout,
             "NetUnusedIdleSocketTimeout",
             base::FEATURE_DISABLED_BY_DEFAULT);

BASE_FEATURE(kShortLaxAllowUnsafeThreshold,
             "ShortLaxAllowUnsafeThreshold",
             base::FEATURE_DISABLED_BY_DEFAULT);

BASE_FEATURE(kSameSiteDefaultChecksMethodRigorously,
             "SameSiteDefaultChecksMethodRigorously",
             base::FEATURE_DISABLED_BY_DEFAULT);

#if BUILDFLAG(TRIAL_COMPARISON_CERT_VERIFIER_SUPPORTED)
// Enables the dual certificate verification trial feature.
// https://crbug.com/649026
BASE_FEATURE(kCertDualVerificationTrialFeature,
             "CertDualVerificationTrial",
             base::FEATURE_DISABLED_BY_DEFAULT);
#if BUILDFLAG(IS_MAC)
const base::FeatureParam<int> kCertDualVerificationTrialImpl{
    &kCertDualVerificationTrialFeature, "impl", 0};
#endif /* BUILDFLAG(IS_MAC) */
#endif

#if BUILDFLAG(CHROME_ROOT_STORE_SUPPORTED)
BASE_FEATURE(kChromeRootStoreUsed,
             "ChromeRootStoreUsed",
#if BUILDFLAG(IS_MAC) || BUILDFLAG(IS_WIN)
             base::FEATURE_ENABLED_BY_DEFAULT
#else
             base::FEATURE_DISABLED_BY_DEFAULT
#endif
);
#if BUILDFLAG(IS_MAC)
const base::FeatureParam<int> kChromeRootStoreSysImpl{&kChromeRootStoreUsed,
                                                      "sysimpl", 0};
#endif /* BUILDFLAG(IS_MAC) */
#endif /* BUILDFLAG(CHROME_ROOT_STORE_SUPPORTED) */

BASE_FEATURE(kTurnOffStreamingMediaCachingOnBattery,
             "TurnOffStreamingMediaCachingOnBattery",
             base::FEATURE_DISABLED_BY_DEFAULT);

BASE_FEATURE(kTurnOffStreamingMediaCachingAlways,
             "TurnOffStreamingMediaCachingAlways",
             base::FEATURE_DISABLED_BY_DEFAULT);

BASE_FEATURE(kSchemefulSameSite,
             "SchemefulSameSite",
             base::FEATURE_ENABLED_BY_DEFAULT);

BASE_FEATURE(kLimitOpenUDPSockets,
             "LimitOpenUDPSockets",
             base::FEATURE_ENABLED_BY_DEFAULT);

extern const base::FeatureParam<int> kLimitOpenUDPSocketsMax(
    &kLimitOpenUDPSockets,
    "LimitOpenUDPSocketsMax",
    6000);

BASE_FEATURE(kTimeoutTcpConnectAttempt,
             "TimeoutTcpConnectAttempt",
             base::FEATURE_DISABLED_BY_DEFAULT);

extern const base::FeatureParam<double> kTimeoutTcpConnectAttemptRTTMultiplier(
    &kTimeoutTcpConnectAttempt,
    "TimeoutTcpConnectAttemptRTTMultiplier",
    5.0);

extern const base::FeatureParam<base::TimeDelta> kTimeoutTcpConnectAttemptMin(
    &kTimeoutTcpConnectAttempt,
    "TimeoutTcpConnectAttemptMin",
    base::Seconds(8));

extern const base::FeatureParam<base::TimeDelta> kTimeoutTcpConnectAttemptMax(
    &kTimeoutTcpConnectAttempt,
    "TimeoutTcpConnectAttemptMax",
    base::Seconds(30));

#if BUILDFLAG(ENABLE_REPORTING)
BASE_FEATURE(kDocumentReporting,
             "DocumentReporting",
             base::FEATURE_ENABLED_BY_DEFAULT);
#endif  // BUILDFLAG(ENABLE_REPORTING)

#if BUILDFLAG(IS_POSIX) || BUILDFLAG(IS_FUCHSIA)
BASE_FEATURE(kUdpSocketPosixAlwaysUpdateBytesReceived,
             "UdpSocketPosixAlwaysUpdateBytesReceived",
             base::FEATURE_ENABLED_BY_DEFAULT);
#endif  // BUILDFLAG(IS_POSIX) || BUILDFLAG(IS_FUCHSIA)

BASE_FEATURE(kCookieSameSiteConsidersRedirectChain,
             "CookieSameSiteConsidersRedirectChain",
             base::FEATURE_DISABLED_BY_DEFAULT);

BASE_FEATURE(kSamePartyCookiesConsideredFirstParty,
             "SamePartyCookiesConsideredFirstParty",
             base::FEATURE_DISABLED_BY_DEFAULT);

BASE_FEATURE(kSamePartyAttributeEnabled,
             "SamePartyAttributeEnabled",
             base::FEATURE_DISABLED_BY_DEFAULT);

BASE_FEATURE(kPartitionedCookies,
             "PartitionedCookies",
             base::FEATURE_DISABLED_BY_DEFAULT);

BASE_FEATURE(kNoncedPartitionedCookies,
             "NoncedPartitionedCookies",
             base::FEATURE_ENABLED_BY_DEFAULT);

BASE_FEATURE(kRecordRadioWakeupTrigger,
             "RecordRadioWakeupTrigger",
             base::FEATURE_DISABLED_BY_DEFAULT);

BASE_FEATURE(kClampCookieExpiryTo400Days,
             "ClampCookieExpiryTo400Days",
             base::FEATURE_ENABLED_BY_DEFAULT);

BASE_FEATURE(kStaticKeyPinningEnforcement,
             "StaticKeyPinningEnforcement",
             base::FEATURE_ENABLED_BY_DEFAULT);

BASE_FEATURE(kCookieDomainRejectNonASCII,
             "CookieDomainRejectNonASCII",
             base::FEATURE_DISABLED_BY_DEFAULT);

BASE_FEATURE(kBlockSetCookieHeader,
             "BlockSetCookieHeader",
             base::FEATURE_ENABLED_BY_DEFAULT);

// Run callbacks optimstically for write calls to the blockfile disk cache
// implementation.
BASE_FEATURE(kOptimisticBlockfileWrite,
             "OptimisticBlockfileWrite",
             base::FEATURE_DISABLED_BY_DEFAULT);

// Read as much of the net::URLRequest as there is space in the Mojo data pipe.
BASE_FEATURE(kOptimizeNetworkBuffers,
             "OptimizeNetworkBuffers2",
             base::FEATURE_ENABLED_BY_DEFAULT);

const base::FeatureParam<int> kOptimizeNetworkBuffersBytesReadLimit{
    &kOptimizeNetworkBuffers, "bytes_read_limit", 64 * 1024};

// If InputStream.available() returns less than this,
// kOptimizeNetworkBuffersMinInputStreamReadSize will be used instead.
const base::FeatureParam<int>
    kOptimizeNetworkBuffersMinInputStreamAvailableValueToIgnore{
        &kOptimizeNetworkBuffers, "min_input_stream_available_value_to_ignore",
        16};

// The smallest amount we'll try to read at a time if InputStream.available()
// returned less than
// kOptimizeNetworkBuffersMinInputStreamAvailableValueToIgnore.
const base::FeatureParam<int> kOptimizeNetworkBuffersMinInputStreamReadSize{
    &kOptimizeNetworkBuffers, "min_input_stream_read_size", 1024};

const base::FeatureParam<int>
    kOptimizeNetworkBuffersMaxInputStreamBytesToReadWhenAvailableUnknown{
        &kOptimizeNetworkBuffers, "max_input_stream_bytes_available_unknown",
        2 * 1024};

const base::FeatureParam<int>
    kOptimizeNetworkBuffersFilterSourceStreamBufferSize{
        &kOptimizeNetworkBuffers, "filter_source_stream_buffer_size",
        32 * 1024};

const base::FeatureParam<bool> kOptimizeNetworkBuffersInputStreamCheckAvailable{
    &kOptimizeNetworkBuffers, "input_stream_check_available", true};

BASE_FEATURE(kStorageAccessAPI,
             "StorageAccessAPI",
             base::FEATURE_ENABLED_BY_DEFAULT);
constexpr int kStorageAccessAPIDefaultImplicitGrantLimit = 5;
const base::FeatureParam<int> kStorageAccessAPIImplicitGrantLimit{
    &kStorageAccessAPI, "storage-access-api-implicit-grant-limit",
    kStorageAccessAPIDefaultImplicitGrantLimit};
const base::FeatureParam<bool> kStorageAccessAPIGrantsUnpartitionedStorage(
    &kStorageAccessAPI,
    "storage-access-api-grants-unpartitioned-storage",
    false);
const base::FeatureParam<bool> kStorageAccessAPIAutoGrantInFPS{
    &kStorageAccessAPI, "storage_access_api_auto_grant_in_fps", true};
const base::FeatureParam<bool> kStorageAccessAPIAutoDenyOutsideFPS{
    &kStorageAccessAPI, "storage_access_api_auto_deny_outside_fps", true};

// Enables partitioning of third party storage (IndexedDB, CacheStorage, etc.)
// by the top level site to reduce fingerprinting.
BASE_FEATURE(kThirdPartyStoragePartitioning,
             "ThirdPartyStoragePartitioning",
             base::FEATURE_DISABLED_BY_DEFAULT);
// Whether to use the new code paths needed to support partitioning Blob URLs.
// This exists as a kill-switch in case an issue is identified with the Blob
// URL implementation that causes breakage.
// TODO(https://crbug.com/1407944): Kill-switch activated - investigate cause of
// increased renderer hangs.
BASE_FEATURE(kSupportPartitionedBlobUrl,
             "SupportPartitionedBlobUrl",
             base::FEATURE_DISABLED_BY_DEFAULT);

BASE_FEATURE(kAlpsParsing, "AlpsParsing", base::FEATURE_ENABLED_BY_DEFAULT);

BASE_FEATURE(kAlpsClientHintParsing,
             "AlpsClientHintParsing",
             base::FEATURE_ENABLED_BY_DEFAULT);

BASE_FEATURE(kShouldKillSessionOnAcceptChMalformed,
             "ShouldKillSessionOnAcceptChMalformed",
             base::FEATURE_DISABLED_BY_DEFAULT);

BASE_FEATURE(kCaseInsensitiveCookiePrefix,
             "CaseInsensitiveCookiePrefix",
             base::FEATURE_ENABLED_BY_DEFAULT);

BASE_FEATURE(kEnableWebsocketsOverHttp3,
             "EnableWebsocketsOverHttp3",
             base::FEATURE_DISABLED_BY_DEFAULT);

BASE_FEATURE(kUseNAT64ForIPv4Literal,
             "UseNAT64ForIPv4Literal",
             base::FEATURE_ENABLED_BY_DEFAULT);

BASE_FEATURE(kBlockNewForbiddenHeaders,
             "BlockNewForbiddenHeaders",
             base::FEATURE_ENABLED_BY_DEFAULT);

#if BUILDFLAG(IS_WIN)
BASE_FEATURE(kPlatformKeyProbeSHA256,
             "PlatformKeyProbeSHA256",
             base::FEATURE_ENABLED_BY_DEFAULT);
#endif

}  // namespace net::features
