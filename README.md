## `Hello World!` Example for the uVisor

This is a `Hello World!` yotta executable created to show some of the security features provided by the uVisor. The application hosts a secure box that is able to generate a secret at runtime. The secret is stored in the box context, so it is protected by uVisor against the rest of the code.

We propose here a challenge. For the seuciry model to be effective, the box secret should not be in any way leaked. You can attempt whatever approach you prefer to try and read the secret. By default, the main loop continuosuly tries to verify its challenge against the box secret.

We already provide one example of such approach. An on-board push-button (`SW2` on the K64F, `USER_BUTTON` on the STM32F) triggers an interrupt service routine that tries to read the secret from the (leaked) box context pointer. If the read is successful the next iteration of the password check will be successful and an LED will blink (blue on K64F, green on STM32F4). If, on the contrary, the uVisor captures the denied access, the system will halt and the red LED will blink.

Supported platforms:
- [Freescale FRDM-K64F](http://developer.mbed.org/platforms/FRDM-K64F/) ([GCC ARM Embedded toolchain](https://launchpad.net/gcc-arm-embedded)).
- [STMicorelectronics STM32F429I-DISCO](http://www.st.com/web/catalog/tools/FM116/SC959/SS1532/PF259090) ([GCC ARM Embedded toolchain](https://launchpad.net/gcc-arm-embedded)).

This example relies on the security features provided by the uVisor yotta module, called uvisor-lib. Refer to its [documentation](https://github.com/ARMmbed/uvisor-lib) for implementation-specific details. Please note that the [official documentation](https://github.com/ARMmbed/uvisor) for the uVisor describes the higher level security model, instead.

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
yt target frdm-k64f-gcc
yt build
```

The resulting binary file will be located in
`build/frdm-k64f-gcc/source/uvisor-helloworld.bin`.

The available targets for this example are:
- `frdm-k64f-gcc`
- `stm32f429i-disco-gcc`

#### Flash

Connect your board to your computer USB port and simply drag & drop the binary file from the previous step into the `MBED` device listed in the file browser. The STM32F4 does not offer yet the possibility to drag & drop a firmware on it, so you will need to use the STMicroelectronics proprietary [software](http://www.st.com/web/en/catalog/tools/PF258168) (available on Windows only) to flash it.

#### Run

Hit the reset button after flashing to start program execution. The application will be running right after you reset the processor. If no LED blinks, it means that the program is simply comparing the password with the challenge, and the comparison is not successful. A blinking blu/green LED will signal a successful password leakage; a blinking red LED means the uVisor halted the system because of a denied access.

### Troubleshooting

If you find any dependency problems during the build process, please make sure to use the most up-to-date versions of the linked modules. You can always update the dependencies of the linked modules:

```bash
yt up -l
```
If the build process still fails, consider cleaning the build environment:

```bash
yt clean
```
Then retry the build procedure.

### Debugging

Failures and faults captured by the uVisor will trigger a system halt. For some specific faults the uVisor will enable an LED blinking pattern which is specific to the error encountered. Please refer to the [uvisor-lib documentation](https://github.com/ARMmbed/uvisor-lib) for details.

Debugging messages are silenced by default. If you want to enabled them, you need to build a new version of uvisor-lib starting from the uVisor source code, with the debug option enabled. All messages are always printed through the semihosting interface; a debugger must be connected to the board to observe them. This process is described in the [uVisor documentation](https://github.com/ARMmbed/uvisor) and is repeated here briefly:

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
yt link

# link your project to the newly created release
cd ~/code/uvisor-helloworld
yt link uvisor-lib
```
