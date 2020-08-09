// Program checks if all the APIs are called

#define _GNU_SOURCE

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <dlfcn.h>
#include <arpa/inet.h>
#include "wpcap.h"

#define LINKADDR_SIZE 8
typedef union {
  unsigned char u8[LINKADDR_SIZE];
  uint16_t u16[LINKADDR_SIZE/2];
} linkaddr_t;

extern linkaddr_t linkaddr_node_addr;

static void *g_self_handle;
void *g_pcap_handle;

/* Hook shared function */
#define HOOK_ORG(RET, F, ARGS)   \
    typedef RET (*func_##F)ARGS;\
    static func_##F org_##F;\
    if(!org_##F) {\
        org_##F = (func_##F)dlsym(RTLD_NEXT, #F);\
        if(!org_##F) {\
            printf("CUDNOT ORVERRIDE %s\n", #F);\
        }\
    }

/* Load local bin function */
#define LOAD_SYM(RET, F, ARGS, CALLARGS)   \
RET F ARGS \
{\
    typedef RET (*func_##F)ARGS;\
    static func_##F myf;\
    load_self();\
    if(!myf && g_self_handle) {\
        myf = (func_##F)dlsym(g_self_handle, #F);\
        if(!myf) {\
            printf("CUDNOT ORVERRIDE %s\n", #F);\
            abort();\
        }\
    }\
    return myf CALLARGS;\
}

int __libc_start_main(int *(main) (int, char * *, char * *),
        int argc, char *ubp_av[],
        void (*init) (void), void (*fini) (void),
        void (*rtld_fini) (void), void (* stack_end))
{
    extern void wfradio_init(void);
    int i;
    HOOK_ORG(int, __libc_start_main,
            (int *(main) (int, char * *, char * *),
             int argc, char * * ubp_av, 
             void (*init) (void), void (*fini) (void),
             void (*rtld_fini) (void),
             void (* stack_end)));

    for(i=0;i<argc;i++) {
        printf("argv[%d]=%s\n", i, ubp_av[i]);
    }
    wfradio_init();
    g_pcap_handle = pcap_init("packet.pcap");
    return org___libc_start_main(main, argc, ubp_av, init, fini, rtld_fini, stack_end);
}

void set_node_id(uint16_t id)
{
    linkaddr_node_addr.u16[3] = htons(id);
}

void load_self(void)
{
    if (g_self_handle) {
        return;
    }
    g_self_handle = dlopen(NULL, RTLD_LAZY);
    if (!g_self_handle) {
        printf("Fucked up! Cant dlopen\n");
        abort();
    }
}

LOAD_SYM(void *, uip_ds6_defrt_choose, (void), ());
LOAD_SYM(void *, uip_ds6_defrt_lookup, (const void *ipaddr), (ipaddr));
LOAD_SYM(void, uip_ds6_defrt_rm, (void *defrt), (defrt));

/* Contiki-ng native platform adds a default route by itself. This is not
 * needed, hence removing it here. */
void rem_default_rt(void)
{
    HOOK_ORG(int, open, (const char *pathname, int flags));
}

#define TUN_FD  0x101
/* Contiki-ng opens up tun interface on startup for local testing */
int open(const char *pathname, int flags)
{
    HOOK_ORG(int, open, (const char *pathname, int flags));

    if(!strcmp(pathname, "/dev/net/tun")) {
        set_node_id(1234);
        printf("Pathname:%s\n", pathname);
        return TUN_FD;
    }
    return org_open(pathname, flags);
}

int ioctl(int fd, unsigned long req, void *ptr)
{
#define TUNSETIFF 0x400454ca
    HOOK_ORG(int, ioctl,(int fd, unsigned long req, void *ptr));

    if (req == TUNSETIFF) {
        printf("fd=%d, req=%lx\n", fd, req);
        return 0;
    }
    return org_ioctl(fd, req, ptr);
}

int system(const char *cmd)
{
    HOOK_ORG(int, system, (const char *cmd));

    printf("cmd=%s\n", cmd);
    return 0;

    return org_system(cmd);
}

void rem_defrt(void)
{
    void *ipa = NULL, *defrt = NULL;
    ipa = uip_ds6_defrt_choose();
    if (!ipa) {
        printf("NO DEFRT\n");
    }
    defrt = uip_ds6_defrt_lookup(ipa);
    if (!defrt) {
        printf("CUDNOT Get DEFRT\n");
    }
    printf("REMOVING DEFRT\n");
    uip_ds6_defrt_rm(defrt);
}

ssize_t write(int fd, const void *data, size_t len)
{
    HOOK_ORG(ssize_t, write, (int fd, const void *data, size_t len));
    rem_defrt();
    if(fd == TUN_FD) {
        printf("Writing %zu to TUN\n", len);
        pcap_write(g_pcap_handle, data, len);
        return len;
    }
    return org_write(fd, data, len);
}

