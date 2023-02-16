/*
 *ch36x_demo_CMD.c for ch365/ch367/ch368 pci card of WCH
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
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <malloc.h>

#include "ch36x_lib.h"

#ifndef CH36x_DEMO_CMD_DEBUG
#define CH36x_DEMO_CMD_DEBUG
#endif

#define CH36x_IO_SPACE_RANGE	0xff
#define CH36x_MEM_SPACE_RANGE	0x8000

#define CH36x_DEMO_CMD_VERSION	"WCH CH36x Demo CMD version-1.2"

extern int dev_fd;

static int ch36x_dev_num = 0x10;
static char ch36x_dev_list[2048];
static char *ch36x_dev_path = "/dev/ch36xpci";
//static char *ch36x_dev_path = "/tmp/ch36xpci";
/*static char get_err;*/

mPCH36x_IO_REG	oIoBaseAddr_IO;
mPCH36x_MEM_REG	oMemBaseAddr_MEM;

void ChrToHex( const char *str, unsigned int *dst );
void HexToChr( char *str, unsigned int dst );

int main_menu_cmd( void );
static int get_ch36x_dev_list( void );
void oper_menu_cmd( char get_chose_main );
int open_choosed_dev( char get_chose_main );
void get_ID_BaseAddr( void );
void read_write_cfg( void );
void read_cfg( void );
void write_cfg( void );
void read_write_io( void );
void read_io( void );
void write_io( void );
void read_io_block( void );
void write_io_block( void );
void read_write_mem( void );
void read_mem( void );
void write_mem( void );
void read_mem_block( void );
void write_mem_block( void );


int main( int argc, char **argv )
{
	if( main_menu_cmd() < 0 )
	{
		/*sleep(3);*/
		getc(stdin);
		return -1;
	}

	return 0;
}


int main_menu_cmd( void )
{
	int retval = 0;
	char get_chose_main;

	system("clear");
	printf("\t\t********WCH CH36x CHIPS Debug********\n");
	retval = get_ch36x_dev_list();
	printf("%s\n", ch36x_dev_list );
	if( -1 == retval )
		return -1;
	printf("\t-->Input index num:");
	scanf(" %c", &get_chose_main );

	oper_menu_cmd( get_chose_main );	

	return 0;
}

/*
 * ********************************************************************
 * oper_menu_cmd( char get_chose_main )
 *
 * FUNCTION : create main window
 * 
 * ********************************************************************
 */
void oper_menu_cmd( char get_chose_main )
{
	char get_oper_chose;
	int retval = 0;
	int exit_quit = 0;
	char *drv_version = (char *)malloc( sizeof(char)*30 );
	char *lib_version = (char *)malloc( sizeof(char)*30 );

	while(1) 
	{
		system("clear");
		printf("\t\t********WCH CH36x CHIPS Debug********\n");
		retval = get_ch36x_dev_list();
		if( retval == -1 )
		{
			printf("\tCan't find CH36x devices\n");
			getc(stdin);
			/*sleep(3);*/
			break;
		}
		open_choosed_dev( get_chose_main );
		printf("\t-------------Current Opened dev Num:[%c]------------\n", get_chose_main );

		retval = CH36xGetDrvVer( drv_version );
		if( retval == false )
		{
			printf("\tErr:Get CH36x Driver version\n");
			/*break;*/
			return;
		}
		printf("\t|           %s          |\n", drv_version );

		retval = CH36xGetLibVer( lib_version );
		if( retval == false )
		{
			printf("\tErr:Get CH36x Lib versioin\n");
			return;
		}
		printf("\t|            %s            |\n", lib_version );
		printf("\t|          %s         |\n", CH36x_DEMO_CMD_VERSION );
		printf("\t---------------------------------------------------\n");
		get_ID_BaseAddr();
		printf("\t--> [1].Read/Write Config Space\n");
		printf("\t--> [2].Read/Write I/O Space\n");
		printf("\t--> [3].Read/Write Memory Space\n");
		printf("\t--> [0].Exit\n");
		printf("\tEnter your chose:");
		scanf(" %c", &get_oper_chose );

		switch( get_oper_chose )
		{
			case '1':
				{
					read_write_cfg();
					break;
				}
			case '2':
				{
					read_write_io();
					break;
				}
			case '3':
				{
					read_write_mem();
					break;
				}
			case '0':
				{
					exit_quit = 1;
					break;
				}
			default:
				{
					printf("\tWarning! input error, Enter again\n");
					getc(stdin);
					/*sleep(1);*/
					break;
				}
		}
		if( exit_quit == 1 )
		{
			CH36xCloseDevice();
			break;
		}
	}

	return;
}

/*
 * ********************************************************************
 * open_choosed_dev( char get_chose_main )
 *
 * FUNCTION : open selected device which is existing
 * 
 * ********************************************************************
 */
int open_choosed_dev( char get_chose_main )
{
	char dev_path[20] = {'\0'};

	memset( dev_path, '\0', sizeof(dev_path) );
	sprintf( dev_path, "%s%c", ch36x_dev_path, get_chose_main - 1 );

	dev_fd = open( dev_path, O_RDWR );
#if 0
//#ifdef CH36x_DEMO_CMD_DEBUG
		printf("--DEBUG-- dev_path:%s dev_fd:%d\n", dev_path, dev_fd );
#endif
	if( dev_fd == -1 )
	{
		printf("\tErr:open %s error\n", dev_path );
		return -1;
	}

	return 0;
}

