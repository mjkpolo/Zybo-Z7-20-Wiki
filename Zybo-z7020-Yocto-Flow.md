## Intro

[Yocto](https://www.yoctoproject.org) is a build system used by Petalinux. It is possible to use Yocto with Xilinx layers to build images for any FPGA without "Petalinux" by using its underlying tools.

The [poky](https://gitlab.ssec.wisc.edu/nextgenshis/yocto/poky) repo has been setup with a default base project on the `mickledore` branch for zynq development. It includes a generated config for this zybo, and the machine configuration generator that was used to generate it, allowing you to automatically add new nodes to the device tree when your `.xsa` file changes

## Objectives

- [Setup a Yocto Build environment with a `.xsa` file](#docker)
- [Generate an SD card image](#basic-build)
- [Add a module](#add-module)
- [Change the device tree](#customize-device-tree)
- [Upload to gitlab](#saving-back-to-gitlab)

## Prerequisites

- A `.xsa` file you've exported from Vivado in the [`yocto-dev`](https://gitlab.ssec.wisc.edu/nextgenshis/yocto/yocto-dev) folder
- A clone of [`poky`](https://gitlab.ssec.wisc.edu/nextgenshis/yocto/poky) on your host machine for pushing changes to gitlab

## Docker

Clone the development repo to get a docker image for yocto

```shell
git clone git@gitlab.ssec.wisc.edu:nextgenshis/yocto/yocto-dev.git
```
A `go.sh` script is provided which builds the container if you pass a `.xsa` file as an argument which you have copied to that directory. Docker doesn't like being passed files outside of its working directory.

```shell
./go.sh design_1_wrapper.xsa
```
The script also creates a volume and links it to `yocto-vol` so you can copy your files to the SD card once you're finished, and files persist even after you close your container.

## Basic Build

Once you've attached to the container, clone our fork of `poky`
```shell
git clone https://gitlab.ssec.wisc.edu/nextgenshis/yocto/poky --depth 1 --recurse
```
To source the bitbake environment do this
```shell
cd poky
. oe-init-build-env
```
this will put you in the build directory. To start a build run this
```shell
bitbake core-image-minimal
```
It is possible to build many different targets, including more feature-full ones by adding recipes to your build and new layers. For example, if you are missing a tool needed, it may be in [`meta-petalinux`](https://github.com/Xilinx/meta-petalinux/tree/rel-v2023.1) or other related layers. A manifest of all the layers and their versions used by Petalinux can be found [here](https://github.com/Xilinx/yocto-manifests/tree/rel-v2023.1) along with [more general yocto instructions](https://xilinx-wiki.atlassian.net/wiki/spaces/A/pages/18841862/Install+and+Build+with+Xilinx+Yocto) to use these manifests.

Finally, format an SD card to have a 128MB+ fat32 partition 1, and ext4 partition 2. Using `fdisk`, do `o`,`n`,`p` to create a new disk image and start a partition. After selecting the sizes (default or `+128MB` for the second boot range) you can mark the first partition bootable with `a` to be safe.
```shell
fdisk /dev/sdX
mkfs.vfat -F32 /dev/sdX1
mkfs.ext4 /dev/sdX2
```
Mount the partitions and go to `poky/build/tmp/deploy/images/zynq-generic-7z020` to copy `uImage`, `boot.bin`, and `boot.scr` to `/dev/sdX1`
```shell
[ -e /mnt/BOOT ] || mkdir /mnt/BOOT
[ -e /mnt/rootfs ] || mkdir /mnt/rootfs
mount /dev/sdX1 /mnt/BOOT
mount /dev/sdX2 /mnt/rootfs
cp uImage boot.bin boot.scr /mnt/BOOT
```
The `boot.scr` script expects your device tree blob to be named `system.dtb` so copy `zynq-generic-7z020-system.dtb` to `/mnt/BOOT/system.dtb`

```shell
cp zynq-generic-7z020-system.dtb /mnt/BOOT/system.dtb
```

finally extract the `core-image-minimal-zynq-generic-7z020.tar.gz` to `/mnt/rootfs` and sync to the SD card

```shell
tar xf core-image-minimal-zynq-generic-7z020.tar.gz -C /mnt/rootfs
sync
minicom -D /dev/ttyUSBX -b 115200
```
You can login with `root` and no password is needed.

## Add Module

> [Xilinx Confluence Wiki Guide](https://xilinx-wiki.atlassian.net/wiki/spaces/A/pages/57836605/Creating+a+Custom+Yocto+Layer)

Our goal is to write a module and have it print `Hello Seaman!` to the screen. To do this, we will create a new layer. The name has to start with `meta-`...
```shell
LAYER=/yocto-vol/poky/meta-seaman
bitbake-layers create-layer $LAYER
mkdir -p $LAYER/{recipes-kernel/linux-xlnx/files,recipes-bsp/device-tree}
bitbake-layers add-layer $LAYER
```
We will add the module in `recipes-kernel` and pull a custom device tree in `recipes-bsp`

earlier when we created folders, we made one inside `recipes-kernel` called `linux-xlnx`. That's the name of the kernel, but the folder name can be anything, as long as you add the following in `/yocto-vol/poky/build/conf/local.conf` and name the files accordingly:
```
CORE_IMAGE_EXTRA_INSTALL += " hello-seaman"
```
in `$LAYER/recipes-kernel/linux-xlnx` add our recipe
```shell
wget https://gitlab.ssec.wisc.edu/nextgenshis/yocto/poky/-/raw/hello-seaman/meta-seaman/recipes-kernel/linux-xlnx/hello-seaman.bb
```
`inherit module` will auto-magically compile everything for us.

Next add the sources to `linux-xlnx/files/`. 
```shell
wget https://gitlab.ssec.wisc.edu/nextgenshis/yocto/poky/-/raw/hello-seaman/meta-seaman/recipes-kernel/linux-xlnx/files/hello-seaman.c
wget https://gitlab.ssec.wisc.edu/nextgenshis/yocto/poky/-/raw/hello-seaman/meta-seaman/recipes-kernel/linux-xlnx/files/Makefile
wget https://gitlab.ssec.wisc.edu/nextgenshis/yocto/poky/-/raw/hello-seaman/meta-seaman/recipes-kernel/linux-xlnx/files/COPYING
```

Rebuild with `bitbake core-image-minimal` and copy your files to the SD card again.

Here is an example of loading and unloading your module

```console
root@zynq-generic-7z020:~# modprobe hello-seaman
hello_seaman: loading out-of-tree module taints kernel.
Hello Seaman!
root@zynq-generic-7z020:~# modprobe -r hello-seaman
Ahoy!
```


## Customize Device tree

We will be customizing a PID IP. The node will appear in `pl.dtsi`. This is what the PID node looks like before customization:

```
...
                pid_0: pid@43c00000 {
                        clock-names = "s00_axi_aclk";
                        clocks = <&clkc 15>;
                        compatible = "xlnx,pid-1.0";
                        reg = <0x43c00000 0x10000>;
                        xlnx,s00-axi-addr-width = <0x4>;
                        xlnx,s00-axi-data-width = <0x20>;
                };
...
```

add [this device tree bbappend file](https://gitlab.ssec.wisc.edu/nextgenshis/yocto/poky/-/blob/hello-seaman/meta-seaman/recipes-bsp/device-tree/device-tree%25.bbappend) to `$LAYER/recipes-bsp/device-tree`

It overrides parameters that [have been made weakly assigned](https://gitlab.ssec.wisc.edu/nextgenshis/yocto/meta-xilinx-tools/-/commit/58516345be292a4927d3c37c1ba9defbfff3602b) to use our forked device tree repo which has my [PID data](https://gitlab.ssec.wisc.edu/nextgenshis/yocto/device-tree/-/tree/6ed5fea3be41365d099e54058c711c2bbeba099c/pid/data)

Just run `bitbake core-image-minimal` again and this is the new contents of `pl.dtsi`
```
...
                pid_0: pid@43c00000 {
                        clock-names = "s00_axi_aclk";
                        clocks = <&clkc 15>;
                        compatible = "xlnx,pid-1.0";
                        reg = <0x43c00000 0x10000>;
                        xlnx,Q = <0x6>;
                        xlnx,s00-axi-addr-width = <0x4>;
                        xlnx,s00-axi-data-width = <0x20>;
                };
...
```
The difference is we've added a `xlnx,Q` from our board design. This tells us how many decimals places our PID module is set to. Actually, we've also added the last two parameters since they disappeared for some reason.

## Generate Machine Config

It will be very difficult to get another FPGA running without generating a machine config for it using your `.xsa` file.

```shell
/yocto-vol/poky/meta-xilinx/meta-xilinx-core/gen-machine-conf/gen-machineconf --soc-family zynq --hw-description /tmp/design_1_wrapper.xsa
```
You can update the default machine in `/yocto-vol/poky/build/conf/local.conf`
```
MACHINE ?= "zynq-generic-7z020"
```
`??=` means weak-weak, `?=` means weak, and `=` means normal assignment. Most of the time `?=` or `=` is fine


# Saving back to Gitlab

The most important part is making sure your work gets saved. git ignores all `meta-*` so we need to add `meta-seaman` to our `/yocto-vol/poky/.gitignore`
```
!meta-seaman
```
This is what you should see when you do `git status`
```shell
$ git status
On branch mickledore
Your branch is up to date with 'origin/mickledore'.

Changes not staged for commit:
  (use "git add <file>..." to update what will be committed)
  (use "git restore <file>..." to discard changes in working directory)
        modified:   .gitignore
        modified:   build/conf/bblayers.conf
        modified:   build/conf/local.conf

Untracked files:
  (use "git add <file>..." to include in what will be committed)
        meta-seaman/

no changes added to commit (use "git add" and/or "git commit -a")
```
stage your changes and make a patch
```shell
git add .
git commit -m 'seaman'
git format-patch HEAD~1
git reset HEAD~1
```
this should make something like `0001-seaman.patch`
detach from your container and go into your host's 'poky' repo to apply the patch
```shell
git apply ../yocto-vol/0001-seaman.patch
```
and the changes should appear.

now push to a new branch. Don't push to `mickledore` and even if you try it shouldn't let you.

```shell
git add .
git commit -m 'hello seaman, I just applied my first patch'
git checkout -b hello-seaman-<your name>
git push --set-upstream origin hello-seaman-<your name>
```

***Fine***