#include <string>
#include <sstream>
#include <iomanip>
#include <openssl/md5.h>
#include <numeric>
#include <random>

#include <util.h>

std::string Util::Buffer2String(const std::vector<char> &buffer)
{
	std::string s;
	
	for(char v : buffer)
	{
		if((v >= 32 && v <= 126) || (v == 9))
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

std::string Util::ToString(const std::vector<int>& list)
{
	if (list.empty())
	{
		return "";
	}

	return std::accumulate(
		list.begin() + 1, list.end(), std::to_string(list[0]),
		[](const std::string& a, const int b)
		{
			return a + ',' + std::to_string(b);
		}
	);
}

std::string Util::generateRandomChallenge()
{
	const int length = 10;
	const std::string characters = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";
	std::random_device rd;
	std::mt19937 generator(rd());
	std::uniform_int_distribution<int> distribution(0, characters.size() - 1);

	std::string randomString;
	randomString.reserve(length);

	for (int i = 0; i < length; ++i) 
	{
		randomString += characters[distribution(generator)];
	}

	return randomString;
}

std::string Util::generateRandomAuthtoken()
{
	const int length = 24;
	const std::string characters = "abcdefghijklmnopqrstuvwxyz0123456789";
	std::random_device rd;
	std::mt19937 generator(rd());
	std::uniform_int_distribution<int> distribution(0, characters.size() - 1);

	std::string randomString;
	randomString.reserve(length);

	for (int i = 0; i < length; ++i) 
	{
		randomString += characters[distribution(generator)];
	}

	return randomString;
}


