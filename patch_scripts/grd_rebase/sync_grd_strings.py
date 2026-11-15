#!/usr/bin/env python3
# Copyright (c) 2026 Alex313031 and gz83.
"""Synchronize reviewed Thorium GRD/GRDP replacements and XTB translations."""

import argparse
import csv
from dataclasses import dataclass
import hashlib
import html
from pathlib import Path
import re
import sys
from typing import Iterable

CHROMIUM_PROJECT_LINK_MESSAGE_IDS = frozenset(
    {
        "IDS_VERSION_UI_LICENSE",
        "IDS_VERSION_UI_LICENSE_CHROMIUM",
    }
)
DINO_SOURCE_TEXT_REPLACEMENTS = {
    "IDS_ERRORPAGES_GAME_INSTRUCTIONS": (
        ("Tap the dino to play", "Tap the Dino to play"),
        ("Press space to play", "Press Space or the Up Arrow to Play"),
    ),
    "IDS_ERRORPAGE_DINO_ARIA_LABEL": (
        (
            "Dino game, press space to play",
            "Dino game, press space or the up arrow to play",
        ),
    ),
    "IDS_ERRORPAGE_DINO_SLOW_SPEED_TOGGLE": (
        ("Start slower", "Start slower&#63;"),
    ),
}
EXTERNAL_ADDITION_MESSAGE_IDS = frozenset(
    {
        "IDS_CHROMIUM_SHORCUT_DESCRIPTION",
        "IDS_EXTENSIONS_SETTINGS_API_SECOND_LINE_START_AND_SEARCH",
        "IDS_EXTENSIONS_SETTINGS_API_SECOND_LINE_START_PAGES",
        "IDS_MODULE_INSTALL_FAILURE_TEXT",
        "IDS_MODULE_INSTALL_START_TEXT",
        "IDS_SETTINGS_SKILLS_SETTING_LABEL",
        "IDS_VERSION_UI_LICENSE",
    }
)
EXTERNAL_ADDITION_TRANSLATION_IDS = frozenset(
    {
        # Chrome Labs enterprise-policy accessibility announcements.
        "2338932653846229367",
        "3828357573682778203",
        # "Press Space or the Up Arrow to Play"
        "8670107939381467459",
        # "Dino game, press space or the up arrow to play"
        "4188259053923371929",
        # Thorium's custom Sad Tab title.
        "7032843997772175977",
    }
)
EXPERIMENT_TITLE_MESSAGE_IDS = frozenset(
    {
        "IDS_ACCNAME_CHROMELABS_BUTTON",
        "IDS_WINDOW_TITLE_EXPERIMENTS",
    }
)
ALEX313031_RECOMMENDATION_MESSAGE_IDS = frozenset(
    {"IDS_CHROME_REENGAGEMENT_NOTIFICATION_3_TITLE"}
)
WEB_STORE_BRAND_MESSAGE_IDS = frozenset(
    {
        "IDS_EXTENSIONS_BLOCKLISTED_CWS_POLICY_VIOLATION",
        "IDS_EXTENSIONS_ITEM_CHROME_WEB_STORE",
        "IDS_EXTENSIONS_SC_POLICY_VIOLATION_OFF",
        "IDS_EXTENSIONS_SC_POLICY_VIOLATION_ON",
    }
)
# Resolve known cases where distinct Chromium messages converge to one
# Thorium-branded source string but one translation source is clearly better.
PREFERRED_CONVERGED_OLD_TRANSLATION_IDS = {
    # Prefer the localized ChromeOS Flex device names over the untranslated
    # Chromebook product names when both converge to ThoriumOS device labels.
    "2001630232173796020": "6755827872271341378",
    "6465856910027769799": "7402503521691663770",
    # The Chromium Autofill AI variant preserves grammar for consonant-ending
    # product names (notably Basque).
    "8208549821385074251": "4534145890176164066",
}
PREFERRED_CONVERGED_OLD_TRANSLATION_IDS_BY_LOCALE = {
    # Basque appends -ek, rather than Chrome's -k, to consonant-ending
    # product names such as Thorium. Keep this preference locale-specific so
    # unrelated translations in the same converged-ID group are unaffected.
    ("1510422777984914550", "eu"): "4050599136622776556",
    ("3175036493293309560", "eu"): "1021753677514347426",
    ("8803989344032574189", "eu"): "7432774160230062882",
}
PRODUCT_TEXT_REPLACEMENTS_BEFORE_CHROME = (
    ("Chromebook Recovery Utility", "ThoriumOS Recovery Utility"),
    ("Chromebook Plus", "ThoriumOS Plus device"),
    ("Chromebook experience", "ThoriumOS experience"),
    ("Chromebook community", "ThoriumOS community"),
    ("Chromebook help forum", "ThoriumOS help forum"),
    ("Chromebook questions", "ThoriumOS questions"),
    ("features of Chromebook", "features of ThoriumOS"),
    ("Chromebooks", "ThoriumOS devices"),
    ("Chromebook", "ThoriumOS device"),
    ("Chromium", "Thorium"),
)
PRODUCT_TEXT_REPLACEMENTS_AFTER_CHROME = (
    ("Google Thorium", "Thorium"),
    ("Google recommends Thorium", "Alex313031 recommends Thorium"),
    ("ThoriumOS Flex", "ThoriumOS"),
    ("made possible by Thorium", "made possible by Chromium"),
)


def _apply_product_text_replacements(
    text: str,
    *,
    replace_chrome: bool,
) -> str:
    """Apply product replacements while optionally preserving Chrome names."""
    for old, new in PRODUCT_TEXT_REPLACEMENTS_BEFORE_CHROME:
        text = text.replace(old, new)
    if replace_chrome:
        text = text.replace("Chrome", "Thorium")
    for old, new in PRODUCT_TEXT_REPLACEMENTS_AFTER_CHROME:
        text = text.replace(old, new)
    return text


def apply_ordered_replacements(text: str) -> str:
    """Apply reviewed Thorium replacements in their required order."""
    text = _apply_product_text_replacements(text, replace_chrome=True)
    text = re.sub(r"(?<!Thorium )Experiments", "Thorium Experiments", text)
    for old, new in (
        ("Aw, Snap!", "Aw, #@%!, this tab's process has gone bye bye..."),
        ("Thorium Web Store", "Chrome Web Store"),
        ("Thorium web store", "Chrome web store"),
        ("Thorium Remote Desktop", "Chrome Remote Desktop"),
    ):
        text = text.replace(old, new)
    return text


