
#include "random.h"

#include "sodium.h"

void GetRandBytes(unsigned char* buf, size_t num)
{
	randombytes_buf(buf, num);
}