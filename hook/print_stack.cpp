#ifndef _MSC_VER

#include "print_stack.h"
#include <cstdarg>
#include <cstdio>
#include <cstdlib>
#include <cxxabi.h>
#include <dlfcn.h>
#include <execinfo.h>
#include <linux/limits.h>
#include <unistd.h>

static FILE* s_log_fp = nullptr;

void print_log(const char* format, ...)
{
    if (!s_log_fp) {
        char path[PATH_MAX];
        snprintf(path, sizeof(path), "/tmp/hook_%d.log", getpid());
        s_log_fp = fopen(path, "wb");
        if (!s_log_fp) {
            return;
        }
    }

    std::va_list args;
    va_start(args, format);
    std::vfprintf(s_log_fp, format, args);
    va_end(args);

    std::fprintf(s_log_fp, "\n");
    std::fflush(s_log_fp);
}

void print_stack(int skip)
{
    constexpr auto MAX_STACK_FRAMES = 128;
    void* stack_frames[MAX_STACK_FRAMES];

    int cnt = backtrace(stack_frames, sizeof(stack_frames) / sizeof(stack_frames[0]));
    if (cnt < skip) {
        return;
    }

    char* demangled = nullptr;
    int status;
    Dl_info dli;

    for (int i = skip; i < cnt; ++i) {
        void* address = stack_frames[i];
        if (dladdr(address, &dli) == 0) {
            continue;
        }

        if (dli.dli_sname) {
            demangled = abi::__cxa_demangle(dli.dli_sname, nullptr, nullptr, &status);
            print_log("\t%d# %s(%p) in %s", i, demangled ? demangled : dli.dli_sname, dli.dli_saddr, dli.dli_fname);
            if (demangled) {
                free(demangled);
                demangled = nullptr;
            }
        } else {
            print_log("\t%d# %p in %s", i, dli.dli_saddr, dli.dli_fname);
        }
    }
    print_log("\n\n");
}

#endif
