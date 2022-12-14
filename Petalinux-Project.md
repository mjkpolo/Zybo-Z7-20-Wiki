## Build Petalinux docker image

We have created a Docker image in which the Petalinux executable can be used.

To build the image, choose a petalinux installer and select it using a build argument (No significant difference has been observed between the two current versions in the repo)

```
docker build --build-arg PETA=petalinux-v2021.1-final-installer.run -t peta .
```

Next, enter the docker container and mount a location to copy the boot files out to

```
docker run --rm -itv $PWD:/media peta
```

The `submodules` folder is copied into the image. Each device will have its own submodule. Each submodule is linked to a specific hash or version in the device's repository. The [Zybo Z7-20](https://gitlab.ssec.wisc.edu/mkurzynski/petalinux-zybo-z7-20) device submodule was copied from [here](https://github.com/Digilent/Zybo-Z7/tree/master). This base project will be used to build our custom Petalinux image

cd into your device's os folder and turn into bucky

```
cd submodules/petalinux-zybo-z7-20/os/ && gosu bucky bash

```

You are now in the Petalinux project folder

## Add application recipe

First, you need to run `petalinux-config` to "install yocto SDK"

You can immediately exit the menu. However, if you are having issues later on downloading files, return to this menu and go to `Yocto Settings -> Enable Network sstate feeds` and press the spacebar to deselect it. The Asterix should be gone now.

To add data to the Petalinux image, we will create an application

```
petalinux-create -t apps --template install -n uart  --enable
```

run `vi project-spec/meta-user/recipes-apps/uart/uart.bb` to open the Bitbake file and make it look like this:

```
#
# This file is the uart recipe.
#

SUMMARY = "Simple uart application"
SECTION = "PETALINUX/apps"
LICENSE = "MIT"
LIC_FILES_CHKSUM = "file://${COMMON_LICENSE_DIR}/MIT;md5=0835ade698e0bcf8506ecda2f7b4f302"

SRC_URI = "file://uart \
           file://bs.bin \
        "

S = "${WORKDIR}"

do_install() {
             install -d ${D}/${bindir}
             install -m 0755 ${S}/uart ${D}/${bindir}
             install -m 0655 ${S}/bs.bin ${D}/${bindir}
}
```

Next, copy `bs.bin` into the files folder:

```
cp /media/bs.bin project-spec/meta-user/recipes-apps/uart/files/
```

run `vi project-spec/meta-user/recipes-apps/uart/files/uart` to make the startup script and make it look like this:

```sh
#!/bin/sh

echo 0 > /sys/class/fpga_manager/fpga0/flags
mkdir -p /lib/firmware
cp /usr/bin/bs.bin /lib/firmware/
echo bs.bin > /sys/class/fpga_manager/fpga0/firmware
```

now we can build the boot files

```
petalinux-build
petalinux-package --boot --u-boot
```

copy `BOOT.BIN`, `image.ub`, and `boot.scr` to your mounted volume. First, you need to type `exit` or `ctrl+d` to go back to root user.
```
cd image/linx && cp BOOT.BIN image.ub boot.scr /media
```