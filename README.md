# Tele Info Monitor

Application used to monitor the user data transmitted by Linky system used by French national energy
provider.

# Hardware Configuration

## Material

* Raspberry Pi Zero
  W ([Buy one](https://www.kubii.fr/les-cartes-raspberry-pi/1851-raspberry-pi-zero-w-kubii-3272496006997.html))
* PiTInfo shield for Linky user
  interface ([Buy one](https://www.tindie.com/products/Hallard/pitinfo/))

## Raspberry Pi Zero

### Version

Board version:

```shell
cat /proc/device-tree/model
Raspberry Pi Zero W Rev 1.1
```

OS version:

```shell
cat /etc/os-release
PRETTY_NAME="Raspbian GNU/Linux 10 (buster)"
```

### Serial Link Configuration

* Disable serial console:
    - Edit /boot/cmdline.txt
    - Remove line: `console=serial0,(...)`
* Disable bluetooth module:
    - Edit /boot/config.txt
    - Add line: `dtoverlay=pi3-miniuart-bt`
* Reboot raspberry.

### How to test serial link

```shell
stty -F /dev/ttyAMA0 1200 sane evenp parenb cs7 -crtscts
cat /dev/ttyAMA0
```

This should output the data collected from Linky. If nothing returned, then serial link is probably
not well configured.

## Software Configuration

### Tools to install on RaspberryPi

- Install WiringPi: `sudo apt-get install wiringpi`

### Setup cross-compilation toolchain

* Setup Raspberry toolchain on host under /opt (not location independent)

```shell
wget https://github.com/Pro/raspi-toolchain/releases/latest/download/raspi-toolchain.tar.gz
sudo tar xfz raspi-toolchain.tar.gz --strip-components=1 -C /opt
```

* Get the includes and libraries from target RaspberryPi

```shell
rsync -vR --progress -rl --delete-after --safe-links pi@192.168.1.37:/{lib,usr,opt/vc/lib} ~/workspace-raspberry/rootfs
```

* Configure environment variables in IDE

```shell
export RASPBIAN_ROOTFS=$HOME/raspberry/rootfs
export PATH=/opt/cross-pi-gcc/bin:$PATH
export RASPBERRY_VERSION=1
```

* Configure CMake RaspberryPi toolchain
  using: `-DCMAKE_TOOLCHAIN_FILE=$HOME/raspberry/Toolchain-rpi.cmake`

## Resources

### Libraries and tools

- [WiringPi](http://wiringpi.com/)
- [Toolchain-rpi.cmake](https://github.com/Pro/raspi-toolchain/blob/master/Toolchain-rpi.cmake)

### Documentation

- [ERDF TeleInfo data](https://www.magdiblog.fr/wp-content/uploads/2014/09/ERDF-NOI-CPT_02E.pdf)
    - Follow: `Tableau 2: Codage des données du compteur «Bleu» électronique monophasé multitarif
      (CBEMM)`

### Tutorials

- [How to install the Raspberry Pi cross compiler on my Linux host machine?](https://stackoverflow.com/questions/19162072/how-to-install-the-raspberry-pi-cross-compiler-on-my-linux-host-machine/58559140#58559140)
- [The Useful RaspberryPi Cross Compile Guide](https://medium.com/@au42/the-useful-raspberrypi-cross-compile-guide-ea56054de187)
- [How to monitor Linky data - magdiblog.fr](https://www.magdiblog.fr/gpio/teleinfo-edf-suivi-conso-de-votre-compteur-electrique/)
- [How to monitor Linky data - jonathandupre.fr](https://www.jonathandupre.fr/articles/24-logiciel-scripts/208-suivi-consommation-electrique-compteur-edf-linky-avec-raspberry-pi-zero-w/)
- [How to use serial link on Raspberry Pi Zero](Utiliser le port série du Raspberry Pi 3 et du Pi Zero)