def apply_replacements_preserving_chrome(text: str) -> str:
    """Apply product replacements while preserving Chrome-owned service names."""
    text = _apply_product_text_replacements(text, replace_chrome=False)
    text = re.sub(r"(?<!Thorium )Experiments", "Thorium Experiments", text)
    text = text.replace(
        "Aw, Snap!", "Aw, #@%!, this tab's process has gone bye bye..."
    )
    text = text.replace("Thorium Remote Desktop", "Chrome Remote Desktop")
    return text


def normalize_xtb_translation_text(text: str) -> str:
    """Normalize copied Thorium XTB text after branding replacements."""
    for old, new in (
        ("ThoriumOS Flex", "ThoriumOS"),
        ("ThoriumOS\u00a0Flex", "ThoriumOS"),
        ("Thorium OS Flex", "ThoriumOS"),
        ("Thorium\u00a0OS Flex", "ThoriumOS"),
        ("Thorium OS\u00a0Flex", "ThoriumOS"),
        ("Thorium\u00a0OS\u00a0Flex", "ThoriumOS"),
        ("ThoriumOs Flex", "ThoriumOS"),
        ("ThoriumOs\u00a0Flex", "ThoriumOS"),
        ("Thorium Flex", "ThoriumOS"),
        ("Thorium\u00a0Flex", "ThoriumOS"),
    ):
        text = text.replace(old, new)
    return "\n".join(line.rstrip() for line in text.splitlines())


TRANSLATION_FILE_RE = re.compile(
    r"<file\b(?=[^>]*\bpath=\"([^\"]+)\")"
    r"(?=[^>]*\blang=\"([^\"]+)\")[^>]*/?>"
)
GRDP_PARENT_GRD_PATHS = {
    "chrome/app/app_management_strings.grdp": "chrome/app/generated_resources.grd",
    "chrome/app/extensions_strings.grdp": "chrome/app/generated_resources.grd",
    "chrome/app/settings_chromium_strings.grdp": "chrome/app/chromium_strings.grd",
    "chrome/app/settings_strings.grdp": "chrome/app/generated_resources.grd",
    "chrome/app/shared_settings_strings.grdp": "chrome/app/generated_resources.grd",
    "components/autofill_payments_strings.grdp": "components/components_strings.grd",
    "components/autofill_strings.grdp": "components/components_strings.grd",
    "components/error_page_strings.grdp": "components/components_strings.grd",
    "components/flags_strings.grdp": "components/components_strings.grd",
    "components/heavy_ad_intervention_strings.grdp": "components/components_strings.grd",
    "components/management_strings.grdp": "components/components_strings.grd",
    "components/new_or_sad_tab_strings.grdp": "components/components_strings.grd",
    "components/page_info_strings.grdp": "components/components_strings.grd",
    "components/reset_password_strings.grdp": "components/components_strings.grd",
    "components/security_interstitials_strings.grdp": "components/components_strings.grd",
    "components/ssl_errors_strings.grdp": "components/components_strings.grd",
    "components/version_ui_strings.grdp": "components/components_strings.grd",
}


@dataclass(frozen=True)
class XtbFile:
    lang: str
    chromium_path: str


@dataclass(frozen=True)
class GrdXtbMapping:
    xtb_files: tuple[XtbFile, ...]


def _normalize_chromium_path(path: str) -> str:
    return path.replace("\\", "/").strip("/")


def get_translation_grd_path(chromium_path: str) -> str:
    normalized = _normalize_chromium_path(chromium_path)
    if normalized.endswith(".grd"):
        return normalized
    if normalized.endswith(".grdp"):
        try:
            return GRDP_PARENT_GRD_PATHS[normalized]
        except KeyError as error:
            raise KeyError(
                f"missing parent GRD mapping for GRDP file: {normalized}"
            ) from error
    raise ValueError(f"not a GRD/GRDP path: {chromium_path}")


def read_xtb_files_for_grd(
    source_root: Path,
    grd_path: str,
) -> tuple[XtbFile, ...]:
    normalized_grd_path = _normalize_chromium_path(grd_path)
    grd_file = source_root / normalized_grd_path
    if not grd_file.is_file():
        raise FileNotFoundError(f"missing GRD file: {normalized_grd_path}")
    grd_text = grd_file.read_text(encoding="utf-8", errors="replace")
    grd_dir = Path(normalized_grd_path).parent
    xtb_files = tuple(
        XtbFile(
            lang=match.group(2),
            chromium_path=_normalize_chromium_path(
                (grd_dir / match.group(1)).as_posix()
            ),
        )
        for match in TRANSLATION_FILE_RE.finditer(grd_text)
    )
    if not xtb_files:
        raise ValueError(f"GRD has no translation files: {normalized_grd_path}")
    return xtb_files


def build_grd_xtb_mapping(
    source_root: Path,
    chromium_paths: list[str],
) -> dict[str, GrdXtbMapping]:
    mappings: dict[str, GrdXtbMapping] = {}
    xtb_cache: dict[str, tuple[XtbFile, ...]] = {}
    for chromium_path in chromium_paths:
        normalized = _normalize_chromium_path(chromium_path)
        parent = get_translation_grd_path(normalized)
        if parent not in xtb_cache:
            xtb_cache[parent] = read_xtb_files_for_grd(source_root, parent)
        mappings[normalized] = GrdXtbMapping(
            xtb_files=xtb_cache[parent],
        )
    return mappings


# GRD descriptions can contain a literal ">" (for example, "Settings > AI").
# Match quoted attribute values as units so that such characters do not end the
# opening tag and become part of the translatable message body.
MESSAGE_RE = re.compile(
    r"<message\b((?:[^>\"']+|\"[^\"]*\"|'[^']*')*)>"
    r"(.*?)</message>",
    re.DOTALL,
)
MESSAGE_NAME_RE = re.compile(r"\bname\s*=\s*\"([^\"]+)\"")
MEANING_RE = re.compile(r"\bmeaning=\"([^\"]*)\"")
GRIT_USE_NAME_FOR_ID_RE = re.compile(r'\buse_name_for_id="true"')
GRIT_PH_BLOCK_RE = re.compile(
    r"<ph\b(?=[^>]*\bname=\"([^\"]+)\")[^>]*>.*?</ph>",
    re.DOTALL,
)
GRIT_PH_SELF_CLOSING_RE = re.compile(
    r"<ph\b(?=[^>]*\bname=\"([^\"]+)\")[^>]*/>"
)
GRIT_EX_BLOCK_RE = re.compile(r"<ex\b[^>]*>.*?</ex>", re.DOTALL)
MARKUP_TAG_RE = re.compile(r"<[^>]+>")
CHROMIUM_LINK_BLOCK_RE = re.compile(
    r'<ph name="BEGIN_LINK_CHROMIUM"\s*/>.*?'
    r'<ph name="END_LINK_CHROMIUM"\s*/>',
    re.DOTALL,
)


