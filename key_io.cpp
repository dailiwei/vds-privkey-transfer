#include <key_io.h>
#include <base58.h>
#include <boost/variant/apply_visitor.hpp>
#include <boost/variant/static_visitor.hpp>
#include <chainparams.h>

namespace
{
	class DestinationEncoder : public boost::static_visitor<std::string>
	{
	private:
		const CChainParams& m_params;

	public:
		DestinationEncoder(const CChainParams& params) : m_params(params) {}

		std::string operator()(const CKeyID& id) const
		{
			std::vector<unsigned char> data = m_params.Base58Prefix(CChainParams::PUBKEY_ADDRESS);
			data.insert(data.end(), id.begin(), id.end());
			return EncodeBase58Check(data);
		}
	};
}

CKeyID DecodeDestination(const std::string& str) {
	std::vector<unsigned char> data;
	uint160 hash;
	if (DecodeBase58Check(str, data)) {
		// base58-encoded vds addresses.
	   // Public-key-hash-addresses have version 0 (or 111 testnet).
	   // The data vector contains RIPEMD160(SHA256(pubkey)), where pubkey is the serialized public key.
		const std::vector<unsigned char>& pubkey_prefix = { 0x10, 0x1C };
		if (data.size() == hash.size() + pubkey_prefix.size() && std::equal(pubkey_prefix.begin(), pubkey_prefix.end(), data.begin())) {
			std::copy(data.begin() + pubkey_prefix.size(), data.end(), hash.begin());
			return CKeyID(hash);
		}
	}
	return CKeyID();
}


std::string EncodeDestination(const CTxDestination& dest)
{
	return boost::apply_visitor(DestinationEncoder(Params()), dest);
}