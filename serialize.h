#ifndef VDS_SERIALIZE_H
#define VDS_SERIALIZE_H

#include <limits>
#include <ios>

static const unsigned int MAX_SIZE = 0x10000000; // Qtum: Increase max serialized size to 256mb

enum {
	// primary actions
	SER_NETWORK = (1 << 0),
	SER_DISK = (1 << 1),
	SER_GETHASH = (1 << 2),
};

/*
 * Lowest-level serialization and conversion.
 * @note Sizes of these types are verified in the tests
 */
template<typename Stream> inline void ser_writedata8(Stream& s, uint8_t obj)
{
	s.write((char*)& obj, 1);
}
template<typename Stream> inline void ser_writedata16(Stream& s, uint16_t obj)
{
	obj = htole16(obj);
	s.write((char*)& obj, 2);
}
template<typename Stream> inline void ser_writedata32(Stream& s, uint32_t obj)
{
	obj = htole32(obj);
	s.write((char*)& obj, 4);
}
template<typename Stream> inline void ser_writedata32be(Stream& s, uint32_t obj)
{
	obj = htobe32(obj);
	s.write((char*)& obj, 4);
}
template<typename Stream> inline void ser_writedata64(Stream& s, uint64_t obj)
{
	obj = htole64(obj);
	s.write((char*)& obj, 8);
}
template<typename Stream> inline uint8_t ser_readdata8(Stream& s)
{
	uint8_t obj;
	s.read((char*)& obj, 1);
	return obj;
}
template<typename Stream> inline uint16_t ser_readdata16(Stream& s)
{
	uint16_t obj;
	s.read((char*)& obj, 2);
	return le16toh(obj);
}
template<typename Stream> inline uint32_t ser_readdata32(Stream& s)
{
	uint32_t obj;
	s.read((char*)& obj, 4);
	return le32toh(obj);
}
template<typename Stream> inline uint32_t ser_readdata32be(Stream& s)
{
	uint32_t obj;
	s.read((char*)& obj, 4);
	return be32toh(obj);
}
template<typename Stream> inline uint64_t ser_readdata64(Stream& s)
{
	uint64_t obj;
	s.read((char*)& obj, 8);
	return le64toh(obj);
}

template<typename Stream>
void WriteCompactSize(Stream& os, uint64_t nSize)
{
	if (nSize < 253) {
		ser_writedata8(os, nSize);
	}
	else if (nSize <= std::numeric_limits<unsigned short>::max()) {
		ser_writedata8(os, 253);
		ser_writedata16(os, nSize);
	}
	else if (nSize <= std::numeric_limits<unsigned int>::max()) {
		ser_writedata8(os, 254);
		ser_writedata32(os, nSize);
	}
	else {
		ser_writedata8(os, 255);
		ser_writedata64(os, nSize);
	}
	return;
}

template<typename Stream>
uint64_t ReadCompactSize(Stream& is)
{
	uint8_t chSize = ser_readdata8(is);
	uint64_t nSizeRet = 0;
	if (chSize < 253) {
		nSizeRet = chSize;
	}
	else if (chSize == 253) {
		nSizeRet = ser_readdata16(is);
		if (nSizeRet < 253)
			throw std::ios_base::failure("non-canonical ReadCompactSize()");
	}
	else if (chSize == 254) {
		nSizeRet = ser_readdata32(is);
		if (nSizeRet < 0x10000u)
			throw std::ios_base::failure("non-canonical ReadCompactSize()");
	}
	else {
		nSizeRet = ser_readdata64(is);
		if (nSizeRet < 0x100000000ULL)
			throw std::ios_base::failure("non-canonical ReadCompactSize()");
	}
	if (nSizeRet > (uint64_t)MAX_SIZE)
		throw std::ios_base::failure("ReadCompactSize(): size too large");
	return nSizeRet;
}

/**
 * Forward declarations
 */

 /**
  *  string
  */
template<typename Stream, typename C> void Serialize(Stream& os, const std::basic_string<C>& str);
template<typename Stream, typename C> void Unserialize(Stream& is, std::basic_string<C>& str);

/**
 * If none of the specialized versions above matched, default to calling member function.
 */
template<typename Stream, typename T>
inline void Serialize(Stream& os, const T& a)
{
	a.Serialize(os);
}

template<typename Stream, typename T>
inline void Unserialize(Stream& is, T& a)
{
	a.Unserialize(is);
}

/**
 * string
 */
template<typename Stream, typename C>
void Serialize(Stream& os, const std::basic_string<C>& str)
{
	WriteCompactSize(os, str.size());
	if (!str.empty())
		os.write((char*)& str[0], str.size() * sizeof(str[0]));
}

template<typename Stream, typename C>
void Unserialize(Stream& is, std::basic_string<C>& str)
{
	unsigned int nSize = ReadCompactSize(is);
	str.resize(nSize);
	if (nSize != 0)
		is.read((char*)& str[0], nSize * sizeof(str[0]));
}

#endif // VDS_SERIALIZE_H