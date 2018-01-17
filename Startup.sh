#!/bin/bash
#This shell script loads both PRUs' firmware and 
#starts the robot control software.

CurrentDirectory=$PWD

#Shut off the PRUs
echo "4a334000.pru0" > /sys/bus/platform/drivers/pru-rproc/unbind
echo "4a338000.pru1" > /sys/bus/platform/drivers/pru-rproc/unbind
sleep 1.0

#possibly add device tree overlay code 
#config-pin overlay am335x-boneblue.dtb

#Copy each PRU's software to its respective firmware file
cp $CurrentDirectory/PRU0/Release/PRU0.out /lib/firmware/am335x-pru0-fw
cp $CurrentDirectory/PRU1/Release/PRU1.out /lib/firmware/am335x-pru1-fw

###################
#This starts the PRUs using modprobe, but the following way seems to
#be a better way of starting the PRUs. This will probably be removed
#later.
#rmmod -f rpmsg_pru
#rmmod -f virtio_rpmsg_bus
#rmmod -f pru_rproc
#modprobe pru_rproc
###################

#This code restarts each PRU by binding each PRU
echo "4a334000.pru0" > /sys/bus/platform/drivers/pru-rproc/bind
echo "4a338000.pru1" > /sys/bus/platform/drivers/pru-rproc/bind

#Sleep for a second to let the PRUs boot up
sleep 1.0

#Start the application
cd ../build
echo $PWD
./MainApplication
