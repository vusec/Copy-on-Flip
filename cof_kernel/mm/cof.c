#include <linux/cof.h>
#include <asm/page.h>
#include <linux/mm.h>
#include <linux/migrate.h>
#include <linux/compiler.h>
#include <linux/mmzone.h>
#include <linux/migrate_ptpages.h>
#include "internal.h"


int migrate_huge(struct page *huge_page)
{

    int rv, target_node;
    LIST_HEAD(migrate);
    
    target_node = page_to_nid(huge_page);
    migrate_prep();
    rv = isolate_huge_page(huge_page, &migrate);
    if(rv == false) {
        pr_info("Couldn't isolate huge page @ PFN : %lx\n", page_to_pfn(huge_page));
        rv = -EBUSY;
        goto out;
    }

    rv = migrate_pages(&migrate, alloc_new_node_page, NULL, target_node, MIGRATE_ASYNC, MR_SYSCALL);
    if(rv) {
        pr_info("Couldn't migrate huge page -- RV : %d\n", rv);
    }

    if(!list_empty(&migrate)) {
        putback_movable_pages(&migrate);
    }
    
out:
    return rv;

}



int migrate_normal(struct page *page)
{

    int rv, target_node;
    LIST_HEAD(migrate);

    rv = MIGRATE_SUCCESS;
    target_node = page_to_nid(page);

    migrate_prep();

    if(PageLRU(page)) {
        rv = isolate_lru_page(page);
    }
    else {
        rv = isolate_movable_page(page, ISOLATE_UNEVICTABLE);
    }

    if(rv) {
        pr_err("Page Isolation Failed\n");
        goto out;
    }

    list_add(&page->lru, &migrate);
    lock_page(page);
    if(PageWriteback(page)) {
        wait_on_page_writeback(page);
        pr_info("Page Writeback Completed\n");
    }
    unlock_page(page);

    rv = migrate_pages(&migrate, alloc_new_node_page, NULL, target_node, MIGRATE_ASYNC, MR_SYSCALL);
    pr_info("migrate_pages RV : %d\n", rv);

    if(rv) {
        putback_movable_pages(&migrate);
    }

out:
    return rv;

}


int bitflip_migrate(const unsigned long pfn)
{
    int rv;
    struct page *target_page;

    rv = MIGRATE_SUCCESS;

    if(unlikely(!pfn_valid(pfn))) {
        rv = -ENXIO;
        goto out;
    }

    target_page = pfn_to_page(pfn);
    target_page = compound_head(target_page);
    
    if(test_bit(PG_ptp, &target_page->flags)) {
        pr_info("Migrating ptp\n");
        rv = migrate_ptpage(target_page);
        goto out;
    } 
    
    if(PageTransHuge(target_page) || PageHuge(target_page)) {
        pr_info("The Page @PFN : %#lx is HP or THP\n", pfn);
		rv = migrate_huge(target_page);
    }
    else {
        rv = migrate_normal(target_page);
    }

out:
    return rv;

}

EXPORT_SYMBOL(bitflip_migrate);
