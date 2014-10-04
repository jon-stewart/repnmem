#include <replicaters.h>
#include <string.h>

/*
 * Function:
 *      reg_cb
 *
 * Description:
 *      Registration callback from germs after successful replication.
 *
 *      Add to scum list for future spawning.
 */
static void
reg_cb(void *addr, size_t length)
{
    printf("[*] cb: %p, 0x%lx\n", addr, (unsigned long) length);

    assert(addr != NULL);
    assert(length > 0);

    vat_scum_add(addr, length);
}

/*
 * Function:
 *      spawn
 *
 * Description:
 *      Thread function for passing execution to germ code in vat pool memory.
 */
void *
spawn(void *arg)
{
    germ_t  *germ = (germ_t *) arg;

    /* Dead germs should have been reaped */
    assert(germ->dead == false);
    assert(germ->tid  == 0);

    germ->tid = pthread_self();

    printf("[*] START %p\n", germ->entry);

    if (germ->entry((void *) reg_cb) == 0) {
        germ->tid = 0;

        printf("[*] PASS %p\n", germ->entry);
    }

    return (NULL);
}

/*
 * Function:
 *      infect
 *
 * Description:
 *      Read the extracted germ binary .text section and write to start of the
 *      vat memory pool.
 */
void
infect(void)
{
    FILE       *file    = NULL;
    char       *buffer  = NULL;
    void       *addr    = NULL;
    size_t      len     = 0;


    file = fopen(GERM_CODE, "rb");

    fseek(file, 0L, SEEK_END);

    len = ftell(file);

    fseek(file, 0L, SEEK_SET);

    buffer = malloc(sizeof(char) * len);

    fread(buffer, len, len, file);

    fclose(file);

    addr = vat_base_address();

    memcpy(addr, buffer, len);

    free(buffer);

    vat_scum_add(addr, len);
}
