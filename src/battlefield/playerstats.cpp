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
	this->_score = score;
	return true;
}

bool Battlefield::PlayerStats::SetRank(uint32_t ran)
{
	if(ran >= 1 && ran <= 20)
	{
		this->_ran = ran;
		
		return true;
	}
	
	return false;
}

bool Battlefield::PlayerStats::SetPPH(uint32_t pph)
{
	this->_pph = pph;
	return true;
}

bool Battlefield::PlayerStats::SetKills(uint32_t kills)
{
	this->_kills = kills;
	return true;
}

bool Battlefield::PlayerStats::SetSuicides(uint32_t suicides)
{
	this->_suicides = suicides;
	return true;
}

bool Battlefield::PlayerStats::SetTime(uint32_t time)
{
	this->_time = time;
	return true;
}

bool Battlefield::PlayerStats::SetLAVsDestroyed(uint32_t lavd)
{
	this->_lavd = lavd;
	return true;
}

bool Battlefield::PlayerStats::SetMAVsDestroyed(uint32_t mavd)
{
	this->_mavd = mavd;
	return true;
}

bool Battlefield::PlayerStats::SetHAVsDestroyed(uint32_t havd)
{
	this->_havd = havd;
	return true;
}

bool Battlefield::PlayerStats::SetHelicoptersDestroyed(uint32_t hed)
{
	this->_hed = hed;
	return true;
}

bool Battlefield::PlayerStats::SetBoatsDestroyed(uint32_t bod)
{
	this->_bod = bod;
	return true;
}

bool Battlefield::PlayerStats::SetKillsAssualtKit(uint32_t kills)
{
	this->_k1 = kills;
	return true;
}

bool Battlefield::PlayerStats::SetKillsSniperKit(uint32_t kills)
{
	this->_k2 = kills;
	return true;
}

bool Battlefield::PlayerStats::SetKillsSpecialOpKit(uint32_t kills)
{
	this->_k3 = kills;
	return true;
}

bool Battlefield::PlayerStats::SetKillsCombatEngineerKit(uint32_t kills)
{
	this->_k4 = kills;
	return true;
}

bool Battlefield::PlayerStats::SetKillsSupportKit(uint32_t kills)
{
	this->_k5 = kills;
	return true;
}

bool Battlefield::PlayerStats::SetMedals(uint32_t medals)
{
	if(medals < (1 << 15))
	{
		this->_medals = medals;
		
		return true;
	}
	
	return false;
}

bool Battlefield::PlayerStats::SetTotalTopPlayer(uint32_t total)
{
	this->_ttb = total;
	return true;
}

bool Battlefield::PlayerStats::SetTotalVictories(uint32_t total)
{
	this->_mv = total;
	return true;
}

bool Battlefield::PlayerStats::SetTotalGameSessions(uint32_t total)
{
	this->_ngp = total;
	return true;
}

/*
	Request responses
*/
std::vector<int> Battlefield::PlayerStats::GetStatsVector()
{
	std::vector<int> v_stats;
	
	v_stats.push_back(this->_score);
	v_stats.push_back(this->_ran);
	v_stats.push_back(this->_pph);
	v_stats.push_back(this->_kills);
	v_stats.push_back(this->_suicides);
	v_stats.push_back(this->_time);
	v_stats.push_back(this->_lavd);
	v_stats.push_back(this->_mavd);
	v_stats.push_back(this->_havd);
	v_stats.push_back(this->_hed);
	v_stats.push_back(this->_pld);
	v_stats.push_back(this->_bod);
	v_stats.push_back(this->_k1);
	v_stats.push_back(this->_s1);
	v_stats.push_back(this->_k2);
	v_stats.push_back(this->_s2);
	v_stats.push_back(this->_k3);
	v_stats.push_back(this->_s3);
	v_stats.push_back(this->_k4);
	v_stats.push_back(this->_s4);
	v_stats.push_back(this->_k5);
	v_stats.push_back(this->_s5);
	v_stats.push_back(this->_tk);
	v_stats.push_back(this->_medals);
	v_stats.push_back(this->_ttb);
	v_stats.push_back(this->_mv);
	v_stats.push_back(this->_ngp);
	
	return v_stats;
}

