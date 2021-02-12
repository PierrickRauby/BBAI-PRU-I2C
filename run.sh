#! /bin/bash


echo "*******************************************************"
echo "This must be compiled on the BEAGLEBONE BLACK itself"
echo "It was tested on 4.9.12-ti-r57 kernel version"
echo "The source code for Hello_PRU ie PRU_gpioToggle was taken from"
echo "pru-software-support-package and can be cloned from"
echo "git clone git://git.ti.com/pru-software-support-package/pru-software-support-package.git"

echo "-Building project"
  make clean
  make

echo "-Placing the firmware"
  cp *.out /lib/firmware/am57xx-pru1_1-fw

echo "-Reboting pru core 1_1"
  echo 'stop' > /sys/class/remoteproc/remoteproc5/state
  echo 'start' > /sys/class/remoteproc/remoteproc5/state
