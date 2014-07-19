#ifndef _REPNMEM_H_
#define _REPNMEM_H_

#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <list.h>

#define SPAWN_POOL_SIZE     (1 << 9)

typedef struct germ {
    list_t         ls;

    void           *address;
    unsigned int    magic;
    int             generation;

} germ_t;

void init_spawn_pool(void);
void destroy_spawn_pool(void);

void froth(void);
void release_scum(void);

void infect(void);

#endif /* _REPNMEM_H_ */
