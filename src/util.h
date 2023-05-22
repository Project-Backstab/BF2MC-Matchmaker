#ifndef UTIL_H
#define UTIL_H

#include <string>
#include <vector>

namespace Util
{
	std::string Buffer2String(const std::vector<char> &buffer);
	
	std::string MD5hash(const std::string& input);
}

#endif // UTIL_H
