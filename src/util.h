#ifndef UTIL_H
#define UTIL_H

#include <string>
#include <vector>

namespace Util
{
	namespace Buffer
	{
		bool ReadString(const std::vector<char>& buffer, size_t& offset, std::string &str);
		bool ReadString(const std::vector<unsigned char>& buffer, size_t& offset, std::string &str);
		
		std::string ToString(const std::vector<char>& buffer);
		std::string ToString(const std::vector<unsigned char>& buffer);
	}
	
	std::string MD5hash(const std::string& input);
	
	std::string ToString(const std::vector<int>& list);
	
	std::string generateRandomChallenge();
	std::string generateRandomAuthtoken();
	
	std::vector<int> convertProfileIdToVector(const std::string& input);
	
	std::string GetNowTime();
	std::string GetNowDateTime();
}

#endif // UTIL_H
