## Thorium for ARM CPUs <img src="https://github.com/Alex313031/Thorium/blob/main/logos/STAGING/arm_logo.png" width="128">
This contains the *BUILD.gn* file for making an ARM32/ARM64 build of Thorium for Linux or Windows. Just run `setup_arm.sh` in here after running the normal `setup.sh` that's in the root of this repo.

## Raspberry Pi Builds &nbsp;<img src="https://github.com/Alex313031/Thorium/blob/main/logos/STAGING/Raspberry_Pi_Logo.svg" width="28">
__IMPORTANT:__ For a [Raspberry Pi](https://www.raspberrypi.org/) build (ARM64), the contents of [raspi_arm.gni](https://github.com/Alex313031/Thorium/blob/main/arm/raspi_arm.gni) should be copied and used in *//chromium/src/build/config/arm.gni*, AND the contents of [raspi_args.gn](https://github.com/Alex313031/Thorium/blob/main/arm/raspi_args.gn) should be used for your *args.gn*.
