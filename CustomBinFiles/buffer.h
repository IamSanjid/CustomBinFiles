#pragma once

#define MAX_BUFF_SIZE 4096

typedef struct buffer_s
{
    unsigned char* bytes;
    size_t size;
    unsigned int r_point;
    unsigned int w_point;
} buffer;

int read_from_buffer(void* stream, void* to, size_t r_size);
int write_to_buffer(void* stream, const void* from, size_t w_size);
int skip_buffer_r(void* stream, int amount);
int skip_buffer_w(void* stream, int amount);

int init_buffer(buffer** buff, size_t size);
void free_buffer(buffer* buff);