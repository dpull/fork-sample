#include "util.h"
#include "traversedir.h"
#include <chrono>
#include <cstdarg>
#include <cstdio>
#include <thread>

#ifdef _MSC_VER
#include <Windows.h>
#else
#include <sys/mman.h>
#include <sys/wait.h>
#include <unistd.h>
#endif

static FILE* s_log_fp = nullptr;

bool log_init()
{
    if (s_log_fp)
        return false;

    s_log_fp = std::fopen("fork-test.log", "w");
    return s_log_fp != nullptr;
}

void log(const char* level, const char* format, ...)
{
    if (!s_log_fp)
        return;

    std::fprintf(s_log_fp, "[%d]%s\t", os::getpid(), level);
    std::va_list args;
    va_start(args, format);
    std::vfprintf(s_log_fp, format, args);
    va_end(args);
    std::fprintf(s_log_fp, "\n");
    if (std::fflush(s_log_fp) != 0) {
        printf("[%d]flush log file failed\n", os::getpid());
    }
}

void log_close()
{
    if (!s_log_fp)
        return;

    std::fclose(s_log_fp);
    s_log_fp = nullptr;
}

void log_fd()
{
    traversedir(
        "/proc/self/fd",
        [](const char* fullpath, const char* filename, void* userdata) {
            log("info", "log fd: %s->%s", fullpath, os::readlink(fullpath));
            return true;
        },
        nullptr, false);
}

namespace os {
void sleep(int ms) { std::this_thread::sleep_for(std::chrono::milliseconds(ms)); }

int getpid()
{
#ifdef _MSC_VER
    return static_cast<int>(GetCurrentProcessId());
#else
    return static_cast<int>(::getpid());
#endif
}

int fork()
{
#ifdef _MSC_VER
    return -1;
#else
    return static_cast<int>(::fork());
#endif
}

int waitpid()
{
#ifdef _MSC_VER
    return -1;
#else
    return static_cast<int>(::waitpid(-1, nullptr, WNOHANG));
#endif
}

const char* readlink(const char* pathname)
{
    static char link_path[1024];
#if _MSC_VER
    link_path[0] = '\0';
#else
    auto len = ::readlink(pathname, link_path, sizeof(link_path));
    if (len >= 0) {
        link_path[len] = '\0';
    } else {
        link_path[0] = '\0';
    }
#endif
    return link_path;
}

void* memory_alloc(size_t sz)
{
#if _MSC_VER
    return VirtualAlloc(nullptr, sz, MEM_RESERVE, PAGE_READWRITE);
#else
    void* ptr = mmap(nullptr, sz, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANON, -1, 0);
    return (ptr != MAP_FAILED) ? ptr : nullptr;
#endif
}

void memory_free(void* addr, size_t sz)
{
    if (!addr)
        return;

#if _MSC_VER
    VirtualFree(addr, 0, MEM_RELEASE);
#else
    munmap(addr, sz);
#endif
}

}