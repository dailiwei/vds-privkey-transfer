#include <parse.h>
#include <blockChainInterface.h>


bool ParseBool(const std::string& strBool, const std::string& strName)
{
	if (strBool == "true" || strBool == "yes" || strBool == "1") {
		return true;
	}
	else if (strBool == "false" || strBool == "no" || strBool == "0") {
		return false;
	}
	throw CException(strName + " must be true, false, yes, no, 1 or 0 (not '" + strBool + "')");
}