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
#include <linux/highmem.h>
#include <asm/io.h>

MODULE_LICENSE("Dual BSD/GPL");

int vtp_major = 0;		/*we shall allocate dynamically*/
int vtp_minor = 0;
int nr_vtp_devs = 1;		/*just one device*/

struct cdev *vtp_cdev;		/*our character device*/


/*
 * Function:  vtp_ioctl 
 * --------------------
 * converts the user-space virtual address in arg to it's corresponding physical
 * address by walking the current process' page tables.
 *
 *  arg: the user-space virtual address to be converted
 *  cmd: ignored
 *
 *  returns: -1 for any failure, zero otherwise
 */
long vtp_ioctl(struct file *filp, unsigned int cmd, unsigned long arg)
{
	//printk(KERN_NOTICE"ioctl has been called on the char device!\n");
	char c;
	
	pgd_t *pgd;
	pte_t *ptep;
	pud_t *pud;
	p4d_t *p4d;
	pmd_t *pmd;
	char *addr, *pf_addr;
	
	struct page *page = NULL;
	struct mm_struct *mm = current->mm;
	
	pgd = pgd_offset(mm, arg);
	if (pgd_none(*pgd) || pgd_bad(*pgd))
		goto out;
	printk(KERN_NOTICE "Valid pgd\n");
	
	p4d = p4d_offset(pgd, arg);
	if (p4d_none(*p4d) || p4d_bad(*p4d))
		goto out;
	printk(KERN_NOTICE "Valid p4d\n");
	
	pud = pud_offset(p4d, arg);
	if (pud_none(*pud) || pud_bad(*pud))
		goto out;
	printk(KERN_NOTICE "Valid pud\n");
	
	pmd = pmd_offset(pud, arg);
	if (pmd_none(*pmd) || pmd_bad(*pmd))
		goto out;
	printk(KERN_NOTICE "Valid pmd\n");
	
	ptep = pte_offset_kernel(pmd, arg);
	if(!ptep)
		goto out;
	
	page = pte_page(*ptep);
	pf_addr = (char *)((unsigned long)pte_val(*ptep) & PTE_PFN_MASK);
	addr = pf_addr + (arg & ~PAGE_MASK);
	c = *((char *) __va(addr));
	printk(KERN_INFO "the physical address is 0x%px\n", (void *)addr);
	printk(KERN_INFO "the physical page frame address is 0x%px\n", (void *)pf_addr);
	printk(KERN_INFO "and the kernel virt addr is 0x%px\n", (void*)__va(addr));
	printk(KERN_INFO "the byte there is 0x%x\n", c);
	pte_unmap(ptep);
	
	return 0;
	
	out:
	printk(KERN_INFO "couldn't walk page tables\n");
	return -1;
}

const struct file_operations vtp_fops = {
   .owner =    THIS_MODULE,
   .unlocked_ioctl = vtp_ioctl,		/*we'll only use ioctl*/
};

int vtp_init(void)
{
	int result, err;
	dev_t dev = 0;
	
	result = alloc_chrdev_region(&dev, vtp_minor, nr_vtp_devs,
				     "vtp");
	vtp_major = MAJOR(dev);
	
	if (result < 0){
		printk(KERN_WARNING "vtp: can't allocate device number\n");
		return result;
	}
	
	vtp_cdev = cdev_alloc();
	vtp_cdev->ops = &vtp_fops;
	vtp_cdev->owner = THIS_MODULE;
	err = cdev_add(vtp_cdev, dev, 1);
	if (err)
		printk(KERN_NOTICE "couldn't add cdev: error %d", err);
	
	return 0;
}

void vtp_exit(void)
{
	dev_t dev = MKDEV(vtp_major, vtp_minor);
	
	unregister_chrdev_region(dev, nr_vtp_devs);
	cdev_del(vtp_cdev);
}
module_init(vtp_init);
module_exit(vtp_exit);

