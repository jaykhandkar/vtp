#include <linux/module.h>
#include <linux/moduleparam.h>
#include <linux/init.h>

#include <linux/kernel.h>       /* printk() */
#include <linux/slab.h>         /* kmalloc() */
#include <linux/fs.h>           /* everything... */
#include <linux/errno.h>        /* error codes */
#include <linux/cdev.h>		/* char device registration*/
#include <linux/types.h> 
#include <linux/fcntl.h>        /* O_ACCMODE */
#include <linux/mm_types.h>     /* struct page and struct mm_struct*/
#include <asm/page.h>           /*pgd_t, pte_t, __va etc.*/
#include <linux/pgtable.h>      /*pgd_offset, pud_offset etc*/
#include <asm/pgtable_types.h>  /*PTE_PFN_MASK*/

MODULE_LICENSE("Dual BSD/GPL");

int vtp_major = 0;		/*we shall allocate dynamically*/
int vtp_minor = 0;
int nr_vtp_devs = 1;		/*just one device*/

int vtp_init(void)
{
	int result;
	dev_t dev = 0;
	
	result = alloc_chrdev_region(&dev, vtp_minor, nr_vtp_devs,
				     "vtp");
	vtp_major = MAJOR(dev);
	
	if (result < 0){
		printk(KERN_WARNING "vtp: can't allocate device number\n");
		return result;
	}
	return 0;
}

void vtp_exit(void)
{
	dev_t dev = MKDEV(vtp_major, vtp_minor);
	
	unregister_chrdev_region(dev, nr_vtp_devs);
}

