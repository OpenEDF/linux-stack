/*
 *ch36x_demo_GUI.c for ch365/ch367/ch368 pci card of WCH
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
 * This file is used for testing I/O\MEM\Configuration\ (Read and Write). 
 * It use gtk+ to develop Interface Program
 */
#include <gtk/gtk.h>
#include <gdk/gdk.h>
#include <glib.h>
#include <glib/gprintf.h>
#include <gdk/gdkkeysyms.h>

#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include "ch36x_lib.h"

#define debug 1

extern int dev_fd;

GtkWidget *window_main;
GtkWidget *window_sub;
static int  gtk_dev_num = 0x10;				// Max Device number
static char gtk_dev_list[2048];				// Scan device list
static char *gtk_dev_path = "/dev/ch36xpci";		// Device Path
//static char *gtk_dev_path = "/tmp/ch36xpci";

GtkWidget *entry_sub_CFG_offset;
static int on_CFG_radio_type = 0;
GtkWidget *entry_sub_IO_addr;
static int on_IO_radio_type = 0;
GtkWidget *entry_sub_MEM_addr;
static int on_MEM_radio_type = 0;

mPCH36x_IO_REG	oIoBaseAddr_IO; //= (mPCH36x_IO_REG)malloc( sizeof(mCH36x_IO_REG) );
mPCH36x_MEM_REG oMemBaseAddr_MEM; //= (mPCH36x_MEM_REG)malloc( sizeof(mCH36x_MEM_REG) );

GtkWidget *create_sub_window();

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

/*
 * ********************************************************************
 * get_ch36x_dev_list()
 *
 * FUNCTION : get device list
 * ********************************************************************
 */
static int get_ch36x_dev_list()
{
	char dev_path[20] = {'\0'};
	//int  dev_fd = 0;
	int  i = 0;
	//unsigned int Vendor_ID = 0x00;
	//unsigned int Device_ID = 0x00;

	memset( dev_path, '\0', sizeof(dev_path) );
	memset( gtk_dev_list, '\0', sizeof(gtk_dev_list) );

	for( i = 0; i < gtk_dev_num; i++ )
	{
		sprintf( dev_path, "%s%c", gtk_dev_path, '0'+i );
		dev_fd = open( dev_path, O_RDWR );
		if( dev_fd == -1 )
		{
			/*CH36xCloseDevice();*/
			break;
		}
		/*printf("line:%d dev_fd:%d i:%d CH36xCloseDevice() execute\n", __LINE__, dev_fd, i );*/
		CH36xCloseDevice(); //????
		/*close( dev_fd ); //choose this way to close CH36x device*/
	}
	
	sprintf( gtk_dev_list, "Current devices number: %d", i );
#ifdef DEBUG
	/*printf( "g_dev_list:%s\n", g_dev_list );*/
#endif

	return 0;
	//return -1;
}

/*
 * ********************************************************************
 * open_choosed_dev( GtkWidget *entry )
 *
 * FUNCTION : select wanted device
 * arg:
 * entry     : device number
 * ********************************************************************
 */
static int open_choosed_dev( GtkWidget *entry )
{
	char dev_path[20] = {'\0'};

	memset( dev_path, '\0', sizeof(dev_path) );
	//sprintf( dev_path, "%s%s", gtk_dev_path, gtk_entry_get_text(GTK_ENTRY(entry)) );
	sprintf( dev_path, "%s%c", gtk_dev_path, *(gtk_entry_get_text(GTK_ENTRY(entry)))-1 );
	dev_fd = open( dev_path, O_RDWR );
	if( dev_fd == -1 )
	{
		printf("open %s error\n", dev_path );
		return -1;
	}

	return 0;
}

/*
 * ********************************************************************
 * get_ch36x_dev_info( GtkWidget *entry )
 *
 * FUNCTION : Get input number of device existing
 * arg:
 * entry     : device number
 * ********************************************************************
 */
static const char *get_ch36x_dev_info( GtkWidget *entry )
{
	const char *dev_num;

	dev_num = gtk_entry_get_text( GTK_ENTRY(entry) );
	return dev_num;
}

/*
 * ********************************************************************
 * on_CFG_radio_clicked( GtkWidget *radio, gint data )
 *
 * FUNCTION : release configuration radio button to callback this function
 * arg:
 * data     : trigger signal arg
 * ********************************************************************
 */
void on_CFG_radio_clicked( GtkWidget *radio, gint data )
{
	switch( (int)data )
	{
		case 1:
			{
				/*printf(" on_CFG_radio_clicked case 1\n" );*/
				on_CFG_radio_type = 1;
				break;
			}
		case 2:
			{
				/*printf(" on_CFG_radio_clicked case 2\n" );*/
				on_CFG_radio_type = 2;
				break;
			}
		case 3:
			{
				/*printf(" on_CFG_radio_clicked case 3\n" );*/
				on_CFG_radio_type = 3;
				break;
			}
	}
}

/*
 * ********************************************************************
 * on_IO_radio_clicked( GtkWidget *radio, gint data )
 *
 * FUNCTION : release I/O radio button to callback this function
 * arg:
 * data     : trigger signal arg
 * ********************************************************************
 */
void on_IO_radio_clicked( GtkWidget *radio, gint data )
{
	switch( (int)data )
	{
		case 1:
			{
				on_IO_radio_type = 1;
				break;
			}
		case 2:
			{
				on_IO_radio_type = 2;
				break;
			}
		case 3:
			{
				on_IO_radio_type = 3;
				break;
			}
	}
}

/*
 * ********************************************************************
 * on_MEM_radio_clicked( GtkWidget *radio, gint data )
 *
 * FUNCTION : release mem radio button to callback this function
 * arg:
 * data     : trigger signal arg
 * ********************************************************************
 */