void get_ID_BaseAddr( void )
{
	//char buffer_tmp[20] = {'\0'};
	PVOID	iOffset = NULL;
	PUSHORT	oByte_word = (PUSHORT)malloc( sizeof(USHORT) );
	PULONG	oByte_dword = (PULONG)malloc( sizeof(ULONG) );

	oIoBaseAddr_IO = (mPCH36x_IO_REG)malloc( sizeof(mCH36x_IO_REG) );
	oMemBaseAddr_MEM = (mPCH36x_MEM_REG)malloc( sizeof(mCH36x_MEM_REG) );
	
	/* Get Vendor ID */
	iOffset = (PVOID)0x00;
	CH36xReadConfigWord( iOffset, oByte_word ); 
	printf("\t|Vendor ID  :[0x%x] |", (unsigned int)(*oByte_word) );

	/* Get I/O Base Address */
	iOffset = (PVOID)0x10;
	CH36xReadConfigDword( iOffset, oByte_dword );
	printf("\tIO Base Addr :[0x%x]    |\n", (unsigned int)(*oByte_dword)-1 );

	/* Get Device ID */
	iOffset = (PVOID)0x02;
	CH36xReadConfigWord( iOffset, oByte_word );
	printf("\t|Device ID  :[0x%x] |", (unsigned int)(*oByte_word) );

	/* Get Mempry Base Address */
	iOffset = (PVOID)0x14;
	CH36xReadConfigDword( iOffset, oByte_dword );
	printf("\tMEM Base Addr:[0x%x]|\n", (unsigned int)(*oByte_dword) );
	printf("\t---------------------------------------------------\n");


	//for operations of I/O and Memory
	CH36xGetIoBaseAddr( &oIoBaseAddr_IO );	
	/*printf("\t IO Base Addr :[%p]", oIoBaseAddr_IO );*/

	//for operations of I/O and Memory
	CH36xGetMemBaseAddr( &oMemBaseAddr_MEM );
	/*printf("\t MEM Base Addr:[%p]\n", oMemBaseAddr_MEM);*/

	return;
}

/*
 * ********************************************************************
 * read_write_cfg( void )
 *
 * FUNCTION : read/write cfg (byte/word/dword)
 * 
 * ********************************************************************
 */
void read_write_cfg( void )
{
	char get_cfg_oper_chose;
	int exit_flag = 0;
	while(1)
	{
		system("clear");
		printf("\t\t********WCH CH36x CHIPS Debug********\n");
		printf("\t-------------Read/Write [ Config ] Space------------\n");
		printf("\t--> [1].Read\n");
		printf("\t--> [2].Write\n");
		printf("\t--> [0].Quit\n");
		printf("\tEnter choose:");
		scanf(" %c", &get_cfg_oper_chose );

		switch( get_cfg_oper_chose )
		{
			case '1':
				{
					read_cfg();
					break;
				}
			case '2':
				{
					write_cfg();
					break;
				}
			case '0':
				{
					exit_flag = 1;
					break;
				}
			default:
				{
					printf("\tChose error, Enter again!\n");
					getc(stdin);
					/*sleep(2);*/
					break;
					//continue;
				}
		}

		if( 1 == exit_flag )
			break;
	}

	return;
}

/*
 * ********************************************************************
 * read_cfg( void )
 *
 * FUNCTION : read configuration through byte/word/dword read 
 * 	-->byte : CH36xReadConfig()
 *	-->word : CH36xReadConfigWord()
 *	-->dword: CH36xReadConfigDword()
 * ********************************************************************
 */
void read_cfg( void )
{
	/*char get_chose;*/
	char get_chose_type;
	int exit_quit = 0;
	char iOffset_tmp[20] = {'\0'};

	unsigned int dst = 0x00;
	PVOID iOffset;
	PUCHAR	oByte_byte = (PUCHAR)malloc( sizeof(UCHAR) );
	PUSHORT	oByte_word = (PUSHORT)malloc( sizeof(USHORT) );
	PULONG	oByte_dword = (PULONG)malloc( sizeof(ULONG) );
	char str_read[20] = {'\0'};

	system("clear");
	printf("\t\t********WCH CH36x CHIPS Debug********\n");
	printf("\t-----------------Read Config Space------------------\n");
	while(1)
	{
		printf("\t-->Input iOffset:0x" );
		setbuf( stdin, NULL );
		memset( iOffset_tmp, '\0', sizeof(iOffset_tmp) );
		scanf("%s", iOffset_tmp );
#if 0
//#ifdef CH36x_DEMO_CMD_DEBUG
		printf("--DEBUG-- iOffset:%s\n", iOffset_tmp );
#endif
		dst = 0x00;
		ChrToHex( iOffset_tmp, &dst );
#if 0
//#ifdef CH36x_DEMO_CMD_DEBUG
		printf("--DEBUG-- dst:0x%x\n", dst );
#endif
		iOffset = (PVOID)(dst);
		printf("\tChoose Read size( 1:byte 2:word 3:Dword )\n");
		printf("\t-->Input Size:");
		scanf(" %c", &get_chose_type );
#if 0
/*#ifdef CH36x_DEMO_CMD_DEBUG*/
		printf("--DEBUG-- get_chose_type:%c\n", get_chose_type );
#endif

		memset( str_read, '\0', sizeof(str_read) );
		switch( get_chose_type )
		{
			case '1':
				{
					CH36xReadConfig( iOffset, oByte_byte );
					HexToChr( str_read, (unsigned int)(*oByte_byte) );
					break;
				}
			case '2':
				{
					CH36xReadConfigWord( iOffset, oByte_word );
					HexToChr( str_read, (unsigned int)(*oByte_word) );
					break;
				}
			case '3':
				{
					CH36xReadConfigDword( iOffset, oByte_dword );
					HexToChr( str_read, (unsigned int)(*oByte_dword) );
					break;
				}
			case '4':
				{
					exit_quit = 1;
					break;
				}
			default:
				{
					printf("\tInput error, Enter again!\n");
					getc(stdin);
					/*sleep(2);*/
					break;	
				}
		}
		if( 1 == exit_quit )
			break;
		printf("\t==>[Value:0x%s]\n\t-----------------------Next-------------------------\n", str_read );

		char	get_continue;
		printf("\t-->Continue Read? (Y/N):");
		scanf(" %c", &get_continue );
		if( get_continue == 'Y' || get_continue == 'y' )
			continue;
		else if( get_continue == 'N' || get_continue == 'n' )
			break;
		else
		{
			printf("\tErr: Invalid input, Will exit\n");
			break;
		}
	}
	
	return;
}