def _grit_fingerprint(text: str) -> int:
    digest = hashlib.md5(
        text.encode("utf-8"),
        usedforsecurity=False,
    )
    value = int(digest.hexdigest()[:16], 16)
    if value & 0x8000000000000000:
        value = -((~value & 0xFFFFFFFFFFFFFFFF) + 1)
    return value


def message_block_to_id(message_block: str) -> str:
    """Calculate Chromium GRIT's translation ID for one message block."""
    match = MESSAGE_RE.fullmatch(message_block.strip())
    if not match:
        raise ValueError("not a complete GRD/GRDP <message> block")
    attrs, body = match.groups()
    name_match = MESSAGE_NAME_RE.search(attrs)
    if not name_match:
        raise ValueError("GRD/GRDP <message> block has no name attribute")
    message_name = name_match.group(1)
    if GRIT_USE_NAME_FOR_ID_RE.search(attrs):
        return message_name
    body = GRIT_PH_BLOCK_RE.sub(lambda item: item.group(1).upper(), body)
    body = GRIT_PH_SELF_CLOSING_RE.sub(
        lambda item: item.group(1).upper(),
        body,
    )
    body = GRIT_EX_BLOCK_RE.sub("", body)
    body = html.unescape(MARKUP_TAG_RE.sub("", body)).strip()
    if body.startswith("'''") and body.endswith("'''"):
        body = body[3:-3].strip()
    value = _grit_fingerprint(body)
    meaning_match = MEANING_RE.search(attrs)
    if meaning_match:
        meaning_value = _grit_fingerprint(html.unescape(meaning_match.group(1)))
        value = meaning_value + (value << 1) + (1 if value < 0 else 0)
    return str(value & 0x7FFFFFFFFFFFFFFF)


DESC_RE = re.compile(r"\bdesc=\"([^\"]*)\"")
TRANSLATEABLE_FALSE_RE = re.compile(r"\btranslateable=\"false\"")
PH_BLOCK_RE = re.compile(r"<ph\b[^>]*>.*?</ph>", re.DOTALL)
XTB_TRANSLATION_RE = re.compile(
    r"<translation\b(?=[^>]*\bid=\"([^\"]+)\")([^>]*)>(.*?)</translation>",
    re.DOTALL,
)
XTB_ID_ATTR_RE = re.compile(r'(\bid=")([^"]+)(")')
CHROMIUM_PROJECT_LINK_END_RE = re.compile(
    r'Thorium(?=<ph\b[^>]*\bname="END_LINK_CHROMIUM")'
)
TEXT_SYNC_FILE_ROLES = frozenset(
    {
        "overlay_text_sync",
        "overlay_text_and_feature_patch",
        "mixed_text_and_structural",
        "mixed_text_and_feature_messages",
    }
)
THORIUM_ADDED_FILE_ROLES = frozenset({"thorium_added_file"})


@dataclass(frozen=True)
class MessageChange:
    """A planned replacement inside one allowlisted GRD/GRDP message."""

    chromium_path: str
    message_id: str
    old_translation_id: str
    new_translation_id: str
    old_block: str
    new_block: str
    xtb_sync_needed: bool


@dataclass(frozen=True)
class XtbTranslationMatch:
    """One old translation ID found in a mapped XTB file."""

    chromium_path: str
    block: str
    attrs: str
    body: str


@dataclass(frozen=True)
class XtbTranslationMissing:
    """One mapped XTB file where a message's old ID was not found."""

    source_chromium_path: str
    message_id: str
    old_translation_id: str
    new_translation_id: str
    lang: str
    xtb_chromium_path: str


@dataclass(frozen=True)
class MessageTranslationLookup:
    """All mapped XTB lookup results for one modified message."""

    change: MessageChange
    matches: tuple[XtbTranslationMatch, ...]
    missing: tuple[XtbTranslationMissing, ...]


@dataclass(frozen=True)
class XtbFileCache:
    """Text and indexed translations for one XTB file."""

    text: str
    by_id: dict[str, tuple[str, str, str]]
    block_spans: dict[str, list[tuple[int, int]]]


@dataclass(frozen=True)
class XtbTranslationInsertion:
    """A copied translation ready to insert into its target XTB file."""

    message_id: str
    chromium_path: str
    old_translation_id: str
    new_translation_id: str
    anchor_block: str
    new_block: str


@dataclass(frozen=True)
class XtbTranslationConflict:
    """A rejected translation candidate for a converged new ID."""

    chromium_path: str
    new_translation_id: str
    selected_message_id: str
    selected_old_translation_id: str
    selected_block: str
    rejected_message_id: str
    rejected_old_translation_id: str
    rejected_block: str


def _sample_values(values: Iterable[str], limit: int = 8) -> str:
    unique = sorted(set(values))
    sample = unique[:limit]
    suffix = "" if len(unique) <= limit else f";...+{len(unique) - limit}"
    return ";".join(sample) + suffix


def xtb_locale_from_path(chromium_path: str) -> str:
    """Best-effort locale extraction from Chromium XTB resource paths."""
    filename = Path(chromium_path).name
    match = re.search(r"_([A-Za-z-]+)\.xtb$", filename)
    return match.group(1) if match else ""


def readable_file(path: str) -> Path:
    resolved = Path(path).resolve()
    if not resolved.is_file():
        raise argparse.ArgumentTypeError(f"not a readable file: {path}")
    return resolved


def readable_source_root(path: str) -> Path:
    resolved = Path(path).resolve()
    if not resolved.is_dir():
        raise argparse.ArgumentTypeError(f"not a directory: {path}")
    if not (resolved / "BUILD.gn").is_file():
        raise argparse.ArgumentTypeError(
            f"not a Chromium source root, missing BUILD.gn: {path}"
        )
    return resolved


