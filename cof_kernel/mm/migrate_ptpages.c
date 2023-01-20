#include <linux/migrate_ptpages.h>                                                                    
#include <linux/mm.h>
#include <linux/printk.h>
#include <linux/string.h>
#include <asm/pgtable.h>
#include <linux/pagewalk.h>

#define MAX_PHYS_ADDR ((1UL << 39) - 1)
#define FLAGS_MASK ((1UL << 11) - 1)

unsigned long get_pfn(unsigned long pte) {
    return (pte & MAX_PHYS_ADDR) >> 12;// & ~FLAGS_MASK;
}

unsigned long get_prot_val(unsigned long pte) {
    return pte & FLAGS_MASK;
}

int migrate_ptpage(struct page *page)
{

    struct mm_struct *mm;
    unsigned long *pt_parent_ptep, parent_pfn;
    struct page *new_page;
    u8 *page_addr;
    int rv;
    unsigned long i;
    rv = 0;
    page_addr = NULL;
    mm = NULL;

    page_addr = page_address(page);


    if((unsigned long) page->pt_mm & 0x1) {
        pt_parent_ptep =  (unsigned long*) ((unsigned long) page->pt_mm - 1);
    }
    else {
        mm = page->pt_mm;
    }

    pr_info("mig ptpage mm=%px\n", mm);
    if(mm) { //pgd case
        spin_lock(&mm->page_table_lock);
        new_page = alloc_page(GFP_KERNEL | __GFP_ZERO);
        if(new_page == NULL) {
            pr_err("could not allocate new_page");
            rv = -1;
            goto out;
        }
        memcpy(page_address(new_page), page_addr, PAGE_SIZE);
        memcpy(new_page, page, sizeof(struct page));
        mm->pgd = page_address(new_page);
        spin_unlock(&mm->page_table_lock);
    }
    else {
        struct page *pt_parent_page = virt_to_page(pt_parent_ptep);
        pr_info("scanning parent pte @ %px\n", pt_parent_ptep);
        pr_info("parent struct page @ %px\n", pt_parent_page);
        spin_lock(&page->ptl);
        spin_lock(&pt_parent_page->ptl);

        new_page = alloc_page(GFP_KERNEL | __GFP_ZERO);
        if(new_page == NULL) {
            pr_err("could not allocate new_page");
            rv = -1;
            goto out;
        }
        memcpy(page_address(new_page), page_addr, PAGE_SIZE);
        memcpy(new_page, page, sizeof(struct page));                
        pr_info("old pte in parent: %lx\n", *pt_parent_ptep);
        *pt_parent_ptep = ((page_to_pfn(new_page) << 12) | get_prot_val(*pt_parent_ptep));
        pr_info("new pte in parent: %lx\n", *pt_parent_ptep);
        
        spin_unlock(&pt_parent_page->ptl);
        spin_unlock(&page->ptl);
        SetPageHWPoison(page);
        put_page(page);
    }
out:
    return rv;
}

