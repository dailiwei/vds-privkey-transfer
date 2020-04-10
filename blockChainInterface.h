#pragma once
#include <string>
using namespace std;

class CException : exception
{
public:
    string msg;
    CException(string s) : msg(s) {}
};

extern "C" unsigned char verifymessage(char* strAddress, char* strSign, char* strMessage);


extern "C" std::string getVAddressFromPrivateKey(char* privateKey);

extern "C" std::string masternode_genkey(bool fCompressed);


extern "C" std::string address_new(bool fCompressed, std::string& privateKey);

