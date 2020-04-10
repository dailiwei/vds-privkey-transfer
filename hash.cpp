#include "hash.h"
#include "pubkey.h"
#include <crypto/hmac_sha512.h>



void BIP32Hash(const ChainCode& chainCode, unsigned int nChild, unsigned char header, const unsigned char data[32], unsigned char output[64])
{
	unsigned char num[4];
	num[0] = (nChild >> 24) & 0xFF;
	num[1] = (nChild >> 16) & 0xFF;
	num[2] = (nChild >> 8) & 0xFF;
	num[3] = (nChild >> 0) & 0xFF;
	CHMAC_SHA512(chainCode.begin(), chainCode.size()).Write(&header, 1).Write(data, 32).Write(num, 4).Finalize(output);
}