void on_MEM_radio_clicked( GtkWidget *radio, gint data )
{
	switch( (int)data )
	{
		case 1:
			{
				on_MEM_radio_type = 1;
				break;
			}
		case 2:
			{
				on_MEM_radio_type = 2;
				break;
			}
		case 3:
			{
				on_MEM_radio_type = 3;
				break;
			}
		default:
			{
				on_MEM_radio_type = 0;
			}
	}
}

/*
 * ********************************************************************
 * gtk_CFG_read( GtkButton *button_sub_CFG_read, gpointer data )
 *
 * FUNCTION : Read configuration 
 * arg:
 * button_sub_CFG_read     : GtkButton
 * data 		   : window entry widget
 * ********************************************************************
 */
void gtk_CFG_read( GtkButton *button_sub_CFG_read, gpointer data )//GtkWidget *entry_sub_CFG_value )
{
	//char offset[20] = {'\0'};
	const char *offset;
	offset = gtk_entry_get_text( GTK_ENTRY(entry_sub_CFG_offset) );
	//printf("entry_sub_CFG_offset:%s\n", offset );
	
	unsigned int dst = 0x00;
	PVOID 	iOffset;
	PUCHAR	oByte_byte = (PUCHAR)malloc( sizeof(UCHAR) );
	PUSHORT	oByte_word = (PUSHORT)malloc( sizeof(USHORT) );
	PULONG	oByte_dword = (PULONG)malloc( sizeof(ULONG) );

	ChrToHex( offset, &dst );
	iOffset = (PVOID)(dst);
	char str_read[20] = {'\0'};
	memset( str_read, '\0', sizeof(str_read) );

	/*printf( " gtk_CFG_read dst:0x%x, iOffset:%p\n", dst, iOffset );*/

	switch( on_CFG_radio_type )
	{
		case 1:
			{
				//read byte from config space
				CH36xReadConfig( iOffset, oByte_byte );
				HexToChr( str_read, (unsigned int)(*oByte_byte) );
				break;
			}
		case 2:
			{
				CH36xReadConfigWord( iOffset, oByte_word );
				HexToChr( str_read, (unsigned int)(*oByte_word) );
				break;
			}
		case 3:
			{
				CH36xReadConfigDword( iOffset, oByte_dword );
				HexToChr( str_read, (unsigned int)(*oByte_dword) );
				break;
			}
		default:
			{
				CH36xReadConfig( iOffset, oByte_byte );
				HexToChr( str_read, (unsigned int)(*oByte_byte) );
				break;
			}
	}
	gtk_entry_set_text( GTK_ENTRY( (GtkWidget *)data ), str_read );

	return;
}

/*
 * ********************************************************************
 * gtk_CFG_write( GtkButton *button_sub_CFG_write, gpointer data )
 *
 * FUNCTION : Write configuration 
 * arg:
 * button_sub_CFG_write    : GtkButton of Write
 * data 		   : window entry widget(input data)
 * ********************************************************************
 */
void gtk_CFG_write( GtkButton *button_sub_CFG_write, gpointer data )//GtkWidget *entry_sub_CFG_value )
{
	const char *offset;
	offset = gtk_entry_get_text( GTK_ENTRY(entry_sub_CFG_offset) );
	//printf("entry_sub_CFG_offset:%s\n", 
	/*gtk_entry_set_text( GTK_ENTRY( (GtkWidget *)data ), offset );*/

	unsigned int dst = 0x00;
	PVOID	iOffset = NULL;
	UCHAR	iByte_byte = 0x00;
	USHORT	iByte_word = 0x00;
	ULONG	iByte_dword = 0x00;

	ChrToHex( offset, &dst );
	iOffset = (PVOID)(dst);
	char str_read[20] = {'\0'};
	memset( str_read, '\0', sizeof(str_read) );

	/*printf( "iOffset:%p\n", iOffset );*/
	/*printf( "gtk_CFG_write dst:0x%x\n", dst );*/

	const char *value;
	value = gtk_entry_get_text( GTK_ENTRY( (GtkWidget *)data ) );
	unsigned int tmp = 0x00;
	switch( on_CFG_radio_type )
	{
		case 1:
			{
				ChrToHex( value, &tmp );
				iByte_byte = (UCHAR)tmp;
				/*printf( "iByte_byte:0x%x\n", iByte_byte );*/
				CH36xWriteConfig( iOffset, iByte_byte );
				break;
			}
		case 2:
			{
				ChrToHex( value, &tmp );
				iByte_word = (USHORT)tmp;
				/*printf( "iByte_word:0x%x\n", iByte_word );*/
				CH36xWriteConfigWord( iOffset, iByte_word );
				break;
			}
		case 3:
			{
				ChrToHex( value, &tmp );
				iByte_dword = (ULONG)tmp;
				/*printf( "iByte_dword:0x%x\n", (unsigned int)iByte_dword );*/
				CH36xWriteConfigDword( iOffset, iByte_dword );
				break;
			}
		default:
			{
				ChrToHex( value, &tmp );
				iByte_byte = (UCHAR)tmp;
				/*printf( "iByte_byte:0x%x\n", iByte_byte );*/
				CH36xWriteConfig( iOffset, iByte_byte );
				break;
			}
	}

	return;
}

/*
 * ********************************************************************
 * gtk_IO_read( GtkButton *button_sub_IO_read, gpointer data )
 *
 * FUNCTION : Read I/O 
 * arg:
 * button_sub_IO_read     : Signal GtkButton
 * data 		  : window entry widget
 * ********************************************************************
 */
