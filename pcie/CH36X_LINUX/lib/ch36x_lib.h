//****************************************
//**  Web:  http://www.winchiphead.com  **
//****************************************
//**  DLL for PCI interface chip CH36x  **
//**  C, linux-GNU                      **
//****************************************
//


#ifndef _CH36X_LIB_H
#define _CH36X_LIB_H

#define DEV_INDEX_NUM	0x00 //This indicate device that be operated currently, The range 0x00-0x0f
//#define DEV_INDEX_NUM	0x01 //This indicate device that be operated currently, The range 0x00-0x0f

#ifndef CH36x_MAX
#define CH36x_MAX( a, b )		( ((a) > (b)) ? (a) : (b) )
#endif

#ifndef CH36x_MIN
#define CH36x_MIN( a, b )		( ((a) < (b)) ? (a) : (b) )
#endif

#ifndef UCHAR	
#define UCHAR		unsigned char
#endif

#ifndef USHORT
#define USHORT		unsigned short
#endif

#ifndef ULONG	
#define ULONG		unsigned long
#endif

#ifndef PUCHAR	
#define PUCHAR		unsigned char *
#endif

#ifndef PCHAR		
#define PCHAR		char *
#endif

#ifndef PUSHORT 
#define PUSHORT		unsigned short *
#endif

#ifndef PULONG
#define PULONG		unsigned long *
#endif

#ifndef VOID   
#define VOID		void
#endif

#ifndef PVOID
#define PVOID		void *
#endif

#define true 1
#define false 0

typedef void ( *mPCH36x_INT_ROUTINE )(void);

typedef enum 
{
	FALSE_H = 0,
	TRUE_H = !FALSE_H
}BOOL;

typedef struct _PCI_CONFIG 
{
	USHORT			mPcVendorId;				//00H Vendor ID
	USHORT			mPcDeviceId;				//02H Devcie ID
	USHORT			mPcCommandReg;				//04H Command Reg
	USHORT			mPcStatusReg;				//06H Status Reg
	UCHAR			mPcRevisionId;				//08H Revision changing Tag
	UCHAR			mPcProgramIf;				//09H Programming Reg Interface
	UCHAR			mPcSubClass;				//0AH Sub Class Code
	UCHAR			mPcBaseClass;				//0BH Base Class Code
	UCHAR			mPcCacheLine;				//0CH Cache Line length
	UCHAR			mPcLatenTimer;				//0DH Latency Timer Reg
	UCHAR			mPcHeaderType;				//0EH Header Type
	UCHAR			mPcBistReg;					//0FH BIST Reg
	ULONG			mPcBaseAddr0;				//10H Base Address 0
	ULONG			mPcBaseAddr1;				//14H Base Address 1
	ULONG			mPcBaseAddr2;				//18H Base Address 2
	ULONG			mPcBaseAddr3;				//1CH Base Address 3
	ULONG			mPcBaseAddr4;				//20H Base Address 4
	ULONG           mPcBaseAddr5;				//24H Base Address 5
	ULONG			mPcCardBusCis;				//28H
	USHORT			mPcSubSysVen;				//2CH Subsystem Vendor ID
	USHORT			mPcSubSysDev;				//2EH Subsystem ID
	ULONG			mPcExpanSRom;				//30H Expansion ROM Base Addr
	UCHAR			mPcCapPtr;					//34H 
	UCHAR			mPcReserved1[3];			//35H
	ULONG			mPcReserved2;				//38H
	UCHAR			mPcInterLine;				//3CH Interrupt Line
	UCHAR			mPcInterPin;				//3DH Interrupt Pin
	UCHAR			mPcMinGrant;				//3EH Min_Gnt
	UCHAR			mPcMaxLatency;				//3FH Max_lat
} mPCI_CONFIG, *mPPCI_CONFIG;

/*
 *typedef struct _PCI_EXP_ROM
 *{
 *} mPCI_EXP_ROM, *mPPCI_EXP_ROM;
 */


typedef struct _CH36x_CFG_REG					//CH36x chip's Configuration Reg
{
	mPCI_CONFIG		mCh36xCfgPci;				//00H-3FH, standard PCI Cfg Space 64 byte
	UCHAR			mCh36xCfgCtrl;				//40H Ctrl Reg, High 5 bits RDONLY
	UCHAR			mCh36xCfgDin;				//41H 8bits input port of bus, RDONLY
	UCHAR			mCh36xCfgState;				//42H status Reg of chip, RDONLY
	UCHAR			mCh36xCfgResv;				//43H
} mCH36x_CFG_REG, *mPCH36x_CFG_REG;


typedef struct _CH36x_IO_REG					//CH36x chip's I/O Space
{
	UCHAR			mCh36xIoPort[0xf0];			//00H-EFH, 240 bytes standard I/O Sports	
	union {										//Accessed by byte or word
		USHORT		mCh36xMemAddr;				//F0H A15-A0 address Setting Reg

		struct {								//Accessed by byte
			UCHAR	mCh36xMemAddrL;				//F0H A7-A0 address Setting Reg
			UCHAR	mCh36xMemAddrH;				//F1H A15-A8 Address Setting Reg
		};
	};
	UCHAR			mCh36xIoResv2;				//F2H
	UCHAR			mCh36xMemData;				//F3H Data accessed Reg of Memory
	UCHAR			mCh36xI2cData;				//F4H I2C Data Reg
	UCHAR			mCh36xI2cCtrl;				//F5H I2C Ctrl and Stat Reg
	UCHAR			mCh36xI2cAddr;				//F6H I2C Setting Address Reg
	UCHAR			mCh36xI2CDev;				//F7H I2C Device addr and Command Reg
	UCHAR			mCh36xIoCtrl;				//F8H chip's Ctrl Reg, High 5bits RDONLY
	UCHAR			mCh36xIoBuf;				//F9H Buf Reg of local data input
	UCHAR			mCh36xSpeed;				//FAH Speed Ctrl Reg of chip
	UCHAR			mCh36xIoResv3;				//FBH
	UCHAR			mCh36xIoTime;				//FCH loop counting Reg
	UCHAR			mCh36xIoResv4[3];			//FDH
} mCH36x_IO_REG, *mPCH36x_IO_REG;