def load_csv_rows(
    path: Path,
    required_columns: set[str] | None = None,
) -> list[dict[str, str]]:
    """Read UTF-8 CSV rows."""
    with path.open(newline="", encoding="utf-8") as csv_file:
        reader = csv.DictReader(csv_file)
        fieldnames = set(reader.fieldnames or ())
        missing_columns = (required_columns or set()) - fieldnames
        if missing_columns:
            missing = ", ".join(sorted(missing_columns))
            raise ValueError(f"CSV {path} is missing required columns: {missing}")
        return list(reader)


def load_allowed_source_files(
    source_root: Path,
    file_allowlist_rows: list[dict[str, str]],
) -> dict[str, str]:
    """Read all allowlisted GRD/GRDP files from the source tree."""
    contents: dict[str, str] = {}
    for row in file_allowlist_rows:
        chromium_path = row.get("chromium_path", "").strip()
        if not chromium_path:
            continue
        source_path = source_root / chromium_path
        if not source_path.is_file():
            raise FileNotFoundError(
                f"allowlisted source file is missing: {chromium_path}"
            )
        contents[chromium_path] = source_path.read_text(
            encoding="utf-8",
            errors="replace",
        )
    return contents


def select_text_sync_file_rows(
    file_allowlist_rows: list[dict[str, str]],
) -> list[dict[str, str]]:
    """Select files owned by the branding/special text sync workflow."""
    return [
        row
        for row in file_allowlist_rows
        if row.get("role", "").strip() in TEXT_SYNC_FILE_ROLES
    ]


def select_thorium_added_file_rows(
    file_allowlist_rows: list[dict[str, str]],
) -> list[dict[str, str]]:
    """Select complete files owned by the Thorium additions workflow."""
    return [
        row
        for row in file_allowlist_rows
        if row.get("role", "").strip() in THORIUM_ADDED_FILE_ROLES
    ]


def build_message_keys(
    rows: list[dict[str, str]],
) -> set[tuple[str, str]]:
    """Return normalized `(chromium_path, message_id)` ownership keys."""
    return {
        (
            row.get("chromium_path", "").strip(),
            row.get("message_id", "").strip(),
        )
        for row in rows
        if row.get("chromium_path", "").strip()
        and row.get("message_id", "").strip()
    }


def validate_feature_messages_excluded(
    replacement_message_rows: list[dict[str, str]],
    feature_message_rows: list[dict[str, str]],
) -> None:
    """Fail if a feature-owned message enters the branding replacement set."""
    overlap = sorted(
        build_message_keys(replacement_message_rows)
        & build_message_keys(feature_message_rows)
    )
    if overlap:
        formatted = ", ".join(f"{path}:{message_id}" for path, message_id in overlap)
        raise ValueError(
            "feature-patch messages must not enter branding replacements: "
            f"{formatted}"
        )


def validate_feature_message_ownership(
    feature_message_rows: list[dict[str, str]],
) -> None:
    """Require every added message to have an explicit retained owner."""
    allowed_owners = {
        ("feature_patch", "keep_in_feature_patch"),
        ("overlay_added_file", "keep_in_overlay_added_file"),
    }
    invalid_rows = [
        row
        for row in feature_message_rows
        if (
            row.get("ownership", "").strip(),
            row.get("destination", "").strip(),
        )
        not in allowed_owners
    ]
    if invalid_rows:
        formatted = ", ".join(
            f"{row.get('chromium_path', '')}:{row.get('message_id', '')}"
            for row in invalid_rows
        )
        raise ValueError(f"added message ownership is unresolved: {formatted}")


def _build_message_allowlist_from_keys(
    message_keys: Iterable[tuple[str, str]],
) -> dict[str, set[str]]:
    """Map source-relative paths to message IDs from normalized keys."""
    allowlist: dict[str, set[str]] = {}
    for chromium_path, message_id in message_keys:
        if not chromium_path or not message_id:
            continue
        allowlist.setdefault(chromium_path, set()).add(message_id)
    return allowlist


def _replace_attr(tag_attrs: str, attr_re: re.Pattern[str]) -> str:
    return attr_re.sub(
        lambda match: match.group(0).replace(
            match.group(1),
            apply_ordered_replacements(match.group(1)),
            1,
        ),
        tag_attrs,
    )


def _replace_outside_markup(text: str) -> str:
    """Apply text replacements without changing XML-like tags."""
    parts: list[str] = []
    cursor = 0
    for match in MARKUP_TAG_RE.finditer(text):
        if match.start() > cursor:
            parts.append(apply_ordered_replacements(text[cursor : match.start()]))
        parts.append(match.group(0))
        cursor = match.end()
    if cursor < len(text):
        parts.append(apply_ordered_replacements(text[cursor:]))
    return "".join(parts)


def _replace_outside_markup_preserving_chrome(text: str) -> str:
    """Apply text replacements without rewriting Chrome-owned service names."""
    parts: list[str] = []
    cursor = 0
    for match in MARKUP_TAG_RE.finditer(text):
        if match.start() > cursor:
            parts.append(
                apply_replacements_preserving_chrome(text[cursor : match.start()])
            )
        parts.append(match.group(0))
        cursor = match.end()
    if cursor < len(text):
        parts.append(apply_replacements_preserving_chrome(text[cursor:]))
    return "".join(parts)


def _replace_outside_markup_preserving_chromium_link(text: str) -> str:
    """Apply branding replacements while preserving Chromium project links."""
    parts: list[str] = []
    cursor = 0
    for match in CHROMIUM_LINK_BLOCK_RE.finditer(text):
        if match.start() > cursor:
            parts.append(_replace_outside_markup(text[cursor : match.start()]))
        parts.append(match.group(0))
        cursor = match.end()
    if cursor < len(text):
        parts.append(_replace_outside_markup(text[cursor:]))
    return "".join(parts)


def _replace_message_body(body: str) -> str:
    """Apply replacements to message text while preserving placeholders.

    Full <ph>...</ph> blocks are protected because their attributes and example
    payloads are placeholders rather than user-facing translated prose. Other
    markup, including <if>, <then>, and <else>, is preserved while the text
    inside those branches is still processed.
    """
    parts: list[str] = []
    cursor = 0
    for match in PH_BLOCK_RE.finditer(body):
        if match.start() > cursor:
            parts.append(_replace_outside_markup(body[cursor : match.start()]))
        parts.append(match.group(0))
        cursor = match.end()
    if cursor < len(body):
        parts.append(_replace_outside_markup(body[cursor:]))
    return "".join(parts)


