/*
 *ch36x_lib.c for ch365/ch367/ch368 pci card of WCH
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
 * This file is used for Linux or Android dynamic .so
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/ioctl.h> //for 'ioctl' #include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <signal.h> //for 'SIGKILL'

#include "ch36x_lib.h"

#ifndef CH36x_LIB_DEBUG
/*#define CH36x_LIB_DEBUG*/
#endif

#define CH36x_LIB_VERSION				"WCH CH36x lib version-1.2"

//CMD Codes
#define CH36x_GET_IO_BASE_ADDR			0x00000001
#define CH36x_SET_IO_BASE_ADDR			0x00000002
#define CH36x_GET_MEM_BASE_ADDR			0x00000003
#define CH36x_SET_MEM_BASE_ADDR			0x00000004
#define CH36x_READ_CONFIG_BYTE			0x00000005
#define CH36x_WRITE_CONFIG_BYTE			0x00000006
#define CH36x_GET_INT_LINE				0x00000007
#define CH36x_SET_INT_LINE				0x00000008
#define CH36x_READ_IO_BYTE				0x00000009
#define CH36x_READ_IO_WORD				0x0000000a
#define CH36x_READ_IO_DWORD				0x0000000b
#define CH36x_WRITE_IO_BYTE				0x0000000c
#define CH36x_WRITE_IO_WORD				0x0000000d
#define CH36x_WRITE_IO_DWORD			0x0000000e
#define CH36x_READ_MEM_BYTE				0x0000000f
#define CH36x_READ_MEM_WORD				0x00000010
#define CH36x_READ_MEM_DWORD			0x00000011
#define CH36x_WRITE_MEM_BYTE			0x00000012
#define CH36x_WRITE_MEM_WORD			0x00000013
#define CH36x_WRITE_MEM_DWORD			0x00000014
#define CH36x_READ_MEM_BLOCK			0x00000015
#define CH36x_WRITE_MEM_BLOCK			0x00000016
#define CH36x_READ_IO_BLOCK				0x00000017
#define CH36x_WRITE_IO_BLOCK			0x00000018
#define CH36x_READ_CONFIG_WORD			0x00000019
#define CH36x_WRITE_CONFIG_WORD			0x0000001a
#define CH36x_READ_CONFIG_DWORD			0x0000001b
#define CH36x_WRITE_CONFIG_DWORD		0x0000001c
#define CH36x_ENABLE_INT				0x0000001d
#define CH36x_DISABLE_INT				0x0000001e
#define CH36x_GET_DRV_VERSION			0x000000ff

int dev_fd;		// Device handler
pid_t child_pid = 0;

/*
 * ********************************************************************
 * FUNCTION : Open PCI Device
 * arg:
 * iEnableMemory  	: Select device
 * iEnableInterrupt     : Enable Interrupt
 * ********************************************************************
 */
int CH36xOpenDevice( BOOL iEnableMemory, BOOL iEnableInterrupt )
{
	int retval = 0;
	char dev_path[20] = {'\0'};
	memset( dev_path, 0, sizeof(dev_path) );
	//if user use not any one,then should use "'0'+DEV_INDEX_NUM" );
	sprintf( dev_path, "%s%c", "/dev/ch36xpci", '0');
#ifdef CH36x_LIB_DEBUG
	printf( "dev_path:%s\n", dev_path );
#endif

	dev_fd = open( dev_path, O_RDWR);
	if( dev_fd == -1 )
	{
		printf("open /dev/ch36xpci error line:%d\n", __LINE__);		
		return -1;
	}

	//just for CH367/CH368, not support CH365
	if( iEnableInterrupt == true )
	{
		retval = ioctl( dev_fd, CH36x_ENABLE_INT, NULL );		
		if( retval == -1 )
		{
			printf("ioctl function error line:%d\n", __LINE__);
			return false;
		}
	}
	/*
	 *else if( iEnableInterrupt == false )
	 *{
	 *    retval = ioctl( dev_fd, CH36x_DISABLE_INT, NULL );
	 *    if( retval == -1 )
	 *    {
	 *        printf("ioctl function error line:%d\n", __LINE__);
	 *        return false;
	 *    }
	 *}
	 */

	return dev_fd;
}

