#include "base58.h"

#include <string.h>
#include <string>
#include "uint256.h"
#include <assert.h>
#include <hash.h>


static const char* pszBase58 = "123456789ABCDEFGHJKLMNPQRSTUVWXYZabcdefghijkmnopqrstuvwxyz";

static const std::vector<unsigned char> mainNetworkSecretKey = { 0x80 };

bool DecodeBase58(const char* psz, std::vector<unsigned char>& vch)
{
	// Skip leading spaces.
	while (*psz && isspace(*psz))
		psz++;
	// Skip and count leading '1's.
	int zeroes = 0;
	while (*psz == '1') {
		zeroes++;
		psz++;
	}
	// Allocate enough space in big-endian base256 representation.
	std::vector<unsigned char> b256(strlen(psz) * 733 / 1000 + 1); // log(58) / log(256), rounded up.
	// Process the characters.
	while (*psz && !isspace(*psz)) {
		// Decode base58 character
		const char* ch = strchr(pszBase58, *psz);
		if (ch == NULL)
			return false;
		// Apply "b256 = b256 * 58 + ch".
		int carry = ch - pszBase58;
		for (std::vector<unsigned char>::reverse_iterator it = b256.rbegin(); it != b256.rend(); it++) {
			carry += 58 * (*it);
			*it = carry % 256;
			carry /= 256;
		}
		assert(carry == 0);
		psz++;
	}
	// Skip trailing spaces.
	while (isspace(*psz))
		psz++;
	if (*psz != 0)
		return false;
	// Skip leading zeroes in b256.
	std::vector<unsigned char>::iterator it = b256.begin();
	while (it != b256.end() && *it == 0)
		it++;
	// Copy result into output vector.
	vch.reserve(zeroes + (b256.end() - it));
	vch.assign(zeroes, 0x00);
	while (it != b256.end())
		vch.push_back(*(it++));
	return true;
}

std::string EncodeBase58(const unsigned char* pbegin, const unsigned char* pend)
{
	// Skip & count leading zeroes.
	int zeroes = 0;
	while (pbegin != pend && *pbegin == 0) {
		pbegin++;
		zeroes++;
	}
	// Allocate enough space in big-endian base58 representation.
	std::vector<unsigned char> b58((pend - pbegin) * 138 / 100 + 1); // log(256) / log(58), rounded up.
	// Process the bytes.
	while (pbegin != pend) {
		int carry = *pbegin;
		// Apply "b58 = b58 * 256 + ch".
		for (std::vector<unsigned char>::reverse_iterator it = b58.rbegin(); it != b58.rend(); it++) {
			carry += 256 * (*it);
			*it = carry % 58;
			carry /= 58;
		}
		assert(carry == 0);
		pbegin++;
	}
	// Skip leading zeroes in base58 result.
	std::vector<unsigned char>::iterator it = b58.begin();
	while (it != b58.end() && *it == 0)
		it++;
	// Translate the result into a string.
	std::string str;
	str.reserve(zeroes + (b58.end() - it));
	str.assign(zeroes, '1');
	while (it != b58.end())
		str += pszBase58[*(it++)];
	return str;
}

std::string EncodeBase58(const std::vector<unsigned char>& vch)
{
	return EncodeBase58(&vch[0], &vch[0] + vch.size());
}

std::string EncodeBase58Check(const std::vector<unsigned char>& vchIn)
{
	// add 4-byte hash check to the end
	std::vector<unsigned char> vch(vchIn);
	uint256 hash = Hash(vch.begin(), vch.end());
	vch.insert(vch.end(), (unsigned char*)& hash, (unsigned char*)& hash + 4);
	return EncodeBase58(vch);
}

bool DecodeBase58(const std::string& str, std::vector<unsigned char>& vchRet)
{
	return DecodeBase58(str.c_str(), vchRet);
}