/*
 * ********************************************************************
 * write_cfg( void )
 *
 * FUNCTION : write configuration through byte/word/dword write 
 * 	-->byte : CH36xWriteConfig()
 *	-->word : CH36xWriteConfigWord()
 *	-->dword: CH36xWriteConfigDword()
 * ********************************************************************
 */
void write_cfg( void )
{
	char get_chose_type;
	//int exit_quit = 0;
	char iOffset_tmp[20] = {'\0'};
	char input_value[20] = {'\0'};

	unsigned int dst = 0x00;
	PVOID iAddr = NULL;
	UCHAR	iByte_byte = 0x00;
	USHORT	iByte_word = 0x00;
	ULONG	iByte_dword = 0x00;
	//char str_read[20] = {'\0'};

	system("clear");
	printf("\t\t********WCH CH36x CHIPS Debug********\n");
	printf("\t-----------------Write Config Space------------------\n");
	while(1)
	{
		printf("\t-->Input iOffset:0x");
		setbuf( stdin, NULL );
		memset( iOffset_tmp, '\0', sizeof(iOffset_tmp) );
		scanf("%s", iOffset_tmp );

		dst = 0x00;
		ChrToHex( iOffset_tmp, &dst );
		iAddr = ((char *)oIoBaseAddr_IO) + dst;
		printf("\tChoose Read size( 1:byte 2:word 3:Dword )\n");
		printf("\t-->Input Size:");
		scanf(" %c", &get_chose_type );

		memset( input_value, '\0', sizeof(input_value) );
		unsigned int tmp = 0x00;
		int retval = 0;
		switch( get_chose_type )
		{
			case '1':
				{
					printf("\t-->Input value:0x");
					setbuf( stdin, NULL );
					scanf("%s", input_value );
					ChrToHex( input_value, &tmp );
					iByte_byte = (UCHAR)tmp;
					retval = CH36xWriteConfig( iAddr, iByte_byte );
					break;
				}
			case '2':
				{
					printf("\t-->Input value:0x");
					setbuf( stdin, NULL );
					scanf("%s", input_value );
					ChrToHex( input_value, &tmp );
					iByte_word = (USHORT)tmp;
					retval = CH36xWriteConfigWord( iAddr, iByte_word );
					break;
				}
			case '3':
				{
					printf("\t-->Input value:0x");
					setbuf( stdin, NULL );
					scanf("%s", input_value );
					ChrToHex( input_value, &tmp );
					iByte_dword = (ULONG)tmp;
					retval = CH36xWriteConfigDword( iAddr, iByte_dword );
					break;
				}
			default:
				{
					printf("\tInput error, Enter again!\n");
					getc(stdin);
					/*sleep(2);*/
					break;
				}
		}

		if( retval == true )
			printf("\t==>[Write Success]\n\t-----------------------Next-------------------------\n" );
		else if( retval == false )
		{
			printf("\t==>[Write Error]\n" );
			getc(stdin);
			/*sleep(3);*/
			break;
		}

		char	get_continue;
		printf("\t-->Continue Write? (Y/N):");
		scanf(" %c", &get_continue );
		if( get_continue == 'Y' || get_continue == 'y' )
			continue;
		else if( get_continue == 'N' || get_continue == 'n' )
			break;
		else
		{
			printf("\tErr: Invalid input, Will exit\n");
			break;
		}
	}

	return;
}

/*
 * ********************************************************************
 * read_write_io( void )
 *
 * FUNCTION : read/write io (byte/word/dword/block)
 * 
 * ********************************************************************
 */
void read_write_io( void )
{
	char get_io_oper_chose;
	int exit_flag = 0;
	while(1)
	{
		system("clear");
		printf("\t\t********WCH CH36x CHIPS Debug********\n");
		printf("\t---------------Read/Write [ I/O ] Space-------------\n");
		printf("\t--> [1].Read\n");
		printf("\t--> [2].Write\n");
		printf("\t--> [3].Read a Block\n");
		printf("\t--> [4].Write a Block\n");
		printf("\t--> [0].Quit\n");
		printf("\tEnter choose:");
		scanf(" %c", &get_io_oper_chose );

		switch( get_io_oper_chose )
		{
			case '1':
				{
					read_io();
					break;
				}
			case '2':
				{
					write_io();
					break;
				}
			case '3':
				{
					read_io_block();
					break;
				}
			case '4':
				{
					write_io_block();
					break;
				}
			case '0':
				{
					exit_flag = 1;
					break;
				}
			default:
				{
					printf("\tChose error, Enter again!\n");
					getc(stdin);
					/*sleep(2);*/
					break;
					//continue;
				}
		}

		if( 1 == exit_flag )
			break;
	}

	return;

}

/*
 * ********************************************************************
 * read_io_block()
 *
 * FUNCTION : read io through block read 
 * 	-->block : CH36xReadIoBlock()
 * ********************************************************************
 */
