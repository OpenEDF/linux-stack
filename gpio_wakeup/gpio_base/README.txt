per-opeartion:
pi@raspberrypi:~/ $ echo 1 > /sys/class/gpio/gpio21/value
pi@raspberrypi:~/ $ echo 0 > /sys/class/gpio/gpio21/value

insmod log:
[50272.977990] gpio_base: loading out-of-tree module taints kernel.
[50272.978799] --->: etx_driver_init
[50272.978817] Major = 239 Minor = 0
[50272.979527] Device Driver Insert...Done!!!
[50272.979544] <---: etx_driver_init

rmmod log:
[55257.317802] --->: etx_driver_exit
[55257.318243] Device Driver Remove...Done!!
[55257.318255] <---: etx_driver_exit

