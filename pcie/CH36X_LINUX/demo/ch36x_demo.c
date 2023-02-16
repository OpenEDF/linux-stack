/*
 *ch36x_demo.c for ch365/ch367/ch368 pci card of WCH
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
 * This file is used for testing I/O\MEM\Configuration\ (Read and Write)
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

#ifdef CH36x_DEMO_DEBUG
#define dbg( format, arg...)	\
	printf(format "\n", ##arg)
#else
#define dbg( format, arg... )	\
do{				\
	if(0)			\
		printf( format "\n", ##arg);	\
}while(0)
#endif

#define err( format, arg... )		\
	printf("err %d: " format "\n", __LINE__, ##arg)

extern int dev_fd;
BOOL retval;
mPCH36x_IO_REG	oIoBaseAddr;
mPCH36x_MEM_REG	oMemBaseAddr;

int GetAddr(void);

// Display console Main menu
void ShowMainMenu(void)
{
	system("clear");
	printf("This is main menu listed:\n");
	printf("1:for IO port\n");
	printf("2:for IO block\n");
	printf("3:for sigle IO(control LED)\n");
	printf("4:for device memory(for ch368)\n");
	printf("5:for configurations space\n");
	printf("\nPlease enter your selection:\n");
}

// I/O Space byte/word/dword Read 
int IoPort_Action(void)
{
	PVOID		iAddr;	
	PUCHAR		oByte_char;	
	PUSHORT		oByte_short;	
	PULONG		oByte_long;		
	PUCHAR		oByte_block_io;	

	//Read by a byte
	iAddr = &oIoBaseAddr->mCh36xIoTime; //loop counting Reg 	
	oByte_char = (PUCHAR)malloc( sizeof(UCHAR) );	
	dbg("char iAddr : %p", iAddr );	
	dbg("char oByte_char : %p", oByte_char );

	retval = CH36xReadIoByte( iAddr, oByte_char );	
	if( retval == false )	
	{		
		err("CH36xReadIoByte error");		
		return -1;	
	}	
	dbg("*oByte_char : 0x%x", *oByte_char );

	//Read by a word	
	iAddr = &oIoBaseAddr->mCh36xIoPort[0x00];	
	oByte_short = (PUSHORT)malloc( sizeof(USHORT) );

	dbg("short iAddr : %p", iAddr );	
	dbg("short oByte_short : %p", oByte_short );	

	retval = CH36xReadIoWord( iAddr, oByte_short );	
	if( retval == false )	
	{		
		err("CH36xReadIoWord error");		
		return -1;	
	}


	//Read by a double word	
	iAddr = &oIoBaseAddr->mCh36xIoPort[0x00];	
	oByte_long = (PULONG)malloc( sizeof(ULONG) );	
	dbg("long iAddr : %p", iAddr );	
	dbg("long oByte_long : %p", oByte_long );	
	retval = CH36xReadIoDword( iAddr, oByte_long );	
	if( retval == false )	
	{		
		err("CH36xReadIoDword error");		
		return -1;	
	}	
	dbg("*oByte_long : 0x%x\n", (unsigned int)*oByte_long );
	
}

/*
 * Read Block I/O 
 * Write Byte/Dword/Block I/O
 */
int IoBlock_Action(void)
{
	PVOID		iAddr;	
	UCHAR		iByte_char;	
	USHORT		iByte_short;	
	ULONG		iByte_long;	
	PUCHAR		oByte_block_io;	
	ULONG		oLength_io;	
	PUCHAR		iByte_block_io;	
	ULONG		iLength_io;

	dbg("\n ----Read IO block----");	
	iAddr = &oIoBaseAddr->mCh36xIoPort[0x00];	
	oLength_io = 0x0a;	
	oByte_block_io = (PUCHAR)malloc( sizeof(UCHAR)*oLength_io );
	dbg(" iAddr : %p", iAddr );	
	dbg(" oByte_block_io : %p", oByte_block_io );	
	dbg(" oLength_io : 0x%x", (unsigned int)oLength_io );
	retval = CH36xReadIoBlock( iAddr, oByte_block_io, oLength_io );
	if( retval == false )	
	{		
		err("CH36xReadIoBlock error");		
		return -1;	
	}
	dbg(" oByte_block_io : %p", oByte_block_io );	

#ifdef CH36x_DEMO_DEBUG	
	int	icount = 0;	
	for( icount = 0; icount < oLength_io; icount++ )	
	{		
		dbg(" oByte_block_io[0x%d] = 0x%x", icount, oByte_block_io[icount] );	
	}
#endif
	//Write one byte 	
	iAddr = &oIoBaseAddr->mCh36xIoPort[0x00];	
        iByte_char = 0xEE;

	 retval = CH36xWriteIoByte( iAddr, iByte_char );	
	 if( retval == false )	
	 {		
	 	err("CH36xWriteIoByte error");		
		return -1;	
	 }

	 
	 //Write double word	
	 iAddr = &oIoBaseAddr->mCh36xIoPort[0x00];	
	 iByte_long = 0xEEEEEEEE;	
	 retval = CH36xWriteIoDword( iAddr, iByte_long );	
	 if( retval == false )	
	 {		
	 	err("CH36xWriteIoDword error");		
		return -1;	
	 }

	 
	 //Write a block into IO	
	 printf("\n\n------Write IO block------\n");	
	 iAddr = &oIoBaseAddr->mCh36xIoPort[0x00];	
	 iLength_io = 0x0a;	
	 /*iByte_block_io = (PUCHAR)malloc( sizeof(UCHAR)*iLength_io );*/	
	 iByte_block_io = (PUCHAR)malloc( sizeof(UCHAR)*iLength_io );	

	 memset( iByte_block_io, 0, iLength_io );	
	 strcpy( (char *)iByte_block_io, "001122334455" );
	 
	 dbg(" iAddr : %p", iAddr );	
	 dbg(" iByte_block_io : %p", iByte_block_io );	
	 dbg(" iLength_io : 0x%x", (unsigned int)iLength_io );	

	 retval = CH36xWriteIoBlock( iAddr, iByte_block_io, iLength_io );	
	 if( retval == false )	
	 {		
	 	err("CH36xWriteIoBlock error");		
		return -1;	
	 }
	
}

