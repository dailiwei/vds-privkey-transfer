#include <stdio.h>
#include "blockChainInterface.h"
#include <vector>
#include <key_io.h>
#include <utilstrencodings.h>
#include <hash.h>
#include <iostream>
#include <base58.h>
using namespace std;

const std::string strMessageMagic = "Bitcoin Signed Message:\n";

string bytestohexstring(const unsigned char* bytes, int bytelength)
{
	string str("");
	string str2("0123456789abcdef");
	for (int i = 0; i < bytelength; i++) {
		int b;
		b = 0x0f & (bytes[i] >> 4);
		char s1 = str2.at(b);
		str.append(1, str2.at(b));
		b = 0x0f & bytes[i];
		str.append(1, str2.at(b));
		char s2 = str2.at(b);
	}
	return str;
}

unsigned char verifymessage(char* strAddressChar, char* strSignChar, char* strMessageChar) {
	std::string strAddress = std::string(strAddressChar);
	std::string strMessage = std::string(strMessageChar);
	CKeyID dest = DecodeDestination(strAddress);
	bool fInvalid = false;
	std::vector<unsigned char> vchSig = DecodeBase64(strSignChar, &fInvalid);
	std::cout << bytestohexstring(&vchSig[1],64) << std::endl;

	if (fInvalid)
		return false;
	CHashWriter ss(SER_GETHASH, 0);
	ss << strMessageMagic;
	ss << strMessage;
	CPubKey pubkey;
	uint256 hash = ss.GetHash();
	std::cout << hash.GetHex() << std::endl;
	if (!pubkey.RecoverCompact(hash, vchSig))
		return false;
	std::cout << pubkey.size()<< std::endl;
	std::cout << bytestohexstring(pubkey.begin(), pubkey.size()) << std::endl;
	return (pubkey.GetID() == dest);
}

std::string getVAddressFromPrivateKey(char* privateKey)
{
	CBitcoinSecret vchSecret;
	bool fGood = vchSecret.SetString(privateKey);
	if (!fGood) {
		throw new CException("Invalid private key encoding");
	}
	CKey key = vchSecret.GetKey();
	if (!key.IsValid()) {
		throw new CException("Private key outside allowed range");
	}
	CPubKey pubkey = key.GetPubKey();

	assert(key.VerifyPubKey(pubkey));
	CKeyID vchAddress = pubkey.GetID();
	std::string address = EncodeDestination(vchAddress);
	//cout << address << endl;
	return address;
}

std::string masternode_genkey(bool fCompressed)
{
	CKey secret;
	secret.MakeNewKey(fCompressed);
	return CBitcoinSecret(secret).ToString();
}


std::string address_new(bool fCompressed,std::string& privateKey)
{
	CKey key;
	key.MakeNewKey(fCompressed);
	privateKey = CBitcoinSecret(key).ToString();
	CPubKey pubkey = key.GetPubKey();
	CKeyID vchAddress = pubkey.GetID();
	return  EncodeDestination(vchAddress);
}



