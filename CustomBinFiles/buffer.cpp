#include "buffer.h"
#include <iostream>

int read_from_buffer(void* stream, void* to, size_t r_size)
{
	buffer* buff = (buffer*)stream;
	if (buff == NULL) return -1;
	if ((unsigned int)buff->bytes + buff->r_point + r_size > (unsigned int)buff->bytes + (sizeof(unsigned char) * buff->size))
		return 0;

	int res = memcpy_s(to, r_size, (void*)((unsigned int)buff->bytes + buff->r_point), r_size);
	if (res == 0)
	{
		buff->r_point += r_size;
		return 1;
	}
	return 0;
}

int write_to_buffer(void* stream, const void* from, size_t w_size)
{
	buffer* buff = (buffer*)stream;
	if (buff == NULL) return -1;
	if ((unsigned int)buff->bytes + buff->w_point + w_size > (unsigned int)buff->bytes + (sizeof(unsigned char) * buff->size))
		return 0;
	
	int res = memcpy_s((void*)((unsigned int)buff->bytes + buff->w_point), w_size, from, w_size);
	if (res == 0)
	{
		buff->w_point += w_size;
		return 1;
	}
	return 0;
}

int skip_buffer_r(void* stream, int amount)
{
	buffer* buff = (buffer*)stream;
	if (buff == NULL) return -1;
	if ((unsigned int)buff->bytes + buff->r_point + amount > (unsigned int)buff->bytes + (sizeof(unsigned char) * buff->size))
		return 0;
	return buff->r_point += amount;
}

int skip_buffer_w(void* stream, int amount)
{
	buffer* buff = (buffer*)stream;
	if (buff == NULL) return -1;
	if ((unsigned int)buff->bytes + buff->w_point + amount > (unsigned int)buff->bytes + (sizeof(unsigned char) * buff->size))
		return 0;
	return buff->w_point += amount;
}

int init_buffer(buffer** buff, size_t size)
{
	buffer* b = (buffer*)malloc(sizeof(buffer));
	if (b)
	{
		memset(b, 0, sizeof(buffer));
		b->bytes = (unsigned char*)malloc(size);
		memset(b->bytes, 0, size);
		b->size = size;
		*buff = b;
		return 1;
	}
	return 0;
}

void free_buffer(buffer* buff)
{
	free(buff->bytes);
	free(buff);
}
