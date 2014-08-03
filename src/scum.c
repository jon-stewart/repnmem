#include <repnmem.h>
#include <string.h>
#include <unistd.h>
#include <sched.h>
//#include <sys/utsname.h>

LIST(scum);

extern void *spawn_pool;

/* XXX Temporary */
static unsigned length = 0;

static int
child_(void *arg)
{
    int         ret;
    germ_t     *germ = (germ_t *) arg;

    ret = germ->entry();
    if (ret == 0) {
        printf("[-] PASS\n");
    }

    return (ret);
}

void
froth(void)
{
    list_t     *ptr  = NULL;
    germ_t     *germ = NULL;
    unsigned    ret;
    pid_t       child_pid;


    for_each_list_ele(&scum, ptr) {
        germ = (germ_t *) ptr;
        assert(germ != NULL);

        printf("[*] spawning\n");

        /* XXX provide each germ with its own stack */
        char *stack = malloc(1024);
        char *stackTop = stack + 1024;
        clone(child_, stackTop, 0, (void *) germ);
    }

    printf("[*] Froth finish\n");

    /* XXX temporary to test */
    add_scum(germ->entry + length);
}

void
infect(void)
{
    FILE       *file    = NULL;
    char       *buffer  = NULL;
    void       *addr    = NULL;
    unsigned    len     = 0;


    file = fopen(GERM_CODE, "rb");

    fseek(file, 0L, SEEK_END);

    len = ftell(file);
    /* XXX temp */
    length = len;

    fseek(file, 0L, SEEK_SET);

    buffer = malloc(sizeof(char) * len);

    fread(buffer, len, len, file);

    fclose(file);

    addr = spawn_pool;

    memcpy(addr, buffer, len);

    free(buffer);

    add_scum(addr);
}

void
add_scum(void *addr)
{
    germ_t     *germ = NULL;


    germ = malloc(sizeof (*germ));
    assert(germ != NULL);

    germ->entry      = addr;
    germ->magic      = 0;
    germ->generation = 0;

    list_add(&scum, &germ->ls);
}

void
release_scum(void)
{
    germ_t     *germ = NULL;


    while (!list_empty(&scum)) {
        germ = (germ_t *) list_rm_back(&scum);
        assert(germ != NULL);

        free(germ);
    }
}