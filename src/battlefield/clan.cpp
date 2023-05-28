#include <battlefield.h>

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
	int clanid = -1;
	
	try
	{
		clanid = std::stoi(str_clanid);
	}
	catch(...)
	{
		return false;
	}
	
	return this->SetClanId(clanid);
}

bool Battlefield::Clan::SetName(const std::string &name)
{
	this->_name = name;
	return true;
}

bool Battlefield::Clan::SetTag(const std::string &tag)
{
	this->_tag = tag;
	return true;
}

bool Battlefield::Clan::SetHomepage(const std::string &homepage)
{
	this->_homepage = homepage;
	return true;
}

bool Battlefield::Clan::SetInfo(const std::string &info)
{
	this->_info = info;
	return true;
}

bool Battlefield::Clan::SetRegion(Battlefield::Clan::Regions region)
{
	this->_region = region;
	return true;
}

bool Battlefield::Clan::SetRegion(std::string str_region)
{
	int region = -1;
	
	try
	{
		region = std::stoi(str_region);
	}
	catch(...)
	{
		return false;
	}
	
	return this->SetRegion(static_cast<Regions>(region));
}

bool Battlefield::Clan::SetDate(const std::string &date)
{
	this->_date = date;
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

Battlefield::Clan::Ranks Battlefield::Clan::GetRank(int profileid) const
{
	try
	{
		return this->_ranks.at(profileid);
	}
	catch(...) {};
	
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
	int rank = -1;
	
	try
	{
		rank = std::stoi(str_rank);
	}
	catch(...) {};
	
	if(rank >= Leader && rank <= Member)
	{
		return static_cast<Ranks>(rank);
	}
	
	return Unknown_Rank;
}