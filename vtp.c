#include <linux/module.h>
#include <linux/moduleparam.h>
#include <linux/init.h>

#include <linux/kernel.h>       /* printk() */
#include <linux/slab.h>         /* kmalloc() */
#include <linux/fs.h>           /* everything... */
#include <linux/errno.h>        /* error codes */
#include <linux/types.h> 
#include <linux/fcntl.h>        /* O_ACCMODE */
#include <linux/mm_types.h>     /* struct page and struct mm_struct*/
#include <asm/page.h>   /*pgd_t, pte_t, __va etc.*/
#include <linux/pgtable.h>      /*pgd_offset, pud_offset etc*/
#include <asm/pgtable_types.h>  /*PTE_PFN_MASK*/