void gtk_IO_read( GtkButton *button_sub_IO_read, gpointer data )
{
	const char *offset;
	offset = gtk_entry_get_text( GTK_ENTRY(entry_sub_IO_addr) );

	unsigned int dst = 0x00;
	PVOID	iAddr = NULL;				
	PUCHAR	oByte_byte = (PUCHAR)malloc( sizeof(UCHAR) );
	PUSHORT	oByte_word = (PUSHORT)malloc( sizeof(USHORT) );
	PULONG	oByte_dword = (PULONG)malloc( sizeof(ULONG) );

	ChrToHex( offset, &dst );
	iAddr = ((char *)oIoBaseAddr_IO) + dst;
	char str_read[20] = {'\0'};
	memset( str_read, '\0', sizeof(str_read) );

	/*printf( "oIoBaseAddr:%p\n", oIoBaseAddr_IO );*/
	/*printf( " gtk_IO_read dst:0x%x, iAddr:%p\n", dst, iAddr );*/

	switch( on_IO_radio_type )
	{
		case 1:
			{
				CH36xReadIoByte( iAddr, oByte_byte );
				HexToChr( str_read, (unsigned int)(*oByte_byte) );
				break;
			}
		case 2:
			{
				CH36xReadIoWord( iAddr, oByte_word );
				HexToChr( str_read, (unsigned int)(*oByte_word) );
				break;
			}
		case 3:
			{
				CH36xReadIoDword( iAddr, oByte_dword );
				HexToChr( str_read, (unsigned int)(*oByte_dword) );
				break;
			}
		default:
			{
				CH36xReadIoByte( iAddr, oByte_byte );
				HexToChr( str_read, (unsigned int)(*oByte_byte) );
				break;
			}
	}
	gtk_entry_set_text( GTK_ENTRY( (GtkWidget *)data ), str_read );

	return;
}

/*
 * ********************************************************************
 * gtk_IO_write( GtkButton *button_sub_IO_write, gpointer data )
 *
 * FUNCTION : Write I/O 
 * arg:
 * button_sub_IO_write    : Signal GtkButton
 * data 		  : window entry widget
 * ********************************************************************
 */
void gtk_IO_write( GtkButton *button_sub_IO_write, gpointer data )
{
	const char *offset;
	offset = gtk_entry_get_text( GTK_ENTRY(entry_sub_IO_addr) );

	unsigned int dst = 0x00;
	PVOID	iAddr = NULL;
	UCHAR	iByte_byte = 0x00;
	USHORT	iByte_word = 0x00;
	ULONG	iByte_dword = 0x00;

	ChrToHex( offset, &dst );
	iAddr = ((char *)oIoBaseAddr_IO) + dst;
	char str_read[20] = {'\0'};
	memset( str_read, '\0', sizeof(str_read) );

	/*printf( "oIoBaseAddr:%p\n", oIoBaseAddr_IO );*/
	/*printf( "gtk_IO_write dst:0x%x, iAddr:%p\n", dst, iAddr );*/

	const char *value;
	value = gtk_entry_get_text( GTK_ENTRY( (GtkWidget *)data ));
	unsigned int tmp = 0x00;
	switch( on_IO_radio_type )
	{
		case 1:
			{
				ChrToHex( value, &tmp );
				iByte_byte = (UCHAR)tmp;
				CH36xWriteIoByte( iAddr, iByte_byte );
				break;
			}
		case 2:
			{
				ChrToHex( value, &tmp );
				iByte_word = (USHORT)tmp;
				CH36xWriteIoWord( iAddr, iByte_word );
				break;
			}
		case 3:
			{
				ChrToHex( value, &tmp );
				iByte_dword = (ULONG)tmp;
				CH36xWriteIoDword( iAddr, iByte_dword );
				break;
			}
		default:
			{
				ChrToHex( value, &tmp );
				iByte_byte = (UCHAR)tmp;
				CH36xWriteIoByte( iAddr, iByte_byte );
				break;
			}
	}

	return;
}

/*
 * ********************************************************************
 * gtk_MEM_read( GtkButton *button_sub_MEM_read, gpointer data )
 *
 * FUNCTION : Read MEM 
 * arg:
 * button_sub_IO_read     : Signal GtkButton
 * data 		  : window entry widget
 * ********************************************************************
 */
void gtk_MEM_read( GtkButton *button_sub_MEM_read, gpointer data )
{
	const char *offset;
	offset = gtk_entry_get_text( GTK_ENTRY(entry_sub_MEM_addr) );

	unsigned int dst = 0x00;
	PVOID	iAddr = NULL;
	PUCHAR	oByte_byte = (PUCHAR)malloc( sizeof(UCHAR) );
	PUSHORT	oByte_word = (PUSHORT)malloc( sizeof(USHORT) );
	PULONG	oByte_dword = (PULONG)malloc( sizeof(ULONG) );

	ChrToHex( offset, &dst );
	iAddr = ((char *)oMemBaseAddr_MEM) + dst;
	char str_read[20] = {'\0'};
	memset( str_read, '\0', sizeof(str_read) );

	/*printf( "oMemBaseAddr:%p\n", oMemBaseAddr_MEM );*/
	/*printf( " gtk_MEM_read dst:0x%x, iAddr:%p\n", dst, iAddr );*/

	switch( on_MEM_radio_type )
	{
		case 1:
			{
				CH36xReadMemByte( iAddr, oByte_byte );
				HexToChr( str_read, (unsigned int)(*oByte_byte) );
				break;
			}
		case 2:
			{
				CH36xReadMemWord( iAddr, oByte_word );
				HexToChr( str_read, (unsigned int)(*oByte_word) );
				break;
			}
		case 3:
			{
				CH36xReadMemDword( iAddr, oByte_dword );
				HexToChr( str_read, (unsigned int)(*oByte_dword) );
				break;
			}
		default:
			{
				CH36xReadMemByte( iAddr, oByte_byte );
				HexToChr( str_read, (unsigned int)(*oByte_byte) );
				break;
			}
	}
	gtk_entry_set_text( GTK_ENTRY( (GtkWidget *)data ), str_read );

	return;
}

/*
 * ********************************************************************
 * gtk_MEM_write( GtkButton *button_sub_MEM_write, gpointer data )
 *
 * FUNCTION : Write MEM 
 * arg:
 * button_sub_MEM_write   : Signal GtkButton
 * data 		  : window entry widget
 * ********************************************************************
 */
