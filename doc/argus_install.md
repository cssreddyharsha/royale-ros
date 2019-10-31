Installing the Argus SDK
=========================
The Argus SDK is distributed as a standalone, binary-only package directly
from pmd. It is typically packaged in a file called `libargus.zip`. This
document assumes you have that zip file available to you. The pmd-distributed
Argus SDK comes with installation instructions and you are free to follow
those. However, in this document, we present an alternate
approach. Specifically, we provide a method to repackage the SDK as a debian
package so that it can be cleanly integrated with the Debian/Ubuntu package
manager.

**NOTE:** We assume you are on a Linux machine with Python 2.7, `dpkg`, and
  `unzip`.

Repackaging Argus as a deb is handled via our
[debianize_argus.py](../utils/debianize_argus.py) script. Assuming you
are at the top-level of this source distribution, you can interface with
`debianize_argus.py` as follows:

```
[ ~/dev/argus-ros ]
tpanzarella@tuna: $ cd utils/

[ ~/dev/argus-ros/utils ]
tpanzarella@tuna: $ ./debianize_argus.py --help
usage: debianize_argus.py [-h] --infile INFILE [--prefix PREFIX]
                           [--arch {x86_32,x86_64,arm_32}] [--full]

Create a deb file for the Argus SDK from libargus.zip

optional arguments:
  -h, --help            show this help message and exit
  --infile INFILE       Input libargus.zip file (default: None)
  --prefix PREFIX       Root directory for the deployed Argus SDK.
                        Specifically, the SDK will be installed in
                        `<prefix>/libargus-<version>-LINUX-<arch>' and a
                        symlink will be be created such that: `<prefix>/argus
                        -> <prefix>/libargus-<version>-LINUX-<arch>' Typical
                        choices for `--prefix' are `/opt' or `/usr/local'.
                        (default: /opt)
  --arch {x86_32,x86_64,arm_32}
                        Architecture for target deb file (default: x86_64)
  --full                Debianize the full SDK (default: False)
```

As noted in the help message above, `debianize_argus.py` is parameterized via
an `--infile` (the full path to your `libargus.zip` file acquired from pmd),
`--prefix` (the root-level prefix for where you want Argus installed), and the
`--arch` (the architecture-specific SDK you wish to re-package as a deb). There
is also the `--full` flag. If included on the command-line it will install the
*full* Argus SDK, including the GUI, example programs, documentation, etc. For
ROS users, **this is not recommended**. The Argus SDK installs its own copies
of various Qt libraries that, depending upon where they are in your runtime
linker's path, could render some ROS tools (e.g., `rqt_graph`) unusable.

For the example that follows, we will install Argus into `/opt` (the default)
and build the 64-bit x86 Linux package (also the default).

```
[ ~/dev/argus-ros/utils ]
tpanzarella@tuna: $ ./debianize_argus.py --infile=~/dev/argus/libargus.zip
Working directory is: /tmp/debianize_argus-vJO1n5
unzip -q ~/dev/argus/libargus.zip -d /tmp/debianize_argus-vJO1n5/unzipped
All available SDK's include: ['libargus-3.5.0.28-LINUX-x86-64Bit.zip', 'libargus-3.5.0.28-LINUX-x86-32Bit.zip', 'libargus-3.5.0.28-APPLE-x86-64Bit.zip', 'libargus-3.5.0.28-LINUX-arm-32Bit.zip', 'libargus-3.5.0.28-ANDROID-arm-64Bit.zip', 'libargus-3.5.0.28-ANDROID-arm-32Bit.zip']
The valid LINUX SDK's include: ['libargus-3.5.0.28-LINUX-x86-64Bit.zip', 'libargus-3.5.0.28-LINUX-x86-32Bit.zip', 'libargus-3.5.0.28-LINUX-arm-32Bit.zip']
Found target SDK: libargus-3.5.0.28-LINUX-x86-64Bit.zip
Parsed out Argus version as: {'major': '3', 'rev': '28', 'minor': '5', 'patch': '0'}
Working deb dir is: /tmp/debianize_argus-vJO1n5/deb
unzip -q /tmp/debianize_argus-vJO1n5/unzipped/libargus-3.5.0.28-LINUX-x86-64Bit.zip -d /tmp/debianize_argus-vJO1n5/deb/opt
SDK extracted to: /tmp/debianize_argus-vJO1n5/deb/opt/libargus-3.5.0.28-LINUX-x86-64Bit
Creating control file...
Creating postinst shell script...
Creating prerm shell script...
Creating udev rules file...
dpkg -b /tmp/debianize_argus-vJO1n5/deb argus-sdk_3.5.0-28_amd64.deb
dpkg-deb: building package 'argus-sdk' in 'argus-sdk_3.5.0-28_amd64.deb'.
Removing tmp directory: /tmp/debianize_argus-vJO1n5

```

The output of `debianize_argus.py` is a bit verbose, however, that is for
debugging should any issues arise. Once the script is complete, you should have
the repackaged deb file in your current working directory:

```
[ ~/dev/argus-ros/utils ]
tpanzarella@tuna: $ ls -l *.deb
-rw-r--r-- 1 tpanzarella tpanzarella 18160002 Jul 21 09:24 argus-sdk_3.5.0-28_amd64.deb
```

You can now install it as you would any other debian package:

```
[ ~/dev/argus-ros/utils ]
tpanzarella@tuna: $ sudo dpkg -i argus-sdk_3.5.0-28_amd64.deb
[sudo] password for tpanzarella:
Selecting previously unselected package argus-sdk.
(Reading database ... 488236 files and directories currently installed.)
Preparing to unpack argus-sdk_3.5.0-28_amd64.deb ...
Unpacking argus-sdk (3.5.0-28) ...
Setting up argus-sdk (3.5.0-28) ...
Creating new symlink: /opt/argus -> /opt/libargus-3.5.0.28-LINUX-x86-64Bit
```

You'll note on the last line of output, a convenience symlink has been created
for you. If you did not install the "full" SDK, you are done. The necessary
libraries, headers, and udev rules are in place and you can begin to
install/use `argus-ros`.

If you did install the "full" SDK, to validate the installation, you should now
plug in your camera. For exemplary purposes, we will use a Pico Flexx. Once
your camera is plugged in, you can follow the steps below:

```
[ ~/dev/argus-ros/utils ]
tpanzarella@tuna: $ cd /opt/argus/bin/

[ /opt/argus/bin ]
tpanzarella@tuna: $ ./argusviewer.sh
```

You should see output similar to the following:

<p align="center">
  <img src="../doc/figures/argus-viewer.png"/>
</p>

Since we have integrated Argus with our package manager, uninstalling it can
be done as follows:

```
[ ~/dev/argus-ros/utils ]
tpanzarella@tuna: $ sudo dpkg --purge argus-sdk
(Reading database ... 489225 files and directories currently installed.)
Removing argus-sdk (3.5.0-28) ...
Removing: /opt/argus
```
