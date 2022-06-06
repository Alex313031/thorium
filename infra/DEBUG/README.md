## Thorium Debugging Infra <img src="https://github.com/Alex313031/Thorium/blob/main/logos/STAGING/bug.svg" width="28">

 - This contains *.gn* files and scripts for generating DEBUG builds of Thorium for debugging, testing, and inspection.
 - The ABOUT_GN_ARGS.txt describes what each line in the args &#42;.gn files do, also useful for the regular build args &#42;.gn files. \
&nbsp;&nbsp; __NOTE:__ You cannot build installers for any platform with a debug build. Running the clean.sh script in the root of the repo is highly reccomended to get your //out/thorium dir from ~6-7 GB to something reasonable, like ~1-2 GB.
 - Running the `build_debug.sh` or `build_win_debug.sh` will build the *Thorium UI Debug Shell (views_examples_with_content)* target as well as the rest of Thorium.
 - Running `build_debug_shell.sh` can be used for any platform, and will build the standalone Thorium UI Debug Shell. \
&nbsp;&nbsp; __NOTE:__ Running either of the above will make a directory in //out/thorium called Thorium_UI_Debug_Shell with all the artifacts and Readme inside, suitable for zipping up and distributing to users.
 - For more information, read the [DEBUG_SHELL_README.md](https://github.com/Alex313031/Thorium/blob/main/infra/DEBUG/DEBUG_SHELL_README.md) file).
 
<img src="https://github.com/Alex313031/Thorium/blob/main/logos/NEW/thorium_infra_256.png">
