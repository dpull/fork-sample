#pragma once
#include <cstddef>

bool log_init();
void log(const char* level, const char* format, ...);
void log_close();

void log_fd();

namespace os {

void sleep(int ms);
int getpid();
int fork();
int waitpid();
const char* readlink(const char* pathname);

void* memory_alloc(size_t sz);
void memory_free(void* addr, size_t sz);
}