/*
 * ********************************************************************
 * FUNCTION : Close PCI Device
 * ********************************************************************
 */
void CH36xCloseDevice()
{
	close(dev_fd);
}

/*
 * ********************************************************************
 * FUNCTION : Get PCI Driver Version
 * arg:
 * drv_version  	: Output buffer to store driver returned data
 * ********************************************************************
 */
BOOL CH36xGetDrvVer( char *drv_version )
{
	int retval = 0;

	retval = ioctl( dev_fd, CH36x_GET_DRV_VERSION, (unsigned long)drv_version );
	if( retval == -1 )
	{
		printf("ioctl function error line:%d\n", __LINE__ );
		return false;
	}

	return true;
}

BOOL CH36xGetLibVer( char *lib_version )
{
	char *lib_version_tmp = CH36x_LIB_VERSION;

	strcpy( lib_version, lib_version_tmp );	

	return true;
}

/*
 * ********************************************************************
 * CH36xGetIoBaseAddr( mPCH36x_IO_REG *oIoBaseAddr )
 *
 * FUNCTION : Get I/O Base Addr
 * arg:
 * oIoBaseAddr  	: Get Addr
 * ********************************************************************
 */
BOOL CH36xGetIoBaseAddr( mPCH36x_IO_REG *oIoBaseAddr )
{
	int retval = 0;
	retval = ioctl( dev_fd, CH36x_GET_IO_BASE_ADDR, (unsigned long)oIoBaseAddr );
	if( retval == -1 )
	{
		printf("ioctl function error line:%d\n", __LINE__);
		return false;
	}

	return true;
}


BOOL CH36xSetIoBaseAddr( mPCH36x_IO_REG oIoBaseAddr )
{
	return true;
}

/*
 * ********************************************************************
 * CH36xGetMemBaseAddr( mPCH36x_MEM_REG *oMemBaseAddr )
 *
 * FUNCTION : Get Mem Base Addr
 * arg:
 * oMemBaseAddr  : Get Addr
 * ********************************************************************
 */
BOOL CH36xGetMemBaseAddr( mPCH36x_MEM_REG *oMemBaseAddr )
{
	int retval = 0;
	retval = ioctl( dev_fd, CH36x_GET_MEM_BASE_ADDR, (unsigned long)oMemBaseAddr );
	if( retval == -1 )
	{
		printf("ioctl function error line:%d\n", __LINE__);
		return false;
	}

	return true;
}


BOOL CH36xSetMemBaseAddr( mPCH36x_MEM_REG iMemBaseAddr )
{
	return true;
}

/*
 * ********************************************************************
 * CH36xGetIntLine( PULONG oIntLine )
 *
 * FUNCTION : Get Mem Base Addr
 * arg:
 * oIntLine  : Get Interrupt 
 * ********************************************************************
 */
BOOL CH36xGetIntLine( PULONG oIntLine )
{
	int retval = 0;
	retval = ioctl( dev_fd, CH36x_GET_INT_LINE, (unsigned long)oIntLine );
	if( retval == -1 )
	{
		printf("ioctl function error line:%d\n", __LINE__);
		return false;
	}

	return true;
}

/*
 * ********************************************************************
 * CH36xSetIntLine( ULONG iIntLine )
 *
 * FUNCTION : Get Mem Base Addr
 * arg:
 * iIntLine  : Set Interrupt 
 * ********************************************************************
 */
BOOL CH36xSetIntLine( ULONG iIntLine )
{
	int retval = 0;
	retval = ioctl( dev_fd, CH36x_SET_INT_LINE, (unsigned long)&iIntLine );
	if( retval == -1 )
	{
		printf("ioctl function error line:%d\n", __LINE__);
		return false;
	}

	return true;
}

