/*
 *ch36x_demo_INT.c for ch365/ch367/ch368 pci card of WCH
 * 
 **********************************************************************
 ***********	  Copyright (C) WCH 2013.10.28		***************
 ***********	      web: www.wch.cn			***************
 ***********	  AUTHOR: TECH33 (tech@wch.cn)		***************
 ***********	 Used for PCI/PCIe Chip (CH365/7/8)	***************
 ***********	Nanjing	QinHeng Electronics Co.,Ltd	***************
 **********************************************************************
 *
 * Running Environment: Linux
 * This file is used for testing Interrupt(callback is in driver (ch36x_interrupt_handler))
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <malloc.h>

#include "ch36x_lib.h"

#ifndef CH36x_DEMO_DEBUG
#define CH36x_DEMO_DEBUG
#endif
#undef CH36x_DEMO_DEBUG

extern int dev_fd;
int icount = 0;

void myIntHandler(void)
{
	//This just for test, do it by users
	icount++;
	printf("icount:%d\n", icount);
}

int main( int argc, char **argv )
{
	/*int dev_fd = 0;	*/
	BOOL retval = 0;

	dev_fd = CH36xOpenDevice( false, true );
	printf("dev_fd:%d\n", dev_fd);
	if( dev_fd == -1 )
	{
		printf("CH36xOpenDevice open error line:%d\n", __LINE__);
		return -1;
	}

//---Set interrupter Routine
	retval = CH36xSetIntRoutine( myIntHandler );
	if( retval == false )
	{
		printf("CH36xSetIntRoutine error line:%d\n", __LINE__);
		return -1;
	}

	mPCH36x_IO_REG oIoBaseAddr;
	oIoBaseAddr = (mPCH36x_IO_REG)malloc(sizeof(mCH36x_IO_REG));
	retval = CH36xGetIoBaseAddr( &oIoBaseAddr );
	if( retval == false )
	{
		printf("CH36xGetIoBaseAddr error line:%d\n", __LINE__);
		return -1;
	}
	printf("oIoBaseAddr:%p\n", oIoBaseAddr );
	
	unsigned int i = 0;
	for( i = 0; i < (1<<16); i++ ) {
	/*for( i = 0; i < 1; i++ ) {*/
		usleep(250); //Note: Recommend parameter greater than 500

//for CH365 interrupt handler test
		PVOID iOffset = (PVOID)0x40;
		PUCHAR oByte = (unsigned char *)malloc(sizeof(UCHAR));

		retval = CH36xReadConfig( iOffset, oByte );
		if( retval == false )
		{
			printf("CH36xReadConfig error line:%d\n", __LINE__);
			return -1;
		}

		retval = CH36xWriteConfig( iOffset, (*oByte) | 0x04 );
		if( retval == false )
		{
			printf("CH36xWriteConfig error line:%d\n", __LINE__);
			return -1;
		}

//for CH367 interrupt handler test
/*
 *        PVOID	iAddr = oIoBaseAddr;
 *        PVOID iOffset_ch367_ch368 = (PVOID)0xf8;
 *        PUCHAR oByte_ch367_ch368 = (unsigned char *)malloc(sizeof(UCHAR));
 *
 *        retval = CH36xReadIoByte( iAddr + (unsigned int)iOffset_ch367_ch368, oByte_ch367_ch368 );
 *        if( retval == false )
 *        {
 *            printf("CH36xReadIoByte error line:%d\n", __LINE__);
 *            return -1;
 *        }
 *
 *        retval = CH36xWriteIoByte( iAddr + (unsigned int)iOffset_ch367_ch368, (*oByte_ch367_ch368) | 0x04 );
 *        if( retval == false )
 *        {
 *            printf("CH36xWriteIoByte error line:%d\n", __LINE__);
 *            return -1;
 *        }
 */
	}

	sleep(2); //Wait child process handler, becouse of next function "CH36xSetIntRoutine"
	CH36xSetIntRoutine(NULL);

	CH36xCloseDevice();

	return 0;
}
