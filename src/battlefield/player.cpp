#include <string>

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
	int profileid = -1;
	
	try
	{
		profileid = std::stoi(str_profileid);
	}
	catch(...) {};
	
	return this->SetProfileId(profileid);
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

bool Battlefield::Player::SetNick(const std::string &nick)
{
	this->_nick = nick;
	return true;
}

bool Battlefield::Player::SetUniquenick(const std::string &uniquenick)
{
	this->_uniquenick = uniquenick;
	return true;
}

bool Battlefield::Player::SetEmail(const std::string &email)
{
	this->_email = email;
	return true;
}

bool Battlefield::Player::SetPassword(const std::string &password)
{
	this->_password = Util::MD5hash(password);
	return true;
}

bool Battlefield::Player::SetMD5Password(const std::string &md5_password)
{
	this->_password = md5_password;
	return true;
}

bool Battlefield::Player::AddFriend(int profileid)
{
	this->_friends.push_back(profileid);
	return true;
}

