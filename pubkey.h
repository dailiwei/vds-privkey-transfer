#ifndef VDS_PUBKEY_H
#define VDS_PUBKEY_H

#include "uint256.h"
#include <string.h>
#include <serialize.h>
#include <hash.h>

class CKeyID : public uint160
{
public:
	CKeyID() : uint160() {}
	CKeyID(const uint160& in) : uint160(in) {
		
	}
};

typedef uint256 ChainCode;

/** An encapsulated public key. */
class CPubKey
{
public:
	/**
	 * secp256k1:
	 */
	static const unsigned int PUBLIC_KEY_SIZE = 65;
	static const unsigned int COMPRESSED_PUBLIC_KEY_SIZE = 33;
	static const unsigned int SIGNATURE_SIZE = 72;
	static const unsigned int COMPACT_SIGNATURE_SIZE = 65;
	/**
	 * see www.keylength.com
	 * script supports up to 75 for single byte push
	 */
	static_assert(
		PUBLIC_KEY_SIZE >= COMPRESSED_PUBLIC_KEY_SIZE,
		"COMPRESSED_PUBLIC_KEY_SIZE is larger than PUBLIC_KEY_SIZE");

private:

	/**
	 * Just store the serialized data.
	 * Its length can very cheaply be computed from the first byte.
	 */
	unsigned char vch[PUBLIC_KEY_SIZE];

	//! Compute the length of a pubkey with a given first byte.
	unsigned int static GetLen(unsigned char chHeader)
	{
		if (chHeader == 2 || chHeader == 3)
			return COMPRESSED_PUBLIC_KEY_SIZE;
		if (chHeader == 4 || chHeader == 6 || chHeader == 7)
			return PUBLIC_KEY_SIZE;
		return 0;
	}

	//! Set this key data to be invalid
	void Invalidate()
	{
		vch[0] = 0xFF;
	}

public:
	//! Construct an invalid public key.
	CPubKey()
	{
		Invalidate();
	}

	//! Initialize a public key using begin/end iterators to byte data.
	template <typename T>
	void Set(const T pbegin, const T pend)
	{
		int len = pend == pbegin ? 0 : GetLen(pbegin[0]);
		if (len && len == (pend - pbegin))
			memcpy(vch, (unsigned char*)& pbegin[0], len);
		else
			Invalidate();
	}

	//! Construct a public key using begin/end iterators to byte data.
	template <typename T>
	CPubKey(const T pbegin, const T pend)
	{
		Set(pbegin, pend);
	}

	//! Construct a public key from a byte vector.
	CPubKey(const std::vector<unsigned char>& vch)
	{
		Set(vch.begin(), vch.end());
	}

	//! Simple read-only vector-like interface to the pubkey data.
	unsigned int size() const { return GetLen(vch[0]); }
	const unsigned char* begin() const { return vch; }
	const unsigned char* end() const { return vch + size(); }
	const unsigned char& operator[](unsigned int pos) const { return vch[pos]; }

	//! Comparator implementation.
	friend bool operator==(const CPubKey& a, const CPubKey& b)
	{
		return a.vch[0] == b.vch[0] &&
			memcmp(a.vch, b.vch, a.size()) == 0;
	}
	friend bool operator!=(const CPubKey& a, const CPubKey& b)
	{
		return !(a == b);
	}
	friend bool operator<(const CPubKey& a, const CPubKey& b)
	{
		return a.vch[0] < b.vch[0] ||
			(a.vch[0] == b.vch[0] && memcmp(a.vch, b.vch, a.size()) < 0);
	}

	//! Implement serialization, as if this was a byte vector.
	template <typename Stream>
	void Serialize(Stream& s) const
	{
		unsigned int len = size();
		::WriteCompactSize(s, len);
		s.write((char*)vch, len);
	}
	template <typename Stream>
	void Unserialize(Stream& s)
	{
		unsigned int len = ::ReadCompactSize(s);
		if (len <= PUBLIC_KEY_SIZE) {
			s.read((char*)vch, len);
		}
		else {
			// invalid pubkey, skip available data
			char dummy;
			while (len--)
				s.read(&dummy, 1);
			Invalidate();
		}
	}

	//! Get the KeyID of this public key (hash of its serialization)
	CKeyID GetID() const
	{
		uint160 uint160 = Hash160(vch, vch + size());
		return CKeyID(uint160);
	}

	//! Get the 256-bit hash of this public key.
	uint256 GetHash() const
	{
		return Hash(vch, vch + size());
	}

	/*
	 * Check syntactic correctness.
	 *
	 * Note that this is consensus critical as CheckSig() calls it!
	 */
	bool IsValid() const
	{
		return size() > 0;
	}

	//! fully validate whether this is a valid public key (more expensive than IsValid())
	bool IsFullyValid() const;

	//! Check whether this is a compressed public key.
	bool IsCompressed() const
	{
		return size() == COMPRESSED_PUBLIC_KEY_SIZE;
	}

	/**
	 * Verify a DER signature (~72 bytes).
	 * If this public key is not fully valid, the return value will be false.
	 */
	bool Verify(const uint256& hash, const std::vector<unsigned char>& vchSig) const;

	/**
	 * Check whether a signature is normalized (lower-S).
	 */
	static bool CheckLowS(const std::vector<unsigned char>& vchSig);

	//! Recover a public key from a compact signature.
	bool RecoverCompact(const uint256& hash, const std::vector<unsigned char>& vchSig);

	//! Turn this public key into an uncompressed public key.
	bool Decompress();

	//! Derive BIP32 child pubkey.
	bool Derive(CPubKey& pubkeyChild, ChainCode& ccChild, unsigned int nChild, const ChainCode& cc) const;
};

#endif // VDS_PUBKEY_H