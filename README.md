## Hello World Example for the uVisor

This is a Hello World yotta executable created to show some of the security features provided by the uVisor. The application hosts a secure box that is able to generate a secret at runtime. The secret is stored in the box context, so it is protected by uVisor against the rest of the code.

This is a challenge. For the security model to be effective, the box secret must not be leaked in any way. You can attempt whatever approach you prefer to try and read the secret. By default, the main event in this application continuously tries to verify its challenge against the box secret.

We already provide one example of such an approach. See the [Run](#run) section for more information.

Supported platforms:
- [Freescale FRDM-K64F](http://developer.mbed.org/platforms/FRDM-K64F/) ([GCC ARM Embedded toolchain](https://launchpad.net/gcc-arm-embedded)).
- [STMicorelectronics STM32F429I-DISCO](http://www.st.com/web/catalog/tools/FM116/SC959/SS1532/PF259090) ([GCC ARM Embedded toolchain](https://launchpad.net/gcc-arm-embedded)).

This example relies on the security features provided by the uVisor yotta module, called `uvisor-lib`. Refer to its [documentation](https://github.com/ARMmbed/uvisor-lib) for implementation-specific details. Please note that the [official documentation](https://github.com/ARMmbed/uvisor) for the uVisor describes the higher level security model, instead.

### How To

The following section explains how to build, flash and run the uVisor on OS X, Linux and Windows.

### Pre-Requisites

Please install the following:

* [yotta](https://github.com/ARMmbed/yotta). Please note that yotta has its own set of dependencies, listed in the [installation instructions](http://armmbed.github.io/yotta/#installing-on-windows).

#### Build

First, navigate to the directory containing your source files:

```bash
cd uvisor-helloworld
```

yotta must know which platform (target) it is building to. So we declare the target, then build.

```bash
yotta target frdm-k64f-gcc
yotta build
```

The resulting binary file will be located in
`build/frdm-k64f-gcc/source/uvisor-helloworld.bin`.

The available targets for this example are:
- `frdm-k64f-gcc`
- `stm32f429i-disco-gcc`

#### Flash

Connect your board to your computer USB port and simply drag & drop the binary file from the previous step into the `MBED` device listed in the file browser. The STM32F4 does not offer yet the possibility to drag & drop a firmware on it, so you will need to use the STMicroelectronics proprietary [software](http://www.st.com/web/en/catalog/tools/PF258168) (available on Windows only) to flash it.

For Linux please use [st-flash](https://github.com/texane/stlink) for uploading new firmware to the device.

On OS X [st-flash](https://github.com/texane/stlink) is available via `brew install stlink` for updating the device firmware. Please refer to [homebrew](http://brew.sh/) for more information.

#### Run

Hit the reset button after flashing to start program execution. The application starts running right after you reset the processor. When the blue (K64F) or green (STM32F4) LED blinks with a 1s period, it means that the program is simply comparing the password with the challenge, the comparison is not successful and it is ready to try again.

If you press the on-board push-button (`SW2` on the K64F, `USER` on the STM32F4), or wait 10 seconds, a routine runs that tries to read the secret from the secure box. If the read is successful the same LED starts blinking faster (a period of 200ms). If the uVisor captures the denied access, the system halts and the red LED blinks (`PERMISSION_DENIED` error code). On the K64F do not use `SW3`, as it triggers a non-maskable interrupt, which is currently unsupported and results in a halt with a `NOT_ALLOWED` error code.

### Debugging

Failures and faults captured by the uVisor will trigger a system halt. For some specific faults the uVisor will enable an LED blinking pattern which is specific to the error encountered. Please refer to the [uvisor-lib documentation](https://github.com/ARMmbed/uvisor-lib/blob/master/DOCUMENTATION.md#error-patterns) for a complete list of the available blinking patterns.

Further debugging messages are silenced by default. If you want to enable them, you need to build a new version of uvisor-lib starting from the uVisor source code, with the debug option enabled. All messages are always printed through the semihosting interface, hence a debugger must be connected to the board to observe them. Make sure you have the latest [uVisor source code](https://github.com/ARMmbed/uvisor) and proceede as follows:

```bash
# assuming this is your code tree:
# ~/code/
#   |
#   `- uvisor-helloworld/
#   |
#   `- uvisor/
#      |
#      `- release/
#      |
#      `- k64f/uvisor/
#      |
#      `- stm32f4/uvisor/

# cd to uvisor (using k64f in this example)
cd ~/code/uvisor/k64f/uvisor

# build a debug release
make OPT= clean release

# link the newly created release to yotta
cd ~/code/uvisor/release
yotta link

# link your project to the newly created release
cd ~/code/uvisor-helloworld
yotta link uvisor-lib
```