void read_io_block( void )
{
	int retval = 0;
	char iOffset_tmp[20] = {'\0'};
	//char get_chose_type;
	//char get_oLength;
	

	unsigned int dst = 0x00;
	PVOID	iAddr_block_io = NULL; 
	ULONG	oLength = 0;
	PUCHAR	oByte_block_io = (PUCHAR)malloc( sizeof(UCHAR)*512 );
	system("clear");
	printf("\t\t********WCH CH36x CHIPS Debug********\n");
	printf("\t------------Read a Block from I/O Space--------------\n");
	while(1)
	{	
		printf("\t-->Input iOffset:0x" );
		setbuf( stdin, NULL );
		memset( iOffset_tmp, '\0', sizeof(iOffset_tmp) );
		scanf("%s", iOffset_tmp );
		dst = 0x00;
		ChrToHex( iOffset_tmp, &dst );
		if( dst < 0x00 || dst > CH36x_IO_SPACE_RANGE )
		{
			printf("\tErr: range of length\n");
			setbuf( stdin, NULL );
			getc(stdin);
			break;
		}
		iAddr_block_io = ((char *)oIoBaseAddr_IO) + dst;

		printf("\t-->Input length:0x");
		setbuf( stdin, NULL );
		memset( iOffset_tmp, '\0', sizeof(iOffset_tmp) );
		scanf("%s", iOffset_tmp );
		dst = 0x00;
		ChrToHex( iOffset_tmp, &dst );
		oLength = dst;
		if( dst < 0x00 || dst > CH36x_IO_SPACE_RANGE )
		{
			printf("\tErr: range of length\n");
			setbuf( stdin, NULL );
			getc(stdin);
			break;
		}

		memset( oByte_block_io, '\0', sizeof(oByte_block_io) );
		retval = CH36xReadIoBlock( iAddr_block_io, oByte_block_io, oLength );

		if( retval == true )
		{
			printf("\tContents of Block:0x\n\t" );
			int i = 0;
			for( i= 0; i < oLength; i ++ )
			/*for( i= 0; i < strlen((char *)oByte_block_io); i ++ )*/ //Note: When read '\0', error will oucr
			{
				if( (i % 8) == 0 && i != 0 && (i % 16) != 0 )
					printf("--- ");
				if( ((i) % 16) == 0 ) 
					printf("\n\t");
				printf("%02x ", (unsigned int)oByte_block_io[i] );
			}
			printf("\n\t-----------------------Next-------------------------\n");
		}
		else if( retval == false )
		{
			printf("\t==>[Read Block Error]\n");
			getc(stdin);
			/*sleep(3);*/
			break;
		}
		
		char	get_continue;
		printf("\t-->Continue Read? (Y/N):");
		scanf(" %c", &get_continue );
		if( get_continue == 'Y' || get_continue == 'y' )
			continue;
		else if( get_continue == 'N' || get_continue == 'n' )
			break;
		else
		{
			printf("\tErr: Invalid input, Will exit\n");
			break;
		}
	}


	return;
}

/*
 * ********************************************************************
 * write_io_block()
 *
 * FUNCTION : write io through block write 
 * 	-->block : CH36xWriteIoBlock()
 * ********************************************************************
 */
void write_io_block( void )
{
	char iOffset_tmp[20] = {'\0'};
	//char get_chose_type;

	unsigned int dst = 0x00;
	PVOID	iAddr_block_io = NULL; 
	ULONG	iLength = 0;
	PUCHAR	iByte_block_io = (PUCHAR)malloc( sizeof(UCHAR)*1024 );

	system("clear");
	printf("\t\t********WCH CH36x CHIPS Debug********\n");
	printf("\t------------Write a Block into I/O Space-------------\n");
	while(1)
	{
		printf("\t-->Input iOffset:0x");
		setbuf( stdin, NULL );
		memset( iOffset_tmp, '\0', sizeof(iOffset_tmp) );
		scanf("%s", iOffset_tmp );
		dst = 0x00;
		ChrToHex( iOffset_tmp, &dst );
		if( dst < 0x00 || dst > CH36x_IO_SPACE_RANGE )
		{
			printf("\tErr: range of length\n");
			setbuf( stdin, NULL );
			getc(stdin);
			break;
		}
		iAddr_block_io = ((char *)oIoBaseAddr_IO) + dst;

		printf("\t-->Input length:0x");
		setbuf( stdin, NULL );
		memset( iOffset_tmp, '\0', sizeof(iOffset_tmp) );
		scanf("%s", iOffset_tmp );
		dst = 0x00;
		ChrToHex( iOffset_tmp, &dst );
		iLength = dst;
		if( dst < 0x00 || dst > CH36x_IO_SPACE_RANGE )
		{
			printf("\tErr: range of length\n");
			setbuf( stdin, NULL );
			getc(stdin);
			break;
		}

		printf("\t-->Input a string:");
		setbuf( stdin, NULL );
		memset( iByte_block_io, '\0', sizeof(iByte_block_io) );
		scanf("%s", iByte_block_io );

		int retval = 0;
		retval = CH36xWriteIoBlock( iAddr_block_io, iByte_block_io, iLength );
		if( retval == true )
			printf("\t==>[Write Block into I/O Space, Success]\n\t-----------------------Next-------------------------\n" );
		else if( retval == false )
		{
			printf("\t==>[Write Block into I/O Space, Error]\n");
			getc(stdin);
			/*sleep(3);*/
			break;
		}

		char	get_continue;
		printf("\t-->Continue Write? (Y/N):");
		scanf(" %c", &get_continue );
		if( get_continue == 'Y' || get_continue == 'y' )
			continue;
		else if( get_continue == 'N' || get_continue == 'n' )
			break;
		else
		{
			printf("\tErr: Invalid input, Will exit\n");
			break;
		}
	}

	return;
}

/*
 * ********************************************************************
 * read_io( void )
 *
 * FUNCTION : read io through byte/word/dword read 
 * 	-->byte : CH36xReadIoByte()
 *	-->word : CH36xReadIoWord()
 *	-->dword: CH36xReadIoDword()
 * ********************************************************************
 */
