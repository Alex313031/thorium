#!/usr/bin/env python3

from pathlib import Path
import json
import os
import traceback
import sys


def migrate_profile(profile: Path):
    prefs_file = profile / 'Preferences'
    with prefs_file.open() as fp:
        prefs = json.load(fp)

    modified = False
    KEYS_TO_REMOVE = ['account_info', 'google']
    for key in KEYS_TO_REMOVE:
        if key in prefs:
            del prefs[key]
            modified = True

    if modified:
        print('Update profile:', profile.name)
        tmp = prefs_file.with_suffix('.tmp')
        with open(tmp, 'w') as fp:
            json.dump(prefs, fp)

        tmp.rename(prefs_file)


def main():
    data_dir = Path(os.environ['XDG_CONFIG_HOME']) / 'chromium'
    profiles = [data_dir / 'Default']
    profiles.extend(data_dir.glob('Profile *'))

    succeeded = True

    for profile in profiles:
        try:
            migrate_profile(profile)
        except Exception:
            traceback.print_exc()
            succeeded = False

    sys.exit(0 if succeeded else 1)


if __name__ == '__main__':
    main()