// Read configuration space register including Get Driver Version
int ConfigurationSpace()
{
	PVOID iOffset = (PVOID)0x01;	
	
	PUCHAR oByte = (unsigned char *)malloc(sizeof(UCHAR));
	
	retval = CH36xReadConfig( iOffset, oByte );	
	if( retval == false )	
	{		
		err("CH36xReadConfig error");		
		return -1;	
	}	
	
	dbg("value of iOffset : %p", iOffset);	
	dbg("value of iByte : %x", *oByte);	
	
	char *drv_version = (char *)malloc( sizeof(char)*30 );	
	
	retval = CH36xGetDrvVer( drv_version );	
	if( retval == false )	
	{		
		err("CH36xGetDrvVer error");		
		return -1;	
	}	
	
	dbg("drv_version:%s", drv_version);
	
}

// Read Mem Space: Byte/Dword/Block
// Write Mem Space : Byte/Dword/Block
int DeviceMemory(void)
{
	ULONG	iOffset_mem;
	
	PVOID	iAddr_mem;	
	
	PUCHAR	oByte_char_mem;	
	PULONG	oByte_long_mem;
	
	UCHAR	iByte_char_mem;	
	ULONG	iByte_long_mem;	
	//--for operations of read or write a block memory	
	PVOID	iAddr_block_mem;	
	PUCHAR	oByte_block_mem;	
	ULONG	oLength;	
	
	PUCHAR	iByte_block_mem;	
	ULONG	iLength;

	//Read one byte from device memory( IO space )	
	iOffset_mem = 0x00;	//0x0000-0x7fff 32K	
	iAddr_mem = &oMemBaseAddr->mCh36xMemPort[iOffset_mem];	
	oByte_char_mem = (PUCHAR)malloc( sizeof(UCHAR) );	

	dbg("iAddr_mem : %p\n", iAddr_mem );	
	dbg("oByte_char_mem : %p\n", oByte_char_mem );

	retval = CH36xReadMemByte( iAddr_mem, oByte_char_mem );	
	if( retval == false )	
	{		
		err("CH36xReadMemByte error");		
		return -1;	
	}	
	dbg("*oByte_char_mem : 0x%x", *oByte_char_mem );

	//Read double words form device memory( IO space )	
	iOffset_mem = 0x00;	
	iAddr_mem = &oMemBaseAddr->mCh36xMemPort[iOffset_mem];	
	oByte_long_mem = (PULONG)malloc( sizeof(ULONG) );	
	
	dbg("iAddr_mem : %p", iAddr_mem );	
	dbg("oByte_long_mem : %p", oByte_long_mem );

	retval = CH36xReadMemDword( iAddr_mem, oByte_long_mem );	
	if( retval == false )	
	{		
		err("CH36xReadMemDword error");		
		return -1;	
	}	
	dbg("*oByte_long_mem : 0x%x", (unsigned int)*oByte_long_mem );

	//Read a block from device memory( IO space )	
	printf("\n------ Read a block ------\n");	
	iOffset_mem = 0x00;		
	iAddr_block_mem = &oMemBaseAddr->mCh36xMemPort[iOffset_mem];	
	oLength = 0x0a;	
	oByte_block_mem = (PUCHAR)malloc( sizeof(UCHAR)*oLength );	
	memset( oByte_block_mem, 0, oLength );	
	
	dbg("iAddr_block_mem : %p", iAddr_block_mem );	
	dbg("oByte_block_mem : %p", oByte_block_mem );

	retval = CH36xReadMemBlock( iAddr_block_mem, oByte_block_mem, oLength );	if( retval == false )	
	{		
		err("CH36xReadMemBlock error");		
		return -1;	
	}	
	
	dbg("\n*oByte_block_mem.str : %s",  oByte_block_mem );

	//Write one byte into device memory( IO space )		
	iOffset_mem = 0x00;		
	iAddr_mem = &oMemBaseAddr->mCh36xMemPort[iOffset_mem];	
	iByte_char_mem = 0x11; //just for test 
	
	dbg("\niAddr_mem : %p", iAddr_mem );	
	dbg("iByte_char_mem:0x%x", iByte_char_mem );	

	retval = CH36xWriteMemByte( iAddr_mem, iByte_char_mem );	
	if( retval == false )	
	{		
		err("CH36xWriteMemByte error");		
		return -1;	
	}

	//Write double words into device memory( IO space )	
	iOffset_mem = 0x00;	
	iAddr_mem = &oMemBaseAddr->mCh36xMemPort[iOffset_mem];	
	iByte_long_mem = 0x61626364; //just for test 	

	dbg("iAddr_mem : %p", iAddr_mem );	
	dbg("iByte_long_mem : 0x%x", (unsigned int)iByte_long_mem );	

	retval = CH36xWriteMemDword( iAddr_mem, iByte_long_mem );
	if( retval == false )	
	{		
		err("CH36xWriteMemByte error");		
		return -1;	
	}

	//Write a block into device memory( IO space )	
	printf("\n------Write a block------\n");	
	iOffset_mem = 0x04;	
	iAddr_block_mem = &oMemBaseAddr->mCh36xMemPort[iOffset_mem];	
	//iByte_block_mem = 0x65666768; //just for test	
	iLength = 0x0a;	
	iByte_block_mem = (PUCHAR)malloc( sizeof(UCHAR)*iLength );	
	memset( iByte_block_mem, 0, iLength );	
	strcpy( (char *)iByte_block_mem, "001122334" );	

	dbg("iAddr_block_mem : %p", iAddr_block_mem);	
	dbg("iByte_block_mem:0x%x", (unsigned int)iByte_block_mem);	

	retval = CH36xWriteMemBlock( iAddr_block_mem, iByte_block_mem, iLength );	
	if( retval == false )	
	{		
		err("CH36xWriteMemBlock error");		
		return -1;	
	}

}

