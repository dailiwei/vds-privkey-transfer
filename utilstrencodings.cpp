#include <utilstrencodings.h>

#include <cstdlib>
#include <cstring>
#include <errno.h>
#include <limits>

using namespace std;

vector<unsigned char> DecodeBase64(const char* p, bool* pfInvalid)
{
	static const int decode64_table[256] = {
		-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
			-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
			-1, -1, -1, 62, -1, -1, -1, 63, 52, 53, 54, 55, 56, 57, 58, 59, 60, 61, -1, -1,
			-1, -1, -1, -1, -1,  0,  1,  2,  3,  4,  5,  6,  7,  8,  9, 10, 11, 12, 13, 14,
			15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, -1, -1, -1, -1, -1, -1, 26, 27, 28,
			29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40, 41, 42, 43, 44, 45, 46, 47, 48,
			49, 50, 51, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
			-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
			-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
			-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
			-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
			-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
			-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1
	};

	if (pfInvalid)
		* pfInvalid = false;

	vector<unsigned char> vchRet;
	vchRet.reserve(strlen(p) * 3 / 4);

	int mode = 0;
	int left = 0;

	while (1) {
		int dec = decode64_table[(unsigned char)* p];
		if (dec == -1) break;
		p++;
		switch (mode) {
		case 0: // we have no bits and get 6
			left = dec;
			mode = 1;
			break;

		case 1: // we have 6 bits and keep 4
			vchRet.push_back((left << 2) | (dec >> 4));
			left = dec & 15;
			mode = 2;
			break;

		case 2: // we have 4 bits and get 6, we keep 2
			vchRet.push_back((left << 4) | (dec >> 2));
			left = dec & 3;
			mode = 3;
			break;

		case 3: // we have 2 bits and get 6
			vchRet.push_back((left << 6) | dec);
			mode = 0;
			break;
		}
	}

	if (pfInvalid)
		switch (mode) {
		case 0: // 4n base64 characters processed: ok
			break;

		case 1: // 4n+1 base64 character processed: impossible
			*pfInvalid = true;
			break;

		case 2: // 4n+2 base64 characters processed: require '=='
			if (left || p[0] != '=' || p[1] != '=' || decode64_table[(unsigned char)p[2]] != -1)
				* pfInvalid = true;
			break;

		case 3: // 4n+3 base64 characters processed: require '='
			if (left || p[0] != '=' || decode64_table[(unsigned char)p[1]] != -1)
				* pfInvalid = true;
			break;
		}

	return vchRet;
}

string DecodeBase64(const string& str)
{
	vector<unsigned char> vchRet = DecodeBase64(str.c_str());
	return (vchRet.size() == 0) ? string() : string((const char*)& vchRet[0], vchRet.size());
}


const signed char p_util_hexdigit[256] = {
	-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
		-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
		-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
		0, 1, 2, 3, 4, 5, 6, 7, 8, 9, -1, -1, -1, -1, -1, -1,
		-1, 0xa, 0xb, 0xc, 0xd, 0xe, 0xf, -1, -1, -1, -1, -1, -1, -1, -1, -1,
		-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
		-1, 0xa, 0xb, 0xc, 0xd, 0xe, 0xf, -1, -1, -1, -1, -1, -1, -1, -1, -1,
		-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
		-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
		-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
		-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
		-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
		-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
		-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
		-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
		-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
};

signed char HexDigit(char c)
{
	return p_util_hexdigit[(unsigned char)c];
}

bool IsHex(const string& str)
{
	for (std::string::const_iterator it(str.begin()); it != str.end(); ++it) {
		if (HexDigit(*it) < 0)
			return false;
	}
	return (str.size() > 0) && (str.size() % 2 == 0);
}