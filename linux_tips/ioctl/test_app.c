/*
 * file: test_app.c
 * brief: test linux ioctl driver
 * Author: macro
 * date: 2023/03/03
 *
 * *************************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <stdint.h>

#define WR_VALUE    _IOW('a', 'a', uint32_t*)
#define RD_VALUE    _IOR('a', 'b', uint32_t*)

#define FILE_PATH  "/dev/etx_device"

/* main */
int main(void)
{
    int fd;
    uint32_t number, value;
    fd = open(FILE_PATH, O_RDWR);
    if(fd < 0 ) {
        printf("cannot open the device...\n");
        return 0;
    }

    printf("Enter the number to send: \n");
    scanf("%d", &number);
    printf("Writing the value to driver!\n");
    ioctl(fd, WR_VALUE, (uint32_t *)&number);

    printf("Reading the value from driver\n");
    ioctl(fd, RD_VALUE, (uint32_t*)&value);
    printf("Value is %d\n", value);

    printf("close driver!\n");
    close(fd);

    return 0;
}

 /* *************************************************************************************/
