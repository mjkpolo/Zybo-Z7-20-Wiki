### Get Prebuilt Files
---
1) Build docker container *(tagged peta)*

```console
docker build -t peta --build-arg BSP=Petalinux-Zybo-Z7-20-2017.4-3.bsp .
```

2) Run container *(bash not ENTRYPOINT)*
```console
docker run --rm -itv $PWD:/home/bucky/tmp peta bash
```

3) Create peta project
```console
pushd /home/bucky/PetaLinux
source settings.sh
popd
petalinux-create -t project -s Petalinux-Zybo-Z7-20-2017.4-3.bsp
```

4) Copy prebuilt images
```console
cp Zybo-Z7-20/pre-built/linux/images/BOOT.BIN Zybo-Z7-20/pre-built/linux/images/image.ub /home/bucky/tmp
```

---
### Setup SD Card
---
1) Format the SD card using fdisk as such *(done on Linux machine)*:

```console
$ sudo fdisk /dev/mmcblk0

Welcome to fdisk (util-linux 2.32.1).
Changes will remain in memory only, until you decide to write them.
Be careful before using the write command.


Command (m for help): o
Created a new DOS disklabel with disk identifier 0x61255c5d.

Command (m for help): n
Partition type
   p   primary (0 primary, 0 extended, 4 free)
   e   extended (container for logical partitions)
Select (default p): p
Partition number (1-4, default 1): 
First sector (2048-249737215, default 2048): 
Last sector, +sectors or +size{K,M,G,T,P} (2048-249737215, default 249737215): 

Created a new partition 1 of type 'Linux' and of size 119.1 GiB.
Partition #1 contains a vfat signature.

Do you want to remove the signature? [Y]es/[N]o: y

The signature will be removed by a write command.

Command (m for help): w
The partition table has been altered.
Calling ioctl() to re-read partition table.
Syncing disks.
```

2) format to FAT32

```console
sudo mkfs.vfat -F32 /dev/mmcblk0

```

3) Mount SD card and copy files

4) open UART connection *(done on a Mac)* and press reset button on FPGA
```console
screen /dev/cu.usbserial-210351B481FA1 115200
```
---