typedef struct _CH36x_MEM_REG					//CH36x Mmemory Space of chip
{
	UCHAR			mCh36xMemPort[0x8000];		//0000H-7FFFH, 32768 bytes(32K) standard memory units
} mCH36x_MEM_REG, *mPCH36x_MEM_REG;


int CH36xOpenDevice(
		BOOL		iEnableMemory,
		BOOL		iEnableInterrupt );

void CH36xCloseDevice( );

BOOL CH36xGetDrvVer(		//Get CH36x driver verion
		PCHAR				drv_version );

BOOL CH36xGetLibVer(		//Get CH36x lib version
		PCHAR				lib_version );	

BOOL CH36xGetIoBaseAddr( 
		mPCH36x_IO_REG		*oIoBaseAddr ); //Save addr of Base register Addr, It's Physical address

BOOL CH36xSetIoBaseAddr(
		mPCH36x_IO_REG		iIoBaseAddr ); //Assigned Base Addr 

BOOL CH36xGetMemBaseAddr(
		mPCH36x_MEM_REG		*oMemBaseAddr ); //Save addr of Memory register Addr, It's Physical address

BOOL CH36xSetMemBaseAddr(
		mPCH36x_MEM_REG		iMemBaseAddr ); //Assigned Base Addr

BOOL CH36xGetIntLine(   //Get interrupter line
		PULONG				oIntLine );

BOOL CH36xSetIntLine(  //Set interrupter line
		ULONG				iIntLine );

BOOL CH36xReadIoByte(  //Read one byte from I/O port
		PVOID				iAddr,
		PUCHAR				oByte );

BOOL CH36xReadIoWord(  //Read one word from I/O port
		PVOID				iAddr,
		PUSHORT				oByte );

BOOL CH36xReadIoDword( //Read double words from I/O port
		PVOID				iAddr,
		PULONG				oByte );

BOOL CH36xReadIoBlock( //Read a block from I/O port
		PVOID				iAddr,
		PUCHAR				oByte,
		ULONG				oLength );	//***

BOOL CH36xWriteIoByte(  //Write one byte into I/O port
		PVOID				iAddr,
		UCHAR				iByte );

BOOL CH36xWriteIoWord(  //Write one word into I/O port
		PVOID				iAddr,
		USHORT				iByte );

BOOL CH36xWriteIoDword( //Write double words into I/O port
		PVOID				iAddr,
		ULONG				iByte );

BOOL CH36xWriteIoBlock( //Write a block into I/O port
		PVOID				iAddr, 
		PUCHAR				iByte,
		ULONG				iLength );	//***

BOOL CH36xReadMemByte( //Read one byte from memory  
		PVOID				iAddr,
		PUCHAR				oByte );

BOOL CH36xReadMemWord( //Read one byte form memory
		PVOID				iAddr,
		PUSHORT				oByte );

BOOL CH36xReadMemDword( //Read double words form memroy
		PVOID				iAddr,
		PULONG				oByte );

BOOL CH36xReadMemBlock( //Read a block from memory
		PVOID				iAddr,
		PUCHAR				oByte,
		ULONG				oLength );	//***

BOOL CH36xWriteMemByte( //Write one byte into memory
		PVOID				iAddr,
		UCHAR				iByte );

BOOL CH36xWriteMemWord( //Write a word into memory
		PVOID				iAddr,
		USHORT				iByte );

BOOL CH36xWriteMemDword( //Write double words into memory
		PVOID				iAddr,
		ULONG				iByte );

BOOL CH36xWriteMemBlock( //Write a block into memory
		PVOID				iAddr,
		PUCHAR				iByte,
		ULONG				iLength ); //***

BOOL CH36xReadConfig(	//Read byte from CFG Space
		PVOID				iOffset,
		PUCHAR				oByte );

BOOL CH36xWriteConfig(	//Write byte into CFG Space
		PVOID				iOffset,
		UCHAR				iByte );

BOOL CH36xReadConfigWord( //Read a word form CFG Sapce
		PVOID				iOffset,
		PUSHORT				oByte );

BOOL CH36xWriteConfigWord( //Write a word into CFG Space
		PVOID				iOffset,
		USHORT				iByte );

BOOL CH36xReadConfigDword( //Read two words from CFG Sapce
		PVOID				iOffset,
		PULONG				oByte );

BOOL CH36xWriteConfigDword( //Write two words into CFG Sapce
		PVOID				iOffset,
		ULONG				iByte );

BOOL CH36xWaitInterrupt( ); //Wait interrupter event

BOOL CH36xAbortInterrupt( ); //Abort waiting for interrupter

BOOL CH36xSetIntRoutine(		//Setting handler for interrupter
		mPCH36x_INT_ROUTINE iIntRoutine );



#endif





