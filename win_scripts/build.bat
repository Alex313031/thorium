## Windows native build compile script.
# Copyright (c) 2022 Alex313031. 

cd C:\src\chromium\src\ &&

set NINJA_SUMMARIZE_BUILD=1 &&

autoninja -C out\thorium chrome chromedriver content_shell setup mini_installer views_examples_with_content -j 8

exit 0
