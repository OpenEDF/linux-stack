insmod log:
[  711.192450] of_dts: loading out-of-tree module taints kernel.
[  711.192502] of_dts: module license 'CPL' taints kernel.
[  711.192525] Disabling lock debugging due to kernel taint
[  711.193060] Linux device tree node test:
[  711.193178] Linux find the node name is: leds.
[  711.193191] Linux find the node full name is: leds.
[  711.193204] Linux find the node property is: gpio-leds.

rmmod log:
[  718.959956] Linux device tree node test exit

device node:
pi@raspberrypi:~/linux_tips/of_dts $ cat /proc/device-tree/leds/name
leds
pi@raspberrypi:~/linux_tips/of_dts $ cat /proc/device-tree/leds/
compatible  led-act/    name        phandle
pi@raspberrypi:~/linux_tips/of_dts $ cat /proc/device-tree/leds/
compatible  led-act/    name        phandle
pi@raspberrypi:~/linux_tips/of_dts $ cat /proc/device-tree/leds/compatible
gpio-leds
pi@raspberrypi:~/linux_tips/of_dts $

