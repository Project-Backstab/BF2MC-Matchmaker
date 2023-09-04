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

bool Battlefield::Player::SetUserId(const std::string& str_userid)
{
	int userid = -1;
	
	try
	{
		userid = std::stoi(str_userid);
	}
	catch(...) {};
	
	return this->SetUserId(userid);
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
	if(md5_password.size() != 32)
	{
		return false;
	}
	
	this->_password = md5_password;
	return true;
}

bool Battlefield::Player::AddFriend(int profileid)
{
	this->_friends.push_back(profileid);
	return true;
}