void gtk_MEM_write( GtkButton *button_sub_MEM_write, gpointer data )
{
	const char *offset = NULL;
	offset = gtk_entry_get_text( GTK_ENTRY(entry_sub_MEM_addr) );

	unsigned int dst = 0x00;
	PVOID	iAddr = NULL;
	UCHAR	iByte_byte = 0x00;
	USHORT	iByte_word = 0x00;
	ULONG	iByte_dword = 0x00;

	ChrToHex( offset, &dst );
	iAddr = ((char *)oMemBaseAddr_MEM) + dst;
	char str_read[20] = {'\0'};
	memset( str_read, '\0', sizeof(str_read) );

	/*printf( "---MEM Write---\n");*/
	/*printf( "oMemBaseAddr:%p\n", oMemBaseAddr_MEM );*/
	/*printf( "offset:%s\n", offset );*/
	/*printf( "gtk_MEM_write dst:0x%x, iAddr:%p\n", dst, iAddr );*/

	const char *value = NULL;
	value = gtk_entry_get_text( GTK_ENTRY( (GtkWidget *)data ) );
	/*printf( "value:%s\n", value );*/
	unsigned int tmp = 0x00;
	switch( on_MEM_radio_type )
	{
		case 1:
			{
				ChrToHex( value, &tmp );
				iByte_byte = (UCHAR)tmp;
				/*printf( "tmp:0x%x\n", (unsigned int)tmp );*/
				/*printf( "iByte_byte:0x%x\n", iByte_byte );*/
				CH36xWriteMemByte( iAddr, iByte_byte );
			}
		case 2:
			{
				ChrToHex( value, &tmp );
				iByte_word = (USHORT)tmp;
				/*printf( "tmp:0x%x\n", (unsigned int)tmp );*/
				/*printf( "iByte_word:0x%x\n", iByte_word );*/
				CH36xWriteMemWord( iAddr, iByte_word );
			}
		case 3:
			{
				ChrToHex( value, &tmp );
				iByte_dword = (ULONG)tmp;
				/*printf( "tmp:0x%x\n", (unsigned int)tmp );*/
				/*printf( "iByte_dword:0x%x\n", (unsigned int)iByte_dword );*/
				CH36xWriteMemDword( iAddr, iByte_dword );
			}
		default:
			{
				ChrToHex( value, &tmp );
				iByte_byte = (UCHAR)tmp;
				/*printf( "iByte_byte:0x%x\n", iByte_byte );*/
				CH36xWriteMemByte( iAddr, iByte_byte );
			}
	}

	return;
}

void gtk_show_sub( GtkButton *button, gpointer data )
{
	window_sub = create_sub_window( (GtkWidget *)data );
	//gtk_widget_show( window_sub );
}

void gtk_destroy_sub( GtkButton *button, gpointer data )
{
	gtk_widget_destroy( window_sub );
	CH36xCloseDevice();	//Note:here, this function must be need for reflush
}

void gtk_delete_sub( GtkWidget *window, GdkEvent *event, gpointer data )
{
	gtk_widget_destroy( window_sub );
}

/*
 * ********************************************************************
 * create_sub_window( GtkWidget *entry )
 *
 * FUNCTION : create sub window 
 * arg:
 * entry   : input sub window the device number
 *  -->if entry is equal to device,then open sub window
 * ********************************************************************
 */
