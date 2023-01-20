#ifndef _LINUX_COF_H
#define _LINUX_COF_H

#include <linux/mm_types.h>


#define MIGRATE_SUCCESS 0

int migrate_huge(struct page *);

int migrate_normal(struct page *);

int bitflip_migrate(const unsigned long);



#endif /* _LINUX_COF_H */
