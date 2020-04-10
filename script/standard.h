#ifndef VDS_SCRIPT_STANDARD_H
#define VDS_SCRIPT_STANDARD_H

#include <boost/variant.hpp>

/**
 * A txout script template with a specific destination. It is either:
 *  * CNoDestination: no destination set
 *  * CKeyID: TX_PUBKEYHASH destination
 *  * CScriptID: TX_SCRIPTHASH destination
 *  A CTxDestination is the internal data type encoded in a CBitcoinAddress
 */
typedef boost::variant<CKeyID> CTxDestination;




#endif // VDS_SCRIPT_STANDARD_H