/*
 * ********************************************************************
 * CH36xReadIoByte( PVOID iAddr, PUCHAR oByte )
 *
 * FUNCTION : Read I/O Space through Byte Read
 * arg:
 * iAddr  : Read addr
 * oByte  : Output buffer 
 * ********************************************************************
 */
BOOL CH36xReadIoByte( PVOID iAddr, PUCHAR oByte )
{
	int retval = 0;

	struct
	{
		PVOID iAddr;
		PUCHAR oByte;
	}CH36xReadIo_t;
	CH36xReadIo_t.iAddr = iAddr;
	CH36xReadIo_t.oByte = oByte;

//#ifdef CH36x_LIB_DEBUG
#if 0
	printf("CH36xReadIo_t ptr:%p\n", &CH36xReadIo_t);
	printf("iAddr:%p\n", iAddr );
	printf("CH36xReadIo_t.iAddr:%p\n", CH36xReadIo_t.iAddr);
	printf("CH36xReadIo_t.iAddr ptr:%p\n", &CH36xReadIo_t.iAddr);
	printf("oByte:%p\n", oByte);
	printf("CH36xReadIo_t.oByte:%p\n", CH36xReadIo_t.oByte);
	printf("CH36xReadIo_t.oByte ptr:%p\n", &CH36xReadIo_t.oByte);
#endif

	retval = ioctl( dev_fd, CH36x_READ_IO_BYTE, (unsigned long)&CH36xReadIo_t );
	if( retval == -1 )
	{
		printf("ioctl function error line:%d\n", __LINE__);
		return false;
	}

	return true;
}

/*
 * ********************************************************************
 * CH36xReadIoWord( PVOID iAddr, PUSHORT oByte )
 *
 * FUNCTION : Read I/O Space Through Word Read
 * arg:
 * iAddr  : Read addr
 * oByte  : Output buffer 
 * ********************************************************************
 */
BOOL CH36xReadIoWord( PVOID iAddr, PUSHORT oByte )
{
	int retval = 0;

	struct
	{
		PVOID iAddr;
		PUSHORT oByte;
	}CH36xReadIo_t;
	CH36xReadIo_t.iAddr = iAddr;
	CH36xReadIo_t.oByte = oByte;

	retval = ioctl( dev_fd, CH36x_READ_IO_WORD, (unsigned long)&CH36xReadIo_t);
	if( retval == -1 )
	{
		printf("ioctl function error line:%d\n", __LINE__);
		return false;
	}

	return true;
}

/*
 * ********************************************************************
 * CH36xReadIoDword( PVOID iAddr, PULONG oByte )
 *
 * FUNCTION : Read I/O Space Through Dword Read
 * arg:
 * iAddr  : Read addr
 * oByte  : Output buffer 
 * ********************************************************************
 */
BOOL CH36xReadIoDword( PVOID iAddr, PULONG oByte )
{
	int retval = 0;

	struct
	{
		PVOID	iAddr;
		PULONG	oByte;
	}CH36xReadIo_t;
	CH36xReadIo_t.iAddr = iAddr;
	CH36xReadIo_t.oByte = oByte;

	retval = ioctl(dev_fd, CH36x_READ_IO_DWORD, (unsigned long)&CH36xReadIo_t );
	if( retval == -1 )
	{
		printf("ioctl function error line:%d\n", __LINE__);
		return false;
	}

	return true;
}

/*
 * ********************************************************************
 * CH36xReadIoBlock( PVOID iAddr, PUCHAR oByte, ULONG oLength )
 *
 * FUNCTION : Read I/O Space Through Block Read
 * arg:
 * iAddr  : Read addr
 * oByte  : Output buffer 
 * oLength: Need to return the Length
 * ********************************************************************
 */
BOOL CH36xReadIoBlock( PVOID iAddr, PUCHAR oByte, ULONG oLength )
{
	int retval = 0;

	struct
	{
		PVOID	iAddr;
		PUCHAR	oByte;
		ULONG	oLength;
	}CH36xReadIoBlock_t;
	CH36xReadIoBlock_t.iAddr = iAddr;
	CH36xReadIoBlock_t.oByte = oByte;
	CH36xReadIoBlock_t.oLength = oLength;

	retval = ioctl( dev_fd, CH36x_READ_IO_BLOCK, (unsigned long)&CH36xReadIoBlock_t );
	if( retval == -1 )
	{
		printf("ioctl function error line:%d\n", __LINE__);
		return -1;
	}

	return true;
}