GtkWidget *create_sub_window( GtkWidget *entry )
{
	GtkWidget *window1;
	/*GtkWidget *viewport;*/
	GtkWidget *frame;

	/*GtkWidget *label_dev_info;*/

	GtkWidget *vbox;

	//Config Space RD/WR
	GtkWidget *label_sub_CFG;
	GtkWidget *table_sub_CFG;
	GtkWidget *label_sub_CFG_read;
	GtkWidget *label_sub_CFG_write;

	GtkWidget *vbox_sub_CFG;
	GtkWidget *hbox_sub_CFG;

	//GtkWidget *entry_sub_CFG_offset;
	GtkWidget *entry_sub_CFG_value;

	GtkWidget *button_sub_CFG_read;
	GtkWidget *button_sub_CFG_write;

	GtkWidget *radio_sub_CFG_byte;
	GtkWidget *radio_sub_CFG_word;
	GtkWidget *radio_sub_CFG_dword;

	//IO Space RD/WR
	GtkWidget *label_sub_IO;
	GtkWidget *table_sub_IO;
	GtkWidget *label_sub_IO_read;
	GtkWidget *label_sub_IO_write;

	GtkWidget *vbox_sub_IO;
	GtkWidget *hbox_sub_IO;

	//GtkWidget *entry_sub_IO_offset;
	GtkWidget *entry_sub_IO_value;

	GtkWidget *button_sub_IO_read;
	GtkWidget *button_sub_IO_write;

	GtkWidget *radio_sub_IO_byte;
	GtkWidget *radio_sub_IO_word;
	GtkWidget *radio_sub_IO_dword;

	//MEM Space RD/WR
	GtkWidget *label_sub_MEM;
	GtkWidget *table_sub_MEM;
	GtkWidget *label_sub_MEM_read;
	GtkWidget *label_sub_MEM_write;

	GtkWidget *vbox_sub_MEM;
	GtkWidget *hbox_sub_MEM;

	//GtkWidget *entry_sub_MEM_offset;
	GtkWidget *entry_sub_MEM_value;

	GtkWidget *button_sub_MEM_read;
	GtkWidget *button_sub_MEM_write;

	GtkWidget *radio_sub_MEM_byte;
	GtkWidget *radio_sub_MEM_word;
	GtkWidget *radio_sub_MEM_dword;

//===============================================
	window1 = gtk_window_new( GTK_WINDOW_TOPLEVEL );
	g_signal_connect( G_OBJECT(window1), "delete_event", 
			G_CALLBACK(gtk_widget_destroy), window1 );
	gtk_window_set_title( GTK_WINDOW(window1), "WCH CH36x Debug" );
	gtk_window_set_default_size( GTK_WINDOW(window1), 500, 100 );
	gtk_window_set_position( GTK_WINDOW(window1), GTK_WIN_POS_CENTER );
	gtk_container_set_border_width( GTK_CONTAINER(window1), 10 );

	char dev_info_tmp[100] = {'\0'};
	sprintf( dev_info_tmp, "Current Opened dev Number:[%s]", get_ch36x_dev_info(entry) );
	vbox = gtk_vbox_new( FALSE, 0 );
	gtk_container_add( GTK_CONTAINER(window1), vbox );
	frame = gtk_frame_new( dev_info_tmp );

	if( open_choosed_dev( entry ) != 0 )
	{
		printf("function 'open_choosed_dev' error\n" );
		return NULL;
	}

	//Config Space
	char buffer_CFG[20] = {'\0'};

	PVOID	iOffset_CFG;	
	/*PUCHAR  oByte_byte_CFG = (PUCHAR)malloc( sizeof(UCHAR) );*/
	/*UCHAR	iByte_byte_CFG;*/
	PUSHORT	oByte_word_CFG = (PUSHORT)malloc( sizeof(USHORT) );
	/*USHORT	iByte_word_CFG;*/
	PULONG	oByte_dword_CFG = (PULONG)malloc( sizeof(ULONG) );
	/*ULONG	iByte_dword_CFG;*/
	//IO Space
	char buffer_IO[20] = {'\0'};

	//PVOID	iOffset_IO;
	//mPCH36x_IO_REG	oIoBaseAddr_IO = (mPCH36x_IO_REG)malloc( sizeof(mCH36x_IO_REG) );
	oIoBaseAddr_IO = (mPCH36x_IO_REG)malloc( sizeof(mCH36x_IO_REG) );
	//mPCH36x_IO_REG	iIoBaseAddr_IO;
	//MEM Space
	char buffer_MEM[20] = {'\0'};
	
	//PVOID	iOffset_MEM;
	//mPCH36x_MEM_REG oMemBaseAddr_MEM = (mPCH36x_MEM_REG)malloc( sizeof(mCH36x_MEM_REG) );
	oMemBaseAddr_MEM = (mPCH36x_MEM_REG)malloc( sizeof(mCH36x_MEM_REG) );
	//mPCH36x_MEM_REG iMemBaseAddr_MEM;

	/*
	 *gtk_container_add( GTK_CONTAINER(frame), label_dev_info );
	 *gtk_label_set_justify( GTK_LABEL(label_dev_info), GTK_JUSTIFY_LEFT );
	 */
	GtkWidget *table_sub_frame;
	table_sub_frame = gtk_table_new( 2, 4, FALSE );
	gtk_container_add( GTK_CONTAINER(frame), table_sub_frame );
	GtkWidget *lable_PID, *lable_DID, *IO_base_addr, *MEM_base_addr;
	GtkWidget *entry_PID, *entry_DID, *entry_IO_base_addr, *entry_MEM_base_addr;
	lable_PID = gtk_label_new( "Vendor ID:" );
	gtk_table_attach_defaults( GTK_TABLE(table_sub_frame), lable_PID, 0, 1, 0, 1 );

	entry_PID = gtk_entry_new( );
	memset( buffer_CFG, '\0', sizeof(buffer_CFG) );
	iOffset_CFG = (PVOID)0x00;
	CH36xReadConfigWord( iOffset_CFG, oByte_word_CFG );
	sprintf( buffer_CFG, "0x%x", (unsigned int)(*oByte_word_CFG) );
	gtk_entry_set_text( GTK_ENTRY(entry_PID), "" );
	gtk_entry_set_text( GTK_ENTRY(entry_PID), buffer_CFG );
	gtk_table_attach_defaults( GTK_TABLE(table_sub_frame), entry_PID, 1, 2, 0, 1 );

	lable_DID = gtk_label_new( "Device ID:" );
	gtk_table_attach_defaults( GTK_TABLE(table_sub_frame), lable_DID, 2, 3, 0, 1 );

	entry_DID = gtk_entry_new( );
	memset( buffer_CFG, '\0', sizeof(buffer_CFG) );
	iOffset_CFG = (PVOID)0x02;
	CH36xReadConfigWord( iOffset_CFG, oByte_word_CFG );
	sprintf( buffer_CFG, "0x%x", (unsigned int)(*oByte_word_CFG) );
	gtk_entry_set_text( GTK_ENTRY(entry_DID), buffer_CFG );
	gtk_table_attach_defaults( GTK_TABLE(table_sub_frame), entry_DID, 3, 4, 0, 1 );

	IO_base_addr = gtk_label_new( "IO Base Addr:" );
	gtk_table_attach_defaults( GTK_TABLE(table_sub_frame), IO_base_addr, 0, 1, 1, 2 );

	entry_IO_base_addr = gtk_entry_new( );
	memset( buffer_IO, '\0', sizeof(buffer_IO) );
	CH36xGetIoBaseAddr( &oIoBaseAddr_IO );
	iOffset_CFG = (PVOID)0x10;
	CH36xReadConfigDword( iOffset_CFG, oByte_dword_CFG );
	sprintf( buffer_IO, "0x%x", (unsigned int)(*oByte_dword_CFG)-1 ); 
	/*sprintf( buffer_IO, "%p", oIoBaseAddr_IO );*/ //It's virtual address by iomap
	gtk_entry_set_text( GTK_ENTRY(entry_IO_base_addr), buffer_IO );
	gtk_table_attach_defaults( GTK_TABLE(table_sub_frame), entry_IO_base_addr, 1, 2, 1, 2 );

	MEM_base_addr = gtk_label_new( "MEM Base Addr:" );
	gtk_table_attach_defaults( GTK_TABLE(table_sub_frame), MEM_base_addr, 2, 3, 1, 2 );

	entry_MEM_base_addr = gtk_entry_new( );
	memset( buffer_MEM, '\0', sizeof(buffer_MEM) );
	CH36xGetMemBaseAddr( &oMemBaseAddr_MEM ); 
	iOffset_CFG = (PVOID)0x14;
	CH36xReadConfigDword( iOffset_CFG, oByte_dword_CFG );
	sprintf( buffer_MEM, "0x%x", (unsigned int)(*oByte_dword_CFG) ); 
	/*sprintf( buffer_MEM, "%p", oMemBaseAddr_MEM ); //It's virtual address by remap*/
	gtk_entry_set_text( GTK_ENTRY(entry_MEM_base_addr), buffer_MEM );
	gtk_table_attach_defaults( GTK_TABLE(table_sub_frame), entry_MEM_base_addr, 3, 4, 1, 2 );

	gtk_box_pack_start( GTK_BOX(vbox), frame, FALSE, FALSE, 5 );

	//Config Space RD/WR
	hbox_sub_CFG = gtk_hbox_new( FALSE, 10 );
	gtk_container_set_border_width( GTK_CONTAINER(hbox_sub_CFG), 1 );
	gtk_container_add( GTK_CONTAINER(vbox), hbox_sub_CFG );

	label_sub_CFG = gtk_label_new( "Config Space RD/WR" );
	gtk_box_pack_start( GTK_BOX(hbox_sub_CFG), label_sub_CFG, FALSE, FALSE, 10 );
	radio_sub_CFG_byte  = gtk_radio_button_new_with_label( NULL, "Byte" );
	g_signal_connect( G_OBJECT(radio_sub_CFG_byte), "released",
			G_CALLBACK(on_CFG_radio_clicked), (gpointer)1 );
	radio_sub_CFG_word  = gtk_radio_button_new_with_label_from_widget( GTK_RADIO_BUTTON(radio_sub_CFG_byte), "Word" );
	g_signal_connect( G_OBJECT(radio_sub_CFG_word), "released",
			G_CALLBACK(on_CFG_radio_clicked), (gpointer)2 );
	radio_sub_CFG_dword = gtk_radio_button_new_with_label_from_widget( GTK_RADIO_BUTTON(radio_sub_CFG_word), "Dword" );
	g_signal_connect( G_OBJECT(radio_sub_CFG_dword), "released",
			G_CALLBACK(on_CFG_radio_clicked), (gpointer)3 );
	gtk_box_pack_start( GTK_BOX(hbox_sub_CFG), radio_sub_CFG_byte, FALSE, FALSE, 2 );
	gtk_box_pack_start( GTK_BOX(hbox_sub_CFG), radio_sub_CFG_word, FALSE, FALSE, 2 );
	gtk_box_pack_start( GTK_BOX(hbox_sub_CFG), radio_sub_CFG_dword, FALSE, FALSE, 2 );

	vbox_sub_CFG = gtk_vbox_new( FALSE, 0 );
	gtk_container_add( GTK_CONTAINER(vbox), vbox_sub_CFG );

	table_sub_CFG = gtk_table_new( 2, 4, FALSE );
	gtk_box_pack_start( GTK_BOX(vbox_sub_CFG), table_sub_CFG, FALSE, FALSE, 10 );

	label_sub_CFG_read = gtk_label_new( "Offset:0x" );
	gtk_table_attach_defaults( GTK_TABLE(table_sub_CFG), label_sub_CFG_read, 0, 1, 0, 1 );
	entry_sub_CFG_offset = gtk_entry_new();
	gtk_table_attach_defaults( GTK_TABLE(table_sub_CFG), entry_sub_CFG_offset, 1, 2, 0, 1 );
	button_sub_CFG_read = gtk_button_new_with_label("Read");
	gtk_table_attach_defaults( GTK_TABLE(table_sub_CFG), button_sub_CFG_read, 2, 3, 0, 1 );
	/*g_signal_connect( button_sub_CFG_read, "clicked", G_CALLBACK(), */

	label_sub_CFG_write = gtk_label_new( "Value:0x" );
	gtk_table_attach_defaults( GTK_TABLE(table_sub_CFG), label_sub_CFG_write, 0, 1, 1, 2 );
	entry_sub_CFG_value = gtk_entry_new();
	gtk_table_attach_defaults( GTK_TABLE(table_sub_CFG), entry_sub_CFG_value, 1, 2, 1, 2 );
	button_sub_CFG_write = gtk_button_new_with_label("Write");
	gtk_table_attach_defaults( GTK_TABLE(table_sub_CFG), button_sub_CFG_write, 2, 3, 1, 2 );
	/*g_signal_connect( button_sub_CFG_read, "clicked", G_CALLBACK(), */
	/*
	 *GtkWidget *label_sub_CFG_tmp;
	 *label_sub_CFG_tmp = gtk_label_new("");
	 *gtk_table_attach_defaults( GTK_TABLE(table_sub_CFG), label_sub_CFG_tmp, 3, 4, 0, 1 );
	 */
	g_signal_connect( button_sub_CFG_read, "clicked", G_CALLBACK(gtk_CFG_read), entry_sub_CFG_value );
	g_signal_connect( button_sub_CFG_write, "clicked", G_CALLBACK(gtk_CFG_write), entry_sub_CFG_value );

	//IO Space RD/WR ======================
	hbox_sub_IO = gtk_hbox_new( FALSE, 10 );
	gtk_container_set_border_width( GTK_CONTAINER(hbox_sub_IO), 1 );
	gtk_container_add( GTK_CONTAINER(vbox), hbox_sub_IO );

	label_sub_IO = gtk_label_new( "IO Space RD/WR" );
	gtk_box_pack_start( GTK_BOX(hbox_sub_IO), label_sub_IO, FALSE, FALSE, 10 );
	radio_sub_IO_byte  = gtk_radio_button_new_with_label( NULL, "Byte" );
	g_signal_connect( GTK_OBJECT(radio_sub_IO_byte), "released",
			G_CALLBACK(on_IO_radio_clicked), (gpointer)1 );
	radio_sub_IO_word  = gtk_radio_button_new_with_label_from_widget( GTK_RADIO_BUTTON(radio_sub_IO_byte), "Word" );
	g_signal_connect( GTK_OBJECT(radio_sub_IO_word), "released",
			G_CALLBACK(on_IO_radio_clicked), (gpointer)2 );
	radio_sub_IO_dword = gtk_radio_button_new_with_label_from_widget( GTK_RADIO_BUTTON(radio_sub_IO_word), "Dword" );
	g_signal_connect( GTK_OBJECT(radio_sub_IO_dword), "released",
			G_CALLBACK(on_IO_radio_clicked), (gpointer)3 );
	gtk_box_pack_start( GTK_BOX(hbox_sub_IO), radio_sub_IO_byte, FALSE, FALSE, 2 );
	gtk_box_pack_start( GTK_BOX(hbox_sub_IO), radio_sub_IO_word, FALSE, FALSE, 2 );
	gtk_box_pack_start( GTK_BOX(hbox_sub_IO), radio_sub_IO_dword, FALSE, FALSE, 2 );

	vbox_sub_IO = gtk_vbox_new( FALSE, 0 );
	gtk_container_add( GTK_CONTAINER(vbox), vbox_sub_IO );

	table_sub_IO = gtk_table_new( 2, 4, FALSE );
	gtk_box_pack_start( GTK_BOX(vbox_sub_IO), table_sub_IO, FALSE, FALSE, 10 );

	label_sub_IO_read = gtk_label_new( "Addr:0x" );
	gtk_table_attach_defaults( GTK_TABLE(table_sub_IO), label_sub_IO_read, 0, 1, 0, 1 );
	entry_sub_IO_addr = gtk_entry_new();
	gtk_table_attach_defaults( GTK_TABLE(table_sub_IO), entry_sub_IO_addr, 1, 2, 0, 1 );
	button_sub_IO_read = gtk_button_new_with_label("Read");
	gtk_table_attach_defaults( GTK_TABLE(table_sub_IO), button_sub_IO_read, 2, 3, 0, 1 );
	/*g_signal_connect( button_sub_IO_read, "clicked", G_CALLBACK(), */

	label_sub_IO_write = gtk_label_new( "Value:0x" );
	gtk_table_attach_defaults( GTK_TABLE(table_sub_IO), label_sub_IO_write, 0, 1, 1, 2 );
	entry_sub_IO_value = gtk_entry_new();
	gtk_table_attach_defaults( GTK_TABLE(table_sub_IO), entry_sub_IO_value, 1, 2, 1, 2 );
	button_sub_IO_write = gtk_button_new_with_label("Write");
	gtk_table_attach_defaults( GTK_TABLE(table_sub_IO), button_sub_IO_write, 2, 3, 1, 2 );
	/*g_signal_connect( button_sub_IO_read, "clicked", G_CALLBACK(), */
	/*
	 *GtkWidget *label_sub_IO_tmp;
	 *label_sub_IO_tmp = gtk_label_new("");
	 *gtk_table_attach_defaults( GTK_TABLE(table_sub_IO), label_sub_IO_tmp, 3, 4, 0, 1 );
	 */
	g_signal_connect( button_sub_IO_read, "clicked", G_CALLBACK(gtk_IO_read), entry_sub_IO_value );
	g_signal_connect( button_sub_IO_write, "clicked", G_CALLBACK(gtk_IO_write), entry_sub_IO_value );

	//MEM Space RD/WR
	hbox_sub_MEM = gtk_hbox_new( FALSE, 10 );
	gtk_container_set_border_width( GTK_CONTAINER(hbox_sub_MEM), 1 );
	gtk_container_add( GTK_CONTAINER(vbox), hbox_sub_MEM );

	label_sub_MEM = gtk_label_new( "MEM Space RD/WR" );
	gtk_box_pack_start( GTK_BOX(hbox_sub_MEM), label_sub_MEM, FALSE, FALSE, 10 );
	radio_sub_MEM_byte  = gtk_radio_button_new_with_label( NULL, "Byte" );
	g_signal_connect( GTK_OBJECT(radio_sub_MEM_byte), "released",
			G_CALLBACK(on_MEM_radio_clicked), (gpointer)1 );
	radio_sub_MEM_word  = gtk_radio_button_new_with_label_from_widget( GTK_RADIO_BUTTON(radio_sub_MEM_byte), "Word" );
	g_signal_connect( GTK_OBJECT(radio_sub_MEM_word), "released",
			G_CALLBACK(on_MEM_radio_clicked), (gpointer)2 );
	radio_sub_MEM_dword = gtk_radio_button_new_with_label_from_widget( GTK_RADIO_BUTTON(radio_sub_MEM_word), "Dword" );
	g_signal_connect( GTK_OBJECT(radio_sub_MEM_dword), "released",
			G_CALLBACK(on_MEM_radio_clicked), (gpointer)3 );
	gtk_box_pack_start( GTK_BOX(hbox_sub_MEM), radio_sub_MEM_byte, FALSE, FALSE, 2 );
	gtk_box_pack_start( GTK_BOX(hbox_sub_MEM), radio_sub_MEM_word, FALSE, FALSE, 2 );
	gtk_box_pack_start( GTK_BOX(hbox_sub_MEM), radio_sub_MEM_dword, FALSE, FALSE, 2 );

	vbox_sub_MEM = gtk_vbox_new( FALSE, 0 );
	gtk_container_add( GTK_CONTAINER(vbox), vbox_sub_MEM );

	table_sub_MEM = gtk_table_new( 2, 4, FALSE );
	gtk_box_pack_start( GTK_BOX(vbox_sub_MEM), table_sub_MEM, FALSE, FALSE, 10 );

	label_sub_MEM_read = gtk_label_new( "Addr:0x" );
	gtk_table_attach_defaults( GTK_TABLE(table_sub_MEM), label_sub_MEM_read, 0, 1, 0, 1 );
	entry_sub_MEM_addr = gtk_entry_new();
	gtk_table_attach_defaults( GTK_TABLE(table_sub_MEM), entry_sub_MEM_addr, 1, 2, 0, 1 );
	button_sub_MEM_read = gtk_button_new_with_label("Read");
	gtk_table_attach_defaults( GTK_TABLE(table_sub_MEM), button_sub_MEM_read, 2, 3, 0, 1 );

	label_sub_MEM_write = gtk_label_new( "Value:0x" );
	gtk_table_attach_defaults( GTK_TABLE(table_sub_MEM), label_sub_MEM_write, 0, 1, 1, 2 );
	entry_sub_MEM_value = gtk_entry_new();
	gtk_table_attach_defaults( GTK_TABLE(table_sub_MEM), entry_sub_MEM_value, 1, 2, 1, 2 );
	button_sub_MEM_write = gtk_button_new_with_label("Write");
	gtk_table_attach_defaults( GTK_TABLE(table_sub_MEM), button_sub_MEM_write, 2, 3, 1, 2 );
	/*g_signal_connect( button_sub_MEM_read, "clicked", G_CALLBACK(), */
	/*
	 *GtkWidget *label_sub_MEM_tmp;
	 *label_sub_MEM_tmp = gtk_label_new("");
	 *gtk_table_attach_defaults( GTK_TABLE(table_sub_MEM), label_sub_MEM_tmp, 3, 4, 0, 1 );
	 */
	g_signal_connect( button_sub_MEM_read, "clicked", G_CALLBACK(gtk_MEM_read), entry_sub_MEM_value );
	g_signal_connect( button_sub_MEM_write, "clicked", G_CALLBACK(gtk_MEM_write), entry_sub_MEM_value );

	g_signal_connect( window1, "destroy", G_CALLBACK(gtk_destroy_sub), NULL );
	//g_signal_connect( window1, "destroy", G_CALLBACK(gtk_main_quit), NULL ); //window and sub window all will exit
	gtk_widget_show_all( window1 );

	return window1;
}