void read_io( void )
{
	int retval = 0;
	char get_chose_type;
	//int exit_quit = 0;
	char iOffset_tmp[20] = {'\0'};

	unsigned int dst = 0x00;
	PVOID iAddr = NULL;
	PUCHAR	oByte_byte = (PUCHAR)malloc( sizeof(UCHAR) );
	PUSHORT	oByte_word = (PUSHORT)malloc( sizeof(USHORT) );
	PULONG	oByte_dword = (PULONG)malloc( sizeof(ULONG) );
	char str_read[20] = {'\0'};

	system("clear");
	printf("\t\t********WCH CH36x CHIPS Debug********\n");
	printf("\t------------------Read I/O Space--------------------\n");
	while(1)
	{
		printf("\t-->Input iOffset:0x" );
		setbuf( stdin, NULL );
		memset( iOffset_tmp, '\0', sizeof(iOffset_tmp) );
		scanf("%s", iOffset_tmp );
#if 0
//#ifdef CH36x_DEMO_CMD_DEBUG
		printf("--DEBUG-- iOffset:%s\n", iOffset_tmp );
#endif
		dst = 0x00; //Note: must set dst by 0x00 
		ChrToHex( iOffset_tmp, &dst );
#if 0
//#ifdef CH36x_DEMO_CMD_DEBUG
		printf("--DEBUG-- dst:0x%x\n", dst );
#endif
		iAddr = ((char *)oIoBaseAddr_IO) + dst;
#if 0
//#ifdef CH36x_DEMO_CMD_DEBUG
		printf("--DEBUG-- iAddr:%p\n", iAddr );
#endif
		printf("\tChoose Read size( 1:byte 2:word 3:Dword )\n");
		printf("\t-->Input Size:");
		scanf(" %c", &get_chose_type );

		memset( str_read, '\0', sizeof(str_read) );
		switch( get_chose_type )
		{
			case '1':
				{
					retval = CH36xReadIoByte( iAddr, oByte_byte );
					HexToChr( str_read, (unsigned int)(*oByte_byte) );
					break;
				}
			case '2':
				{
					retval = CH36xReadIoWord( iAddr, oByte_word );
					HexToChr( str_read, (unsigned int)(*oByte_word) );
					break;
				}
			case '3':
				{
					retval = CH36xReadIoDword( iAddr, oByte_dword );
					HexToChr( str_read, (unsigned int)(*oByte_dword) );
					break;
				}
			default:
				{
					printf("\tInput error, Enter again!\n");
					getc(stdin);
					/*sleep(2);*/
					break;
				}
		}

		if( retval == true )
			printf("\t==>[Value:0x%s]\n\t-----------------------Next-------------------------\n", str_read );
		else if( retval == false )
		{
			printf("\t==>[Read Error]\n" );
			getc(stdin);
			/*sleep(3);*/
			break;
		}

		char	get_continue;
		printf("\t-->Continue Read? (Y/N):");
		scanf(" %c", &get_continue );
		if( get_continue == 'Y' || get_continue == 'y' )
			continue;
		else if( get_continue == 'N' || get_continue == 'n' )
			break;
		else
		{
			printf("\tErr: Invalid input, Will exit\n");
			break;
		}

	}

	return;

}

/*
 * ********************************************************************
 * write_io( void )
 *
 * FUNCTION : write io through byte/word/dword write 
 * 	-->byte : CH36xWriteIoByte()
 *	-->word : CH36xWriteIoWord()
 *	-->dword: CH36xWriteIoDword()
 * ********************************************************************
 */
void write_io( void )
{
	int retval = 0;
	char get_chose_type;
	//int exit_quit = 0;
	char iOffset_tmp[20] = {'\0'};
	char input_value[20] = {'\0'};

	unsigned int dst = 0x00;
	PVOID iAddr = NULL;
	UCHAR	iByte_byte = 0x00;
	USHORT	iByte_word = 0x00;
	ULONG	iByte_dword = 0x00;
	//char str_read[20] = {'\0'};

	system("clear"); 		//clear console window
	printf("\t\t********WCH CH36x CHIPS Debug********\n");
	printf("\t------------------Write I/O Space--------------------\n");
	while(1)
	{
		printf("\t-->Input iOffset:0x");
		setbuf( stdin, NULL );
		memset( iOffset_tmp, '\0', sizeof(iOffset_tmp) );
		scanf("%s", iOffset_tmp );

		dst = 0x00;
		ChrToHex( iOffset_tmp, &dst );
		iAddr = ((char *)oIoBaseAddr_IO) + dst;
		printf("\tChoose Read size( 1:byte 2:word 3:Dword )\n");
		printf("\t-->Input Size:");
		scanf(" %c", &get_chose_type );

		memset( input_value, '\0', sizeof(input_value) );
		unsigned int tmp = 0x00;
		switch( get_chose_type )
		{
			case '1':
				{
					printf("\t-->Input value:0x");
					setbuf( stdin, NULL );
					scanf("%s", input_value );
					ChrToHex( input_value, &tmp );
					iByte_byte = (UCHAR)tmp;
					retval = CH36xWriteIoByte( iAddr, iByte_byte );
					break;
				}
			case '2':
				{
					printf("\t-->Input value:0x");
					setbuf( stdin, NULL );
					scanf("%s", input_value );
					ChrToHex( input_value, &tmp );
					iByte_word = (USHORT)tmp;
					retval = CH36xWriteIoWord( iAddr, iByte_word );
					break;
				}
			case '3':
				{
					printf("\t-->Input value:0x");
					setbuf( stdin, NULL );
					scanf("%s", input_value );
					ChrToHex( input_value, &tmp );
					iByte_dword = (ULONG)tmp;
					retval = CH36xWriteIoDword( iAddr, iByte_dword );
					break;
				}
			default:
				{
					printf("\tInput error, Enter again!\n");
					getc(stdin);
					/*sleep(2);*/
					break;
				}
		}

		if( retval == true )
			printf("\t==>[Write Success]\n\t-----------------------Next-------------------------\n" );
		else if( retval == false )
		{
			printf("\t==>[Write Error]\n" );
			getc(stdin);
			/*sleep(3);*/
			break;
		}
		
		char	get_continue;
		printf("\t-->Continue Write? (Y/N):");
		scanf(" %c", &get_continue );
		if( get_continue == 'Y' || get_continue == 'y' )
			continue;
		else if( get_continue == 'N' || get_continue == 'n' )
			break;
		else
		{
			printf("\tErr: Invalid input, Will exit\n");
			break;
		}
	}

	return;
}

/*
 * ********************************************************************
 * read_write_mem( void )
 *
 * FUNCTION : read/write mem 
 * 
 * ********************************************************************
 */
