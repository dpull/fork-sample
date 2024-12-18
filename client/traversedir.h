#ifndef TRAVERSEDIR_H
#define TRAVERSEDIR_H
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef bool(traversedir_callback)(const char* fullpath, const char* filename, void* userdata);
void traversedir(const char dir[], traversedir_callback* callback, void* userdata, bool ignore_hidefile);

#ifdef __cplusplus
}
#endif

#endif
