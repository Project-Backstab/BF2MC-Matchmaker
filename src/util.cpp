#include <string>
#include <sstream>
#include <iomanip>
#include <openssl/md5.h>

#include <util.h>

std::string Util::Buffer2String(const std::vector<char> &buffer)
{
	std::string s;
	
	for(char v : buffer)
	{
		if(v >= 32 && v <= 126)
		{
			s.push_back(v);
		}
		else
		{
			s.push_back('.');
		}
	}
	
	return s;
}

std::string Util::MD5hash(const std::string& input)
{
    unsigned char digest[MD5_DIGEST_LENGTH];
	
    MD5(reinterpret_cast<const unsigned char*>(input.c_str()), input.length(), digest);
    
	// Convert to hex string
	std::stringstream ss;
    for (int i = 0; i < MD5_DIGEST_LENGTH; ++i)
	{
        ss << std::hex << std::setw(2) << std::setfill('0') << static_cast<int>(digest[i]);
    }
	
    return ss.str();
}

