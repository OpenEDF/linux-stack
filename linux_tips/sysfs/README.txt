read and write value to sysfs:
sudo cat /sys/kernel/etx_sysfs/etx_value
[172016.449707] sysfs show: read!
sudo -i 
echo 80 > /sys/kernel/etx_sysfs/etx_value
[172555.862475] sysfs show: write|
[172562.806825] sysfs show: read!

insmod log:
[171969.773626] [EXPORT TEST]: Linux sysfs file test:
[171969.773665] module init done!

rmmod log:
[172627.020450] [EXPORT TEST]: Linux sysfs file test exit!
