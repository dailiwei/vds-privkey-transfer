// testCmake.cpp: 定义应用程序的入口点。
//

#include "main.h"
#include <blockChainInterface.h>
#include <secp256k1.h>
#include <parse.h>

using namespace std;

int main(int argc, char* argv[])
{
	if (argc <= 1) {
		cout << "Please input your method" << endl;
		return 0;
	}
	string command = argv[1];
	if (command == "getaddress") {
		if (argc <= 2) {
			cout << "Please input Private Key" << endl;
			return 0;
		}
		try {
			string str = getVAddressFromPrivateKey(argv[2]);
			cout << str << endl;
		}
		catch (CException* e) {
			cout << e->msg << endl;
			return 0;
		}
	}

	if (command == "masternode") {
		try {
			if (argc <= 2) {
				cout << "Please input your method" << endl;
				return 0;
			}
			string subCommand = argv[2];
			if (subCommand == "genkey") {
				bool comporessed;
				if (argc <= 3) {
					comporessed = false;
				}
				else
				{
					comporessed = ParseBool(argv[3], "compressed");
				}
				string privateKey = masternode_genkey(comporessed);
				cout << privateKey << endl;
			}
		}
		catch (CException* e) {
			cout << e->msg << endl;
			return 0;
		}
	}

	if (command == "vaddress") {
		if (argc <= 2) {
			cout << "Please input your method" << endl;
			return 0;
		}
		string subCommand = argv[2];
		if (subCommand == "genkey") {
			genkey();
		}
	}

	if (command == "genkey") {
		genkey();
	}

	return 0;
}

void genkey() {
	string privatekey = "";
	string address = address_new(false, privatekey);
	cout << "privatekey:" << privatekey << endl;
	cout << "address:" << address << endl;
}