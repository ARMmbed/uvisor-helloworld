# Hello World example for the uVisor

This is a Hello World yotta executable created to show some of the security features provided by the uVisor. The application hosts a secure box that is able to generate a secret at runtime. The secret is stored in the box context, so it is protected by uVisor against the rest of the code.

This is a challenge. For the security model to be effective, the box secret must not be leaked in any way. You can attempt whatever approach you prefer to try and read the secret. By default, the main event in this application continuously tries to verify its challenge against the box secret.

We already provide one example of such an approach. See the [Run](#run) section for more information.

This example relies on the security features provided by the uVisor. Refer to its [library documentation](https://github.com/ARMmbed/uvisor-lib/blob/master/README.md) to know more about its use cases and about the `uvisor-lib` APIs. Please note that the [official uVisor documentation](https://github.com/ARMmbed/uvisor/blob/master/README.md) describes the higher level security model, instead.

### Supported platforms

The following platforms are currently supported:

* [NXP FRDM-K64F](http://developer.mbed.org/platforms/FRDM-K64F/)
* [STMicorelectronics STM32F429I-DISCO](http://www.st.com/web/catalog/tools/FM116/SC959/SS1532/PF259090)

The uVisor pre-linked binary images are built with the Launchpad [GCC ARM Embedded](https://launchpad.net/gcc-arm-embedded) toolchain. Currently only applications built with the same toolchain are supported.

## How to

The following section explains how to build, flash and run the uVisor on OS X, Linux and Windows.

### Pre-requisites

Please install the following:

* yotta. Please note that yotta has its own set of dependencies, listed in the [installation instructions](http://yottadocs.mbed.com/#installing).

### Build

First, navigate to the directory containing your source files:

```bash
cd uvisor-helloworld
```

yotta must know which platform (target) it is building to. So we declare the target, then build.

```bash
yotta target frdm-k64f-gcc
yotta build
```

The resulting binary file will be located in `build/frdm-k64f-gcc/source/uvisor-helloworld.bin`.

The available targets for this example are:

* `frdm-k64f-gcc`
* `stm32f429i-disco-gcc`

### Flash

Connect your board to your computer USB port and simply drag & drop the binary file from the previous step into the `MBED` device listed in the file browser. The STM32F4 does not offer yet the possibility to drag & drop a firmware on it, so you will need to use the STMicroelectronics proprietary [software](http://www.st.com/web/en/catalog/tools/PF258168) (available on Windows only) to flash it.

For Linux please use [st-flash](https://github.com/texane/stlink) for uploading new firmware to the device.

On OS X st-flash is available via `brew install stlink` for updating the device firmware. Please refer to [homebrew](http://brew.sh/) for more information.

### Run

Hit the reset button after flashing to start program execution. The application starts running right after you reset the processor. When the main LED blinks with a 1s period, it means that the program is simply comparing the password with the challenge, the comparison is not successful and it is ready to try again.

You can observe some logging messages on the UART port. On Linux/OS X you can use `screen <path to serial> 115200`. On Windows, make sure to have the serial port drivers installed; you can use a terminal application, like [PuTTY](http://www.putty.org/), to connect to the device. In this example the UART baud rate is set to 115200.

Please note that on some devices (STM32F429I-DISCO) the on-board USB port cannot be used for UART output. In that case you need to connect an external port.

Messages printed through the UART port are not blocking, so you can still use the blue/green and red LEDs to determine the outcome of the application. Otherwise, You will observe this output:

```
***** uvisor-helloworld example *****
main unprivileged box configured
verifying secret... done
verifying secret... done
verifying secret... done
...
```

If you press the on-board push-button, or wait 10 seconds, a routine runs that tries to read the secret from the secure box.

```
...
verifying secret... done
attempting to read the secret...
```

If the read is successful the same LED starts blinking faster (a period of 200ms) and a confirmation message is appended to the previous one:

```
attempting to read the secret... done
```

If instead the uVisor captures the denied access, the system halts and a different LED blinks with an error-specific pattern (see [Debug](#debug) for more information). The system then reboots and the application starts again.

#### Hardware-specific configurations

This example uses LEDs and push-buttons, the configuration of which depends on the target:

| Target                 | Main LED   | uVisor LED | Button        | Comments                                         |
|------------------------|------------|------------|---------------|--------------------------------------------------|
| `frdm-k64f-gcc`        | `LED_BLUE` | `LED_RED`  | `SW2`         | Do not use `SW3`; uVisor will halt               |
| `stm32f429i-disco-gcc` | `LED1`     | `LED2`     | `USER_BUTTON` | Use an external UART port for the program output |

### Debug

Failures and faults captured by the uVisor will trigger a system halt. Upon halt, the uVisor issues an error code to the device, which can be used for debugging. Please refer to the [API documentation](https://github.com/ARMmbed/uvisor-lib/blob/master/DOCUMENTATION.md#error-codes) for a complete list of the uVisor error codes.

In this application we provide a sample of what we call a *debug box*. A debug box is a secure box that can sign up to uVisor to handle debug events. In this way, every privileged uVisor debug handler can also trigger the execution of an unprivileged handler.

The current version of the debug box driver only supports one handler — the system halt handler:

```C
void halt_error(int reason)
```

The debug box in this example receives the uVisor error codes and encodes them in red LED blinking patterns, to provide a faster feedback to the user.

The uVisor is also able to print additional debug messages, which are silenced by default. If you want to enable them, you need to build this application in debug mode:

```bash
yotta build -d
```

This command will ensure that a debug version of uVisor is linked to the application. In debug mode, uVisor outputs logging messages via semihosting, which requires a connected debugger. If a debugger is not connected, the application will halt.

Please refer to the [Debugging uVisor](https://github.com/ARMmbed/uvisor/blob/master/docs/DEBUGGING.md) document for more information about the built-in uVisor debug features and for documentation on the debug box driver.