def _apply_message_specific_body_exceptions(
    message_id: str,
    body: str,
) -> str:
    """Restore product/project names that broad branding cannot distinguish."""
    if message_id in CHROMIUM_PROJECT_LINK_MESSAGE_IDS:
        return CHROMIUM_PROJECT_LINK_END_RE.sub("Chromium", body)
    return body


def _apply_source_message_body_overrides(
    message_id: str,
    body: str,
) -> str:
    """Apply exact overlay-owned source rewrites outside global branding."""
    for old_text, new_text in DINO_SOURCE_TEXT_REPLACEMENTS.get(message_id, ()):
        if new_text not in body:
            body = body.replace(old_text, new_text)
    return body


def build_branding_replacement_block(
    message_id: str,
    attrs: str,
    body: str,
    *,
    apply_source_overrides: bool,
) -> str:
    """Build a replaced message block for standard branding sync."""
    new_attrs = _replace_attr(attrs, DESC_RE)
    new_attrs = _replace_attr(new_attrs, MEANING_RE)
    new_body = _replace_message_body(body)
    if apply_source_overrides:
        new_body = _apply_source_message_body_overrides(
            message_id,
            new_body,
        )
    new_body = _apply_message_specific_body_exceptions(
        message_id,
        new_body,
    )
    return f'<message{new_attrs}>{new_body}</message>'


def discover_auto_branding_message_keys(
    source_contents: dict[str, str],
    feature_message_keys: set[tuple[str, str]],
) -> set[tuple[str, str]]:
    """Find plain branding replacements in allowed source files.

    Explicit special rows still live in message_allowlist.csv. Auto discovery
    intentionally skips feature-owned messages, because those strings are owned
    by the feature patch that introduces them.
    """
    message_keys: set[tuple[str, str]] = set()
    for chromium_path, contents in source_contents.items():
        for match in MESSAGE_RE.finditer(contents):
            attrs, body = match.groups()
            name_match = MESSAGE_NAME_RE.search(attrs)
            if not name_match:
                raise ValueError(
                    f"message in {chromium_path} has no name attribute"
                )
            message_id = name_match.group(1)
            key = (chromium_path, message_id)
            if key in feature_message_keys:
                continue
            new_block = build_branding_replacement_block(
                message_id,
                attrs,
                body,
                apply_source_overrides=False,
            )
            if new_block != match.group(0):
                message_keys.add(key)
    return message_keys


def _apply_translation_specific_replacements(
    message_id: str,
    body: str,
) -> str:
    """Apply safe locale-independent transformations to copied translations."""
    body = _apply_message_specific_body_exceptions(message_id, body)
    if message_id in EXPERIMENT_TITLE_MESSAGE_IDS and "Thorium" not in body:
        body = f"Thorium {body}"
    if message_id in ALEX313031_RECOMMENDATION_MESSAGE_IDS:
        body = body.replace("Google", "Alex313031", 1)
    return body


def xtb_translation_strategy(change: MessageChange) -> str:
    if not change.xtb_sync_needed:
        return "not_translateable"
    if change.old_translation_id == change.new_translation_id:
        return "id_unchanged"
    if (
        change.message_id in EXTERNAL_ADDITION_MESSAGE_IDS
        or change.new_translation_id in EXTERNAL_ADDITION_TRANSLATION_IDS
    ):
        return "external_xtb_addition"
    return "copy_old_translation"


def apply_message_replacements(
    source_contents: dict[str, str],
    message_allowlist: dict[str, set[str]],
) -> tuple[dict[str, str], list[MessageChange]]:
    """Apply replacements to body, desc, and meaning of allowed messages."""
    updated_contents: dict[str, str] = {}
    changes: list[MessageChange] = []
    for chromium_path, contents in source_contents.items():
        allowed_ids = message_allowlist.get(chromium_path, set())
        if not allowed_ids:
            updated_contents[chromium_path] = contents
            continue

        def replace_match(match: re.Match[str]) -> str:
            attrs, body = match.groups()
            name_match = MESSAGE_NAME_RE.search(attrs)
            if not name_match:
                raise ValueError(
                    f"message in {chromium_path} has no name attribute"
                )
            message_id = name_match.group(1)
            if message_id not in allowed_ids:
                return match.group(0)
            new_block = build_branding_replacement_block(
                message_id,
                attrs,
                body,
                apply_source_overrides=True,
            )
            old_block = match.group(0)
            if new_block != old_block:
                changes.append(
                    MessageChange(
                        chromium_path=chromium_path,
                        message_id=message_id,
                        old_translation_id=message_block_to_id(old_block),
                        new_translation_id=message_block_to_id(new_block),
                        old_block=old_block,
                        new_block=new_block,
                        xtb_sync_needed=not TRANSLATEABLE_FALSE_RE.search(attrs),
                    )
                )
            return new_block

        updated_contents[chromium_path] = MESSAGE_RE.sub(
            replace_match,
            contents,
        )
    return updated_contents, changes


def _build_xtb_cache(
    xtb_text: str,
    chromium_path: str,
) -> XtbFileCache:
    """Index translation IDs and exact block spans for one XTB file."""
    by_id: dict[str, tuple[str, str, str]] = {}
    block_spans: dict[str, list[tuple[int, int]]] = {}
    for match in XTB_TRANSLATION_RE.finditer(xtb_text):
        translation_id = match.group(1)
        if translation_id in by_id:
            raise ValueError(
                f"duplicate translation ID {translation_id} in {chromium_path}"
            )
        block = match.group(0)
        by_id[translation_id] = (block, match.group(2), match.group(3))
        block_spans[block] = [(match.start(), match.end())]
    return XtbFileCache(
        text=xtb_text,
        by_id=by_id,
        block_spans=block_spans,
    )