/*
 * ********************************************************************
 * CH36xWriteIoByte( PVOID iAddr, UCHAR iByte )
 *
 * FUNCTION : Write I/O Space through Byte Write
 * arg:
 * iAddr  : Write addr
 * iByte  : Iutput buffer 
 * ********************************************************************
 */
BOOL CH36xWriteIoByte( PVOID iAddr, UCHAR iByte )
{
	int retval = 0;

	struct
	{
		PVOID	iAddr;
		UCHAR	iByte;
	}CH36xWriteIo_t;
	CH36xWriteIo_t.iAddr = iAddr;
	CH36xWriteIo_t.iByte = iByte;

	retval = ioctl( dev_fd, CH36x_WRITE_IO_BYTE, (unsigned long)&CH36xWriteIo_t );
	if( retval == -1 )
	{
		printf("ioctl function error line:%d\n", __LINE__);
		return false;
	}

	return true;
}

/*
 * ********************************************************************
 * CH36xWriteIoWord( PVOID iAddr, USHORT iByte )
 *
 * FUNCTION : Write I/O Space through Word Write
 * arg:
 * iAddr  : Write addr
 * iByte  : Iutput buffer 
 * ********************************************************************
 */
BOOL CH36xWriteIoWord( PVOID iAddr, USHORT iByte )
{
	int retval = 0;

	struct
	{
		PVOID	iAddr;
		USHORT	iByte;
	}CH36xWriteIo_t;
	CH36xWriteIo_t.iAddr = iAddr;
	CH36xWriteIo_t.iByte = iByte;

	retval = ioctl( dev_fd, CH36x_WRITE_IO_WORD, (unsigned long)&CH36xWriteIo_t );
	if( retval == -1 )
	{
		printf("ioctl function error line:%d\n", __LINE__);
		return false;
	}

	return true;
}

/*
 * ********************************************************************
 * CH36xWriteIoDword( PVOID iAddr, ULONG iByte )
 *
 * FUNCTION : Write I/O Space through Dword Write
 * arg:
 * iAddr  : Write addr
 * iByte  : Iutput buffer 
 * ********************************************************************
 */
BOOL CH36xWriteIoDword( PVOID iAddr, ULONG iByte )
{
	int retval = 0;

	struct
	{
		PVOID	iAddr;
		ULONG	iByte;
	}CH36xWriteIo_t;
	CH36xWriteIo_t.iAddr = iAddr;
	CH36xWriteIo_t.iByte = iByte;

	retval = ioctl( dev_fd, CH36x_WRITE_IO_DWORD, (unsigned long)&CH36xWriteIo_t );
	if( retval == -1 )
	{
		printf("ioctl function error line:%d\n", __LINE__);
		return false;
	}

	return true;
}

/*
 * ********************************************************************
 * CH36xWriteIoBlock( PVOID iAddr, PUCHAR iByte, ULONG iLength )
 *
 * FUNCTION : Write I/O Space through Block Write
 * arg:
 * iAddr  : Write addr
 * iByte  : Iutput buffer 
 * iLength: Input length
 * ********************************************************************
 */
BOOL CH36xWriteIoBlock( PVOID iAddr, PUCHAR iByte, ULONG iLength )
{
	int retval = 0;

	struct
	{
		PVOID	iAddr;
		PUCHAR	iByte;
		ULONG	iLength;
	}CH36xWriteIoBlock_t;
	CH36xWriteIoBlock_t.iAddr = iAddr;
	CH36xWriteIoBlock_t.iByte = iByte;
	CH36xWriteIoBlock_t.iLength = iLength;

	retval = ioctl( dev_fd, CH36x_WRITE_IO_BLOCK, (unsigned long)&CH36xWriteIoBlock_t );
	if( retval == -1 )
	{
		printf("ioctl function error line:%d\n", __LINE__);
		return -1;
	}

	return true;
}