// Select Mode to Read/Write
int main(int argc, char **argv)
{
	int ch;
	char button = '\0';
	dev_fd = CH36xOpenDevice( false, true );
	//the second arg for interrupt : ch365 selcet false;
	//		   ch367/ch368  true;
	if(dev_fd == -1)
	{
		err("CH36xOpenDevice open error");
		return -1;
	}
	GetAddr();
	sleep(5);
	
	while(1)
   	{

	ShowMainMenu();
	scanf("%d",&ch);
	printf("You choose %d \n",ch);
	switch(ch)
	{
	    case 1:
		printf("------I/O Port------\n");
		IoPort_Action();
		break;
	    case 2:
		printf("------I/O Block------\n");
		IoBlock_Action();
		break;
	    case 3:
		printf("currently under development...\n");
		break;
	    case 4:
		printf("------device memory------\n");
		DeviceMemory();
		break;
	    case 5:
		printf("------configuration space------\n");
		ConfigurationSpace();
		break;
	    default:
		break;
	}
	do
	{
		printf("\nenter 'q' to exit or 'b' to come back\n");	
		scanf(" %c",&button);
	}while(button != 'q' && button != 'b');
	
	if( button == 'q')
		break;	
	}
	CH36xCloseDevice();
	return 0;
}

// Get I/O\MEM Space Base Addr
int GetAddr(void)
{
	BOOL retval;	
	oIoBaseAddr = (mPCH36x_IO_REG)malloc(sizeof(mCH36x_IO_REG));
	retval = CH36xGetIoBaseAddr( &oIoBaseAddr );	
	if( retval == false )	
	{		
		err("CH36xGetIoBaseAddr error");		
		return -1;	
	}
	dbg("oIoBaseAddr : %p", oIoBaseAddr );
		
		
	oMemBaseAddr = (mPCH36x_MEM_REG)malloc( sizeof(mCH36x_MEM_REG) );
	retval = CH36xGetMemBaseAddr( &oMemBaseAddr );	
	if( retval == false )	
	{		
		err("CH36xGetMemBaseAddr error");		
		return -1;	
	}	
	dbg("oMemBaseAddr : %p", oMemBaseAddr );	
		
	//---Get or Set Interrupter Line	
	dbg("-----Get or Set Interrupter Line-----");	
	PULONG	oIntLine;	
	ULONG	iIntLine = 0x55; //just for test		
	oIntLine = (PULONG)malloc( sizeof(ULONG) );		
	retval = CH36xGetIntLine( oIntLine );	
	if( retval == false )	
	{		
		err("CH36xGetIntLine error");		
		return -1;	
	}	
	dbg("*oIntLine : 0x%x, %d", (unsigned int)*oIntLine, (unsigned int)*oIntLine );	
	retval = CH36xSetIntLine( iIntLine );	
	if( retval == false )	
	{		
		err("CH36xSetIntLine error");		
		return -1;	
	}	
	dbg("iIntLine : 0x%x, %d", (unsigned int)iIntLine, (int)iIntLine );
}