def find_old_translations(
    source_root: Path,
    changes: list[MessageChange],
    mappings: dict[str, GrdXtbMapping],
) -> tuple[list[MessageTranslationLookup], dict[str, XtbFileCache]]:
    """Find each modified message's old ID in every mapped XTB file."""
    xtb_cache: dict[str, XtbFileCache] = {}
    lookups: list[MessageTranslationLookup] = []

    for change in changes:
        try:
            mapping = mappings[change.chromium_path]
        except KeyError as error:
            raise KeyError(
                f"missing XTB mapping for modified file: {change.chromium_path}"
            ) from error

        matches: list[XtbTranslationMatch] = []
        missing: list[XtbTranslationMissing] = []
        for xtb_file in mapping.xtb_files:
            if xtb_file.chromium_path not in xtb_cache:
                xtb_path = source_root / xtb_file.chromium_path
                if not xtb_path.is_file():
                    raise FileNotFoundError(
                        f"mapped XTB file is missing: {xtb_file.chromium_path}"
                    )
                xtb_text = xtb_path.read_text(encoding="utf-8", errors="replace")
                xtb_cache[xtb_file.chromium_path] = _build_xtb_cache(
                    xtb_text,
                    xtb_file.chromium_path,
                )

            old_translation = xtb_cache[xtb_file.chromium_path].by_id.get(
                change.old_translation_id
            )
            if old_translation is None:
                missing.append(
                    XtbTranslationMissing(
                        source_chromium_path=change.chromium_path,
                        message_id=change.message_id,
                        old_translation_id=change.old_translation_id,
                        new_translation_id=change.new_translation_id,
                        lang=xtb_file.lang,
                        xtb_chromium_path=xtb_file.chromium_path,
                    )
                )
                continue
            block, attrs, body = old_translation
            matches.append(
                XtbTranslationMatch(
                    chromium_path=xtb_file.chromium_path,
                    block=block,
                    attrs=attrs,
                    body=body,
                )
            )

        lookups.append(
            MessageTranslationLookup(
                change=change,
                matches=tuple(matches),
                missing=tuple(missing),
            )
        )

    return lookups, xtb_cache


def build_translation_insertions(
    lookups: list[MessageTranslationLookup],
) -> list[XtbTranslationInsertion]:
    """Copy old XTB translations, apply replacements, and prepare inserts."""
    insertions: list[XtbTranslationInsertion] = []
    for lookup in lookups:
        change = lookup.change
        if change.old_translation_id == change.new_translation_id:
            continue

        for match in lookup.matches:
            copied_attrs, replacement_count = XTB_ID_ATTR_RE.subn(
                rf"\g<1>{change.new_translation_id}\g<3>",
                match.attrs,
                count=1,
            )
            if replacement_count != 1:
                raise ValueError(
                    "could not replace translation ID "
                    f"{change.old_translation_id} in {match.chromium_path}"
                )

            if change.message_id in WEB_STORE_BRAND_MESSAGE_IDS:
                replaced_body = _replace_outside_markup_preserving_chrome(match.body)
            else:
                replaced_body = _replace_outside_markup_preserving_chromium_link(
                    match.body
                )
            replaced_body = _apply_translation_specific_replacements(
                change.message_id,
                replaced_body,
            )
            replaced_body = normalize_xtb_translation_text(replaced_body)
            replaced_block = (
                f"<translation{copied_attrs}>"
                f"{replaced_body}</translation>"
            )
            insertions.append(
                XtbTranslationInsertion(
                    message_id=change.message_id,
                    chromium_path=match.chromium_path,
                    old_translation_id=change.old_translation_id,
                    new_translation_id=change.new_translation_id,
                    anchor_block=match.block,
                    new_block=replaced_block,
                )
            )
    return insertions


def _validate_anchor_blocks(anchors: set[str]) -> None:
    """Validate pending insertion anchors before using cached spans."""
    for anchor in anchors:
        match = XTB_TRANSLATION_RE.fullmatch(anchor)
        if not match:
            raise ValueError("invalid XTB insertion anchor block")


def _find_anchor_spans(
    anchors: set[str],
    block_spans: dict[str, list[tuple[int, int]]],
) -> dict[str, list[tuple[int, int]]]:
    """Find pending insertion anchors from cached exact block spans."""
    _validate_anchor_blocks(anchors)
    return {
        anchor: list(block_spans.get(anchor, ()))
        for anchor in anchors
    }


def _insert_blocks_at_anchor_spans(
    xtb_text: str,
    blocks_by_anchor: dict[str, list[str]],
    anchor_spans: dict[str, list[tuple[int, int]]],
) -> str:
    """Insert pending blocks after their previously scanned anchor spans."""
    if not blocks_by_anchor:
        return xtb_text

    parts: list[str] = []
    cursor = 0
    spans_by_position = sorted(
        (spans[0][0], spans[0][1], anchor_block)
        for anchor_block, spans in anchor_spans.items()
        if anchor_block in blocks_by_anchor
    )
    for _start, end, anchor_block in spans_by_position:
        pending_blocks = blocks_by_anchor[anchor_block]
        parts.append(xtb_text[cursor:end])
        for new_block in reversed(pending_blocks):
            parts.append("\n")
            parts.append(new_block)
        cursor = end

    if len(spans_by_position) != len(blocks_by_anchor):
        raise AssertionError("validated XTB insertion anchor was not found")
    parts.append(xtb_text[cursor:])
    return "".join(parts)