/*
 * ********************************************************************
 * CH36xReadMemByte( PVOID iAddr, PUCHAR oByte )
 *
 * FUNCTION : Read Mem Space through Byte Read
 * arg:
 * iAddr  : Read addr
 * oByte  : Output buffer 
 * ********************************************************************
 */
BOOL CH36xReadMemByte( PVOID iAddr, PUCHAR oByte )
{
	int retval = 0;

	struct 
	{
		PVOID	iAddr;
		PUCHAR	oByte;
	}CH36xReadMem_t;
	CH36xReadMem_t.iAddr = iAddr;
	CH36xReadMem_t.oByte = oByte;

	retval = ioctl( dev_fd, CH36x_READ_MEM_BYTE, (unsigned long)&CH36xReadMem_t );
	if( retval == -1 )
	{
		printf("ioctl function error line:%d\n", __LINE__);
		return false;
	}

	return true;
}

/*
 * ********************************************************************
 * CH36xReadMemWord( PVOID iAddr, PUSHORT oByte )
 *
 * FUNCTION : Read Mem Space through Word Read
 * arg:
 * iAddr  : Read addr
 * oByte  : Output buffer 
 * ********************************************************************
 */
BOOL CH36xReadMemWord( PVOID iAddr, PUSHORT oByte )
{
	int retval = 0;

	struct 
	{
		PVOID	iAddr;
		PUSHORT	oByte;
	}CH36xReadMem_t;
	CH36xReadMem_t.iAddr = iAddr;
	CH36xReadMem_t.oByte = oByte;

	retval = ioctl( dev_fd, CH36x_READ_MEM_WORD, (unsigned long)&CH36xReadMem_t );
	if( retval == -1 )
	{
		printf("ioctl function error line:%d\n", __LINE__);
		return false;
	}

	return true;
}

/*
 * ********************************************************************
 * CH36xReadMemDword( PVOID iAddr, PULONG oByte )
 *
 * FUNCTION : Read Mem Space through Dword Read
 * arg:
 * iAddr  : Read addr
 * oByte  : Output buffer 
 * ********************************************************************
 */
BOOL CH36xReadMemDword( PVOID iAddr, PULONG oByte )
{
	int retval = 0;

	struct
	{
		PVOID	iAddr;
		PULONG	oByte;
	}CH36xReadMem_t;
	CH36xReadMem_t.iAddr = iAddr;
	CH36xReadMem_t.oByte = oByte;

	retval = ioctl( dev_fd, CH36x_READ_MEM_DWORD, (unsigned long)&CH36xReadMem_t );
	if( retval == -1 )
	{
		printf("ioctl function error line:%d\n", __LINE__);
		return false;
	}

	return true;
}

/*
 * ********************************************************************
 * CH36xReadMemBlock( PVOID iAddr, PUCHAR oByte, ULONG oLength )
 *
 * FUNCTION : Read Mem Space through Block Read
 * arg:
 * iAddr  : Read addr
 * oByte  : Output buffer 
 * oLength: need to return the length
 * ********************************************************************
 */
BOOL CH36xReadMemBlock( PVOID iAddr, PUCHAR oByte, ULONG oLength )
{
	int retval = 0;

	struct 
	{
		PVOID	iAddr;
		PUCHAR	oByte;
		ULONG	oLength;
	}CH36xReadMemBlock_t;
	CH36xReadMemBlock_t.iAddr = iAddr;
	CH36xReadMemBlock_t.oByte = oByte;
	CH36xReadMemBlock_t.oLength = oLength;

	retval = ioctl( dev_fd, CH36x_READ_MEM_BLOCK, (unsigned long)&CH36xReadMemBlock_t );
	if( retval == -1 )
	{
		printf("ioctl function error line:%d\n", __LINE__);
		return false;
	}

	return true;
}

