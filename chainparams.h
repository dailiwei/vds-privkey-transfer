#ifndef VDS_CHAINPARAMS_H
#define VDS_CHAINPARAMS_H
#include <vector>
using namespace std;

class CChainParams
{
public:
	enum Base58Type {
		PUBKEY_ADDRESS,
		SCRIPT_ADDRESS,
		SECRET_KEY,
		EXT_PUBLIC_KEY,
		EXT_SECRET_KEY,

		ZCPAYMENT_ADDRRESS,
		ZCSPENDING_KEY,
		ZCVIEWING_KEY,

		MAX_BASE58_TYPES
	};

	const std::vector<unsigned char>& Base58Prefix(Base58Type type) const
	{
		return base58Prefixes[type];
	}
protected:
	CChainParams() {}
	std::vector<unsigned char> base58Prefixes[MAX_BASE58_TYPES];
};

/**
 * Return the currently selected parameters. This won't change after app
 * startup, except for unit tests.
 */
const CChainParams& Params();

#endif // VDS_CHAINPARAMS_H