bool DecodeBase58Check(const char* psz, std::vector<unsigned char>& vchRet)
{
	if (!DecodeBase58(psz, vchRet) ||
		(vchRet.size() < 4)) {
		vchRet.clear();
		return false;
	}
	// re-calculate the checksum, insure it matches the included 4-byte checksum
	uint256 hash = Hash(vchRet.begin(), vchRet.end() - 4);
	if (memcmp(&hash, &vchRet.end()[-4], 4) != 0) {
		vchRet.clear();
		return false;
	}
	vchRet.resize(vchRet.size() - 4);
	return true;
}

bool DecodeBase58Check(const std::string& str, std::vector<unsigned char>& vchRet)
{
	return DecodeBase58Check(str.c_str(), vchRet);
}

CBase58Data::CBase58Data()
{
	vchVersion.clear();
	vchData.clear();
}

void CBase58Data::SetData(const std::vector<unsigned char>& vchVersionIn, const void* pdata, size_t nSize)
{
	vchVersion = vchVersionIn;
	vchData.resize(nSize);
	if (!vchData.empty())
		memcpy(&vchData[0], pdata, nSize);
}

void CBase58Data::SetData(const std::vector<unsigned char>& vchVersionIn, const unsigned char* pbegin, const unsigned char* pend)
{
	SetData(vchVersionIn, (void*)pbegin, pend - pbegin);
}

bool CBase58Data::SetString(const char* psz, unsigned int nVersionBytes)
{
	std::vector<unsigned char> vchTemp;
	bool rc58 = DecodeBase58Check(psz, vchTemp);
	if ((!rc58) || (vchTemp.size() < nVersionBytes)) {
		vchData.clear();
		vchVersion.clear();
		return false;
	}
	vchVersion.assign(vchTemp.begin(), vchTemp.begin() + nVersionBytes);
	vchData.resize(vchTemp.size() - nVersionBytes);
	if (!vchData.empty())
		memcpy(&vchData[0], &vchTemp[nVersionBytes], vchData.size());
	memory_cleanse(&vchTemp[0], vchData.size());
	return true;
}

bool CBase58Data::SetString(const std::string& str, unsigned int nVersionBytes)
{
	return SetString(str.c_str(), nVersionBytes);
}

std::string CBase58Data::ToString() const
{
	std::vector<unsigned char> vch = vchVersion;
	vch.insert(vch.end(), vchData.begin(), vchData.end());
	return EncodeBase58Check(vch);
}

std::string CBase58Data::ToStringPrefix(const std::vector< unsigned char >& vchVersionIn, std::vector< unsigned char> data) const
{
	std::vector<unsigned char> vch = vchVersionIn;
	vch.insert(vch.end(), data.begin(), data.end());
	return EncodeBase58Check(vch);
}

int CBase58Data::CompareTo(const CBase58Data& b58) const
{
	if (vchVersion < b58.vchVersion)
		return -1;
	if (vchVersion > b58.vchVersion)
		return 1;
	if (vchData < b58.vchData)
		return -1;
	if (vchData > b58.vchData)
		return 1;
	return 0;
}


void CBitcoinSecret::SetKey(const CKey& vchSecret)
{
	assert(vchSecret.IsValid());
	SetData(mainNetworkSecretKey, vchSecret.begin(), vchSecret.size());
	if (vchSecret.IsCompressed())
		vchData.push_back(1);
}

CKey CBitcoinSecret::GetKey()
{
	CKey ret;
	assert(vchData.size() >= 32);
	ret.Set(vchData.begin(), vchData.begin() + 32, vchData.size() > 32 && vchData[32] == 1);
	return ret;
}

bool CBitcoinSecret::IsValid() const
{
	bool fExpectedFormat = vchData.size() == 32 || (vchData.size() == 33 && vchData[32] == 1);
	bool fCorrectVersion = vchVersion == mainNetworkSecretKey;
	return fExpectedFormat && fCorrectVersion;
}

bool CBitcoinSecret::SetString(const char* pszSecret)
{
	return CBase58Data::SetString(pszSecret, 1) && IsValid();
}

bool CBitcoinSecret::SetString(const std::string& strSecret)
{
	return SetString(strSecret.c_str());
}