/*
 * ********************************************************************
 * CH36xWriteMemByte( PVOID iAddr, UCHAR iByte )
 *
 * FUNCTION : Write Mem Space through Byte Write
 * arg:
 * iAddr  : Write addr
 * iByte  : Iutput buffer 
 * ********************************************************************
 */
BOOL CH36xWriteMemByte( PVOID iAddr, UCHAR iByte )
{
	int retval = 0;

	struct
	{
		PVOID	iAddr;
		UCHAR	iByte;
	}CH36xWriteMem_t;
	CH36xWriteMem_t.iAddr = iAddr;
	CH36xWriteMem_t.iByte = iByte;

	retval = ioctl( dev_fd, CH36x_WRITE_MEM_BYTE, (unsigned long)&CH36xWriteMem_t );
	if( retval == -1 )
	{
		printf("ioctl function error line:%d\n", __LINE__);
		return false;
	}

	return true;
}

/*
 * ********************************************************************
 * CH36xWriteMemWord( PVOID iAddr, USHORT iByte )
 *
 * FUNCTION : Write Mem Space through Word Write
 * arg:
 * iAddr  : Write addr
 * iByte  : Iutput buffer 
 * ********************************************************************
 */
BOOL CH36xWriteMemWord( PVOID iAddr, USHORT iByte )
{
	int retval = 0;

	struct
	{
		PVOID	iAddr;
		USHORT	iByte;
	}CH36xWriteMem_t;
	CH36xWriteMem_t.iAddr = iAddr;
	CH36xWriteMem_t.iByte = iByte;

	retval = ioctl( dev_fd, CH36x_WRITE_MEM_WORD, (unsigned long)&CH36xWriteMem_t );
	if( retval == -1 )
	{
		printf("ioctl function error line:%d\n", __LINE__);
		return false;
	}

	return true;
}

/*
 * ********************************************************************
 * CH36xWriteMemDword( PVOID iAddr, ULONG iByte )
 *
 * FUNCTION : Write Mem Space through Dword Write
 * arg:
 * iAddr  : Write addr
 * iByte  : Iutput buffer 
 * ********************************************************************
 */
BOOL CH36xWriteMemDword( PVOID iAddr, ULONG iByte )
{
	int retval = 0;

	struct
	{
		PVOID	iAddr;
		ULONG	iByte;
	}CH36xWriteMem_t;
	CH36xWriteMem_t.iAddr = iAddr;
	CH36xWriteMem_t.iByte = iByte;

	retval = ioctl( dev_fd, CH36x_WRITE_MEM_DWORD, (unsigned long)&CH36xWriteMem_t );
	if( retval == -1 )
	{
		printf("ioctl function error line:%d\n", __LINE__);
		return false;
	}

	return true;
}

/*
 * ********************************************************************
 * CH36xWriteMemBlock( PVOID iAddr, PUCHAR iByte, ULONG iLength )
 *
 * FUNCTION : Write Mem Space through Block Write
 * arg:
 * iAddr  : Write addr
 * iByte  : Iutput buffer
 * iLength: Need input the length 
 * ********************************************************************
 */
BOOL CH36xWriteMemBlock( PVOID iAddr, PUCHAR iByte, ULONG iLength )
{
	int retval = 0;

	struct 
	{
		PVOID	iAddr;
		PUCHAR	iByte;
		ULONG	iLength;
	}CH36xWriteMemBlock_t;
	CH36xWriteMemBlock_t.iAddr = iAddr;
	CH36xWriteMemBlock_t.iByte = iByte;
	CH36xWriteMemBlock_t.iLength = iLength;

	retval = ioctl( dev_fd, CH36x_WRITE_MEM_BLOCK, (unsigned long)&CH36xWriteMemBlock_t );
	if( retval == -1 )
	{
		printf("ioctl function error line:%d\n", __LINE__);
		return false;
	}

	return true;
}

/*
 * ********************************************************************
 * CH36xReadConfig( PVOID iOffset, PUCHAR oByte )
 *
 * FUNCTION : Read Configuration Space
 * arg:
 * iOffset  : Read addr offset
 * oByte    : Output buffer
 * ********************************************************************
 */
