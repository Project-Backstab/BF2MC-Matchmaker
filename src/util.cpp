#include <string>
#include <sstream>
#include <iomanip>
#include <openssl/md5.h>
#include <numeric>
#include <random>
#include <chrono>

#include <util.h>

bool Util::Buffer::ReadString(const std::vector<char>& buffer, size_t& offset, std::string &str)
{	
	str.clear();
	
	for(;offset < buffer.size(); offset++)
	{
		if(buffer[offset] == 0x0)
		{
			offset++;
			return true;
		}
		
		str += buffer[offset];
	}
	
	return false;
}

bool Util::Buffer::ReadString(const std::vector<unsigned char>& buffer, size_t& offset, std::string &str)
{
	std::vector<char> buffer2(buffer.begin(), buffer.end());
	
	return Util::Buffer::ReadString(buffer2, offset, str);
}

std::string Util::Buffer::ToString(const std::vector<char>& buffer)
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

std::string Util::Buffer::ToString(const std::vector<unsigned char>& buffer)
{
	std::vector<char> buffer2(buffer.begin(), buffer.end());
	
	return Util::Buffer::ToString(buffer2);
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

std::vector<int> Util::convertProfileIdToVector(const std::string& input)
{
	std::vector<int> result;
	std::stringstream ss(input);
	std::string item;

	while (std::getline(ss, item, ',')) {
		try
		{
			int value = std::stoi(item);
			result.push_back(value);
		}
		catch (...) {};
	}

	return result;
}

std::string Util::GetNowTime()
{
	char timeStr[9];

	auto now = std::chrono::system_clock::now();
	std::time_t time = std::chrono::system_clock::to_time_t(now);
	std::tm* timeInfo = std::localtime(&time);
	std::strftime(timeStr, sizeof(timeStr), "%H:%M:%S", timeInfo);

	return timeStr;
}

std::string Util::GetNowDateTime()
{
	auto now = std::chrono::system_clock::now();
	std::time_t time = std::chrono::system_clock::to_time_t(now);
	std::tm* timeInfo = std::localtime(&time);

	std::ostringstream oss;
	oss << std::put_time(timeInfo, "%Y%m%d-%H%M%S");
	return oss.str();
}

