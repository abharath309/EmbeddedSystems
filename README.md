# EmbeddedSystems

In this embedded systems project, we are able to control a car on the presence of a STOP sign
or a YIELD sign. The goal of this project is to detect two different signs: stop sign and yield sign
in an image. A usb camera is connected to the gumstix board in order to get the video stream
in. The properties of color, shape and size of the sign is used to separate them from other part
of the image. We do all of our image processing on the gumstix, and then using device drivers,
communicate output of our results to a kernel module which then drives the GPIO pins that
moves the car in the appropriate direction. Our method here, illustrates a novel way to use a
low-power, space efficient device like gumstix, do image processing without any reliance of
open-source software and an application that is capable of performing in near-real time.

1)Go to gumstix

2)Do the following to insert the camera modules

insmod v4l2-common.ko
insmod v4l1-compat.ko
insmod compat_ioctl32.ko
insmod videodev.ko
insmod uvcvideo.ko

3)Go inside km folder and run the below

make clean
make

4)Trasnfer the motor.ko file to gumstix
5)run the command mknod /dev/motor c 61 0 to initialise the device file
6)Go inside ul folder and run make
7)Transfer the capture_yuyv file to gumstix inside the same folder
8)insmod motor.ko to insert the kernel module
9)run ./capture_yuyv
