#include <linux/init.h>
#include <linux/module.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/device.h>
#include <linux/slab.h>
#include <linux/uaccess.h>

MODULE_LICENSE("GPL");
static int device_file_major_number = 0;
static const char device_name[] = "hello_driver";

int my_open(struct inode *, struct file *);
int my_release(struct inode *, struct file *);
ssize_t my_read(struct file *, char __user *, size_t , loff_t *);
ssize_t my_write(struct file *, const char __user *, size_t , loff_t *);

struct file_operations fops = {
	.owner=THIS_MODULE,
	.read=my_read,
	.write=my_write,
	.release=my_release,
	.open=my_open,
};
#define BUF_SIZE 100
struct cdd_cdev{
	struct cdev cdev;
	struct device *dev_device;
	u8 led;
	char kbuf[BUF_SIZE];
};

int my_open(struct inode *inode, struct file *file)
{
	struct cdd_cdev *pcdevp = NULL;
	printk("my_open() execute successfully!\n");
	pcdevp = container_of(inode->i_cdev, struct cdd_cdev, cdev);
	printk("led = %d\n", pcdevp->led);
	file->private_data = pcdevp;
	return 0;
}

int my_release(struct inode *inode, struct file *file)
{
	printk("my_release() execute successfully!\n");
	return 0;
}

ssize_t my_read (struct file *file, char __user *buf, size_t count, loff_t *offset)
{
	int ret = 0;
	struct cdd_cdev *cdevp = file->private_data;
	printk("my_read() execute successfully!\n");
	ret = copy_to_user(buf, cdevp->kbuf, count);
	printk("kernel kbuf content:%s\n", cdevp->kbuf);
	return ret;
}
ssize_t my_write(struct file *file, const char __user *buf, size_t count, loff_t *offset)
{
	int ret = 0;
	struct cdd_cdev *cdevp = file->private_data;	
	printk("my_write() execute successfully!\n");
	ret = copy_from_user(cdevp->kbuf, buf, count);
	return ret;
}



int __init my_init(void)
{
	int result = 0;
	printk( KERN_NOTICE "hello-driver: my_init() is called." );
	result = register_chrdev( 0, device_name, &fops );
	if( result < 0 )
	{
		printk( KERN_WARNING "hello-driver:  can\'t register character device with errorcode = %i", result );
		return result;
	}
	device_file_major_number = result;
	printk( KERN_NOTICE "hello-driver: registered character device with major number = %i and minor numbers 0...255"
  , device_file_major_number );
	return 0;	
}

void __exit my_exit(void)
{
	printk( KERN_NOTICE "hello-driver: my_exit() is called" );
	if(device_file_major_number != 0)
	{
		unregister_chrdev(device_file_major_number, device_name);
	}
}

module_init(my_init);
module_exit(my_exit);
