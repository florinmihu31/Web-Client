#ifndef __BUFFER_H__
#define __BUFFER_H__

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

typedef struct {
    char *data;
    size_t size;
} buffer;

buffer buffer_init(void);

void buffer_add(buffer *buffer, const char *data, size_t data_size);

int buffer_find(buffer *buffer, const char *data, size_t data_size);

int buffer_find_insensitive(buffer *buffer, const char *data, size_t data_size);

#endif
