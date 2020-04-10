#include "chainparams.h"

class CMainParams : public CChainParams
{
public:
	CMainParams()
	{
		base58Prefixes[PUBKEY_ADDRESS] = { 0x10, 0x1C };
		// guarantees the first 2 characters, when base58 encoded, are "Vs"
		base58Prefixes[SCRIPT_ADDRESS] = { 0x10, 0x41 };
		// the first character, when base58 encoded, is "5" or "K" or "L" (as in Bitcoin)
		base58Prefixes[SECRET_KEY] = { 0x80 };
		// do not rely on these BIP32 prefixes; they are not specified and may change
		base58Prefixes[EXT_PUBLIC_KEY] = { 0x04, 0x88, 0xB2, 0x1E };
		base58Prefixes[EXT_SECRET_KEY] = { 0x04, 0x88, 0xAD, 0xE4 };
		// guarantees the first 2 characters, when base58 encoded, are "zc"
		base58Prefixes[ZCPAYMENT_ADDRRESS] = { 0x0B, 0x36 };
		// guarantees the first 4 characters, when base58 encoded, are "ZiVK"
		base58Prefixes[ZCVIEWING_KEY] = { 0xA8, 0xAB, 0xD3 };
		// guarantees the first 2 characters, when base58 encoded, are "SK"
		base58Prefixes[ZCSPENDING_KEY] = { 0x0F, 0xDB };
	}
};


static CMainParams mainParams;


const CChainParams& Params()
{
	return mainParams;
}