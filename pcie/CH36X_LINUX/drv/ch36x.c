/*
 *ch36x.c for ch365/ch367/ch368 pci card of WCH
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
 */

#include <linux/version.h>
#ifndef KERNEL_VERSION
#define KERNEL_VERSION(ver, rel, seq)	((ver << 16) | (rel << 8 ) | seq)
#endif

#define CH365_WCH_CHIP_SUPPORT
#define CH367_WCH_CHIP_SUPPORT
#define CH368_WCH_CHIP_SUPPORT

#include <linux/pci.h>
#include <linux/module.h>
#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/cdev.h>		
#include <linux/slab.h>		
#include <linux/types.h>	
#include <linux/fs.h>	
#include <linux/device.h>	
#include <linux/fcntl.h>
#include <linux/ioport.h>

#include <asm/uaccess.h>	
#include <asm/io.h>			
#include <linux/string.h>
#include <linux/list.h>		
#include <linux/mutex.h>	
#include <linux/irq.h>
#include <linux/interrupt.h>
#include <asm/signal.h>		
#include <linux/poll.h>
#include <linux/wait.h>
#include <linux/sched.h>

//for debug
#define DEBUG
#undef DEBUG

#ifdef DEBUG
#define dbg( format, arg... )		\
	printk( KERN_DEBUG "%s %d: " format "\n", __FILE__, __LINE__, ##arg )
#else
#define dbg( format, arg... )		\
do{									\
	if(0)							\
		printk(KERN_DEBUG "%s %d: " format "\n", __FILE__, __LINE__, ##arg);	\
} while (0)
#endif

#define err( format, arg... )		\
	printk(KERN_ERR KBUILD_MODNAME ": " format "\n", ##arg)

#if (LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,0))
#define READ8( addr )				ioread8( (void __iomem *)(addr) )
#define READ16( addr )				ioread16( (void __iomem *)(addr) )
#define READ32( addr )				ioread32( (void __iomem *)(addr) )
#define WRITE8( value, addr )		iowrite8( (value), (void *)(addr) )
#define WRITE16( value, addr )		iowrite16( (value), (void *)(addr) )
#define WRITE32( value, addr )		iowrite32( (value), (void *)(addr) )
#else
#define READ8( addr )				readb( (unsigned long)(addr) )
#define READ16( addr )				readw( (unsigned long)(addr) )
#define READ32( addr )				readl( (unsigned long)(addr) )
#define WRITE8( value, addr )		writeb( (value), (unsigned long)(addr) )
#define WRITE16( value, addr )		writew( (value), (unsigned long)(addr) )
#define WRITE32( value, addr )		writel( (value), (unsigned long)(addr) )
#endif

//error codes
#define ERR_CLASS_CREATE		-50

#ifdef CONFIG_PM
#undef CONFIG_PM
#endif

//#define CH36x_WCH                             
#define CH36x_DEV_NUM			0x10			//Max Support Device
#define DRV_NAME				"ch36xpci"	//This drive name
#define DRV_VERSION				"WCH CH36x Driver Version-1.2"
//1.0-1.2 modified to support 32bit and 64bit systems

#define CH365_VID				0x4348	//Vendor id
#define CH365_DID				0x5049	//Device id
#define CH367_VID				0x1C00	//Vendor id
#define CH367_DID_SID_HIGH		0x5831	//Device id when SID high
#define CH367_DID_SID_LOW		0x5830	//Device id when SID low
#define CH367_SUB_VID			0x1C00	//Subsystem vendor id
#define CH367_SUB_DID_SID_HIGH	0x5831	//Subsystem Vendor id when SID high
#define CH367_SUB_DID_SID_LOW	0x5830	//Subsystem Device id when SID low
#define CH368_VID				0x1C00	//Vendor id
#define CH368_DID				0x5834	//Device id
#define CH368_SUB_VID			0x1C00	//Subsystem Vendor id
#define CH368_SUB_DID			0x5834	//Subsystem Device id

//size of read or write 
#define SIZE_BYTE				0x1
#define SIZE_WORD				0x2
#define SIZE_DWORD				0x3

//interrupt state for ch367/ch368
#define TRIGGER_LOW				0xe0	//Low enable
#define TRIGGER_HIGH				0xe4	//High enable
#define TRIGGER_RISING				0xe8	//Rising enable
#define TRIGGER_FALLING				0xec	//Falling enable

//address of cfg/IO/MEM register
#define OFFSET_IO_BASE_ADDR			0x10	// I/O Base Address
#define OFFSET_MEM_BASE_ADDR			0x14	// Memory Base Address	
#define OFFSET_INT_LINE				0x3C	// Interrupt Line & Pin

//types be supported of chips
#ifdef CH365_WCH_CHIP_SUPPORT
#define CH365_OFFSET_INT_FUN_STAT			0x42	// Chip State Register
#define CH365_OFFSET_INT_ACTIVE_STAT		0x40		// Chip Control Register
#define CH365_IO_SPACE_RANGE				0xff	// I/O Space Range
#define CH365_MEM_SPACE_RANGE				0x8000
#endif

#ifdef CH367_WCH_CHIP_SUPPORT
#define CH367_OFFSET_INT_FUN_STAT			0xeb	// Interrupt Control Register
#define CH367_OFFSET_INT_ACTIVE_STAT		0xf8		// State Register
#define CH367_IO_SPACE_RANGE				0xff
#endif

#ifdef CH368_WCH_CHIP_SUPPORT
#define CH368_OFFSET_INT_FUN_STAT			0xeb	// Interrupt Control Register
#define CH368_OFFSET_INT_ACTIVE_STAT		0xf8		// State Register
#define CH368_IO_SPACE_RANGE				0xff
#define CH368_MEM_SPACE_RANGE				0x8000
#endif

//irq flags
#ifndef IRQF_SHARED
#define IRQF_SHARED		SA_SHIRQ
#endif

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

static DEFINE_MUTEX(ch36x_fops_ioctl_mutex);		//define mutex
static DECLARE_WAIT_QUEUE_HEAD(ch36x_poll_queue);	//Wait queue

typedef enum {
	CH36x_WCH = 0,
} intel_x86_t;

static unsigned char g_dev_count = 0;
static struct class *ch36x_class = NULL;

static struct list_head g_private_head;

static int CH36x_MAJOR = 0x00;
static int CH36x_MINOR = 0x00;


struct ch36x_dev_private {
	//list header for multi devices
	struct list_head ch36x_dev_list;

	//when register char driver and find PCI devices to use
	struct pci_dev *ch36x_pdev;
	struct cdev ch36x_cdev;

	dev_t ch36x_dev;
	atomic_t created;

	//reserve configure information of ch36x
	unsigned long mmio_addr;
	unsigned long regs_len;
	int irq;

	//reserve Memory space information of PCI devices 
	dma_addr_t rx_dma_addrp;
	dma_addr_t tx_dma_addrp;

	//based SYNC ways
	spinlock_t lock_send;
	spinlock_t lock_recv;

	//reserve address information that IO port converted
	void __iomem *ioaddr;	//phy address
	unsigned long io_virts_addr;
	unsigned long io_len;

	//reserve address informantion that IO space (device memory) converted
	void __iomem *memaddr;	//phy address
	unsigned long mem_virts_addr;
	unsigned long mem_len;

	char dev_file_name[20];

	int open_flag;		//flag of device opened

	//reserve device interrupter types, likely CH367 and CH368
	unsigned char ch365_int_enable;
	unsigned char ch367_int_enable;
	unsigned char ch368_int_enable;

	//poll flag
	unsigned char poll_flag;

};

//Enum PCI Device
static struct pci_device_id ch36x_id_table[] =	
{
#ifdef CH365_WCH_CHIP_SUPPORT
	{CH365_VID, CH365_DID, PCI_ANY_ID, PCI_ANY_ID, 0, 0, CH36x_WCH},
#endif
#ifdef CH367_WCH_CHIP_SUPPORT
	{CH367_VID, CH367_DID_SID_HIGH, CH367_SUB_VID, CH367_SUB_DID_SID_HIGH,
	 0, 0, CH36x_WCH},
	{CH367_VID, CH367_DID_SID_LOW, CH367_SUB_VID, CH367_SUB_DID_SID_LOW, 0,
	 0, CH36x_WCH},
#endif
#ifdef CH368_WCH_CHIP_SUPPORT
	{CH368_VID, CH368_DID, CH368_SUB_VID, CH368_SUB_DID, 0, 0, CH36x_WCH},
#endif
	{0,}
};
MODULE_DEVICE_TABLE(pci, ch36x_id_table);	


/*
 * ********************************************************************
 * FUNCTION : Read CFG Register
 * arg:
 * type  	 : Read Bytes(BYTE/WORD/DWORD)
 * offset_addr   : Read Addr
 * ch36x_arg     : Output Buffer Space
 * struct pci_dev: PCI Device 
 * ********************************************************************
 */
static int
ch36x_cfg_read(int type, unsigned char offset_addr, unsigned long ch36x_arg,
	       struct pci_dev *cfg_read_pdev)
{
	int retval = 0;
	unsigned char read_byte;
	unsigned short read_word;
	unsigned int read_dword;

	switch (type) {
	case SIZE_BYTE:
		{
			pci_read_config_byte(cfg_read_pdev, offset_addr,
					     &read_byte);
			retval =
			    put_user(read_byte, (long __user *) (ch36x_arg));
				/*put_user(read_byte, (int __user *) (ch36x_arg));*/

			dbg("read_byte:%x", read_byte);
			dbg("put_user retval:%d line:%d", retval, __LINE__);
			break;
		}
	case SIZE_WORD:
		{
			pci_read_config_word(cfg_read_pdev, offset_addr,
					     &read_word);
			retval =
			    put_user(read_word, (long __user *) (ch36x_arg));

			dbg("read_word:%x offset_addr:%x", read_word,
			    offset_addr);
			dbg("put_user retval:%d line:%d", retval, __LINE__);
			break;
		}
	case SIZE_DWORD:
		{
			pci_read_config_dword(cfg_read_pdev, offset_addr,
					      &read_dword);
			retval =
			    put_user(read_dword, (long __user *) (ch36x_arg));

			dbg("read_dword:%x", read_dword);
			dbg("put_user retval:%d line:%d", retval, __LINE__);
			break;
		}
	default:
		return -EINVAL;
	}

	if (retval != 0) {
		return -EFAULT;
	}

	return 0;

}


/*
 * ********************************************************************
 * FUNCTION : Write CFG Register
 * arg:
 * type  	 : Write Bytes(BYTE/WORD/DWORD)
 * offset_addr   : Write Addr
 * ch36x_arg     : Iutput Buffer 
 * struct pci_dev: PCI Device 
 * ********************************************************************
 */
static int
ch36x_cfg_write(int type, unsigned char offset_addr, unsigned long ch36x_arg,
		struct pci_dev *cfg_write_pdev)
{
	int retval = 0;
	unsigned char write_byte;
	unsigned short write_word;
	unsigned int write_dword;

	switch (type) {
	case SIZE_BYTE:
		{
			retval = get_user(write_byte, (long __user *) ch36x_arg);
			pci_write_config_byte(cfg_write_pdev, offset_addr,
					      write_byte);

			dbg("get_user retval:%d line:%d", retval, __LINE__);
			dbg("write_byte:%x", write_byte);
			break;
		}
	case SIZE_WORD:
		{
			retval = get_user(write_word, (long __user *) ch36x_arg);
			pci_write_config_word(cfg_write_pdev, offset_addr,
					      write_word);

			dbg("get_user retval:%d line:%d", retval, __LINE__);
			dbg("write_word:%x", write_word);
			break;
		}
	case SIZE_DWORD:
		{
			retval = get_user(write_dword, (long __user *) ch36x_arg);
			pci_write_config_dword(cfg_write_pdev, offset_addr,
					       write_dword);

			dbg("get_user retval:%d line:%d", retval, __LINE__);
			dbg("get_dword:%x", write_dword);
			break;
		}
	}

	if (retval != 0) {
		return -EFAULT;
	}

	return 0;
}


/*
 * ********************************************************************
 * FUNCTION : Read I/O Space Register
 * arg:
 * type  	 : Read Bytes(BYTE/WORD/DWORD)
 * addr   	 : Read I/O Addr
 * ch36x_arg     : Output Buffer  
 * ********************************************************************
 */
static int
ch36x_io_read(int type, unsigned long addr, unsigned long ch36x_arg)
{
	int retval = 0;
	unsigned char read_byte;
	unsigned short read_word;
	unsigned int read_dword;

	switch (type) {
	case SIZE_BYTE:
		{
			read_byte = READ8( addr );
			retval = put_user(read_byte, (long __user *) (ch36x_arg));

			dbg("READ8 read_byte:%x", read_byte);
			break;
		}
	case SIZE_WORD:
		{
			read_word = READ16( addr );
			retval = put_user(read_word, (long __user *) (ch36x_arg));

			dbg("READ16 read_word:%x", read_word);
			break;
		}
	case SIZE_DWORD:
		{
			read_dword = READ32( addr );
			retval = put_user(read_dword, (long __user *) (ch36x_arg));

			dbg("READ32 read_dword:%x", read_dword);
			break;
		}
	default:
		return -EINVAL;
	}

	if (retval != 0) {
		return -EFAULT;
	}

	return 0;
}


/*
 * ********************************************************************
 * FUNCTION : Write I/O Space Register
 * arg:
 * type  	 : Write Bytes(BYTE/WORD/DWORD)
 * addr   	 : Write I/O Addr
 * ch36x_arg     : Iutput Buffer  
 * ********************************************************************
 */
static int
ch36x_io_write(int type, unsigned long addr, unsigned long ch36x_arg)
{
	int retval = 0;
	unsigned char write_byte;
#ifdef DEBUG
	unsigned char read_byte;
#endif
	unsigned short write_word;
	unsigned int write_dword;

	switch (type) {
	case SIZE_BYTE:
		{
			retval = get_user(write_byte, (long __user *) ch36x_arg);
			WRITE8(write_byte, addr);

			dbg(" write_byte:%x", write_byte);
			dbg(" write_byte addr ptr:%p", (void __iomem *) addr);
#ifdef DEBUG
			read_byte = READ8( addr );
			dbg("+--read_byte:%x", read_byte);
#endif
			break;
		}
	case SIZE_WORD:
		{
			retval = get_user(write_word, (long __user *) ch36x_arg);
			WRITE16(write_word, addr);

			dbg(" write_word:%x", write_word);
			dbg(" write_word addr ptr:%p", (void __iomem *) addr);
			break;
		}
	case SIZE_DWORD:
		{
			retval =
			    get_user(write_dword, (long __user *) ch36x_arg);
			WRITE32(write_dword, addr);

			dbg(" write_dword:%x", write_dword);
			dbg(" write_dword addr ptr:%p", (void __iomem *) addr);
			break;
		}
	default:
		return -EINVAL;
	}

	if (retval != 0) {
		return -EFAULT;
	}

	return 0;
}

/*
 * ********************************************************************
 * FUNCTION : Read MEM Space Register
 * arg:
 * type  	 : Read Bytes(BYTE/WORD/DWORD)
 * addr   	 : Read Mem Addr
 * ch36x_arg     : Output Buffer  
 * ********************************************************************
 */
static int
ch36x_mem_read(int type, unsigned long addr, unsigned long ch36x_arg)
{
	int retval = 0;
	unsigned char read_byte;
	unsigned short read_word;
	unsigned int read_dword;

	switch (type) {
	case SIZE_BYTE:
		{
			read_byte = READ8( addr );
			retval = put_user(read_byte, (long __user *) (ch36x_arg));

			dbg(" 'addr':%x", (unsigned int) addr);
			dbg(" 'read_byte' :%x", read_byte);
			break;
		}
	case SIZE_WORD:
		{
			read_word = READ16( addr );
			retval = put_user(read_word, (long __user *) (ch36x_arg));

			dbg(" 'addr':%x", (unsigned int) addr);
			dbg(" 'read_word' :%x", read_word);
			break;
		}
	case SIZE_DWORD:
		{
			read_dword = READ32( addr );
			retval = put_user(read_dword, (long __user *) (ch36x_arg));

			dbg(" 'addr':%x", (unsigned int) addr);
			dbg(" 'read_dword' :%x", read_dword);
			break;
		}
	default:
		return -EINVAL;
	}

	if (retval != 0) {
		return -EFAULT;
	}

	return 0;
}

/*
 * ********************************************************************
 * FUNCTION : Write MEM Space Register
 * arg:
 * type  	 : Write Bytes(BYTE/WORD/DWORD)
 * addr   	 : Write Mem Addr
 * ch36x_arg     : Iutput Buffer  
 * ********************************************************************
 */
static int
ch36x_mem_write(int type, unsigned long addr, unsigned long ch36x_arg)
{
	int retval = 0;
	unsigned char write_byte;
	unsigned short write_word;
	unsigned long write_dword;

	switch (type) {
	case SIZE_BYTE:
		{
			retval = get_user(write_byte, (long __user *) ch36x_arg);
			WRITE8(write_byte, addr);
			break;
		}
	case SIZE_WORD:
		{
			retval = get_user(write_word, (long __user *) ch36x_arg);
			WRITE16(write_word, addr);
			break;
		}
	case SIZE_DWORD:
		{
			retval = get_user(write_dword, (long __user *) ch36x_arg);
			WRITE32(write_dword, addr);
			break;
		}
	default:
		return -EINVAL;
	}

	if (retval != 0) {
		return -EINVAL;
	}

	return 0;
}

/*
 * ********************************************************************
 * FUNCTION : Read MEM Space 
 * arg:
 * addr  	 : Read Mem Addr
 * read_buf   	 : Output Buffer
 * size     	 : Length of Reading Data  
 * ********************************************************************
 */
static int
ch36x_mem_read_block(unsigned long addr, unsigned long read_buf,
		     unsigned long size)
{
	int retval = 0;

	dbg(" \taddr:0x%x", (unsigned int) addr);
	dbg(" \tread_buf:0x%x", (unsigned int) read_buf);
	dbg(" \tsize:0x%x", (unsigned int) size);

#ifdef CH365_WCH_CHIP_SUPPORT
	if (size < 0x00 || size > CH365_MEM_SPACE_RANGE)
		return -1;
#endif

#ifdef CH368_WCH_CHIP_SUPPROT
	if (size < 0x00 || size > CH368_MEM_SPACE_RANGE)
		return -1;
#endif

	retval = copy_to_user((char __user *) read_buf, (char *) addr, size);
	if (retval != 0) {
		dbg(" \tcopy_to_use error retval:%d, line:%d", retval,
		    __LINE__);
		return -1;
	}

	return 0;
}

/*
 * ********************************************************************
 * FUNCTION : Write MEM Space 
 * arg:
 * addr  	 : Write Mem Addr
 * write_buf   	 : Iutput Buffer
 * size     	 : Length of Writing Data  
 * ********************************************************************
 */
static int
ch36x_mem_write_block(unsigned long addr, unsigned long write_buf,
		      unsigned long size)
{
	int retval = 0;

	dbg(" \taddr:0x%x", (unsigned int) addr);
	dbg(" \twrite_buf:0x%x", (unsigned int) write_buf);
	dbg(" \tsize:0x%x", (unsigned int) size);

#ifdef CH365_WCH_CHIP_SUPPORT
	if (size < 0x00 || size > CH365_MEM_SPACE_RANGE)
		return -1;
#endif

#ifdef CH368_WHC_CHIP_SUPPORT
	if (size < 0x00 || size > CH365_MEM_SPACE_RAGNE)
		return -1;
#endif

	retval = copy_from_user((char *) addr, (char __user *) write_buf, size);
	if (retval != 0) {
		dbg(" \tcopy_from_user error retval:%d, line:%d", retval,
		    __LINE__);
		return -1;
	}

	return 0;
}

/*
 * ********************************************************************
 * FUNCTION : Read I/O Space 
 * arg:
 * addr  	 : Read I/O Addr
 * read_buf   	 : Output buffer
 * size     	 : Length of Reading I/O  
 * ********************************************************************
 */
static int
ch36x_io_read_block(unsigned long addr, unsigned long read_buf,
		    unsigned long size)
{
	int retval = 0;
	int i = 0;
	unsigned char read_byte;

	dbg("=======\tIO_READ_BLOCK======");
	dbg("---\taddr:0x%x", (unsigned int) addr);
	dbg("---\tread_buf:0x%x", (unsigned int) read_buf);
	dbg("---\tsize:0x%x\n", (unsigned int) size);

#ifdef CH365_WCH_CHIP_SUPPORT
	if (size < 0x00 || size > CH365_IO_SPACE_RANGE)
		return -1;
#endif

#ifdef CH368_WHC_CHIP_SUPPORT
	if (size < 0x00 || size > CH368_IO_SPACE_RANGE)
		return -1;
#endif

	for (i = 0; i < size; i++) {
		read_byte = READ8( addr + i );
		retval = put_user(read_byte, (((char __user *) read_buf) + i));
		dbg("+++++++++++ i=%d read_byte :%x", i,
		    (unsigned int) read_byte);
	}

	return 0;
}

/*
 * ********************************************************************
 * FUNCTION : Write I/O Space 
 * arg:
 * addr  	 : Write I/O Addr
 * write_buf   	 : Iutput buffer
 * size     	 : Length of Writing I/O  
 * ********************************************************************
 */
static int
ch36x_io_write_block(unsigned long addr, unsigned long write_buf,
		     unsigned long size)
{
	int retval = 0;
	int i = 0;
	unsigned char write_byte;

	dbg("=======\tIO_WRITE_BLOCK======");
	dbg("---\taddr:0x%x", (unsigned int) addr);
	dbg("---\twrite_byte:0x%x", (unsigned int) write_buf);
	dbg("---\tsize:0x%x", (unsigned int) size);

#ifdef CH365_WCH_CHIP_SUPPORT
	if (size < 0x00 || size > CH365_IO_SPACE_RANGE)
		return -1;
#endif

#ifdef CH367_WCH_CHIP_SUPPORT
	if (size < 0x00 || size > CH367_IO_SPACE_RANGE)
		return -1;
#endif
#ifdef CH368_WCH_CHIP_SUPPORT
	if (size < 0x00 || size > CH368_IO_SPACE_RANGE)
		return -1;
#endif

	for (i = 0; i < size; i++) {
		retval = get_user(write_byte, (char __user *) write_buf + i);
		WRITE8(write_byte, addr + i);
		dbg("+++++++++++ i=%d write_byte :%x", i,
		    (unsigned int) write_byte);
	}

	return 0;
}

//interrupter handler for CH365 chips
static irqreturn_t
ch365_interrupt_handler(int irq, void *dev_id)
{
	unsigned char read_byte;
	struct ch36x_dev_private *tmp = (struct ch36x_dev_private *) dev_id;

	//check out 
	pci_read_config_byte(tmp->ch36x_pdev, CH365_OFFSET_INT_ACTIVE_STAT,
			     &read_byte);
	if (!(read_byte & 0x04)) {
		return IRQ_NONE;
	}

	dbg("############read_byte:0x%x, (read_byte & 0xfb):0x%x",
	    (unsigned int) read_byte, (unsigned int) (read_byte & 0xfb));
	dbg("############interrupter had occured on CH365 chips");

	/*
	 *icount ++;
	 *printk( KERN_INFO "icount:%d\n", icount);
	 */

	tmp->poll_flag = 1;
	wake_up(&ch36x_poll_queue);
	/*wake_up_interruptible(&ch36x_poll_queue);*/

	//clear interrupt register
	pci_write_config_byte(tmp->ch36x_pdev, CH365_OFFSET_INT_ACTIVE_STAT,
			      (read_byte & 0xfb));

	return IRQ_HANDLED;

}

//interrupter handler for CH367 chips
int cout;
static irqreturn_t
ch367_interrupt_handler(int irq, void *dev_id)
{
	unsigned char read_byte;
	struct ch36x_dev_private *tmp = (struct ch36x_dev_private *) dev_id;

	
	//check out
	read_byte = READ8(tmp->io_virts_addr +
		    CH367_OFFSET_INT_ACTIVE_STAT);
	if (!(read_byte & 0x04))	//register MICSR-bit:2
		return IRQ_NONE;

	dbg("**********interrupt,Line : %d, cout : %d *********",__LINE__,cout++);   //lv

	tmp->poll_flag = 1;
	wake_up(&ch36x_poll_queue);

	WRITE8((read_byte & 0xfb), tmp->io_virts_addr +
			 CH367_OFFSET_INT_ACTIVE_STAT);
	return IRQ_HANDLED;
}

//interrupter handler for CH368 chips
static irqreturn_t
ch368_interrupt_handler(int irq, void *dev_id)
{
	unsigned char read_byte;
	struct ch36x_dev_private *tmp = (struct ch36x_dev_private *) dev_id;

	
	//check out
	read_byte = READ8(tmp->io_virts_addr +
		    CH367_OFFSET_INT_ACTIVE_STAT);
	if (!(read_byte & 0x04))
		return IRQ_NONE;

	dbg("**********interrupt,Line : %d, cout : %d *********",__LINE__,cout++);   //lv

	tmp->poll_flag = 1;
	wake_up(&ch36x_poll_queue);

	WRITE8((read_byte & 0xfb), tmp->io_virts_addr +
			 CH368_OFFSET_INT_ACTIVE_STAT);

	return IRQ_HANDLED;
}

int
ch36x_fops_open(struct inode *ch36x_inode, struct file *ch36x_file)
{
	int retval = 0;

#if (LINUX_VERSION_CODE < KERNEL_VERSION(2,6,35))
	unsigned int minor = iminor(ch36x_inode);
#else
	unsigned int minor = iminor(ch36x_file->f_path.dentry->d_inode);
#endif
	unsigned char read_byte;

	struct list_head *pos;
	struct list_head *pos_tmp;
	struct ch36x_dev_private *ch36x_dev_private_tmp;

	unsigned char CH365_INT_OPEN_FLAG = 0x00;
	unsigned char CH367_INT_OPEN_TYPE = 0x00;
	unsigned char CH368_INT_OPEN_TYPE = 0x00;	

	list_for_each_safe(pos, pos_tmp, &g_private_head) {
		ch36x_dev_private_tmp =
		    list_entry(pos, struct ch36x_dev_private, ch36x_dev_list);
		if (minor == MINOR(ch36x_dev_private_tmp->ch36x_dev)) {
			break;
		}
	}
	if (pos == &g_private_head) {
		dbg("Fun:%s Can't find minor:0x%x line:%d", __FUNCTION__, minor,
		    __LINE__);
		return -ENODEV;
	}
	 read_byte = READ8( ch36x_dev_private_tmp->io_virts_addr +
                           CH367_OFFSET_INT_FUN_STAT );
         dbg("------- interrupt_handle CH367 read_byte:0x%x,LINE : %d",read_byte,__LINE__);
	 WRITE8(((read_byte & 0xe0) | 0xec),ch36x_dev_private_tmp->io_virts_addr +
			CH367_OFFSET_INT_FUN_STAT);	

	dbg("->ch36x_fops_open minor:%d", minor);
	return 0;
}


int
ch36x_fops_release(struct inode *ch36x_inode, struct file *ch36x_file)
{
	//int retval = 0;
	unsigned char read_byte = 0x00;
	unsigned int minor = iminor(ch36x_inode);

	struct list_head *pos;
	struct list_head *pos_tmp;
	struct ch36x_dev_private *ch36x_dev_private_tmp;

	list_for_each_safe(pos, pos_tmp, &g_private_head) {
		ch36x_dev_private_tmp =
		    list_entry(pos, struct ch36x_dev_private, ch36x_dev_list);
		if (minor == MINOR(ch36x_dev_private_tmp->ch36x_dev)) {
			break;
		}
	}
	if (pos == &g_private_head) {
		dbg("Fun:%s Can't find minor:0x%x line:%d\n", __FUNCTION__,
		    minor, __LINE__);
		return -ENODEV;
	}
	dbg("### CH36x Demo Vendor ID : 0x%x,Device ID : 0x%x",  \
		ch36x_dev_private_tmp->ch36x_pdev->vendor,
		ch36x_dev_private_tmp->ch36x_pdev->device);
	if (CH365_VID == ch36x_dev_private_tmp->ch36x_pdev->vendor &&
	    CH365_DID == ch36x_dev_private_tmp->ch36x_pdev->device) {
		pci_read_config_byte(ch36x_dev_private_tmp->ch36x_pdev,
				     CH365_OFFSET_INT_FUN_STAT, &read_byte);
		dbg("####### interrupt_handle CH365 read_byte:0x%x", read_byte);
		if (!(read_byte & 0x80)) {
			dbg("### Don't enable CH365 interrupter controller");
			goto err_free_irq;
		}
	} else if (CH367_VID == ch36x_dev_private_tmp->ch36x_pdev->vendor &&
		   (CH367_DID_SID_HIGH == ch36x_dev_private_tmp->ch36x_pdev->device
		    || CH367_DID_SID_LOW == ch36x_dev_private_tmp->ch36x_pdev->device)) {
		read_byte = READ8( ch36x_dev_private_tmp->io_virts_addr +
					      CH367_OFFSET_INT_FUN_STAT );
		dbg("######## interrupt_handle CH367 read_byte:0x%x",read_byte);
		if (!(read_byte & 0x02)) {
			dbg("### Don't enable CH367 interrupter controller");
			goto err_free_irq;
		}
	} else if (CH368_VID == ch36x_dev_private_tmp->ch36x_pdev->vendor &&
		   CH368_DID == ch36x_dev_private_tmp->ch36x_pdev->device) {
		read_byte = READ8( ch36x_dev_private_tmp->io_virts_addr +
						  CH368_OFFSET_INT_FUN_STAT );
		dbg("######## interrupt_handle CH368 read_byte:0x%x",read_byte);
		if (!(read_byte & 0x02)) {
			dbg("### Don't enable CH368 interrupter controller");
			goto err_free_irq;
		}
	}
	//free irq above requested
	free_irq(ch36x_dev_private_tmp->irq, (void *) ch36x_dev_private_tmp);
	//printk( "\n####line:%d irq:%d 'free_irq' execute\n", __LINE__, ch36x_dev_private_tmp->irq );

	return 0;

err_free_irq:
	err("ch36x_fops_release: Don't enable device");
	return 0;
}

ssize_t
ch36x_fops_read(struct file * ch36x_file, char __user * to_user,
		size_t read_size, loff_t * file_pos)
{
	return 0;
}

ssize_t
ch36x_fops_write(struct file * ch36x_file, const char __user * from_user,
		 size_t write_size, loff_t * file_pos)
{
	return 0;
}

#if	(LINUX_VERSION_CODE < KERNEL_VERSION(2,6,35))
static int
ch36x_fops_ioctl_do(struct inode *ch36x_node, struct file *ch36x_file,
		    unsigned int ch36x_cmd, unsigned long ch36x_arg)
#else
static int
ch36x_fops_ioctl_do(struct file *ch36x_file, unsigned int ch36x_cmd,
		    unsigned long ch36x_arg)
#endif
{
	int retval = 0;
	unsigned char read_byte_int;
	unsigned char interrupt_state;
	char *drv_version_tmp = DRV_VERSION;
	struct list_head *pos;
	struct list_head *pos_tmp;
	struct ch36x_dev_private *ch36x_dev_private_tmp;

#if (LINUX_VERSION_CODE < KERNEL_VERSION(2,6,35))
	unsigned int minor = iminor(ch36x_node);
#else
	unsigned int minor = iminor(ch36x_file->f_path.dentry->d_inode);
#endif

	list_for_each_safe(pos, pos_tmp, &g_private_head) {
		ch36x_dev_private_tmp =
		    list_entry(pos, struct ch36x_dev_private, ch36x_dev_list);
		if (minor == MINOR(ch36x_dev_private_tmp->ch36x_dev)) {
			break;
		}
	}
	if (pos == &g_private_head) {
		err("Can't find minor:0x%x line:%d", minor, __LINE__);
		return -ENODEV;
	}

	switch (ch36x_cmd) {
	case CH36x_ENABLE_INT:
		{
			if( ch36x_dev_private_tmp->ch36x_pdev->vendor == CH367_VID  &&
					(CH367_DID_SID_HIGH == 
					 ch36x_dev_private_tmp->ch36x_pdev->device ||
					 CH367_DID_SID_LOW ==
					 ch36x_dev_private_tmp->ch36x_pdev->device) ) {
				read_byte_int = READ8( ch36x_dev_private_tmp->io_virts_addr + CH367_OFFSET_INT_FUN_STAT );
				switch(ch36x_arg)
				{
					case 1:
						WRITE8(read_byte_int | TRIGGER_LOW, (ch36x_dev_private_tmp->io_virts_addr + 
							CH367_OFFSET_INT_FUN_STAT));
						interrupt_state = IRQF_TRIGGER_LOW;
						break;
					case 2:
						WRITE8(read_byte_int | TRIGGER_HIGH, (ch36x_dev_private_tmp->io_virts_addr + 							     CH367_OFFSET_INT_FUN_STAT));
						interrupt_state = IRQF_TRIGGER_HIGH;
						break;
					case 3:
						WRITE8(read_byte_int | TRIGGER_RISING, (ch36x_dev_private_tmp->io_virts_addr                                                         + CH367_OFFSET_INT_FUN_STAT));
						interrupt_state = IRQF_TRIGGER_RISING;
						break;
					case 4:
						WRITE8(read_byte_int | TRIGGER_FALLING, (ch36x_dev_private_tmp->io_virts_addr							     + CH367_OFFSET_INT_FUN_STAT));
						interrupt_state = IRQF_TRIGGER_FALLING;
						break;
					default :
						return -EINVAL;
				}
				dbg("CH367_OFFSET_INT_FUN_STAT is 0x%x",read_byte_int);	
				read_byte_int = READ8( ch36x_dev_private_tmp->io_virts_addr + CH367_OFFSET_INT_FUN_STAT );
				WRITE8( (read_byte_int | 0x02), (ch36x_dev_private_tmp->io_virts_addr + CH367_OFFSET_INT_FUN_STAT) );
			//global interrupter enable bit:1
			//Request irq that need to 'free_irq'
			read_byte_int = READ8( ch36x_dev_private_tmp->io_virts_addr + CH367_OFFSET_INT_FUN_STAT );
			dbg("------- interrupt_handle CH367 read_byte:0x%x,LINE : %d",read_byte_int,__LINE__);
			retval = request_irq( ch36x_dev_private_tmp->irq,
						ch367_interrupt_handler,IRQF_SHARED | interrupt_state,
						ch36x_dev_private_tmp->dev_file_name,
						(void *)ch36x_dev_private_tmp);
				if( retval != 0 )
					err( "-->ioctl CH36x_ENABLE_INT ch367 error\n");
			}
			else if(ch36x_dev_private_tmp->ch36x_pdev->vendor == CH368_VID &&
					CH368_DID == ch36x_dev_private_tmp->ch36x_pdev->device ) {
				read_byte_int = READ8( ch36x_dev_private_tmp->io_virts_addr + CH368_OFFSET_INT_FUN_STAT );
				
				WRITE8( (read_byte_int | 0x02), (ch36x_dev_private_tmp->io_virts_addr + CH368_OFFSET_INT_FUN_STAT) );
				
				
				ch36x_dev_private_tmp->ch368_int_enable = 0x08; //low level trigger
				retval = request_irq( ch36x_dev_private_tmp->irq,
						ch368_interrupt_handler, IRQF_SHARED,
						ch36x_dev_private_tmp->dev_file_name,
						(void *)ch36x_dev_private_tmp);
				if( retval != 0 )
					err( "-->ioctl CH36x_ENABLE_INT ch368 error\n");
			}
			else
				err( "-->Can't enable CH365 device, operation invalid\n");
			
			dbg("CH36x Interrupt enable, line :%d",__LINE__);   //lv
			break;
		}
		case CH36x_DISABLE_INT:
		{
			if( (ch36x_dev_private_tmp->ch36x_pdev->vendor == CH367_VID) || 
					(ch36x_dev_private_tmp->ch36x_pdev->vendor == CH368_VID) ) {
				read_byte_int = 
					READ8( ch36x_dev_private_tmp->io_virts_addr + CH367_OFFSET_INT_FUN_STAT );
				WRITE8( (read_byte_int & (~( 1 << 2 ))), 
						(ch36x_dev_private_tmp->io_virts_addr + CH367_OFFSET_INT_FUN_STAT) );
			}
			else if(ch36x_dev_private_tmp->ch36x_pdev->vendor == CH368_VID &&
					CH368_DID == ch36x_dev_private_tmp->ch36x_pdev->device ) {
				read_byte_int = 
					READ8( ch36x_dev_private_tmp->io_virts_addr + CH368_OFFSET_INT_FUN_STAT );
				WRITE8( (read_byte_int & (~( 1 << 2 ))), 
						(ch36x_dev_private_tmp->io_virts_addr + CH368_OFFSET_INT_FUN_STAT) );
				
			} else {
				err( "-->Can't disable CH365 device, operation invalid\n");
			}
			dbg("CH36x Interrupt disable, line : %d", __LINE__);
			break;
		}
	case CH36x_GET_DRV_VERSION:
		{
			/*retval = copy_to_user( (char __user *)read_buf, (char *)addr, size ); */
			retval =
			    copy_to_user((char __user *) (ch36x_arg),
					 (char *) drv_version_tmp,
					 strlen(DRV_VERSION));
			/*retval = put_user( (unsigned long)drv_version_tmp, (char  __user *)(ch36x_arg) ); */
			break;
		}
	case CH36x_GET_IO_BASE_ADDR:
		{
			//retval = ch36x_cfg_read( SIZE_DWORD, OFFSET_IO_BASE_ADDR, ch36x_arg ); //It's right, phy address
			//retval = put_user( (unsigned long)private->io_virts_addr, (long __user *)(ch36x_arg) );        //virts address
			retval = put_user((unsigned long) ch36x_dev_private_tmp->io_virts_addr, 
					(long __user *) (ch36x_arg));	//virts address
			break;
		}
	case CH36x_SET_IO_BASE_ADDR:
		{
			break;
		}
	case CH36x_GET_MEM_BASE_ADDR:
		{
			//retval = ch36x_cfg_read( SIZE_DWORD, OFFSET_MEM_BASE_ADDR, ch36x_arg );       //It's right, phy address
			retval = put_user((unsigned long) ch36x_dev_private_tmp->mem_virts_addr, 
					(long __user *) (ch36x_arg));	//virts address
			break;
		}
	case CH36x_SET_MEM_BASE_ADDR:
		{
			break;
		}
	case CH36x_GET_INT_LINE:
		{
			retval =
			    put_user(ch36x_dev_private_tmp->irq,
				     (long __user *) (ch36x_arg));
			dbg("'CH36x_GET_INT_LINE' put_user retval:%d line:%d",
			    retval, __LINE__);
			break;
		}
	case CH36x_SET_INT_LINE:
		{
			retval = ch36x_cfg_write(SIZE_BYTE, OFFSET_INT_LINE,
						 ch36x_arg,
						 ch36x_dev_private_tmp->ch36x_pdev);
			break;
		}
	case CH36x_READ_CONFIG_BYTE:
		{
			retval = ch36x_cfg_read(SIZE_BYTE, *((long *) ch36x_arg),
						(unsigned long)*(((long *) ch36x_arg) + 1),
						ch36x_dev_private_tmp->ch36x_pdev);
			break;
		}
	case CH36x_WRITE_CONFIG_BYTE:
		{
			retval =
			    ch36x_cfg_write(SIZE_BYTE, *((long *) ch36x_arg),
					    (unsigned long) (((long *) ch36x_arg)+ 1),
					    ch36x_dev_private_tmp->ch36x_pdev);
			break;
		}
	case CH36x_READ_CONFIG_WORD:
		{
			retval = ch36x_cfg_read(SIZE_WORD, *((long *) ch36x_arg),
						(unsigned long)*(((long *) ch36x_arg) + 1),
						ch36x_dev_private_tmp->ch36x_pdev);
			break;
		}
	case CH36x_WRITE_CONFIG_WORD:
		{
			retval =
			    ch36x_cfg_write(SIZE_WORD, *((long *) ch36x_arg),
					    (unsigned long) (((long *) ch36x_arg)+ 1),
					    ch36x_dev_private_tmp->ch36x_pdev);
			break;
		}
	case CH36x_READ_CONFIG_DWORD:
		{
			retval =
			    ch36x_cfg_read(SIZE_DWORD, *((long *) ch36x_arg),
					   (unsigned long) *(((long *) ch36x_arg)+ 1),
					   ch36x_dev_private_tmp->ch36x_pdev);
			break;
		}
	case CH36x_WRITE_CONFIG_DWORD:
		{
			retval =
			    ch36x_cfg_write(SIZE_DWORD, *((long *) ch36x_arg),
					    (unsigned long) (((long *) ch36x_arg)+ 1),
					    ch36x_dev_private_tmp->ch36x_pdev);
			break;
		}
	case CH36x_READ_IO_BYTE:
		{
			dbg(" 'CH36x_READ_IO_BYTE' CMD handle case ");
			retval =
			    ch36x_io_read(SIZE_BYTE, *((long *) ch36x_arg),
					  (unsigned long) *(((long *) ch36x_arg)+ 1));
			break;
		}
	case CH36x_READ_IO_WORD:
		{
			dbg(" 'CH36x_READ_IO_WORD' CMD handle case ");
			retval =
			    ch36x_io_read(SIZE_WORD, *((long *) ch36x_arg),
					  (unsigned long) *(((long *) ch36x_arg)+ 1));
			break;
		}
	case CH36x_READ_IO_DWORD:
		{
			dbg(" 'CH36x_READ_IO_DWORD' CMD handle case ");
			retval =
			    ch36x_io_read(SIZE_DWORD, *((long *) ch36x_arg),
					  (unsigned long) *(((long *) ch36x_arg)+ 1));
			break;
		}
	case CH36x_WRITE_IO_BYTE:
		{
			dbg(" 'CH36x_WRITE_IO_BYTE' CMD handle case ");
			retval =
			    ch36x_io_write(SIZE_BYTE, *((long *) ch36x_arg),
					   (unsigned long) (((long *) ch36x_arg)+ 1));
			break;
		}
	case CH36x_WRITE_IO_WORD:
		{
			dbg(" 'CH36x_WRITE_IO_WORD' CMD handle case ");
			retval =
			    ch36x_io_write(SIZE_WORD, *((long *) ch36x_arg),
					   (unsigned long) (((long *) ch36x_arg)
							    + 1));
			break;
		}
	case CH36x_WRITE_IO_DWORD:
		{
			dbg(" 'CH36x_WRITE_IO_DWORD' CMD handle case ");
			retval =
			    ch36x_io_write(SIZE_DWORD, *((long *) ch36x_arg),
					   (unsigned long) (((long *) ch36x_arg)+ 1));
			break;
		}
	case CH36x_READ_MEM_BYTE:
		{
			dbg(" 'CH36x_READ_MEM_BYTE' CMD handle case");
			retval =
			    ch36x_mem_read(SIZE_BYTE, *((long *) ch36x_arg),
					   (unsigned long) *(((long *) ch36x_arg)+ 1));
			break;
		}
	case CH36x_READ_MEM_WORD:
		{
			dbg(" 'CH36x_READ_MEM_WORD' CMD handle case");
			retval =
			    ch36x_mem_read(SIZE_WORD, *((long *) ch36x_arg),
					   (unsigned long) *(((long *) ch36x_arg)+ 1));
			break;
		}
	case CH36x_READ_MEM_DWORD:
		{
			dbg(" 'CH36x_READ_MEM_DWORD' CMD handle case");
			retval =
			    ch36x_mem_read(SIZE_DWORD, *((long *) ch36x_arg),
					   (unsigned long) *(((long *) ch36x_arg)+ 1));
			break;
		}
	case CH36x_WRITE_MEM_BYTE:
		{
			dbg(" 'CH36x_WRITE_MEM_BYTE' CMD handle case");
			retval =
			    ch36x_mem_write(SIZE_BYTE, *((long *) ch36x_arg),
					    (unsigned long) (((long *) ch36x_arg)+ 1));
			break;
		}
	case CH36x_WRITE_MEM_WORD:
		{
			dbg(" 'CH36x_WRITE_MEM_WORD' CMD handle case");
			retval =
			    ch36x_mem_write(SIZE_WORD, *((long *) ch36x_arg),
					    (unsigned long) (((long *) ch36x_arg)+ 1));
			break;
		}
	case CH36x_WRITE_MEM_DWORD:
		{
			dbg(" 'CH36x_WRITE_MEM_DWORD' CMD handle case");
			retval =
			    ch36x_mem_write(SIZE_DWORD, *((long *) ch36x_arg),
					    (unsigned long) (((long *) ch36x_arg)+ 1));
			break;
		}
	case CH36x_READ_MEM_BLOCK:
		{
			dbg(" 'CH36x_READ_MEM_BLOCK' CMD handle case");
			retval =
			    ch36x_mem_read_block((unsigned long)
						 *((long *) ch36x_arg),
						 (unsigned long)*(((long *) ch36x_arg) + 1),
						 *(((long *) ch36x_arg) + 2));
			break;
		}
	case CH36x_WRITE_MEM_BLOCK:
		{
			dbg(" 'CH36x_WRITE_MEM_BLOCK' CMD handle case");
			retval =
			    ch36x_mem_write_block((unsigned long)
						  *((long *) ch36x_arg),
						  (unsigned long)*(((long *) ch36x_arg) + 1),
						  *(((long *) ch36x_arg) + 2));
			break;
		}
	case CH36x_READ_IO_BLOCK:
		{
			dbg(" 'CH36x_READ_IO_BLOCK' CMD handle case");
			retval =
			    ch36x_io_read_block((unsigned long)
						*((long *) ch36x_arg),
						(unsigned long)*(((long *) ch36x_arg) + 1),
						*(((long *) ch36x_arg) + 2));
			break;
		}
	case CH36x_WRITE_IO_BLOCK:
		{
			dbg(" 'CH36x_WRITE_IO_BLOCK' CMD handle case");
			retval =
			    ch36x_io_write_block((unsigned long)
						 *((long *) ch36x_arg),
						 (unsigned long)*(((long *) ch36x_arg) + 1),
						 *(((long *) ch36x_arg) + 2));
			break;
		}
	default:
		return -EINVAL;
	}

	if (retval != 0) {
		return -EINVAL;
	}

	return 0;
}

#if (LINUX_VERSION_CODE < KERNEL_VERSION(2,6,35))
int
ch36x_fops_ioctl(struct inode *ch36x_node, struct file *ch36x_file,
		 unsigned int ch36x_cmd, unsigned long ch36x_arg)
#else
long
ch36x_fops_ioctl(struct file *ch36x_file, unsigned int ch36x_cmd,
		 unsigned long ch36x_arg)
#endif
{
	int retval = 0;

#if ( LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,35) )
	mutex_lock(&ch36x_fops_ioctl_mutex);
#endif

#if ( LINUX_VERSION_CODE < KERNEL_VERSION(2,6,35) )
	retval = ch36x_fops_ioctl_do(ch36x_node, ch36x_file, ch36x_cmd, ch36x_arg);
#else
	retval = ch36x_fops_ioctl_do(ch36x_file, ch36x_cmd, ch36x_arg);
#endif

#if ( LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,35) )
	mutex_unlock(&ch36x_fops_ioctl_mutex);
#endif

	return retval;
}

//unsigned int ch36x_fops_poll( struct file *ch36x_file, struct poll_table_struct *wait )
#if (LINUX_VERSION_CODE < KERNEL_VERSION(2,6,35))
unsigned int ch36x_fops_poll(struct inode *ch36x_node, struct file *ch36x_file, struct poll_table_struct *wait )
#else
unsigned int ch36x_fops_poll(struct file *ch36x_file, struct poll_table_struct *wait )
#endif

{
	unsigned int mask = 0;

	struct list_head *pos;
	struct list_head *pos_tmp;
	struct ch36x_dev_private *ch36x_dev_private_tmp;
//	unsigned int minor = iminor(ch36x_file->f_path.dentry->d_inode);
	#if (LINUX_VERSION_CODE < KERNEL_VERSION(2,6,35))
		unsigned int minor = iminor(ch36x_node);
	#else
 		unsigned int minor = iminor(ch36x_file->f_path.dentry->d_inode);
	#endif


	list_for_each_safe(pos, pos_tmp, &g_private_head) {
		ch36x_dev_private_tmp = 
			list_entry(pos, struct ch36x_dev_private, ch36x_dev_list);
		if( minor == MINOR(ch36x_dev_private_tmp->ch36x_dev)) {
			break;
		}
	}
	if(pos == &g_private_head) {
		dbg("Fun:%s Can't find minor:0x%x line:%d", __FUNCTION__, minor,
				__LINE__);
		return -ENODEV;
	}

	poll_wait(ch36x_file, &ch36x_poll_queue, wait);

	if( ch36x_dev_private_tmp->poll_flag ) {
		/*mask |= POLLIN | POLLRDNORM；*/
		mask |= POLLIN;
		ch36x_dev_private_tmp->poll_flag = 0;
	}

	return mask;
}

static const struct file_operations ch36x_fops = {
	.owner = THIS_MODULE,
	.open = ch36x_fops_open,
	.release = ch36x_fops_release,
	.read = ch36x_fops_read,
	.write = ch36x_fops_write,
#if	(LINUX_VERSION_CODE < KERNEL_VERSION(2,6,35))	// Kernel version < 2.6.35
	.ioctl = ch36x_fops_ioctl,
#else
	.unlocked_ioctl = ch36x_fops_ioctl,
	//kernel version > 2.6.38 used to 'unlocked_ioctl' function
#endif
	.poll  = ch36x_fops_poll,
};

static int
ch36x_register_chrdev(struct ch36x_dev_private *private)
{
	int retval = 0;
	dev_t ch36x_dev_tmp = 0;
	char dev_file_name[20] = { '\0' };

	//register CH36x driver
	if (CH36x_MAJOR) {
		dbg("register_chrdev_region function");

		//private->ch36x_dev = MKDEV( CH36x_MAJOR, CH36x_MINOR );
		private->ch36x_dev = MKDEV(CH36x_MAJOR, CH36x_MINOR + g_dev_count);	//to support multi devices 
		ch36x_dev_tmp = private->ch36x_dev;
		//retval = register_chrdev_region( private->ch36x_dev, CH36x_DEV_NUM, DRV_NAME );
	} else {
		dbg("alloc_chrdev_region function");

		retval =
		    alloc_chrdev_region(&ch36x_dev_tmp, CH36x_MINOR,
					CH36x_DEV_NUM, DRV_NAME);
		CH36x_MAJOR = MAJOR(ch36x_dev_tmp);
		CH36x_MINOR = MINOR(ch36x_dev_tmp);
		private->ch36x_dev = ch36x_dev_tmp;

		//dynamically create device node
		ch36x_class = class_create(THIS_MODULE, "ch36x_class");
		if (IS_ERR(ch36x_class)) {
			err("failed in creating ch36x_class");
			//retval = ERR_CLASS_CREATE;
			goto err_class_create;
		}
	}
	if (retval < 0) {
		err("ch36x can't get major: %d", CH36x_MAJOR);
		err("ch36x can't get minor: %d", CH36x_MINOR);
		goto err_register_chrdev_region;
	} else {
		dbg("register success Major:%d, Minor:%d", CH36x_MAJOR,
		    CH36x_MINOR);
	}

	//setup cdev
	cdev_init(&private->ch36x_cdev, &ch36x_fops);
	private->ch36x_cdev.owner = THIS_MODULE;	//???
	retval = cdev_add(&private->ch36x_cdev, private->ch36x_dev, 1);
	if (retval < 0) {
		err("error 'cdev_add' retval:%d", retval);
		goto err_cdev_add;
		//return retval;
	}

	memset(dev_file_name, '\0', sizeof (dev_file_name));
	sprintf(dev_file_name, "%s%c", DRV_NAME, '0' + g_dev_count);
	memcpy(private->dev_file_name, dev_file_name, strlen(dev_file_name));
	dbg("-----------dev_file_name:%s", dev_file_name);
	dbg("-----------memcpy->dev_file_name:%s", private->dev_file_name);

	//register our own device in sysfs, and this will cause udevd to create corresponding device node
#if ( (LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,17)) && (LINUX_VERSION_CODE < KERNEL_VERSION(2,6,38)) )
	device_create(ch36x_class, NULL, private->ch36x_dev, "%s",
		      dev_file_name);
#elif ( LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,38) )
	device_create(ch36x_class, NULL, private->ch36x_dev, NULL, "%s",
		      dev_file_name);
#endif

	return 0;

err_cdev_add:
	cdev_del(&private->ch36x_cdev);
err_class_create:
	class_destroy(ch36x_class);
err_register_chrdev_region:
	//unregister_chrdev_region( MKDEV(CH36x_MAJOR,CH36x_MINOR), 1 );
	//as if this function can't be used 
	unregister_chrdev_region(private->ch36x_dev, CH36x_DEV_NUM);

	return -1;
}

static void
device_init_buf(struct ch36x_dev_private *private)
{
	//depends on hardware
	return;
}

static void
device_hw_start(struct ch36x_dev_private *private)
{
	//depends on hardware
	return;
}

#ifdef DEBUG
static void 
ch36x_dbg_probe(struct pic_dev *pdev, const struct pci_device_id *ent)
{
	unsigned short vendor, device;
	unsigned int addr1, addr2, addr3, addr4, addr5, addr6;
	unsigned int ROM_addr;
	unsigned int irq_addr;
	//unsigned int irq_addr_tmp;
	unsigned char read_enable_irq;
	unsigned char read_active_irq;

	char str[20] = { '\0' };
	dbg("--------------------g_dev_count------------------");
	dbg("--------g_dev_count:0x%x", g_dev_count);
	sprintf(str, "just a test str %c", '0' + g_dev_count);
	dbg("--------g_dev_count str:%s", str);

	// read VID and DID
	pci_read_config_word(pdev, 0, &vendor);
	pci_read_config_word(pdev, 2, &device);
	printk(KERN_INFO "VID:%x, PID:%x \n", vendor, device);

	// read I/O address regions
	//pci_read_config_dword( pdev, 16, &addr1 );
	pci_read_config_dword(pdev, 0x10, &addr1);
	pci_read_config_dword(pdev, 20, &addr2);
	pci_read_config_dword(pdev, 24, &addr3);
	pci_read_config_dword(pdev, 28, &addr4);
	pci_read_config_dword(pdev, 32, &addr5);
	pci_read_config_dword(pdev, 36, &addr6);
	printk(KERN_INFO "I/O address regions:\n%x, %x, %x, %x, %x, %x \n",
	       addr1, addr2, addr3, addr4, addr5, addr6);

	// read Extended ROM based address
	pci_read_config_dword(pdev, 48, &ROM_addr);
	printk(KERN_INFO "Extended ROM based address:\n%x \n", ROM_addr);

	// read irq-line and irq-pin
	pci_read_config_dword(pdev, 60, &irq_addr);
	printk(KERN_INFO "irq_addr:%x\n", irq_addr);
	/*
	 *pci_write_config_byte( pdev, 60, 0x55);
	 *pci_read_config_dword( pdev, 60, &irq_addr_tmp );
	 *printk( KERN_INFO "irq_addr_tmp:%x\n", irq_addr_tmp );
	 */
	pci_read_config_byte(pdev, 0x42, &read_enable_irq);
	printk(KERN_INFO "########read_enable_irq:0x%x\n",
	       (unsigned int) read_enable_irq);
	pci_read_config_byte(pdev, 0x40, &read_active_irq);
	printk(KERN_INFO "########read_active_irq:0x%x\n",
	       (unsigned int) read_active_irq);
	pci_write_config_byte(pdev, 0x40, (read_active_irq | 0x04));	//write interrupter register status

	return;
}
#else
static void 
ch36x_dbg_probe( struct pci_dev *pdev, const struct pci_device_id *ent)
{
	return;
}
#endif

#if	(LINUX_VERSION_CODE < KERNEL_VERSION(3,6,10))
static int __devinit
#else
static int
#endif
ch36x_pci_probe(struct pci_dev *pdev, const struct pci_device_id *ent)
{
	int retval = 0;
	static struct ch36x_dev_private *private = NULL;

	unsigned long mmio_start;
	unsigned long mmio_end;
	unsigned long mmio_flags;
	unsigned long mmio_len;
	unsigned long *ioaddr1 = NULL;
	//struct ch36x_dev *dev = NULL;

	unsigned long mmio_start_mem;
	unsigned long mmio_end_mem;
	unsigned long mmio_flags_mem;
	unsigned long mmio_len_mem;
	unsigned long *ioaddr1_mem = NULL;

	ch36x_dbg_probe(pdev, ent);

	//enable ch36x devices
	if (pci_enable_device(pdev)) {
		err("%s: Can't enable device", pci_name(pdev));
		/*return -EIO; */
		return -ENODEV;
		goto err_pci_enable_device;
	}
	/*
	 * //set dma flags of ch36x
	 *if( pci_set_dma_mask( pdev, CH36x_DMA_MASK ) )
	 *    return -ENODEV;
	 */
	//allocate memory from kernel space
	if ((private =
	     kmalloc(sizeof (struct ch36x_dev_private), GFP_KERNEL)) == NULL) {
		err("ch36x pci kmalloc error");
		return -ENOMEM;
		goto err_kmalloc;
	}

	memset(private, 0, sizeof (*private));
	private->ch36x_pdev = pdev;

	mmio_start = pci_resource_start(pdev, 0);
	mmio_end = pci_resource_end(pdev, 0);
	mmio_flags = pci_resource_flags(pdev, 0);
	mmio_len = pci_resource_len(pdev, 0);

	mmio_start_mem = pci_resource_start(pdev, 1);
	mmio_end_mem = pci_resource_end(pdev, 1);
	mmio_flags_mem = pci_resource_flags(pdev, 1);
	mmio_len_mem = pci_resource_len(pdev, 1);

	/*if( !(mmio_flags & IORESOURCE_MEM) ) */
	if (!(mmio_flags & IORESOURCE_IO))	// CH36x_PCI IO regions 
	{
		err("can not find proper PCI device IO base address, aborting.");
		retval = -ENODEV;
		goto err_out;
	}

	if (!(mmio_flags_mem & IORESOURCE_MEM))	//CH36x_PCI IO memory regions
	{
		err("can not find proper PCI devcie memory address (IO space), aborint.");
		retval = -ENODEV;
		goto err_out;
	}
	//marking PCI region, indicate this region has been assigned
	retval = pci_request_regions(pdev, DRV_NAME);
	if (retval)
		goto err_pci_request_regions;

	/*
	 * //setting master DMA mode of bus
	 *pci_set_master( pdev );
	 */

	//'ioremap' remap physical address region to virtual address region of CPU, than can be used by kernel
	ioaddr1 = ioport_map(mmio_start, mmio_len);	//need to release by function 'ioport_unmap'
	if (ioaddr1 == NULL) {
		err("%s:can not ioport_map mmio, aborting", pci_name(pdev));
		//retval = -EIO;
		goto err_ioport_map;
	}
	dbg("--->>ioaddr1 = 0x%p", ioaddr1);

	ioaddr1_mem = ioremap(mmio_start_mem, mmio_len_mem);
	if (ioaddr1_mem == NULL) {
		err("%s:can not ioremap device memory( IO space), aborting",
		    pci_name(pdev));
		//retval = -EIO;
		goto err_ioremap;
	}
	dbg("ioaddr1_mem = 0x%p", ioaddr1_mem);

	private->ioaddr = (void __iomem *) mmio_start;	//IO ports
	private->io_virts_addr = (unsigned long) ioaddr1;
	private->io_len = mmio_len;

	private->memaddr = (void __iomem *) mmio_start_mem;	//memory of device
	private->mem_virts_addr = (unsigned long) ioaddr1_mem;
	private->mem_len = mmio_len_mem;

	private->irq = pdev->irq;

	dbg("private->ioaddr: %p", private->ioaddr);
	dbg("private->io_virts_addr: 0x%x",
	    (unsigned int) private->io_virts_addr);
	dbg("private->io_len: 0x%x", (unsigned int) private->io_len);
	dbg("private->memaddr: %p", private->memaddr);
	dbg("private->mem_virts_addr: 0x%x",
	    (unsigned int) private->mem_virts_addr);
	dbg("private->mem_len: 0x%x", (unsigned int) private->mem_len);
	dbg("irq is: %d", pdev->irq);

	//initialize spinlock
	/*
	 *spin_lock_init( &private->lock_send );
	 *spin_lock_init( &private->lock_recv );
	 */
	if (ch36x_register_chrdev(private)) {
		err("ch36x_register_chrdev fail");
		goto err_ch36x_register_chrdev;
	}
	//followed two function need to handle by real hardware
	device_init_buf(private);	//allocate memory, memory remap, obtain irq number
	device_hw_start(private);	//write value to registers, reset hardware, open irq, open DMA

	//put address of device pointer in device pointer of PCI device, so that call 'pci_get_drvdata' at laster
	pci_set_drvdata(pdev, private);

	list_add_tail(&(private->ch36x_dev_list), &g_private_head);	//add member into 'g_private_head' list
	g_dev_count++;		//It's very important for many parts

	dbg("-->ch36x_pci_probe function finshed.");
	return 0;

err_ch36x_register_chrdev:
err_ioremap:
	iounmap((void *) private->mem_virts_addr);
err_ioport_map:
	ioport_unmap((void __iomem *) private->io_virts_addr);
err_pci_request_regions:
	pci_release_regions(pdev);
err_out:
err_kmalloc:
	kfree(private);
err_pci_enable_device:
	pci_disable_device(pdev);

	err(" ch36x_pci_probe error ");
	//resource_cleanup_dev( private );
	return -1;
}

#if (LINUX_VERSION_CODE < KERNEL_VERSION(3,6,10))
// '__devexit' for supporting hotplug
static void __devexit
#else
static void
#endif
ch36x_pci_remove(struct pci_dev *pdev)
{
	//to reserved
	dbg("@@@@@@@@@@@ enter: ch36x_pci_remove");
	return;
}

static struct pci_driver ch36x_pci_driver = {
	.name = DRV_NAME,
	.id_table = ch36x_id_table,
	.probe = ch36x_pci_probe,
	.remove = ch36x_pci_remove,
#ifdef CONFIG_PM
	.suspend = ch36x_pci_suspend,
	.resume = ch36x_pci_resume,
#endif				
	/*end CONFIG_PM */
};


static int __init
ch36x_init(void)
{
	int retval = 0;

	printk(KERN_INFO "-->WCH ch36x dirver init %s, %s\n", __DATE__,
	       __TIME__);
//	printk(KERN_INFO "-->%s\n", DRV_VERSION);
	printk(KERN_ALERT "-->%s\n", DRV_VERSION);

	INIT_LIST_HEAD(&g_private_head);	//initializer the header of list

	retval = pci_register_driver(&ch36x_pci_driver);
	if (retval >= 0)
		return retval;
	else {
		err("pci_register_driver error retval:%d line:%d", retval,
		    __LINE__);
		goto err_out;
	}

err_out:
	pci_unregister_driver(&ch36x_pci_driver);

	return 0;
}


static void __exit
ch36x_exit(void)
{
	struct list_head *pos;
	struct list_head *pos_tmp;
	struct ch36x_dev_private *ch36x_dev_private_tmp;

	list_for_each_safe(pos, pos_tmp, &g_private_head) {
		//to release resources of ch36x_register_chrdev function
		ch36x_dev_private_tmp =
		    list_entry(pos, struct ch36x_dev_private, ch36x_dev_list);

		device_destroy(ch36x_class, ch36x_dev_private_tmp->ch36x_dev);	//delete device file
		cdev_del(&ch36x_dev_private_tmp->ch36x_cdev);
		//here unregister a signal device when occured hotplug, to release device number(major and minor)
		unregister_chrdev_region(ch36x_dev_private_tmp->ch36x_dev, 1);

		//to release resources of probe function
		iounmap((void *) ch36x_dev_private_tmp->mem_virts_addr);
		//iounmap((void *) ch36x_dev_private_tmp->io_virts_addr);
		ioport_unmap((void __iomem *) ch36x_dev_private_tmp->
				 io_virts_addr);
		pci_release_regions(ch36x_dev_private_tmp->ch36x_pdev);
		//here, must to delete 'ch36x_dev_list' list node from list_head
		//maybe here needs mutex_lock for the safe of 'list_for_each_safe'
		list_del(&(ch36x_dev_private_tmp->ch36x_dev_list));
		kfree(ch36x_dev_private_tmp);	//now it's safe because of 'list_del' above
		pci_disable_device(ch36x_dev_private_tmp->ch36x_pdev);
	}
	if (list_empty(&g_private_head))
		class_destroy(ch36x_class);

	g_dev_count = 0;
	pci_unregister_driver(&ch36x_pci_driver);

	printk(KERN_INFO "-->WCH ch36x driver exit %s, %s\n", __DATE__,
	       __TIME__);
	return;
}


module_init(ch36x_init);
module_exit(ch36x_exit);
MODULE_AUTHOR("WCH TECH GRP");
MODULE_DESCRIPTION("WCH CH36x PCI,PCIe chips Drivers");
MODULE_LICENSE("Dual BSD/GPL");
