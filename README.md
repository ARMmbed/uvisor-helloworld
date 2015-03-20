## `Hello World!` Example for uVisor

This is a `Hello World!` yotta executable created to show some of the security
features provided by the uVisor.

The application endlessly calls the `secure_print()` function, which prints a
message containing a secret. The output is shown on the USB serial port.

Currently only the following configuration is supported:
- Freescale FRDM-K64F board (GCC ARM Embedded toolchain)

This example relies on the security features provided by the uVisor yotta
module, called uvisor-lib. Refer to its
[documentation](https://github.com/ARMmbed/uvisor-lib-private) for
implementation-specific details. Please note that the [official
documentation](https://github.com/ARMmbed/uvisor-private) for the uVisor
describes the higher level security model, instead.

### HowTo

#### Build

This is a yotta executable. You need the latest
[yotta](https://github.com/ARMmbed/yotta) release version (latest tested:
0.1.0) and all of its dependencies. See also the [yotta
documentation](http://docs.yottabuild.org/) for further details about the
installation and the build process.

First, select the correct targetl and build:
```bash
cd uvisor-helloworld
yt target frdm-k64f-gcc
yt build
```

The resulting binary file will be located in
`build/frdm-k64f-gcc/source/uvisor-helloworld/`.

#### Flash

Connect your board to a computer USB port and simply drag & drop the binary
file into the attached `MBED` device.

#### Run

The application will be running right after flashing. You can observe the UART
output on the USB serial port.

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
First, make sure you have the correct
[driver](http://developer.mbed.org/handbook/Windows-serial-configuration)
installed for the serial port. You also need a terminal application: We suggest
[PuTTY](http://www.chiark.greenend.org.uk/~sgtatham/putty/download.html). The
port should be `COMx` (`COM1`, for example), the baud rate is 115200.

### Troubleshooting
If you find any dependency problem during the build process, please make sure
to use the most up-to-date versions of the linked modules. You can always use:
```bash
yt update --update-linked
```
to update the dependecies of the linked modules, too. If the build process
still fails, consider cleaning the build environment:
```bash
yt clean
```
and retrying the build procedure.

### Debugging
For failures or faults captured by the uVisor, a debugging message is printed
on the same serial port of the uvisor-helloworld application.

Further debug messages are disabled by default. If you want to enable them, you
need to build a new version of uvisor-lib starting from the uVisor source code,
with the debug option enabled. This process is described in the [uVisor
documentation](https://github.com/ARMmbed/uvisor-private) and is here reported
briefly:
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
