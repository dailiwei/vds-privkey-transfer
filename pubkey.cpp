#include "pubkey.h"

#include <secp256k1.h>
#include <secp256k1_recovery.h>

#include <iostream>


namespace
{
	/* Global secp256k1_context object used for verification. */
	secp256k1_context* secp256k1_context_verify = secp256k1_context_create(SECP256K1_CONTEXT_VERIFY);
}


bool CPubKey::Verify(const uint256& hash, const std::vector<unsigned char>& vchSig) const {
	if (!IsValid())
		return false;
	secp256k1_pubkey pubkey;
	secp256k1_ecdsa_signature sig;
	if (!secp256k1_ec_pubkey_parse(secp256k1_context_verify, &pubkey, &(*this)[0], size())) {
		return false;
	}
	if (vchSig.size() == 0) {
		return false;
	}
	if (!secp256k1_ecdsa_signature_parse_der(secp256k1_context_verify, &sig, &vchSig[0], vchSig.size())) {
		return false;
	}
	/* libsecp256k1's ECDSA verification requires lower-S signatures, which have
	 * not historically been enforced in Bitcoin, so normalize them first. */
	secp256k1_ecdsa_signature_normalize(secp256k1_context_verify, &sig, &sig);
	return secp256k1_ecdsa_verify(secp256k1_context_verify, &sig, hash.begin(), &pubkey);
}



bool CPubKey::RecoverCompact(const uint256& hash, const std::vector<unsigned char>& vchSig) {
	if (vchSig.size() != COMPACT_SIGNATURE_SIZE)
		return false;
	int recid = (vchSig[0] - 27) & 3;
	bool fComp = ((vchSig[0] - 27) & 4) != 0;
	secp256k1_pubkey pubkey;
	secp256k1_ecdsa_recoverable_signature sig;
	std::cout << recid << std::endl;
	if (!secp256k1_ecdsa_recoverable_signature_parse_compact(secp256k1_context_verify, &sig, &vchSig[1], recid)) {
		return false;
	}
	if (!secp256k1_ecdsa_recover(secp256k1_context_verify, &pubkey, &sig, hash.begin())) {
		return false;
	}
	unsigned char pub[PUBLIC_KEY_SIZE];
	size_t publen = PUBLIC_KEY_SIZE;
	secp256k1_ec_pubkey_serialize(secp256k1_context_verify, pub, &publen, &pubkey, fComp ? SECP256K1_EC_COMPRESSED : SECP256K1_EC_UNCOMPRESSED);
	Set(pub, pub + publen);
	return true;
}

bool CPubKey::IsFullyValid() const {
	if (!IsValid())
		return false;
	secp256k1_pubkey pubkey;
	return secp256k1_ec_pubkey_parse(secp256k1_context_verify, &pubkey, &(*this)[0], size());
}

bool CPubKey::Decompress() {
	if (!IsValid())
		return false;
	secp256k1_pubkey pubkey;
	if (!secp256k1_ec_pubkey_parse(secp256k1_context_verify, &pubkey, &(*this)[0], size())) {
		return false;
	}
	unsigned char pub[PUBLIC_KEY_SIZE];
	size_t publen = PUBLIC_KEY_SIZE;
	secp256k1_ec_pubkey_serialize(secp256k1_context_verify, pub, &publen, &pubkey, SECP256K1_EC_UNCOMPRESSED);
	Set(pub, pub + publen);
	return true;
}



/* static */ bool CPubKey::CheckLowS(const std::vector<unsigned char>& vchSig) {
	secp256k1_ecdsa_signature sig;

	if (!secp256k1_ecdsa_signature_parse_der(secp256k1_context_verify, &sig, &vchSig[0], vchSig.size())) {
		return false;
	}
	return (!secp256k1_ecdsa_signature_normalize(secp256k1_context_verify, NULL, &sig));
}