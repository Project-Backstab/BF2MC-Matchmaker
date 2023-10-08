#include <mysql/mysql_time.h>
#include <regex>

#include <util.h>

#include <battlefield/player.h>

void Battlefield::Player::useExample()
{
	
}

bool Battlefield::Player::SetProfileId(int profileid)
{
	if(profileid >= 0)
	{
		this->_profileid = profileid;
		return true;
	}
	
	return false;
}

bool Battlefield::Player::SetProfileId(const std::string& str_profileid)
{
	try
	{
		int profileid = std::stoi(str_profileid);
		
		return this->SetProfileId(profileid);
	}
	catch(...) {};
	
	return false;
}

bool Battlefield::Player::SetUserId(int userid)
{
	if(userid >= 0)
	{
		this->_userid = userid;
		return true;
	}
	
	return false;
}

bool Battlefield::Player::SetUserId(const std::string& str_userid)
{
	try
	{
		int userid = std::stoi(str_userid);
		
		return this->SetUserId(userid);
	}
	catch(...) {};
	
	return false;
}

bool Battlefield::Player::SetNick(const std::string& nick)
{
	if(nick.size() > 41)
	{
		this->_nick = nick.substr(0, 40);
	}
	else
	{
		this->_nick = nick;
	}
	
	return true;
}

bool Battlefield::Player::SetUniquenick(const std::string& uniquenick)
{
	if(uniquenick.size() > 32)
	{
		this->_uniquenick = uniquenick.substr(0, 31);
	}
	else
	{
		this->_uniquenick = uniquenick;
	}
	
	return true;
}

bool Battlefield::Player::SetUniquenickWithoutClanTag(const std::string& uniquenick)
{
	std::regex pattern;
	std::smatch matches;
	std::string no_clan_tag_uniquenick = uniquenick;

	// Find: gamever='V1.31a'
	pattern = std::regex(R"(^(.{2,3} ))");

	if (std::regex_search(no_clan_tag_uniquenick, matches, pattern) && matches.size() >= 2)
	{
		std::string clan_tag = matches[1];

		no_clan_tag_uniquenick.erase(0, clan_tag.size());
	}

	this->_uniquenick = no_clan_tag_uniquenick;

	return true;
}

bool Battlefield::Player::SetEmail(const std::string& email)
{
	if(email.size() > 50)
	{
		this->_email = email.substr(0, 49);
	}
	else
	{
		this->_email = email;
	}
	
	return true;
}

bool Battlefield::Player::SetPassword(const std::string& password)
{
	this->_password = Util::MD5hash(password);
	return true;
}

bool Battlefield::Player::SetMD5Password(const std::string& md5_password)
{
	if(md5_password.size() == 32)
	{
		this->_password = md5_password;
		return true;
	}
	
	return false;
}

bool Battlefield::Player::SetLastLogin(MYSQL_TIME last_login)
{
    this->_last_login = Util::Time::GetDateTime(last_login);
	
    return true;
}

bool Battlefield::Player::SetLastLoginIp(const std::string& last_login_ip)
{
	this->_last_login_ip = last_login_ip;
	return true;
}

bool Battlefield::Player::SetCreatedAt(MYSQL_TIME created_at)
{
	char formatted_datetime[21]; // Sufficient to hold "YYYY-MM-DD HH:mm:SS\0"

    // Set up the struct tm for strftime
    struct tm timeinfo;
    timeinfo.tm_year = created_at.year - 1900;
    timeinfo.tm_mon = created_at.month - 1;
    timeinfo.tm_mday = created_at.day;
    timeinfo.tm_hour = created_at.hour;
    timeinfo.tm_min = created_at.minute;
    timeinfo.tm_sec = created_at.second;

    strftime(formatted_datetime, sizeof(formatted_datetime), "%Y-%m-%d %H:%M:%S ", &timeinfo);
	
    this->_created_at = formatted_datetime;
	this->_created_at += Util::Time::GetTimeZone();
	
    return true;
}

bool Battlefield::Player::SetVerified(bool verified)
{
	this->_verified = verified;
	return true;
}

bool Battlefield::Player::AddFriend(int profileid)
{
	this->_friends.push_back(profileid);
	return true;
}

