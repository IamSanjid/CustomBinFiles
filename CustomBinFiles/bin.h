#pragma once

typedef struct
{
    unsigned int len;
    unsigned char* bytes;

} bin_bytes;

typedef struct
{
    unsigned int n_Len;
    char* name;
    unsigned int se_Len;
    unsigned int ex_c_len;

    bin_bytes* searches;
    bin_bytes* ex_codes;
} bin;

int read_bin(const char* path, bin** out_bin);
int write_bin(const char* path, const bin* in_bin);

int init_bin(bin** b, int n_len, const char* name);
void free_bin(bin* b);