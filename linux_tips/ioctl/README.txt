insmod log:
[ 1354.601389] [IOCTL TEST]: Linux proc file test:
[ 1354.601419] Major = 239 Minor = 0
[ 1354.601772] device driver insert...done!

write and read:
pi@raspberrypi:~/linux_tips/ioctl $ sudo ./test_app
Enter the number to send:
12345678
Writing the value to driver!
Reading the value from driver
Value is 12345678
close driver!

log:
[ 1384.138705] device file open
[ 1388.989562] device file ioctl opeation
[ 1388.989588] Value = 12345678
[ 1388.989630] device file ioctl opeation
[ 1388.989778] device file release

rmmod log:
[ 1419.008038] [IOCTL TEST]: Linux proc file test exit!