BOOL CH36xReadConfig( PVOID iOffset, PUCHAR oByte )
{
	int retval = 0;
	//struct CH36xReadConfig_t
	struct
	{
		PVOID iOffset;
		PUCHAR oByte;
	}CH36xReadConfig_t;
	CH36xReadConfig_t.iOffset = iOffset;
	CH36xReadConfig_t.oByte = oByte;

//#ifdef CH36x_LIB_DEBUG
#if 0
printf("CH36xReadConfig_t ptr:%p\n", &CH36xReadConfig_t );
printf("iOffset:%p\n", iOffset);
printf("CH36xReadConfig_t iOffset:%p\n", CH36xReadConfig_t.iOffset);
printf("CH36xReadConfig_t iOffset ptr:%p\n", &CH36xReadConfig_t.iOffset);
printf("CH36xReadConfig_t oByte:%p\n", CH36xReadConfig_t.oByte);
printf("CH36xReadConfig_t oByte ptr:%p\n", &CH36xReadConfig_t.oByte);
#endif

	retval = ioctl( dev_fd, CH36x_READ_CONFIG_BYTE, (unsigned long)(&CH36xReadConfig_t) );
	if( retval == -1 )
	{
		printf("ioctl function error line:%d\n", __LINE__);
		return false;
	}
	return true;
}

/*
 * ********************************************************************
 * CH36xWriteConfig( PVOID iOffset, UCHAR iByte )
 *
 * FUNCTION : Write Configuration Space
 * arg:
 * iOffset  : Write addr offset
 * iByte    : Iutput buffer
 * ********************************************************************
 */
BOOL CH36xWriteConfig( PVOID iOffset, UCHAR iByte )
{
	int retval = 0;
	//struct CH36xWriteConfig_t
	struct
	{
		PVOID iOffset;
		UCHAR iByte;
	}CH36xWriteConfig_t;
	CH36xWriteConfig_t.iOffset = iOffset;
	CH36xWriteConfig_t.iByte   = iByte;

#ifdef CH36x_LIB_DEBUG
//printf("CH36xWriteConfig_t ptr:%p\n", &CH36xWriteConfig_t );
printf("iOffset:%p\n", iOffset);
printf("CH36xWriteConfig_t iOffset:%p\n", CH36xWriteConfig_t.iOffset);
#endif
	
	retval = ioctl( dev_fd, CH36x_WRITE_CONFIG_BYTE, (unsigned long)(&CH36xWriteConfig_t) );
	if( retval == -1 )
	{
		printf("ioctl function error line:%d\n", __LINE__);
		return false;
	}

	return true;
}

/*
 * ********************************************************************
 * CH36xReadConfigWord( PVOID iOffset, PUSHORT oByte )
 *
 * FUNCTION : Read Configuration Space Through Word Read
 * arg:
 * iOffset  : Read addr offset
 * oByte    : Output buffer
 * ********************************************************************
 */
BOOL CH36xReadConfigWord( PVOID iOffset, PUSHORT oByte )
{
	int retval = 0;

	struct 
	{
		PVOID	iOffset;
		PUSHORT oByte;
	}CH36xReadConfigWord_t;
	CH36xReadConfigWord_t.iOffset = iOffset;
	CH36xReadConfigWord_t.oByte   = oByte;

#if 0
//#ifdef CH36x_LIB_DEBUG
	printf("\n=====def_fd:%d\n", dev_fd );
#endif
	retval = ioctl( dev_fd, CH36x_READ_CONFIG_WORD, (unsigned long)(&CH36xReadConfigWord_t) );
	if( retval == -1 )
	{
		printf(" ioctl function error line:%d\n", __LINE__);
		return false;
	}

	return true;
}

/*
 * ********************************************************************
 * CH36xWriteConfigWord( PVOID iOffset, USHORT iByte )
 *
 * FUNCTION : Write Configuration Space Through Word Write
 * arg:
 * iOffset  : Write addr offset
 * iByte    : Iutput buffer space
 * ********************************************************************
 */
