#include "bin.h"
#include "buffer.h"

#include <iostream>
#include <fstream>


int read_bin(const char* path, bin** o_bin)
{
    std::ifstream file(path, std::ios::in | std::ios::binary);
    buffer* buff = {};
    bin* out_bin = {};
    int is_okay = 1;

    if (file.bad()) return -1;

    file.seekg(0, std::ios::end);
    auto len = file.tellg();
    file.seekg(0, std::ios::beg);

    if (len > MAX_BUFF_SIZE) return -1;

    if (!init_buffer(&buff, sizeof(unsigned char) * len))
    {
        is_okay = 0;
        goto END;
    }

    if (!init_bin(&out_bin, 0, 0))
    {
        is_okay = 0;
        goto END;
    }

    file.read((char*)buff->bytes, len);

    if (!file)
    {
        is_okay = 0;
        goto END;
    }
    if ((buff->bytes[0] ^ 0x69) != (len & 0b11111111))
    {
        is_okay = 0;
        goto END;
    }
    if (!skip_buffer_r(buff, 1))
    {
        is_okay = 0;
        goto END;
    }

    is_okay &= read_from_buffer(buff, &out_bin->n_Len, sizeof(unsigned int));

    if (out_bin->n_Len)
    {
        out_bin->name = (char*)malloc(sizeof(char) * (out_bin->n_Len + 1));
        is_okay &= read_from_buffer(buff, out_bin->name, sizeof(char) * out_bin->n_Len);
        out_bin->name[out_bin->n_Len] = '\0';
    }

    is_okay &= read_from_buffer(buff, &out_bin->se_Len, sizeof(unsigned int));

    out_bin->searches = (bin_bytes*)malloc(sizeof(bin_bytes) * out_bin->se_Len);
    for (unsigned int i = 0; i < out_bin->se_Len; i++)
    {
        is_okay &= read_from_buffer(buff, &out_bin->searches[i].len, sizeof(unsigned int));
        size_t size = sizeof(unsigned char) * out_bin->searches[i].len;
        out_bin->searches[i].bytes = (unsigned char*)malloc(size);
        is_okay &= read_from_buffer(buff, out_bin->searches[i].bytes, size);
    }

    is_okay &= read_from_buffer(buff, &out_bin->ex_c_len, sizeof(unsigned int));

    out_bin->ex_codes = (bin_bytes*)malloc(sizeof(bin_bytes) * out_bin->ex_c_len);
    for (unsigned int i = 0; i < out_bin->ex_c_len; i++)
    {
        is_okay &= read_from_buffer(buff, &out_bin->ex_codes[i].len, sizeof(unsigned int));
        size_t size = sizeof(unsigned char) * out_bin->ex_codes[i].len;
        out_bin->ex_codes[i].bytes = (unsigned char*)malloc(size);
        is_okay &= read_from_buffer(buff, out_bin->ex_codes[i].bytes, size);
    }

END:
    file.close();
    *o_bin = out_bin;
    free_buffer(buff);

    return is_okay;
}

int write_bin(const char* path, const bin* in_bin)
{
    buffer* buff = {};
    int bin_raw_size = 1; /* first byte is the magic number */
    int skipped_byte = 0;
    int is_okay = 1;

    if (!in_bin)
        return 0;

    if (!init_buffer(&buff, MAX_BUFF_SIZE))
    {
        return 0;
    }

    skip_buffer_w(buff, 1);

    bin_raw_size += sizeof(unsigned int);
    if (in_bin->n_Len && in_bin->name)
    {
        is_okay &= write_to_buffer(buff, &in_bin->n_Len, sizeof(unsigned int));
        size_t size = sizeof(char) * in_bin->n_Len;
        bin_raw_size += size;
        is_okay &= write_to_buffer(buff, in_bin->name, size);
    }
    else
    {
        is_okay &= write_to_buffer(buff, &skipped_byte, sizeof(unsigned int));
    }

    bin_raw_size += sizeof(unsigned int);
    if (in_bin->se_Len && in_bin->searches)
    {
        is_okay &= write_to_buffer(buff, &in_bin->se_Len, sizeof(unsigned int));
        for (int i = 0; i < in_bin->se_Len; i++)
        {
            if (in_bin->searches[i].len && in_bin->searches[i].bytes)
            {
                bin_raw_size += sizeof(unsigned int);
                is_okay &= write_to_buffer(buff, &in_bin->searches[i].len, sizeof(unsigned int));

                size_t size = sizeof(unsigned char) * in_bin->searches[i].len;
                bin_raw_size += size;
                is_okay &= write_to_buffer(buff, in_bin->searches[i].bytes, size);
            }
        }
    }
    else
    {
        is_okay &= write_to_buffer(buff, &skipped_byte, sizeof(unsigned int));
    }

    bin_raw_size += sizeof(unsigned int);
    if (in_bin->ex_c_len && in_bin->ex_codes)
    {
        is_okay &= write_to_buffer(buff, &in_bin->ex_c_len, sizeof(unsigned int));
        for (int i = 0; i < in_bin->ex_c_len; i++)
        {
            if (in_bin->ex_codes[i].len && in_bin->ex_codes[i].bytes)
            {
                bin_raw_size += sizeof(unsigned int);
                is_okay &= write_to_buffer(buff, &in_bin->ex_codes[i].len, sizeof(unsigned int));

                size_t size = sizeof(unsigned char) * in_bin->ex_codes[i].len;
                bin_raw_size += size;
                is_okay &= write_to_buffer(buff, in_bin->ex_codes[i].bytes, size);
            }
        }
    }
    else
    {
        is_okay &= write_to_buffer(buff, &skipped_byte, sizeof(unsigned int));
    }

    if ((is_okay & 1) == 1 && bin_raw_size > sizeof(unsigned int) * 4)
    {
        buff->bytes[0] = (bin_raw_size & 0xff) ^ 0x69;
        std::ofstream file(path, std::ios::out | std::ios::binary);
        if (file.is_open())
        {
            file.write((char*)buff->bytes, bin_raw_size);
            file.close();
        }
    }

    free_buffer(buff);
    return is_okay;
}

int init_bin(bin** out_bin, int n_len, const char* name)
{
    bin* b = (bin*)malloc(sizeof(bin));
    if (b)
    {
        memset(b, 0, sizeof(bin));
        *out_bin = b;
        if (n_len && name)
        {
            size_t size = sizeof(char) * n_len;
            b->n_Len = n_len;
            b->name = (char*)malloc(size);
            return memcpy_s(b->name, size, name, size);
        }
        return 1;
    }
    return 0;
}

void free_bin(bin* b)
{
    if (b == NULL)
        return;

    if (b->name)
        free(b->name);
    if (b->searches && b->se_Len)
    {
        for (int i = 0; i < b->se_Len; i++)
        {
            if (b->searches[i].bytes)
                free(b->searches[i].bytes);
        }
        free(b->searches);
    }
    if (b->ex_codes && b->ex_c_len)
    {
        for (int i = 0; i < b->ex_c_len; i++)
        {
            if (b->ex_codes[i].bytes)
                free(b->ex_codes[i].bytes);
        }
        free(b->ex_codes);
    }
    free(b);
}