/*
 * ********************************************************************
 * create_main_window()
 *
 * FUNCTION : create Main window 
 * Note: When sub window display, main window do not appear
 * And when create main window, scan device number
 *
 * ********************************************************************
 */
GtkWidget *create_main_window()
{
	int retval = 0;
	GtkWidget *window;
	GtkWidget *hbox;
	GtkWidget *vbox;
	
	GtkWidget *label_dev_list;
	GtkWidget *label_input;
	
	GtkWidget *button_next;
	GtkWidget *button_exit;
	
	GtkWidget *entry_main;
	GtkWidget *frame;

	window = gtk_window_new( GTK_WINDOW_TOPLEVEL );
	gtk_window_set_title( GTK_WINDOW(window), "WCH CH36x Chips Debug" );
	gtk_window_set_default_size( GTK_WINDOW(window), 350, 50 );		//Set window dimensions
	gtk_window_set_position( GTK_WINDOW(window), GTK_WIN_POS_CENTER );

	vbox = gtk_vbox_new( FALSE, 0 );
	gtk_container_add( GTK_CONTAINER(window), vbox );
	frame = gtk_frame_new( "Current CH36x devices list");

	retval = get_ch36x_dev_list();
	if( retval == 0 )  //retval:0, it's true negative means false
	{
		label_dev_list = gtk_label_new( gtk_dev_list );
	}
	else
	{
		label_dev_list = gtk_label_new( "Can't find devices" );
	}
	gtk_container_add( GTK_CONTAINER(frame), label_dev_list );
	gtk_label_set_justify( GTK_LABEL(label_dev_list), GTK_JUSTIFY_LEFT );
	gtk_box_pack_start( GTK_BOX(vbox), frame, FALSE, FALSE, 5 );
	/*label_dev_list = gtk_label_new( "" );*/

	hbox = gtk_hbox_new( FALSE, 10 );
	gtk_container_set_border_width( GTK_CONTAINER(hbox), 10 );
	gtk_container_add( GTK_CONTAINER(vbox), hbox );

	label_input = gtk_label_new( "Input Dev Num:" );
	gtk_box_pack_start( GTK_BOX(hbox), label_input, FALSE, FALSE, 10 );

	entry_main = gtk_entry_new();
	gtk_box_pack_start( GTK_BOX(hbox), entry_main, FALSE, FALSE, 10 );

	button_next = gtk_button_new_with_label("Next");
	gtk_box_pack_start( GTK_BOX(hbox), button_next, FALSE, FALSE, 10 );

	button_exit = gtk_button_new_with_label("Exit");
	gtk_box_pack_start( GTK_BOX(hbox), button_exit, TRUE, TRUE, 10 );

	gtk_widget_show_all( window );
	//g_signal_connect( button_next, "clicked", G_CALLBACK(gtk_show_sub), NULL );
	g_signal_connect( button_next, "clicked", G_CALLBACK(gtk_show_sub), entry_main );
	g_signal_connect( button_exit, "clicked", G_CALLBACK(gtk_main_quit), NULL );
	g_signal_connect( window, "destroy", G_CALLBACK(gtk_main_quit), NULL );

	return window;
}

int main( int argc, char **argv )
{
	gtk_init( &argc, &argv );
	window_main = create_main_window();
	gtk_widget_show( window_main );

	gtk_main();

	return 0;
}











