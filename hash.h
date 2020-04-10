#ifndef VDS_HASH_H
#define VDS_HASH_H

#include <uint256.h>
#include <crypto/sha256.h>
#include <crypto/ripemd160.h>
#include <cstring>
#include "serialize.h"
#include <string>
#include <string.h>
#include <iostream>
typedef uint256 ChainCode;

/** A hasher class for Bitcoin's 256-bit hash (double SHA-256). */
class CHash256 {
private:
	CSHA256 sha;
public:
	static const size_t OUTPUT_SIZE = CSHA256::OUTPUT_SIZE;

	void Finalize(unsigned char hash[OUTPUT_SIZE]) {
		unsigned char buf[OUTPUT_SIZE];
		sha.Finalize(buf);
		sha.Reset().Write(buf, sha.OUTPUT_SIZE).Finalize(hash);
	}

	CHash256& Write(const unsigned char* data, size_t len) {
		sha.Write(data, len);
		return *this;
	}

	CHash256& Reset() {
		sha.Reset();
		return *this;
	}
};

/** A hasher class for Bitcoin's 160-bit hash (SHA-256 + RIPEMD-160). */
class CHash160 {
private:
	CSHA256 sha;
public:
	static const size_t OUTPUT_SIZE = CRIPEMD160::OUTPUT_SIZE;

	void Finalize(unsigned char hash[OUTPUT_SIZE]) {
		unsigned char buf[sha.OUTPUT_SIZE];
		sha.Finalize(buf);
		CRIPEMD160().Write(buf, sha.OUTPUT_SIZE).Finalize(hash);
	}

	CHash160& Write(const unsigned char* data, size_t len) {
		sha.Write(data, len);
		return *this;
	}

	CHash160& Reset() {
		sha.Reset();
		return *this;
	}
};

/** Compute the 256-bit hash of an object. */
template<typename T1>
inline uint256 Hash(const T1 pbegin, const T1 pend)
{
	static const unsigned char pblank[1] = {};
	uint256 result;
	CHash256().Write(pbegin == pend ? pblank : (const unsigned char*)& pbegin[0], (pend - pbegin) * sizeof(pbegin[0]))
		.Finalize((unsigned char*)& result);
	return result;
}

/** Compute the 160-bit hash an object. */
template<typename T1>
inline uint160 Hash160(const T1 pbegin, const T1 pend)
{
	static unsigned char pblank[1] = {};
	uint160 result;
	CHash160().Write(pbegin == pend ? pblank : (const unsigned char*)& pbegin[0], (pend - pbegin) * sizeof(pbegin[0]))
		.Finalize((unsigned char*)& result);
	return result;
}


class CHashWriter
{
private:
	CHash256 ctx;

public:
	int nType;
	int nVersion;

	CHashWriter(int nTypeIn, int nVersionIn) : nType(nTypeIn), nVersion(nVersionIn) {}

	CHashWriter& write(const char* pch, size_t size) {
		ctx.Write((const unsigned char*)pch, size);
		return (*this);
	}

	// invalidates the object
	uint256 GetHash() {
		uint256 result;
		ctx.Finalize((unsigned char*)& result);
		return result;
	}

	template<typename T>
	CHashWriter& operator<<(const T& obj) {
		// Serialize to this stream
		::Serialize(*this, obj);
		return (*this);
	}
};


void BIP32Hash(const ChainCode& chainCode, unsigned int nChild, unsigned char header, const unsigned char data[32], unsigned char output[64]);

#endif // VDS_HASH_H