def insert_new_translations(
    source_root: Path,
    insertions: list[XtbTranslationInsertion],
    xtb_cache: dict[str, XtbFileCache] | None = None,
) -> tuple[dict[str, str], list[XtbTranslationConflict]]:
    """Insert copied translations after their old-ID blocks in memory."""
    insertions_by_path: dict[str, list[XtbTranslationInsertion]] = {}
    unique_insertions: dict[tuple[str, str], XtbTranslationInsertion] = {}
    conflicts: list[XtbTranslationConflict] = []
    for insertion in insertions:
        key = (insertion.chromium_path, insertion.new_translation_id)
        existing = unique_insertions.get(key)
        if existing is None:
            unique_insertions[key] = insertion
            continue
        if existing.new_block != insertion.new_block:
            preferred_old_translation_id = (
                PREFERRED_CONVERGED_OLD_TRANSLATION_IDS_BY_LOCALE.get(
                    (
                        insertion.new_translation_id,
                        xtb_locale_from_path(insertion.chromium_path),
                    ),
                    PREFERRED_CONVERGED_OLD_TRANSLATION_IDS.get(
                        insertion.new_translation_id
                    ),
                )
            )
            if preferred_old_translation_id is not None:
                if existing.old_translation_id == preferred_old_translation_id:
                    continue
                if insertion.old_translation_id == preferred_old_translation_id:
                    unique_insertions[key] = insertion
                    continue
            conflicts.append(
                XtbTranslationConflict(
                    chromium_path=insertion.chromium_path,
                    new_translation_id=insertion.new_translation_id,
                    selected_message_id=existing.message_id,
                    selected_old_translation_id=existing.old_translation_id,
                    selected_block=existing.new_block,
                    rejected_message_id=insertion.message_id,
                    rejected_old_translation_id=insertion.old_translation_id,
                    rejected_block=insertion.new_block,
                )
            )

    for insertion in unique_insertions.values():
        insertions_by_path.setdefault(insertion.chromium_path, []).append(insertion)

    updated_contents: dict[str, str] = {}
    for chromium_path, file_insertions in insertions_by_path.items():
        cached_xtb = (xtb_cache or {}).get(chromium_path)
        if cached_xtb is None:
            xtb_path = source_root / chromium_path
            xtb_text = xtb_path.read_text(encoding="utf-8", errors="replace")
            cached_xtb = _build_xtb_cache(xtb_text, chromium_path)
        else:
            xtb_text = cached_xtb.text
        existing_translations = dict(cached_xtb.by_id)
        anchor_spans = _find_anchor_spans(
            {insertion.anchor_block for insertion in file_insertions},
            cached_xtb.block_spans,
        )
        blocks_by_anchor: dict[str, list[str]] = {}
        for insertion in file_insertions:
            existing_new_translation = existing_translations.get(
                insertion.new_translation_id
            )
            if existing_new_translation is not None:
                existing_block = existing_new_translation[0]
                if existing_block != insertion.new_block:
                    raise ValueError(
                        "translation ID already exists with different content: "
                        f"{insertion.new_translation_id} in {chromium_path}"
                    )
                continue

            anchor_count = len(anchor_spans.get(insertion.anchor_block, ()))
            if anchor_count != 1:
                raise ValueError(
                    "expected exactly one old translation block "
                    f"{insertion.old_translation_id} in {chromium_path}, "
                    f"found {anchor_count}"
                )
            blocks_by_anchor.setdefault(insertion.anchor_block, []).append(
                insertion.new_block
            )
            existing_translations[insertion.new_translation_id] = (
                insertion.new_block,
                "",
                "",
            )
        xtb_text = _insert_blocks_at_anchor_spans(
            xtb_text,
            blocks_by_anchor,
            anchor_spans,
        )
        updated_contents[chromium_path] = xtb_text
    return updated_contents, conflicts


def write_xtb_conflict_summary_report(
    conflicts: list[XtbTranslationConflict],
    output_path: Path,
) -> None:
    """Write a compact summary of converged-ID translation conflicts."""
    grouped: dict[tuple[str, str, str, str, str], list[XtbTranslationConflict]] = {}
    for conflict in conflicts:
        key = (
            conflict.new_translation_id,
            conflict.selected_message_id,
            conflict.selected_old_translation_id,
            conflict.rejected_message_id,
            conflict.rejected_old_translation_id,
        )
        grouped.setdefault(key, []).append(conflict)

    output_path.parent.mkdir(parents=True, exist_ok=True)
    with output_path.open("w", encoding="utf-8", newline="") as report_file:
        writer = csv.writer(report_file, delimiter="\t", lineterminator="\n")
        writer.writerow(
            [
                "new_translation_id",
                "selected_message_id",
                "selected_old_translation_id",
                "rejected_message_id",
                "rejected_old_translation_id",
                "conflict_count",
                "affected_locale_count",
                "sample_locales",
                "sample_chromium_paths",
                "sample_selected_block",
                "sample_rejected_block",
                "risk",
            ]
        )
        for (
            new_translation_id,
            selected_message_id,
            selected_old_translation_id,
            rejected_message_id,
            rejected_old_translation_id,
        ), items in sorted(grouped.items()):
            locales = [
                locale
                for locale in (xtb_locale_from_path(item.chromium_path) for item in items)
                if locale
            ]
            risk = (
                "review_different_message_ids"
                if selected_message_id != rejected_message_id
                else "low_same_message_converged_ids"
            )
            writer.writerow(
                [
                    new_translation_id,
                    selected_message_id,
                    selected_old_translation_id,
                    rejected_message_id,
                    rejected_old_translation_id,
                    str(len(items)),
                    str(len(set(locales))),
                    _sample_values(locales),
                    _sample_values(item.chromium_path for item in items),
                    items[0].selected_block,
                    items[0].rejected_block,
                    risk,
                ]
            )


def write_xtb_missing_summary_report(
    missing: list[XtbTranslationMissing],
    output_path: Path,
) -> None:
    """Write a compact summary of missing old-ID translation lookups."""
    grouped: dict[tuple[str, str, str, str], list[XtbTranslationMissing]] = {}
    for item in missing:
        key = (
            item.source_chromium_path,
            item.message_id,
            item.old_translation_id,
            item.new_translation_id,
        )
        grouped.setdefault(key, []).append(item)

    output_path.parent.mkdir(parents=True, exist_ok=True)
    with output_path.open("w", encoding="utf-8", newline="") as report_file:
        writer = csv.writer(report_file, delimiter="\t", lineterminator="\n")
        writer.writerow(
            [
                "source_chromium_path",
                "message_id",
                "old_translation_id",
                "new_translation_id",
                "missing_locale_count",
                "sample_locales",
                "sample_xtb_paths",
                "strategy",
                "risk",
            ]
        )
        for (
            source_chromium_path,
            message_id,
            old_translation_id,
            new_translation_id,
        ), items in sorted(grouped.items()):
            locales = [item.lang for item in items if item.lang]
            risk = "review_many_missing_locales" if len(set(locales)) >= 40 else "low"
            writer.writerow(
                [
                    source_chromium_path,
                    message_id,
                    old_translation_id,
                    new_translation_id,
                    str(len(set(locales))),
                    _sample_values(locales),
                    _sample_values(item.xtb_chromium_path for item in items),
                    "missing_old_id_no_translation_inserted",
                    risk,
                ]
            )


def write_updated_files(source_root: Path, updated_contents: dict[str, str]) -> None:
    """Write prepared source-relative file contents."""
    for chromium_path, contents in updated_contents.items():
        (source_root / chromium_path).write_bytes(contents.encode("utf-8"))


