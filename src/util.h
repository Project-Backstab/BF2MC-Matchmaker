#ifndef UTIL_H
#define UTIL_H

#include <string>
#include <vector>

namespace Util
{
	std::string Buffer2String(const std::vector<char>& buffer);
	
	std::string MD5hash(const std::string& input);
	
	std::string ToString(const std::vector<int>& list);
	
	std::string generateRandomChallenge();
	std::string generateRandomAuthtoken();
	
	std::vector<int> convertProfileIdToVector(const std::string& input);
	
	std::string GetNowTime();
	std::string GetNowDateTime();
}

#endif // UTIL_H
