#ifndef VDS_BASE58_H
#define VDS_BASE58_H

#include <string>
#include <vector>
#include "support/allocators/zeroafterfree.h"
#include <key.h>

/**
 * Decode a base58-encoded string (psz) into a byte vector (vchRet).
 * return true if decoding is successful.
 * psz cannot be NULL.
 */
bool DecodeBase58(const char* psz, std::vector<unsigned char>& vchRet);

/**
 * Decode a base58-encoded string (str) into a byte vector (vchRet).
 * return true if decoding is successful.
 */
bool DecodeBase58(const std::string& str, std::vector<unsigned char>& vchRet);

/**
 * Encode a byte vector into a base58-encoded string, including checksum
 */
std::string EncodeBase58Check(const std::vector<unsigned char>& vchIn);

bool DecodeBase58Check(const std::string& str, std::vector<unsigned char>& vchRet);

bool DecodeBase58Check(const char* psz, std::vector<unsigned char>& vchRet);


/**
 * Base class for all base58-encoded data
 */
class CBase58Data
{
protected:
	//! the version byte(s)
	std::vector<unsigned char> vchVersion;

	//! the actually encoded data
	typedef std::vector<unsigned char, zero_after_free_allocator<unsigned char> > vector_uchar;
	vector_uchar vchData;

	CBase58Data();
	void SetData(const std::vector<unsigned char>& vchVersionIn, const void* pdata, size_t nSize);
	void SetData(const std::vector<unsigned char>& vchVersionIn, const unsigned char* pbegin, const unsigned char* pend);

public:
	bool SetString(const char* psz, unsigned int nVersionBytes);
	bool SetString(const std::string& str, unsigned int nVersionBytes);
	std::string ToStringPrefix(const std::vector<unsigned char>& vchVersionIn, std::vector<unsigned char> data) const;
	std::string ToString() const;
	int CompareTo(const CBase58Data& b58) const;

	bool operator!=(const CBase58Data& b58) const
	{
		return CompareTo(b58) != 0;
	}
	bool operator==(const CBase58Data& b58) const
	{
		return CompareTo(b58) == 0;
	}
	bool operator<=(const CBase58Data& b58) const
	{
		return CompareTo(b58) <= 0;
	}
	bool operator>=(const CBase58Data& b58) const
	{
		return CompareTo(b58) >= 0;
	}
	bool operator< (const CBase58Data& b58) const
	{
		return CompareTo(b58) < 0;
	}
	bool operator> (const CBase58Data& b58) const
	{
		return CompareTo(b58) > 0;
	}
};

class CBitcoinSecret : public CBase58Data
{
public:
	void SetKey(const CKey& vchSecret);
	CKey GetKey();
	bool IsValid() const;
	bool SetString(const char* pszSecret);
	bool SetString(const std::string& strSecret);

	CBitcoinSecret(const CKey& vchSecret)
	{
		SetKey(vchSecret);
	}
	CBitcoinSecret() {}
};

#endif // VDS_BASE58_H