#ifndef VDS_UINT256_H
#define VDS_UINT256_H

#include <assert.h>
#include <cstring>
#include <stdexcept>
#include <stdint.h>
#include <vector>
#include <string>
#include <string.h>

template<unsigned int BITS>
class base_blob
{
protected:
	enum { WIDTH = BITS / 8 };
	uint8_t data[WIDTH];
public:
	base_blob()
	{
		memset(data, 0, sizeof(data));
	}

	explicit base_blob(const std::vector<unsigned char>& vch);

	bool IsNull() const
	{
		for (int i = 0; i < WIDTH; i++)
			if (data[i] != 0)
				return false;
		return true;
	}

	void SetNull()
	{
		memset(data, 0, sizeof(data));
	}

	inline int Compare(const base_blob& other) const { return memcmp(data, other.data, sizeof(data)); }

	friend inline bool operator==(const base_blob& a, const base_blob& b) { return a.Compare(b) == 0; }
	friend inline bool operator!=(const base_blob& a, const base_blob& b) { return a.Compare(b) != 0; }
	friend inline bool operator<(const base_blob& a, const base_blob& b) { return a.Compare(b) < 0; }

	std::string GetHex() const;
	std::string GetReverseHex() const;
	void SetHex(const char* psz);
	void SetHex(const std::string& str);
	void SetReverseHex(const char* psz);
	void SetReverseHex(const std::string& str);
	std::string ToString() const;

	unsigned char* begin()
	{
		return &data[0];
	}

	unsigned char* end()
	{
		return &data[WIDTH];
	}

	const unsigned char* begin() const
	{
		return &data[0];
	}

	const unsigned char* end() const
	{
		return &data[WIDTH];
	}

	unsigned int size() const
	{
		return sizeof(data);
	}

	uint64_t GetUint64(int pos) const
	{
		const uint8_t* ptr = data + pos * 8;
		return ((uint64_t)ptr[0]) | \
			((uint64_t)ptr[1]) << 8 | \
			((uint64_t)ptr[2]) << 16 | \
			((uint64_t)ptr[3]) << 24 | \
			((uint64_t)ptr[4]) << 32 | \
			((uint64_t)ptr[5]) << 40 | \
			((uint64_t)ptr[6]) << 48 | \
			((uint64_t)ptr[7]) << 56;
	}

	template<typename Stream>
	void Serialize(Stream& s) const
	{
		s.write((char*)data, sizeof(data));
	}

	template<typename Stream>
	void Unserialize(Stream& s)
	{
		s.read((char*)data, sizeof(data));
	}
};

/** 160-bit opaque blob.
 * @note This type is called uint160 for historical reasons only. It is an opaque
 * blob of 160 bits and has no integer operations.
 */
class uint160 : public base_blob<160> {
public:
	uint160() {}
	uint160(const base_blob<160> & b) : base_blob<160>(b) {}
	explicit uint160(const std::vector<unsigned char>& vch) : base_blob<160>(vch) {}
};

class uint256 : public base_blob<256> {
public:
	uint256() {}
	uint256(const base_blob<256> & b) : base_blob<256>(b) {}
	explicit uint256(const std::vector<unsigned char>& vch) : base_blob<256>(vch) {}

};

#endif // VDS_UINT256_H