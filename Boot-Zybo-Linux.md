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
1) Format the SD card using gdisk as such:

```console

```

2) Mount SD card and copy files

3) Viola! You did it.
---