void read_write_mem( void )
{
	char get_mem_oper_chose;
	int exit_flag = 0;
	while(1)
	{
		system("clear");
		printf("\t\t********WCH CH36x CHIPS Debug********\n");
		printf("\t--------------Read/Write [ MEM ] Space--------------\n");
		printf("\t--> [1].Read\n");
		printf("\t--> [2].Write\n");
		printf("\t--> [3].Read a Block\n");
		printf("\t--> [4].Write a Block\n");
		printf("\t--> [0].Quit\n");
		printf("\tEnter choose:");
		scanf(" %c", &get_mem_oper_chose );

		switch( get_mem_oper_chose )
		{
			case '1':
				{
					read_mem();
					break;
				}
			case '2':
				{
					write_mem();
					break;
				}
			case '3':
				{
					read_mem_block();
					break;
				}
			case '4':
				{
					write_mem_block();
					break;
				}
			case '0':
				{
					exit_flag = 1;
					break;
				}
			default:
				{
					printf("\tChose error, Enter again!\n");
					getc(stdin);
					/*sleep(2);*/
					break;
					//continue;
				}
		}
		if( 1 == exit_flag )
			break;
	}

	return;
}

/*
 * ********************************************************************
 * read_mem_block()
 *
 * FUNCTION : read mem through block read 
 * 	-->block : CH36xReadMemBlock()
 * ********************************************************************
 */
void read_mem_block()
{
	int retval = 0;
	char iOffset_tmp[20] = {'\0'};
	//char  get_oLength;
	//char  get_chose_type;

	unsigned int dst = 0x00;
	PVOID	iAddr_block_mem = NULL;
	ULONG	oLength = 0;
	PUCHAR	oByte_block_mem = (PUCHAR)malloc( sizeof(UCHAR)*0x8000 );
	//PUCHAR	oByte_block_mem = (PUCHAR)malloc( sizeof(UCHAR)*oLength );

	system("clear");
	printf("\t\t********WCH CH36x CHIPS Debug********\n");
	printf("\t-----------Read a Block from Memory Space-----------\n");
	while(1)
	{
		printf("\t-->Input iOffset:0x" );
		setbuf( stdin, NULL );
		memset( iOffset_tmp, '\0', sizeof(iOffset_tmp) );
		scanf("%s", iOffset_tmp );

		dst = 0x00;
		ChrToHex( iOffset_tmp, &dst );
		if( dst < 0x00 || dst > CH36x_MEM_SPACE_RANGE )
		{
			printf("\tErr: range of iOffset\n");
			setbuf( stdin, NULL );
			getc(stdin);
			break;
		}
		iAddr_block_mem = ((char *)oMemBaseAddr_MEM) + dst;

		printf("\t-->Input length:0x");
		setbuf( stdin, NULL );
		memset( iOffset_tmp, '\0', sizeof(iOffset_tmp) );
		scanf("%s", iOffset_tmp );
		dst = 0x00;
		ChrToHex( iOffset_tmp, &dst );
		oLength = dst;
		if( dst < 0x00 || dst > CH36x_MEM_SPACE_RANGE )
		{
			printf("\tErr: range of length\n");
			setbuf( stdin, NULL );
			getc(stdin);
			break;
		}

		memset( oByte_block_mem, '\0', sizeof(oByte_block_mem) );
		retval = CH36xReadMemBlock( iAddr_block_mem, oByte_block_mem, oLength );
		
		if( retval == true )
		{
			printf("\tContents of Block:0x\n\t" );
			int i = 0;
			for( i= 0; i < oLength; i ++ )
			/*for( i= 0; i < strlen((char *)oByte_block_mem); i ++ )*/
			{
				if( (i % 8) == 0 && (i != 0) && ( (i % 16) != 0 ) )
					printf("--- ");
				if( ((i) % 16) == 0 ) 
					printf("\n\t");
				printf("%02x ", (unsigned int)oByte_block_mem[i] );
			}
			printf("\n\t-----------------------Next-------------------------\n");
		}
		else if( retval == false )
		{
			printf("\t==>[Read Block Error]\n");
			getc(stdin);
			/*sleep(3);*/
			break;
		}

		char	get_continue;
		printf("\t-->Continue Read? (Y/N):");
		scanf(" %c", &get_continue );
		if( get_continue == 'Y' || get_continue == 'y' )
			continue;
		else if( get_continue == 'N' || get_continue == 'n' )
			break;
		else
		{
			printf("\tErr: Invalid input, Will exit\n");
			break;
		}
	}

	return;
}

/*
 * ********************************************************************
 * write_mem_block()
 *
 * FUNCTION : write mem through block write 
 * 	-->block : CH36xWriteMemBlock()
 * ********************************************************************
 */
void write_mem_block()
{
	char iOffset_tmp[20] = {'\0'};	
	//char write_tmp[1024] = {'\0'};
	//char get_oLength;
	//char get_chose_type;

	unsigned int dst = 0x00;
	PVOID	iAddr_block_mem = NULL;
	ULONG	iLength = 0;
	PUCHAR	iByte_block_mem = (PUCHAR)malloc( sizeof(UCHAR)*1024 );

	system("clear");
	printf("\t\t********WCH CH36x CHIPS Debug********\n");
	printf("\t-----------Write a Block into Memory Space-----------\n");
	while(1)
	{
		printf("\t-->Input iOffset:0x");
		setbuf( stdin, NULL );
		memset( iOffset_tmp, '\0', sizeof(iOffset_tmp) );
		scanf("%s", iOffset_tmp );
		dst = 0x00;
		ChrToHex( iOffset_tmp, &dst );
		if( dst < 0x00 || dst > CH36x_MEM_SPACE_RANGE )
		{
			printf("\tErr: range of length\n");
			setbuf( stdin, NULL );
			getc(stdin);
			break;
		}
		// Get Write Addr
		iAddr_block_mem = ((char *)oMemBaseAddr_MEM) + dst; 

		printf("\t-->Input length:0x");
		setbuf( stdin, NULL );
		memset( iOffset_tmp, '\0', sizeof(iOffset_tmp) );
		scanf("%s", iOffset_tmp );
		dst = 0x00;
		ChrToHex( iOffset_tmp, &dst );
		iLength = dst;
		if( dst < 0x00 || dst > CH36x_MEM_SPACE_RANGE )
		{
			printf("\tErr: range of length\n");
			setbuf( stdin, NULL );
			getc(stdin);
			break;
		}

		printf("\t-->Input a string:");
		setbuf( stdin, NULL );
		memset( iByte_block_mem, '\0', sizeof(iByte_block_mem) );
		scanf("%s", iByte_block_mem );

		int retval = 0;
		retval = CH36xWriteMemBlock( iAddr_block_mem, iByte_block_mem, iLength );
		if( retval == true )
			printf("\t==>[Write Block into MEM Space, Success]\n\t-----------------------Next-------------------------\n" );
		else if( retval == false )
		{
			printf("\t==>[Write Block into Memory Sapce, Error]\n");
			getc(stdin);
			/*sleep(3);*/
			break;
		}	

		char	get_continue;
		printf("\t-->Continue Write? (Y/N):");
		scanf(" %c", &get_continue );
		if( get_continue == 'Y' || get_continue == 'y' )
			continue;
		else if( get_continue == 'N' || get_continue == 'n' )
			break;
		else
		{
			printf("\tErr: Invalid input, Will exit\n");
			break;
		}

	}

	return;
}

