#ifndef UTIL_H
#define UTIL_H

#include <string>
#include <vector>
#include <map>

namespace Util
{
	namespace Buffer
	{
		bool ReadString(const std::vector<char>& buffer, size_t& offset, std::string &str);
		bool ReadString(const std::vector<unsigned char>& buffer, size_t& offset, std::string &str);
		
		std::string ToString(const std::vector<char>& buffer);
		std::string ToString(const std::vector<unsigned char>& buffer);
	}
	
	namespace Url
	{
		/*
			Type definitions
		*/
		typedef std::map<std::string, std::string> Variables;
		
		void GetElements(const std::string& url, std::string& url_base, Util::Url::Variables& url_variables);
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
