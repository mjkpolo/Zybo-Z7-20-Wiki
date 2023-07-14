## Intro

[Yocto](https://www.yoctoproject.org) is a build system used by Petalinux. It is possible to use Yocto with Xilinx layers to build images for any FPGA without "Petalinux" by using its underlying tools.

The [poky](https://gitlab.ssec.wisc.edu/nextgenshis/yocto/poky) repo has been setup with a default base project on the `mickledore` branch for zynq development. It includes a generated config for this zybo, and the machine configuration generator that was used to generate it, allowing you to automatically add new nodes to the device tree when your `.xsa` file changes

## Docker

Clone the development repo to get a docker image for yocto

```
git clone git@gitlab.ssec.wisc.edu:nextgenshis/yocto/yocto-dev.git
```
A `go.sh` script is provided which builds the container if you pass a `.xsa` file as an argument which you have copied to that directory. Docker doesn't like being passed files outside of its working directory.

```
./go.sh design_1_wrapper.xsa
```
The script also creates a volume and links it to `yocto-vol` so you can copy your files to the SD card once you're finished, and files persist even after you close your container.

## Yocto

Once you've attached to the container, clone our fork of `poky`
```
git clone https://gitlab.ssec.wisc.edu/nextgenshis/yocto/poky --depth 1 --recurse
```
To source the bitbake environment do this
```
cd poky
. oe-init-build-env
```
this will put you in the build directory. To start a build run this
```
bitbake core-image-minimal
```
It is possible to build many different targets, including more feature-full ones by adding recipes to your build and new layers. For example, if you are missing a tool needed, it may be in [`meta-petalinux`](https://github.com/Xilinx/meta-petalinux/tree/rel-v2023.1) or other related layers. A manifest of all the layers and their versions used by Petalinux can be found [here](https://github.com/Xilinx/yocto-manifests/tree/rel-v2023.1) along with [more general yocto instructions](https://xilinx-wiki.atlassian.net/wiki/spaces/A/pages/18841862/Install+and+Build+with+Xilinx+Yocto) to use these manifests.