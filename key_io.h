#ifndef BITCOIN_KEYIO_H
#define BITCOIN_KEYIO_H

#include <pubkey.h>
#include "script/standard.h"

CKeyID DecodeDestination(const std::string& str);

std::string EncodeDestination(const CTxDestination& dest);

#endif // BITCOIN_KEYIO_H