def write_dry_run_report(
    changes: list[MessageChange],
    output: object = sys.stdout,
) -> None:
    """Print planned changes in a stable TSV format."""
    writer = csv.writer(output, delimiter="\t", lineterminator="\n")
    writer.writerow(
        [
            "chromium_path",
            "message_id",
            "old_translation_id",
            "new_translation_id",
            "xtb_sync_needed",
            "xtb_translation_strategy",
            "old_text",
            "new_text",
        ]
    )
    for change in changes:
        writer.writerow(
            [
                change.chromium_path,
                change.message_id,
                change.old_translation_id,
                change.new_translation_id,
                "yes" if change.xtb_sync_needed else "no",
                xtb_translation_strategy(change),
                change.old_block,
                change.new_block,
            ]
        )


def configure_stdout_utf8() -> None:
    """Use UTF-8 for redirected dry-run reports on Windows."""
    reconfigure = getattr(sys.stdout, "reconfigure", None)
    if reconfigure is not None:
        reconfigure(encoding="utf-8", newline="")


def build_arg_parser() -> argparse.ArgumentParser:
    parser = argparse.ArgumentParser(
        description="Synchronize Thorium GRD/GRDP string replacements.",
    )
    parser.add_argument(
        "source_root",
        type=readable_source_root,
        help="Chromium source tree root to inspect or update.",
    )
    parser.add_argument(
        "--file-allowlist",
        type=readable_file,
        required=True,
        help="CSV file listing GRD/GRDP files allowed for processing.",
    )
    parser.add_argument(
        "--message-allowlist",
        type=readable_file,
        required=True,
        help="CSV file listing message IDs allowed for processing.",
    )
    parser.add_argument(
        "--dry-run",
        action="store_true",
        help="Report planned changes without modifying files.",
    )
    parser.add_argument(
        "--xtb-conflict-report",
        type=Path,
        default=None,
        help="Optional TSV path for summarized converged new-ID conflicts.",
    )
    parser.add_argument(
        "--xtb-missing-report",
        type=Path,
        default=None,
        help="Optional TSV path for summarized missing old-ID lookups.",
    )
    parser.add_argument(
        "--feature-message-ownership",
        type=Path,
        default=None,
        help=(
            "CSV defining feature-patch message ownership; defaults to "
            "feature_patch_message_ownership.csv beside the message allowlist."
        ),
    )
    return parser


def main() -> int:
    parser = build_arg_parser()
    args = parser.parse_args()

    # Validate all configuration inputs before preparing or writing changes.
    file_allowlist_rows = load_csv_rows(
        args.file_allowlist,
        required_columns={"chromium_path", "role"},
    )
    message_allowlist_rows = load_csv_rows(
        args.message_allowlist,
        required_columns={
            "chromium_path",
            "message_id",
            "allowlist_category",
            "translation_required",
        },
    )
    feature_message_ownership_path = args.feature_message_ownership
    if feature_message_ownership_path is None:
        feature_message_ownership_path = (
            args.message_allowlist.parent / "feature_patch_message_ownership.csv"
        )
    if not feature_message_ownership_path.is_file():
        raise FileNotFoundError(
            "feature message ownership file is missing: "
            f"{feature_message_ownership_path}"
        )
    feature_message_rows = load_csv_rows(
        feature_message_ownership_path,
        required_columns={
            "patch_path",
            "chromium_path",
            "message_id",
            "ownership",
            "destination",
        },
    )
    validate_feature_message_ownership(feature_message_rows)

    text_sync_file_rows = select_text_sync_file_rows(file_allowlist_rows)
    added_file_rows = select_thorium_added_file_rows(file_allowlist_rows)
    validate_feature_messages_excluded(
        message_allowlist_rows,
        feature_message_rows,
    )
    if not text_sync_file_rows:
        raise ValueError("file allowlist contains no text-sync roles")
    source_contents = load_allowed_source_files(
        args.source_root,
        text_sync_file_rows,
    )
    explicit_message_keys = build_message_keys(message_allowlist_rows)
    feature_message_keys = build_message_keys(feature_message_rows)
    auto_branding_message_keys = discover_auto_branding_message_keys(
        source_contents,
        feature_message_keys,
    )
    message_allowlist = _build_message_allowlist_from_keys(
        auto_branding_message_keys | explicit_message_keys
    )
    updated_contents, changes = apply_message_replacements(
        source_contents,
        message_allowlist,
    )
    xtb_sync_changes = [
        change
        for change in changes
        if xtb_translation_strategy(change) == "copy_old_translation"
    ]
    xtb_mappings = build_grd_xtb_mapping(
        args.source_root,
        sorted({change.chromium_path for change in xtb_sync_changes}),
    )
    translation_lookups, xtb_cache = find_old_translations(
        args.source_root,
        xtb_sync_changes,
        xtb_mappings,
    )
    missing_old_translations = [
        missing
        for lookup in translation_lookups
        for missing in lookup.missing
    ]
    translation_insertions = build_translation_insertions(translation_lookups)
    updated_xtb_contents, xtb_conflicts = insert_new_translations(
        args.source_root,
        translation_insertions,
        xtb_cache,
    )
    if args.xtb_conflict_report is not None:
        write_xtb_conflict_summary_report(xtb_conflicts, args.xtb_conflict_report)
    if args.xtb_missing_report is not None:
        write_xtb_missing_summary_report(
            missing_old_translations,
            args.xtb_missing_report,
        )
    if xtb_conflicts:
        print(
            "info: selected the first translation candidate for "
            f"{len(xtb_conflicts)} converged XTB conflicts",
            file=sys.stderr,
        )
    if added_file_rows:
        print(
            "info: routed "
            f"{len(added_file_rows)} Thorium-added files to the separate "
            "additions workflow",
            file=sys.stderr,
        )
    if missing_old_translations:
        missing_messages = {
            (item.source_chromium_path, item.message_id)
            for item in missing_old_translations
        }
        print(
            "info: old translation IDs were missing from "
            f"{len(missing_old_translations)} mapped XTB lookups across "
            f"{len(missing_messages)} messages; continuing without them",
            file=sys.stderr,
        )
    if args.dry_run:
        configure_stdout_utf8()
        write_dry_run_report(changes)
    else:
        write_updated_files(args.source_root, updated_contents)
        write_updated_files(args.source_root, updated_xtb_contents)

    return 0


if __name__ == "__main__":
    raise SystemExit(main())
