# Command-line switches

Chromium command-line switches are implementation details and change between
revisions. Thorium does not keep a copied switch database in this repository.

Useful references are:

- the current checked-out Chromium source, searched through
  [Chromium Code Search](https://source.chromium.org/chromium/chromium/src);
- the community-maintained [Chromium command-line switch
  list](https://peter.sh/experiments/chromium-command-line-switches/).

The community list is not an official Chromium specification and may not match
Thorium's exact Chromium revision. Confirm security-sensitive or
platform-specific switches in source before relying on them.

Thorium's development launch examples remain in
[`infra/DEV_CMDLINE_FLAGS.txt`](../infra/DEV_CMDLINE_FLAGS.txt). Some disable
security boundaries or alter profile behavior; use them only with disposable
test data.

The command line used by a running desktop build is shown on
`thorium://version` (or `chrome://version`). Flags exposed through
`thorium://flags` are experiments and do not necessarily map one-to-one to a
single command-line switch.

## WebRTC IP handling policy

Open `chrome://flags/#webrtc-ip-handling-policy` (also available through
`thorium://flags`) to select **WebRTC IP Handling Policy**, then restart Thorium.
The dropdown includes all four policies supported by the current Chromium
source, plus an option to keep the existing settings:

| Flag option | Policy value | Behavior |
| --- | --- | --- |
| Default | No command-line override | Uses the current profile settings; Thorium's built-in default is `default_public_and_private_interfaces`. |
| All interfaces (Chromium default) | `default` | Allows WebRTC to enumerate all network interfaces. |
| Default public and private interfaces | `default_public_and_private_interfaces` | Uses the default HTTP route and its associated private address. |
| Default public interface only | `default_public_interface_only` | Uses the default HTTP route without exposing local addresses. |
| Disable non-proxied UDP | `disable_non_proxied_udp` | Uses TCP unless the proxy supports UDP; does not expose local addresses. |

Explicit selections use Chromium's existing
`--webrtc-ip-handling-policy=<policy value>` switch and its normal preference
precedence. Mandatory enterprise policy and extension settings take precedence
over the flag. Site-specific WebRTC IP handling policies continue to apply.
Selecting **Default** removes the flag's override without changing saved
profile preferences. This controls WebRTC networking; it does not disable
WebRTC itself.
