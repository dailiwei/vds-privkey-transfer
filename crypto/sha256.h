#ifndef BITCOIN_CRYPTO_SHA256_H
#define BITCOIN_CRYPTO_SHA256_H

#include <stdint.h>
#include <stdlib.h>
#include <string>


/** A hasher class for SHA-256. */
class CSHA256
{
public:
	static const size_t OUTPUT_SIZE = 32;

	CSHA256();

	CSHA256& Write(const unsigned char* data, size_t len);
	void Finalize(unsigned char hash[OUTPUT_SIZE]);
	void FinalizeNoPadding(unsigned char hash[OUTPUT_SIZE])
	{
		FinalizeNoPadding(hash, true);
	};
	CSHA256& Reset();

private:
	uint32_t s[8];
	unsigned char buf[64];
	uint64_t bytes;
	void FinalizeNoPadding(unsigned char hash[OUTPUT_SIZE], bool enforce_compression);
};

#endif // BITCOIN_CRYPTO_SHA256_H