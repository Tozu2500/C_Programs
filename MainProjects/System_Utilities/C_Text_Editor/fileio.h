#ifndef FILEIO_H
#define FILEIO_H

#include "buffer.h"
#include <stddef.h>
#include <stdbool.h>

bool file_load(const char *filename, TextBuffer *tb, char *err, size_t errsz);
bool file_save(const char *filename, const TextBuffer *tb, char *err, size_t errsz);

#endif  // FILEIO_H