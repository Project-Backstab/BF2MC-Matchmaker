#include <battlefield/clan.h>

void Battlefield::Clan::useExample()
{
	this->SetClanId(543151);
	this->SetName("AT-STARS");
	this->SetTag("SOS");
	this->SetHomepage("UMBRELLA");
	this->SetInfo("G%2dVIRUS");
	this->SetRegion(Regions::Europe);
	this->SetDate("1%2f16%2f2013+3%3a10%3a41+AM");
	this->SetStats(1851, 1671, 10, 40);
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
	if(tag.size() > 3)
	{
		this->_tag = tag.substr(0, 2);
	}
	else
	{
		this->_tag = tag;
	}
	
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

bool Battlefield::Clan::SetRegion(int int_region)
{
	if(int_region >= America && int_region <= Asia)
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
		int int_region = std::stoi(str_region);
		
		return this->SetRegion(int_region);
	}
	catch(...) {};
	
	return false;
}

bool Battlefield::Clan::SetDate(const std::string &date)
{
	if(date.size() > 15)
	{
		this->_date = date.substr(0, 15);
	}
	else
	{
		this->_date = date;
	}
	
	return true;
}

bool Battlefield::Clan::SetStats(uint32_t rating, uint32_t wins, uint32_t losses, uint32_t draws)
{
	this->_rating = rating;
	this->_wins = wins;
	this->_losses = losses;
	this->_draws = draws;
	return true;
}

void Battlefield::Clan::AddRank(int profileid, Ranks rank)
{
	this->_ranks.insert(std::make_pair(profileid, rank));
}

void Battlefield::Clan::AddRank(int profileid, int int_rank)
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
	
	return Unknown_Rank;
}

/*
	Request responses
*/
std::string Battlefield::Clan::responseGetClanInfo()
{
	std::string response = "\r\n";
	
	if(this->_clanid != -1)
	{
		response = "OK\r\n";
		response += "clanID," + std::to_string(this->_clanid) + "\r\n";
		response += "name," + this->_name + "\r\n";
		response += "tag," + this->_tag + "\r\n";
		response += "homepage," + this->_homepage + "\r\n";
		response += "info," + this->_info + "\r\n";
		response += "region," + std::to_string(this->_region) + "\r\n";
		response += "lastreportdate," + this->_date + "\r\n";
		
		response += "rating," + std::to_string(this->_rating) + "\r\n";
		response += "wins," + std::to_string(this->_wins) + "\r\n";
		response += "losses," + std::to_string(this->_losses) + "\r\n";
		response += "draws," + std::to_string(this->_draws) + "\r\n";
		
		response += "membercount," + std::to_string(this->_ranks.size()) + "\r\n";
	}
	
	return response;
}

std::string Battlefield::Clan::responseGetClanMembers()
{
	std::string response = "\r\n";
	
	if(this->_clanid != -1)
	{
		response = "OK\r\n";
		
		for (const auto& pair : this->_ranks)
		{
			response += "\r\n" + std::to_string(pair.first) + "," + std::to_string(pair.second);
		}
	}
	
	return response;
}

void Battlefield::Clan::updateInformation(const UrlRequest::UrlVariables &url_variables, bool is_update)
{
	for(const auto &url_variable : url_variables)
	{
		if(url_variable.first == "name" && !is_update)
		{
			this->SetName(url_variable.second);
		}
		else if(url_variable.first == "tag")
		{
			this->SetTag(url_variable.second);
		}
		else if(url_variable.first == "homepage")
		{
			this->SetHomepage(url_variable.second);
		}
		else if(url_variable.first == "info")
		{
			this->SetInfo(url_variable.second);
		}
		else if(url_variable.first == "region")
		{
			this->SetRegion(url_variable.second);
		}
	}
}

/*
	Static
*/
Battlefield::Clan::Ranks Battlefield::Clan::convertRank(const std::string& str_rank)
{
	try
	{
		int int_rank = std::stoi(str_rank);
		
		return convertRank(int_rank);
	}
	catch(...) {};
	
	return Unknown_Rank;
}

Battlefield::Clan::Ranks Battlefield::Clan::convertRank(int int_rank)
{
	if(int_rank >= Leader && int_rank <= Member)
	{
		return static_cast<Ranks>(int_rank);
	}
	
	return Unknown_Rank;
}

Battlefield::Clan::Regions Battlefield::Clan::convertRegion(const std::string& str_region)
{
	try
	{
		int int_region = std::stoi(str_region);
		
		return convertRegion(int_region);
	}
	catch(...) {};
	
	return Unknown_Region;
}

Battlefield::Clan::Regions Battlefield::Clan::convertRegion(int int_region)
{
	if(int_region >= America && int_region <= Asia)
	{
		return static_cast<Regions>(int_region);
	}
	
	return Unknown_Region;
}

