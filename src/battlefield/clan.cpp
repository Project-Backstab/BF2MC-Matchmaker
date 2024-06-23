#include <mysql/mysql_time.h>

#include <util.h>

#include <battlefield/clan.h>

std::unordered_map<std::string, Battlefield::Clan::SetterFunc> Battlefield::Clan::SetterMap = {
	{"name",      &Battlefield::Clan::SetName      },
	{"tag",       &Battlefield::Clan::SetTag       },
	{"homepage",  &Battlefield::Clan::SetHomepage  },
	{"info",      &Battlefield::Clan::SetInfo      },
	{"region",    &Battlefield::Clan::SetRegion    },
};

void Battlefield::Clan::useExample()
{
	this->SetClanId(543151);
	this->SetName("AT-STARS");
	this->SetTag("SOS");
	this->SetHomepage("UMBRELLA");
	this->SetInfo("G%2dVIRUS");
	this->SetRegion(Regions::Europe);
}

bool Battlefield::Clan::SetClanId(int clanid)
{
	if(clanid >= 0)
	{
		this->_clanid = clanid;
		return true;
	}
	
	this->_clanid = -1;
	return false;
}

bool Battlefield::Clan::SetClanId(const std::string& str_clanid)
{
	try
	{
		int clanid = std::stoi(str_clanid);
		
		return this->SetClanId(clanid);
	}
	catch(...) {}
	
	return false;
}

bool Battlefield::Clan::SetName(const std::string& name)
{
	if(name.size() > 32)
	{
		this->_name = name.substr(0, 31);
	}
	else
	{
		this->_name = name;
	}
	
	return true;
}

bool Battlefield::Clan::SetTag(const std::string& tag)
{
	if(tag.size() < 2 || tag.size() > 3)
	{
		return false;
	}

	this->_tag = tag;
	return true;
}

bool Battlefield::Clan::SetHomepage(const std::string& homepage)
{
	if(homepage.size() > 256)
	{
		this->_homepage = homepage.substr(0, 255);
	}
	else
	{
		this->_homepage = homepage;
	}
	
	return true;
}

bool Battlefield::Clan::SetInfo(const std::string& info)
{
	if(info.size() > 1024)
	{
		this->_info = info.substr(0, 1023);
	}
	else
	{
		this->_info = info;
	}
	
	return true;
}

bool Battlefield::Clan::SetRegion(Battlefield::Clan::Regions region)
{
	this->_region = region;
	return true;
}

bool Battlefield::Clan::SetRegion(uint8_t int_region)
{
	if(int_region >= static_cast<uint8_t>(Regions::America) && int_region <= static_cast<uint8_t>(Regions::Asia))
	{
		Battlefield::Clan::Regions region = static_cast<Regions>(int_region);
		
		return this->SetRegion(region);
	}
	
	return false;
}

bool Battlefield::Clan::SetRegion(const std::string& str_region)
{
	try
	{
		uint8_t int_region = static_cast<uint8_t>(std::stoul(str_region));
		
		return this->SetRegion(int_region);
	}
	catch(...) {};
	
	return false;
}

bool Battlefield::Clan::SetScore(uint32_t score)
{
	this->_score = score;
	return true;
}

bool Battlefield::Clan::SetWins(uint32_t wins)
{
	this->_wins = wins;
	return true;
}

bool Battlefield::Clan::SetLosses(uint32_t losses)
{
	this->_losses = losses;
	return true;
}

bool Battlefield::Clan::SetDraws(uint32_t draws)
{
	this->_draws = draws;
	return true;
}

bool Battlefield::Clan::SetCreatedAt(MYSQL_TIME created_at)
{
	this->_created_at = Util::Time::GetDateTime(created_at);

	return true;
}

bool Battlefield::Clan::SetDisable(bool disable)
{
	this->_disable = disable;
	return true;
}

bool Battlefield::Clan::SetDisable(uint8_t disable)
{
	return this->SetDisable(disable == 1);
}

void Battlefield::Clan::AddRank(int profileid, Ranks rank)
{
	this->_ranks.insert(std::make_pair(profileid, rank));
}

void Battlefield::Clan::AddRank(int profileid, uint8_t int_rank)
{	
	this->AddRank(profileid, convertRank(int_rank));
}

Battlefield::Clan::Ranks Battlefield::Clan::GetRank(int profileid) const
{
	auto it = this->_ranks.find(profileid);
	if (it != this->_ranks.end())
	{
		return it->second;
	}
	
	return Ranks::Unknown;
}

// Static

Battlefield::Clan::Ranks Battlefield::Clan::convertRank(const std::string& str_rank)
{
	try
	{
		int int_rank = std::stoi(str_rank);
		
		return convertRank(int_rank);
	}
	catch(...) {};
	
	return Ranks::Unknown;
}

Battlefield::Clan::Ranks Battlefield::Clan::convertRank(uint8_t int_rank)
{
	if(int_rank >= static_cast<uint8_t>(Ranks::Leader) && int_rank <= static_cast<uint8_t>(Ranks::Member))
	{
		return static_cast<Ranks>(int_rank);
	}
	
	return Ranks::Unknown;
}

Battlefield::Clan::Regions Battlefield::Clan::convertRegion(const std::string& str_region)
{
	try
	{
		uint8_t int_region = static_cast<uint8_t>(std::stoul(str_region));
		
		return convertRegion(int_region);
	}
	catch(...) {};
	
	return Regions::Unknown;
}

Battlefield::Clan::Regions Battlefield::Clan::convertRegion(uint8_t int_region)
{
	if(int_region >= static_cast<uint8_t>(Regions::America) && int_region <= static_cast<uint8_t>(Regions::Asia))
	{
		return static_cast<Regions>(int_region);
	}
	
	return Regions::Unknown;
}

