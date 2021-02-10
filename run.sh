#! /bin/bash
echo "*******************************************************"

echo "-Building project"
  make clean
  make

echo "-Placing the firmware"
  cp *.out /lib/firmware/am57xx-pru1_1-fw

echo "-Reboting pru core 1_1"
  echo 'stop' > /sys/class/remoteproc/remoteproc5/state
  echo 'start' > /sys/class/remoteproc/remoteproc5/state
