#ifndef _REPNMEM_H_
#define _REPNMEM_H_

#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <assert.h>
#include <list.h>
#include <pthread.h>

#ifdef DEBUG
#define dev (true)
#define debug(format, ...) \
    printf(format, ##__VA_ARGS__)
#else
#define dev (false)
#define debug(format, ...)
#endif

#define RW_INIT(rw) \
    pthread_rwlock_init((rw), NULL)

#define RW_WRLOCK(rw)   \
    pthread_rwlock_wrlock((rw))

#define RW_RDLOCK(rw)   \
    pthread_rwlock_rdlock((rw))

#define RW_UNLOCK(rw)   \
    pthread_rwlock_unlock((rw))

#define MTX_INIT(mtx)   \
    pthread_mutex_init((mtx), NULL)

#define MTX_ENTER(mtx)  \
    pthread_mutex_lock((mtx))

#define MTX_EXIT(mtx)   \
    pthread_mutex_unlock((mtx))

#define VAT_SIZE            (1024 * 1024)
#define GERM_CODE           ("/tmp/germ.sc")
#define GERM_FD             ("/germfd")

typedef struct germ {
    unsigned        magic;
#define GERM_MAGIC  (0xC0FFEE)
    list_t          ls;

    unsigned (*entry)(void *, void *);
    size_t          len;

    unsigned int    generation;
    bool            dead;
    pthread_t       tid;
} germ_t;

/* vat.c */
void vat_init(void);
void vat_destroy(void);
void *vat_base_address(void);
void vat_scum_add(void *, size_t, unsigned);
void vat_scum_release(void);
void stir(void);
void mark_dead(pthread_t);

/* germ.c */
void foam(void);
void froth(void);
void infect(void);
void *spawn(void *);

/* reaper.c */
void reaper_init(void);
void reaper_cleanse(germ_t *);

/* terminal.c */
void terminal_key_input(void);

#endif /* _REPNMEM_H_ */
