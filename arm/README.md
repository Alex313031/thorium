## Thorium for ARM CPUs <img src="https://github.com/Alex313031/Thorium/blob/main/logos/STAGING/arm_logo.png" width="128">
This contains a special [*BUILD.gn*](https://github.com/Alex313031/Thorium/blob/main/arm/config/compiler/BUILD.gn) file for making an ARM32/ARM64 build of Thorium for Raspberry Pi or Windows. Just run `setup_arm.sh` in here after running the normal `setup.sh` that's in the root of this repo.

__NOTE:__ This dir is only for Raspberry Pi and Windows On ARM. For regular Linux ARM builds, and MacOS ARM builds, you can just use the regular files in the repo.

 -  Windows On ARM (WoA) Builds: Use the [win_ARM_args.gn](https://github.com/Alex313031/Thorium/blob/main/arm/win_ARM_args.gn) file for your *args.gn*.

## Raspberry Pi Builds &nbsp;<img src="https://github.com/Alex313031/Thorium/blob/main/logos/STAGING/Raspberry_Pi_Logo.svg" width="28">
__IMPORTANT:__ For a [Raspberry Pi](https://www.raspberrypi.org/) build (ARM64), the contents of [raspi_arm.gni](https://github.com/Alex313031/Thorium/blob/main/arm/raspi_arm.gni) should be manually copied and used in *//chromium/src/build/config/arm.gni*, AND the contents of [raspi_args.gn](https://github.com/Alex313031/Thorium/blob/main/arm/raspi_args.gn) should be used for your *args.gn*. \
__&ndash;&nbsp;Note that ARM32 Raspberry Pi builds of Thorium are not supported.__ They should only be used on Raspi 3B, 3B+, 4, and 400 models with a 64 Bit OS. For more info, see > [Here](https://www.raspberrypi.com/news/raspberry-pi-os-64-bit/).
