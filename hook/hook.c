#ifndef _MSC_VER

#include "print_stack.h"
#include <dlfcn.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#ifndef PROT_WRITE 
#define PROT_WRITE 0x2  
#endif

#ifndef MAP_PRIVATE 
#define MAP_PRIVATE 0x2  
#endif

static inline void* get_address(const char* symbol)
{
    void* address = dlsym(RTLD_NEXT, symbol);
    if (!address)
        abort();
    return address;
}

static void* (*g_mmap)(void*, size_t, int, int, int, off_t) = NULL;
static __thread int g_disable_mmap_hook = 0;

__attribute__((constructor)) static void init(void) { g_mmap = get_address("mmap"); }

__attribute__((destructor)) static void uninit(void) { }

__attribute__((visibility("default"))) void* mmap(void* addr, size_t length, int prot, int flags, int fd, off_t offset)
{
    if (g_disable_mmap_hook) {
        return g_mmap(addr, length, prot, flags, fd, offset);
    }
    g_disable_mmap_hook = 1;

    int write_able = prot & PROT_WRITE;
    int private_able = prot & MAP_PRIVATE;
    if (write_able && !private_able) {
        print_log("mmap len:%llu", length);
        print_stack(2);
    }

    void* ret = g_mmap(addr, length, prot, flags, fd, offset);
    g_disable_mmap_hook = 0;
    return ret;
}

#endif
