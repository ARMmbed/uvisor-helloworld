## `Hello World!` Example for the uVisor

This is a `Hello World!` yotta executable created to show some of the security features provided by the uVisor.

The application endlessly calls the `secure_print()` function, which prints a message containing a secret. The output is shown on the USB serial port.

Currently the only supported platform is the [Freescale FRDM-K64F](http://developer.mbed.org/platforms/FRDM-K64F/) ([GCC ARM Embedded toolchain](https://launchpad.net/gcc-arm-embedded)).

This example relies on the security features provided by the uVisor yotta module, called uvisor-lib. Refer to its [documentation](https://github.com/ARMmbed/uvisor-lib-private) for implementation-specific details. Please note that the [official documentation](https://github.com/ARMmbed/uvisor-private) for the uVisor describes the higher level security model, instead.

### How To

The following section explains how to build, flash and run the uVisor on OS X, Linux and Windows.

### Pre-Requisites

Please install the following:

* [yotta](https://github.com/ARMmbed/yotta). Please note that **yotta has its own set of dependencies**, listed in the [installation instructions](http://armmbed.github.io/yotta/#installing-on-windows).
* If your OS is Windows, please follow the installation instructions [for the serial port driver](https://developer.mbed.org/handbook/Windows-serial-configuration).
* A terminal application: We suggest [PuTTY](http://www.chiark.greenend.org.uk/~sgtatham/putty/download.html).

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
`build/frdm-k64f-gcc/source/uvisor-helloworld/`.

#### Flash

Connect your board to your computer's USB port (using the OpenSDA port on the board) and simply drag & drop the binary file from the previous step into the `MBED` device listed in the file browser.

#### Run

The application will be running right after flashing. You can observe the UART output on the USB serial port. Follow the instructions below.

**Tip:** you can identify the serial port the board is connected to by using ``mbedls`` (found in this release), as well as from your operating system's device manager.

##### **OS X**

Assuming your serial port is `/dev/tty.usbmodemfa142`:

```bash
screen /dev/tty.usbmodemfa142 115200 # Control-a D D to logout
```

##### **Linux**

Assuming your serial port is `/dev/ttyACM0`:

```bash
# this could be needed if the baud rate is not correctly detected
stty -F /dev/ttyACM0 115200
cat /dev/ttyACM0
```

##### **Windows**

Use PuTTY. The port should be `COMx` (`COM1`, for example) and the baud rate (speed) is 115200.

### Troubleshooting

If you find any dependency problems during the build process, please make sure to use the most up-to-date versions of the linked modules. You can always update the dependencies of the linked modules:

```bash
yt update --update-linked
```
If the build process still fails, consider cleaning the build environment:

```bash
yt clean
```
Then retry the build procedure.

### Debugging

For failures or faults captured by the uVisor, a debugging message is printed on the same serial port of the uvisor-helloworld application.

Further debug messages are disabled by default. If you want to enable them, you need to build a new version of uvisor-lib starting from the uVisor source code, with the debug option enabled. This process is described in the [uVisor documentation](https://github.com/ARMmbed/uvisor-private) and is repeated here briefly:

```bash
# assuming this is your code tree:
# ~/code/
#   |
#   `- my_project/
#   |
#   `- uvisor/
#      |
#      `- release/
#      |
#      `- k64f/uvisor/

# cd to uvisor
cd ~/code/uvisor/k64f/uvisor

# build a debug release
make OPT= clean release

# link the newly created release to yotta
cd ../../release
yt link

# link your project to the newly created release
cd ~/code/my_project
yt link uvisor-lib
```
