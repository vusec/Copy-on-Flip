#ifndef _LINUX_COF_TYPES_H
#define _LINUX_COF_TYPES_H

#include <linux/mm_types.h>
#include <linux/vmalloc.h>
#include <linux/types.h>


struct cof_page {
	struct page page;
	struct vm_struct *vm_area;
};

void *cof_page_address(struct kmem_cache *, struct page *);
struct page *cof_virt_to_head_page(struct kmem_cache *, const void *);
void cof_free_pages(struct kmem_cache *, struct page *);
const unsigned int cof_compound_order(struct kmem_cache *, struct page *); 
void cof_prep_compound_page(struct kmem_cache *s, struct page *, unsigned int order);
#endif


