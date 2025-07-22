#include "file_utils.h"
#include <string.h>

int file_exists(const char* filepath) {
    DWORD attributes = GetFileAttributesA(filepath);
    return (attributes != INVALID_FILE_ATTRIBUTES);
}