BOOL CH36xWriteConfigWord( PVOID iOffset, USHORT iByte )
{
	int retval = 0;

	struct 
	{
		PVOID	iOffset;
		USHORT  oByte;
	}CH36xWriteConfigWord_t;
	CH36xWriteConfigWord_t.iOffset	= iOffset;
	CH36xWriteConfigWord_t.oByte	= iByte;

	retval = ioctl( dev_fd, CH36x_WRITE_CONFIG_WORD, (unsigned long)(&CH36xWriteConfigWord_t) );
	if( retval == -1 )
	{
		printf(" ioctl function error line:%d\n", __LINE__);
		return false;
	}

	return true;
}

/*
 * ********************************************************************
 * CH36xReadConfigDword( PVOID iOffset, PULONG oByte )
 *
 * FUNCTION : Read Configuration Space Through Dword Read
 * arg:
 * iOffset  : Read addr offset
 * oByte    : Output buffer
 * ********************************************************************
 */
BOOL CH36xReadConfigDword( PVOID iOffset, PULONG oByte )
{
	int retval = 0;

	struct 
	{
		PVOID	iOffset;
		PULONG oByte;
	}CH36xReadConfigDword_t;
	CH36xReadConfigDword_t.iOffset	= iOffset;
	CH36xReadConfigDword_t.oByte	= oByte;

	retval = ioctl( dev_fd, CH36x_READ_CONFIG_DWORD, (unsigned long)(&CH36xReadConfigDword_t) );
	if( retval == -1 )
	{
		printf(" ioctl function error line:%d\n", __LINE__);
		return false;
	}

	return true;
}

/*
 * ********************************************************************
 * CH36xWriteConfigDword( PVOID iOffset, ULONG iByte )
 *
 * FUNCTION : Write Configuration Space Through Dword Write
 * arg:
 * iOffset  : Write addr offset
 * iByte    : Iutput buffer space
 * ********************************************************************
 */
BOOL CH36xWriteConfigDword( PVOID iOffset, ULONG iByte )
{
	int retval = 0;

	struct 
	{
		PVOID	iOffset;
		ULONG   oByte;
	}CH36xWriteConfigDword_t;
	CH36xWriteConfigDword_t.iOffset		= iOffset;
	CH36xWriteConfigDword_t.oByte		= iByte;

	retval = ioctl( dev_fd, CH36x_WRITE_CONFIG_DWORD, (unsigned long)(&CH36xWriteConfigDword_t) );
	if( retval == -1 )
	{
		printf(" ioctl function error line:%d\n", __LINE__);
		return false;
	}

	return true;
}

BOOL CH36xWaitInterrupt( )
{
	//This function just for windows
	return true;
}

BOOL CH36xAbortInterrupt( )
{
	//This function just for windows
	return true;
}

//This function just for windows
BOOL CH36xSetIntRoutine( mPCH36x_INT_ROUTINE iIntRoutine )
{
	//pid_t child_pid = 0;

	if( iIntRoutine == NULL )
	{
		if( child_pid != 0)
		{
			if( kill(child_pid, SIGKILL) != 0 )
				printf("kill error, child_pid:%d, line:%d\n",
						child_pid, __LINE__);
			printf("kill success\n");
		}
		return true;
	}

	if(	(child_pid = fork()) == 0 ) {
		for(;;) 
			{
			fd_set rds;	
			int ret = 0;

			FD_ZERO( &rds );
			FD_SET( dev_fd, &rds );

	//		ret = select( dev_fd+1, &rds, NULL, NULL, NULL );
			if( ret < 0 ) {
				perror("select error\n");
				exit(1);
			}
			else if( FD_ISSET( dev_fd, &rds ) ) {
				/*printf("####\ninterrupter coming\n####\n");*/
				iIntRoutine();
			}
		}
	}
	else {
		printf("****\nchild_pid:%d\n****\n", child_pid);
	}

	return true;
}