/*
 * ********************************************************************
 * read_mem( void )
 *
 * FUNCTION : read mem through byte/word/dword read 
 * 	-->byte : CH36xReadMemByte()
 *	-->word : CH36xReadMemWord()
 *	-->dword: CH36xReadMemDword()
 * ********************************************************************
 */
void read_mem( void )
{	
	int retval = 0;
	char get_chose_type;
	//int exit_quit = 0;
	char iOffset_tmp[20] = {'\0'};

	unsigned int dst = 0x00;
	PVOID iAddr = NULL;
	PUCHAR	oByte_byte = (PUCHAR)malloc( sizeof(UCHAR) );
	PUSHORT	oByte_word = (PUSHORT)malloc( sizeof(USHORT) );
	PULONG	oByte_dword = (PULONG)malloc( sizeof(ULONG) );
	char str_read[20] = {'\0'};

	system("clear");
	printf("\t\t********WCH CH36x CHIPS Debug********\n");
	printf("\t-----------------Read Memory Space------------------\n");
	while(1)
	{
		printf("\t-->Input iOffset:0x" );
		setbuf( stdin, NULL );
		memset( iOffset_tmp, '\0', sizeof(iOffset_tmp) );
		scanf("%s", iOffset_tmp );
#if 0
//#ifdef CH36x_DEMO_CMD_DEBUG
		printf("--DEBUG-- iOffset:%s\n", iOffset_tmp );
#endif
		dst = 0x00;
		ChrToHex( iOffset_tmp, &dst );
#if 0
//#ifdef CH36x_DEMO_CMD_DEBUG
		printf("--DEBUG-- dst:0x%x\n", dst );
#endif
		iAddr = ((char *)oMemBaseAddr_MEM) + dst;
		printf("\tChoose Read size( 1:byte 2:word 3:Dword )\n");
		printf("\t-->Input Size:");
		scanf(" %c", &get_chose_type );

		memset( str_read, '\0', sizeof(str_read) );
		switch( get_chose_type )
		{
			case '1':
				{
					retval = CH36xReadMemByte( iAddr, oByte_byte );
					HexToChr( str_read, (unsigned int)(*oByte_byte) );
					break;
				}
			case '2':
				{
					retval = CH36xReadMemWord( iAddr, oByte_word );
					HexToChr( str_read, (unsigned int)(*oByte_word) );
					break;
				}
			case '3':
				{
					retval = CH36xReadMemDword( iAddr, oByte_dword );
					HexToChr( str_read, (unsigned int)(*oByte_dword) );
					break;
				}
			default:
				{
					printf("\tInput error, Enter again!\n");
					getc(stdin);
					/*sleep(2);*/
					break;
				}
		}

		if( retval == true )
			printf("\t==>[Value:0x%s]\n\t-----------------------Next-------------------------\n", str_read );
		else if( retval == false )
		{
			printf("\t==>[Read Error]\n" );
			getc(stdin);
			/*sleep(3);*/
			break;
		}

		char	get_continue;
		printf("\t-->Continue Read? (Y/N):");
		scanf(" %c", &get_continue );
		if( get_continue == 'Y' || get_continue == 'y' )
			continue;
		else if( get_continue == 'N' || get_continue == 'n' )
			break;
		else
		{
			printf("\tErr: Invalid input, Will exit\n");
			break;
		}

	}

	return;


}

/*
 * ********************************************************************
 * write_mem( void )
 *
 * FUNCTION : write mem through byte/word/dword write 
 * 	-->byte : CH36xWriteMemByte()
 *	-->word : CH36xWriteMemWord()
 *	-->dword: CH36xWriteMemDword()
 * ********************************************************************
 */
