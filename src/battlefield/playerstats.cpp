#include <battlefield.h>

#include <battlefield/playerstats.h>

void Battlefield::PlayerStats::useExample()
{
	this->SetScore(41476);
	this->SetRank(Five_Star_General);
	this->SetPPH(5236);
	this->SetKills(31848);
	this->SetSuicides(41);
	this->SetTime(3627752);
	this->SetLAVsDestroyed(342);
	this->SetMAVsDestroyed(182);
	this->SetHAVsDestroyed(118);
	this->SetHelicoptersDestroyed(173);
	this->SetBoatsDestroyed(3);
	this->SetKillsAssualtKit(20880);
	this->SetKillsSniperKit(7445);
	this->SetKillsSpecialOpKit(2364);
	this->SetKillsCombatEngineerKit(730);
	this->SetKillsSupportKit(414);
	this->SetMedals(All_Medals);
	this->SetTotalTopPlayer(56);
	this->SetTotalVictories(1202);
	this->SetTotalGameSessions(3613);
}

bool Battlefield::PlayerStats::SetScore(uint32_t score)
{
	this->score = score;
	return true;
}

bool Battlefield::PlayerStats::SetRank(uint32_t ran)
{
	if(ran >= 1 && ran <= 20)
	{
		this->ran = ran;
		
		return true;
	}
	
	return false;
}

bool Battlefield::PlayerStats::SetPPH(uint32_t pph)
{
	this->pph = pph;
	return true;
}

bool Battlefield::PlayerStats::SetKills(uint32_t kills)
{
	this->kills = kills;
	return true;
}

bool Battlefield::PlayerStats::SetSuicides(uint32_t suicides)
{
	this->suicides = suicides;
	return true;
}

bool Battlefield::PlayerStats::SetTime(uint32_t time)
{
	this->time = time;
	return true;
}

bool Battlefield::PlayerStats::SetLAVsDestroyed(uint32_t lavd)
{
	this->lavd = lavd;
	return true;
}

bool Battlefield::PlayerStats::SetMAVsDestroyed(uint32_t mavd)
{
	this->mavd = mavd;
	return true;
}

bool Battlefield::PlayerStats::SetHAVsDestroyed(uint32_t havd)
{
	this->havd = havd;
	return true;
}

bool Battlefield::PlayerStats::SetHelicoptersDestroyed(uint32_t hed)
{
	this->hed = hed;
	return true;
}

bool Battlefield::PlayerStats::SetBoatsDestroyed(uint32_t bod)
{
	this->bod = bod;
	return true;
}

bool Battlefield::PlayerStats::SetKillsAssualtKit(uint32_t kills)
{
	this->k1 = kills;
	return true;
}

bool Battlefield::PlayerStats::SetKillsSniperKit(uint32_t kills)
{
	this->k2 = kills;
	return true;
}

bool Battlefield::PlayerStats::SetKillsSpecialOpKit(uint32_t kills)
{
	this->k3 = kills;
	return true;
}

bool Battlefield::PlayerStats::SetKillsCombatEngineerKit(uint32_t kills)
{
	this->k4 = kills;
	return true;
}

bool Battlefield::PlayerStats::SetKillsSupportKit(uint32_t kills)
{
	this->k5 = kills;
	return true;
}

bool Battlefield::PlayerStats::SetMedals(uint32_t medals)
{
	if(medals < (1 << 15))
	{
		this->medals = medals;
		
		return true;
	}
	
	return false;
}

bool Battlefield::PlayerStats::SetTotalTopPlayer(uint32_t total)
{
	this->ttb = total;
	return true;
}

bool Battlefield::PlayerStats::SetTotalVictories(uint32_t total)
{
	this->mv = total;
	return true;
}

bool Battlefield::PlayerStats::SetTotalGameSessions(uint32_t total)
{
	this->ngp = total;
	return true;
}

std::string Battlefield::PlayerStats::ToString()
{
	std::string str;
	
	str += std::to_string(this->score);
	str += ",";
	str += std::to_string(this->ran);
	str += ",";
	str += std::to_string(this->pph);
	str += ",";
	str += std::to_string(this->kills);
	str += ",";
	str += std::to_string(this->suicides);
	str += ",";
	str += std::to_string(this->time);
	str += ",";
	str += std::to_string(this->lavd);
	str += ",";
	str += std::to_string(this->mavd);
	str += ",";
	str += std::to_string(this->havd);
	str += ",";
	str += std::to_string(this->hed);
	str += ",";
	str += std::to_string(this->pld);
	str += ",";
	str += std::to_string(this->bod);
	str += ",";
	str += std::to_string(this->k1);
	str += ",";
	str += std::to_string(this->s1);
	str += ",";
	str += std::to_string(this->k2);
	str += ",";
	str += std::to_string(this->s2);
	str += ",";
	str += std::to_string(this->k3);
	str += ",";
	str += std::to_string(this->s3);
	str += ",";
	str += std::to_string(this->k4);
	str += ",";
	str += std::to_string(this->s4);
	str += ",";
	str += std::to_string(this->k5);
	str += ",";
	str += std::to_string(this->s5);
	str += ",";
	str += std::to_string(this->tk);
	str += ",";
	str += std::to_string(this->medals);
	str += ",";
	str += std::to_string(this->ttb);
	str += ",";
	str += std::to_string(this->mv);
	str += ",";
	str += std::to_string(this->ngp);
	
	return str;
}

