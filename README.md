## `Hello World!` Example for uVisor

This is a `Hello World!` yotta executable created to show some of the security features provided by the uVisor.

The application endlessly calls the `secure_print()` function, which prints a message containing a secret. The output is shown on the USB serial port.

Currently only the following configuration is supported:
- Freescale FRDM-K64F board
- GCC ARM Embedded Toolchain

### Installation

This is a yotta executable. You need the latest [yotta](https://github.com/ARMmbed/yotta) release version (latest tested: 0.0.36) and all of its dependencies. See also the [yotta documentation](http://armmbed.github.io/yotta/) for further details about the installation and the build process.

Assuming you have cloned this repository, move to its folder. Select target, install and build:
```bash
cd /path/to/uvisor-helloworld
yotta target frdm-k64f-gcc
yotta install
yotta build
```

The resulting binary file will be located in `build/frdm-k64f-gcc/source/uvisor-helloworld/`. After flashing, you can observe the UART output on the USB serial port.

On **OS X** (supposing your serial port is `/dev/tty.usbmodemfa142`):
```bash
screen /dev/ttyACM0 9600 # Control-a D D to logout
```
On **Linux** (supposing your serial port is `/dev/ttyACM0`):
```bash
stty -F /dev/ttyACM0 9600 # this could be needed if the baud rate is not
			  # correctly detected
cat /dev/ttyACM0	  # Control-a D D to logout
```

### Troubleshooting
If you find any dependency problem during the build process, please make sure to use the most up-to-date versions of the linked modules. You can always use:
```bash
yotta update --update-linked
```
to update the dependecies of the linked modules, too. If the build process still fails, consider cleaning the build environment:
```bash
rm -rf build
```
and retrying the build procedure.

For failures or faults captured by the uVisor, a debugging message is printed through the SWO port. A JLink debugger could be used to show this output; the application is anyway halted at the print message.

For more information on the debugging features of the uVisor visit [uvisor-lib](https://github.com/ARMmbed/uvisor).