void write_mem( void )
{
	int retval = 0;
	char get_chose_type;
	//int exit_quit = 0;
	char iOffset_tmp[20] = {'\0'};
	char input_value[20] = {'\0'};

	unsigned int dst = 0x00;
	PVOID iAddr = NULL;
	UCHAR	iByte_byte = 0x00;
	USHORT	iByte_word = 0x00;
	ULONG	iByte_dword = 0x00;
	//char str_read[20] = {'\0'};

	system("clear");
	printf("\t\t********WCH CH36x CHIPS Debug********\n");
	printf("\t------------------Write MEM Space--------------------\n");
	while(1)
	{
		printf("\t-->Input iOffset:0x");
		setbuf( stdin, NULL );
		memset( iOffset_tmp, '\0', sizeof(iOffset_tmp) );
		scanf("%s", iOffset_tmp );

		dst = 0x00;
		ChrToHex( iOffset_tmp, &dst );
		iAddr = ((char *)oMemBaseAddr_MEM) + dst;
#if 0
//#ifdef CH36x_DEMO_CMD_DEBUG
		printf("--DEBUG-- iAddr:%p\n", iAddr );
#endif
		printf("\tChoose Read size( 1:byte 2:word 3:Dword )\n");
		printf("\t-->Input Size:");
		scanf(" %c", &get_chose_type );

		memset( input_value, '\0', sizeof(input_value) );
		unsigned int tmp = 0x00;
		switch( get_chose_type )
		{
			case '1':
				{
					printf("\t-->Input value:0x");
					setbuf( stdin, NULL );//attach NULL to the standard input stream
					scanf("%s", input_value );
					ChrToHex( input_value, &tmp );
					iByte_byte = (UCHAR)tmp;
					retval = CH36xWriteMemByte( iAddr, iByte_byte );
					break;
				}
			case '2':
				{
					printf("\t-->Input value:0x");
					setbuf( stdin, NULL );
					scanf("%s", input_value );
					ChrToHex( input_value, &tmp );
					iByte_word = (USHORT)tmp;
					retval = CH36xWriteMemWord( iAddr, iByte_word );
					break;
				}
			case '3':
				{
					printf("\t-->Input value:0x");
					setbuf( stdin, NULL );
					scanf("%s", input_value );
					ChrToHex( input_value, &tmp );
					iByte_dword = (ULONG)tmp;
					retval = CH36xWriteMemDword( iAddr, iByte_dword );
					break;
				}
			default:
				{
					printf("\tInput error, Enter again!\n");
					getc(stdin);
					/*sleep(2);*/
					break;
				}

		}
#if 0
//#ifdef CH36x_DEMO_CMD_DEBUG
		printf("--DEBUG-- tmp:0x%x\n", (unsigned int)tmp );
#endif

		if( retval == true )
			printf("\t==>[Write Success]\n\t-----------------------Next-------------------------\n" );
		else if( retval == false )
		{
			printf("\t==>[Write Error]\n");
			getc(stdin);
			/*sleep(3);*/
			break;
		}

		char	get_continue;
		printf("\t-->Continue Write? (Y/N):");
		scanf(" %c", &get_continue );
		if( get_continue == 'Y' || get_continue == 'y' )
			continue;
		else if( get_continue == 'N' || get_continue == 'n' )
			break;
		else
		{
			printf("\tErr: Invalid input, Will exit\n");
			break;
		}

	}

	return;
}

/*
 * ********************************************************************
 * int get_ch36x_dev_list( void )
 *
 * FUNCTION : Enum 0~ch36x_dev_num existed device,keep result into 
 *  ch36x_dev_list.
 * ********************************************************************
 */
static int get_ch36x_dev_list( void )
{
	char dev_path[20] = {'\0'};
	int i = 0;

	memset( dev_path, '\0', sizeof(dev_path) );
	memset( ch36x_dev_list, '\0', sizeof(ch36x_dev_list) );

	for( i = 0; i < ch36x_dev_num; i++ )
	{
		sprintf( dev_path, "%s%c", ch36x_dev_path, '0'+i );
		/*printf("dev_path:%s\n", dev_path);*/
		dev_fd = open( dev_path, O_RDWR );
		if( dev_fd == -1 )
		{
			break;
		}
//#ifdef CH36x_DEMO_CMD_DEBUG
#if 0
		printf("line:%d dev_fd:%d i:%d CH36xCloseDevice() execute\n", __LINE__, dev_fd, i );
#endif
	
		CH36xCloseDevice();	//extern int dev_fd
		//close( dev_fd ); //Note: here
	}
	if( 0 == i )
	{
		sprintf( ch36x_dev_list, "\tErr:Can't find any devices" );
		getc(stdin);
		/*sleep(2);*/
		return -1;
	}

	/*printf("\t-------------Current Opened dev Num:[%c]------------\n", get_chose_main );*/
	/*sprintf( ch36x_dev_list, "\t\tCurrent devices number: %d", i );*/
	sprintf( ch36x_dev_list, "\t-------------Current devices number:[%d]------------", i );

	return 0;
}

/*
 * ********************************************************************
 * ChrToHex( const char *str, unsigned int *dst )
 *
 * FUNCTION : char ------> hex
 * arg:
 * str     : char number
 * dst     : hex number
 * ********************************************************************
 */
void ChrToHex( const char *str, unsigned int *dst )
{
	int str_len = strlen( str );
	int i = 0;

	for( i = 0; i < str_len; i ++ )
	{
		if( *(str + i) >= '0' && *(str + i) <= '9' )
		{
			if( 0 != i )
				*dst = (*dst << 4);
			*dst += *(str + i) - '0';
			//printf("*dst:0x%x\n", *dst);
		}
		else if( *(str + i) >= 'a' && *(str + i) <= 'f' ) 
		{
			if( 0 != i )
				*dst = (*dst << 4);
			*dst += *(str + i) - 'a' + 10;
			//printf("*dst:0x%x\n", *dst);
		}
		else if( *(str + i) >= 'A' && *(str + i ) <= 'F' )  
		{
			if( 0 != i )
				*dst = (*dst << 4);
			*dst += *(str + i) - 'A' + 10;
			//printf("*dst:0x%x\n", *dst);
		}
	}

	return;
}

/*
 * ********************************************************************
 * HexToChr( char *str, unsigned int dst )
 *
 * FUNCTION : hex ------> char
 * arg:
 * str     : char number
 * dst     : hex number
 * ********************************************************************
 */
void HexToChr( char *str, unsigned int dst )
{
	char tmp[10] = {'\0'};
	int  i = 0;
	int  j = 0;

	memset( tmp, '\0', sizeof(tmp) );
	while(1)
	{
		if( 0x00 == dst )
			break;
		if( (dst & 0xf) >= 0 && (dst & 0xf) <= 9 )
			*(tmp+(i++)) = (dst & 0xf) + '0';	
		else
			*(tmp+(i++)) = (dst & 0xf) - 10 + 'a';
			//*(tmp+i) = (dst & 0xf) - 10 + 'A';
		dst = dst >> 4;
	}
	//printf( "tmp:%s\n", tmp);
	for( j = 0; j < i; j ++ )
		*(str+j) = *( tmp+(i-1-j) );
	//printf( "str:%s\n", str);

	return;
}




