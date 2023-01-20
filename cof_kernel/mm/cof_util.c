#include <linux/cof_types.h>
#include <linux/slab.h>
#include <linux/mm.h>
#include <linux/slub_def.h>
#include <linux/log2.h>

#include "internal.h"

void *cof_page_address(struct kmem_cache *s, struct page *page)
{
	void *addr;
	struct cof_page *cof_page;

	//if(test_bit(PG_cof, &page->flags)) {
	//	pr_info("SET BIT PG_cof for slab: %s\n", s->name);
	//}
	//pr_info("cof_page_address\n");
	if(s) {
		if(s->flags & SLAB_COF) {
			//pr_info("Cache name: %s\n", s->name);
			cof_page = (struct cof_page *) page;
			addr = cof_page->vm_area->addr;
		}
		else {
			addr = page_address(page);
		}
	}
	else if(test_bit(PG_cof, &page->flags)) {
		//pr_info("SLAB COF in cof_page_address\n");
		cof_page = (struct cof_page *) page;
		addr = cof_page->vm_area->addr;
	}
	else {
		addr = page_address(page);
	}

	return addr;
}

struct page *cof_virt_to_head_page(struct kmem_cache *s, const void *x)
{
	struct vm_struct *vm_area;
	
	if(!s) {
		if(is_vmalloc_addr(x)) {
			//pr_info("is_vmalloc_addr -- cof_virt_to_head_page\n");
			vm_area = find_vm_area(x);
			return vm_area->cof_page;
		}
		else {
			//pr_info("NOT vmalloc_addr -- cof_virt_to_head_page\n");
			return virt_to_head_page(x);
		}
	}
	else if(s->flags & SLAB_COF) {
		//pr_info("SLAB COF in cof_virt_to_head_page -- cache: %s\n", s->name);
		vm_area = find_vm_area(x);
		return vm_area->cof_page;
	}
	else {
		//pr_info("NOT SLAB_COF cof_virt_to_head_page\n");
		return virt_to_head_page(x);
	}
}
EXPORT_SYMBOL(cof_virt_to_head_page);

void cof_free_pages(struct kmem_cache *s, struct page *page)
{
	struct cof_page *cof_page;
	struct vm_struct *vm_area;

	if(s->flags & SLAB_COF || test_bit(PG_cof, &page->flags)) {
		//pr_info("Freeing COF Slab\n");
		cof_page = (struct cof_page *) page;
		vm_area = cof_page->vm_area;
		clear_bit(PG_cof, &cof_page->page.flags);
		vfree(vm_area->addr);
		kfree(cof_page);
	}
	else {
		__free_pages(page, compound_order(page));
	}

}

const unsigned int cof_compound_order(struct kmem_cache *s, struct page *page)
{
	struct cof_page *cof_page;
	struct vm_struct *vm_area;

	if(s->flags & SLAB_COF) {
		cof_page = (struct cof_page *) page;
		vm_area = cof_page->vm_area;
		return ilog2(vm_area->nr_pages);
	}
	else {
		return compound_order(page);
	}
}

