#include <dlfcn.h>
#include <string.h>

typedef int (*rand_type)(void);

int unlink(const char* filename) {
    if (strstr(filename, "PROTECT") != NULL) {
        return 0;
    }
    return ((int (*)(const char*))dlsym(RTLD_NEXT, "unlink"))(filename);
}
