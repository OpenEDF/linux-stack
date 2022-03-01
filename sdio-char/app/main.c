/*
 * @file: app.c
 * @brief: test rpi-sdio driver 
 * @step: 
 *  1. compiler: make app
 *  2. run: test_sdio.sh
 * @command and argument:
 *
 *
 * */
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <stdlib.h>

/* @func: main
 * @brief: test rpi-sdio
 * @param: command and argument
 *
 * */
int main(int argc, char *argv[])
{
    char *sdio_dev = "/dev/rpi-sdio";
    int fd = 0;
    int ret = -1;

    /* open device */
    if ((fd = open(sdio_dev, O_RDWR)) < 0) {
        printf("open file: %s failed.\n", argv[0]);
        return -1;
    }

    /* close device */
    if ((ret = close(fd)) < 0) {
        printf("close device: %s failed.\n", argv[0]);    
        return ret;
    }

    /* exit */
    return 0; 
}

/****************************************** END ***********************************************/
