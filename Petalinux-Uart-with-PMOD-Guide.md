# Setup

To use Petalinux, we will build a docker container. To build 2022.2 petalinux and tag it as `peta`, run this command in the root of this repo:
```
docker build -t peta --build-arg PETA=petalinux-v2022.2-10141622-installer.run .
```
Now, we will run the container. Petalinux doesn't like being ran as root, so switch to our user named `bucky`:
```
docker run --rm -itv $HOME:/mnt peta
gosu bucky bash
```
If you'd like to copy images back to your computer, Press `Ctrl+d` or type `exit` to change back to root. Then, copy to `/mnt`.

Be careful pressing `Ctrl+d` since you can accidentally wipe your container and lose all your progress. A simple way to avoid this is to open multiple bash shells when you login as root before switching to `bucky`:
```
bash
bash
bash
```
This gives you three chances to accidentally exit without losing your progress

*before we continue, if you are unsure how to add IP to a block design, or would like to use bare-metal, checkout [this guide](Baremetal Uart with PMOD Guide)*

# Petalinux Project
There are three ways to build this project:
1) [BSP](#BSP)
2) [Submodule](#Submodule)
3) [Manual](#Manual)

### BSP
Unpack the BSP like this:
```
petalinux-create -t project -s bsps/BSP0.bsp && cd newos
```
To test the image you can boot qemu:
```
petalinux-boot --qemu --prebuilt 3
```
login is `petalinux` and you will be prompted to change your password

Now, you can test out the character devices. This will echo back to you over the qemu serial console:
```
sudo su
echo 'Hello SSEC!' > /dev/ttyPS0
```
Uartlite will not function properly in QEMU, but you can see it at `/dev/ttyUL1`. To test it, let's format an SD card.

First, detach from docker with `Ctrl+P Ctrl+Q` and attach later with `docker attach --latest`. Run `lsblk` and find your SD card. If you're struggling to do this, run `lsblk` once before inserting your SD card, and once after to see which label it was assigned.

Once you have the label (ex. sdb), run: `sudo fdisk /dev/sdb`
```
Welcome to fdisk (util-linux 2.32.1).
Changes will remain in memory only, until you decide to write them.
Be careful before using the write command.


Command (m for help): o
Created a new DOS disklabel with disk identifier 0x62b71f77.

Command (m for help): n
Partition type
   p   primary (0 primary, 0 extended, 4 free)
   e   extended (container for logical partitions)
Select (default p):

Using default response p.
Partition number (1-4, default 1):
First sector (2048-15267839, default 2048):
Last sector, +sectors or +size{K,M,G,T,P} (2048-15267839, default 15267839): +33MiB

Created a new partition 1 of type 'Linux' and of size 33 MiB.
Partition #1 contains a vfat signature.

Do you want to remove the signature? [Y]es/[N]o: y

The signature will be removed by a write command.

Command (m for help): a
Selected partition 1
The bootable flag on partition 1 is enabled now.

Command (m for help): n
Partition type
   p   primary (1 primary, 0 extended, 3 free)
   e   extended (container for logical partitions)
Select (default p):

Using default response p.
Partition number (2-4, default 2):
First sector (69632-15267839, default 69632):
Last sector, +sectors or +size{K,M,G,T,P} (69632-15267839, default 15267839):

Created a new partition 2 of type 'Linux' and of size 7.3 GiB.

Command (m for help): w
The partition table has been altered.
Calling ioctl() to re-read partition table.
Syncing disks.
```
Now that partitions are made, we need to make the filesystem:
```
sudo mkfs.vfat -F32 /dev/sdb1
sudo mkfs.ext4 /dev/sdb2
```
Let's make mount points for the drive and mount the drive:
```
sudo mkdir /mnt/rootfs /mnt/BOOT
sudo mount /dev/sdb1 /mnt/BOOT
sudo mount /dev/sdb2 /mnt/rootfs
```
Return to your docker container. If you are still `bucky`, exit back to root and run this:
```
cd newos/pre-built/linux/images/
cp BOOT.BIN boot.scr image.ub rootfs.tar.gz /mnt
```
Now your files are in your home directory. You can detach your container or just exit out.

Let's copy the files to the SD card:
```
cd
sudo cp BOOT.BIN boot.scr image.ub rootfs.tar.gz /mnt/BOOT
sudo tar -xvf rootfs.tar.gz -C /mnt/rootfs
sync
sudo umount /dev/sdb1
sudo umount /dev/sdb2
rm -f BOOT.BIN boot.scr image.ub rootfs.tar.gz
```
Your SD card can now be removed from your computer, and inserted into the Zybo. Make sure your jumper configuration on the Zybo corresponds to SD card, not JTAG or QSPI.

Insert the RS232 PMOD into the top row of header JD.

> TODO find better way to identify which tty is which device

Make sure that you use the correct baudrates. The Zybo uses 115200 and the Uartlite uses 9600. Once you've opened a serial terminal to both the rs232 adapter and the Zybo, try echoing a message from the Zybo:
```
sudo su
echo "Not in qemu anymore :D" > /dev/ttyUL1
```
#### Congratulations!

### Submodule
TODO
### Manual
TODO