#include <battlefield.h>
#include <util.h>
#include <logger.h>

#include <battlefield/gamestat.h>

#include <battlefield/playerstats.h>

std::unordered_map<std::string, Battlefield::PlayerStats::SetterFunc> Battlefield::PlayerStats::SetterMap = {
	{"pph",      [](Battlefield::PlayerStats& player, uint32_t value) { return player.SetPPH(value); }},
	{"vehicles", [](Battlefield::PlayerStats& player, uint32_t value) { return player.SetVehiclesDestroyed(value); }},
	{"lavd",     [](Battlefield::PlayerStats& player, uint32_t value) { return player.SetLAVsDestroyed(value); }},
	{"mavd",     [](Battlefield::PlayerStats& player, uint32_t value) { return player.SetMAVsDestroyed(value); }},
	{"havd",     [](Battlefield::PlayerStats& player, uint32_t value) { return player.SetHAVsDestroyed(value); }},
	{"hed",      [](Battlefield::PlayerStats& player, uint32_t value) { return player.SetHelicoptersDestroyed(value); }},
	{"bod",      [](Battlefield::PlayerStats& player, uint32_t value) { return player.SetBoatsDestroyed(value); }},
	{"kills",    [](Battlefield::PlayerStats& player, uint32_t value) { return player.SetKills(value); }},
	{"deaths",   [](Battlefield::PlayerStats& player, uint32_t value) { return player.SetDeaths(value); }},
	{"k1",       [](Battlefield::PlayerStats& player, uint32_t value) { return player.SetKillsAssualtKit(value); }},
	{"s1",       [](Battlefield::PlayerStats& player, uint32_t value) { return player.SetDeathsAssualtKit(value); }},
	{"k2",       [](Battlefield::PlayerStats& player, uint32_t value) { return player.SetKillsSniperKit(value); }},
	{"s2",       [](Battlefield::PlayerStats& player, uint32_t value) { return player.SetDeathsSniperKit(value); }},
	{"k3",       [](Battlefield::PlayerStats& player, uint32_t value) { return player.SetKillsSpecialOpKit(value); }},
	{"s3",       [](Battlefield::PlayerStats& player, uint32_t value) { return player.SetDeathsSpecialOpKit(value); }},
	{"k4",       [](Battlefield::PlayerStats& player, uint32_t value) { return player.SetKillsCombatEngineerKit(value); }},
	{"s4",       [](Battlefield::PlayerStats& player, uint32_t value) { return player.SetDeathsCombatEngineerKit(value); }},
	{"k5",       [](Battlefield::PlayerStats& player, uint32_t value) { return player.SetKillsSupportKit(value); }},
	{"s5",       [](Battlefield::PlayerStats& player, uint32_t value) { return player.SetDeathsSupportKit(value); }}
};

void Battlefield::PlayerStats::useExample()
{
	this->SetScore(41476);
	this->SetRank(Ranks::Five_Star_General);
	this->SetPPH(5236);
	this->SetKills(31848);
	this->SetDeaths(26394);
	this->SetSuicides(41);
	this->SetTime(3627752);
	this->SetVehiclesDestroyed(818);
	this->SetLAVsDestroyed(342);
	this->SetMAVsDestroyed(182);
	this->SetHAVsDestroyed(118);
	this->SetHelicoptersDestroyed(173);
	this->SetBoatsDestroyed(3);
	this->SetKillsAssualtKit(20880);
	this->SetDeathsAssualtKit(16556);
	this->SetKillsSniperKit(7445);
	this->SetDeathsSniperKit(4790);
	this->SetKillsSpecialOpKit(2364);
	this->SetDeathsSpecialOpKit(3286);
	this->SetKillsCombatEngineerKit(730);
	this->SetDeathsCombatEngineerKit(1411);
	this->SetKillsSupportKit(414);
	this->SetDeathsSupportKit(351);
	this->SetMedals(Medals::All);
	this->SetTotalTopPlayer(56);
	this->SetTotalVictories(1202);
	this->SetTotalGameSessions(3613);
}

double Battlefield::PlayerStats::GetRatio() const
{
	if(this->_kills == 0 or this->_deaths == 0)
	{
		return 0.0;
	}
	
	return static_cast<double>(this->_kills * 100) / this->_deaths;
}

double Battlefield::PlayerStats::GetRatioAssualtKit() const
{
	if(this->_k1 == 0 or this->_s1 == 0)
	{
		return 0.0;
	}
	
	return static_cast<double>(this->_k1 * 100) / this->_s1;
}

double Battlefield::PlayerStats::GetRatioSniperKit() const
{
	if(this->_k2 == 0 or this->_s2 == 0)
	{
		return 0.0;
	}
	
	return static_cast<double>(this->_k2 * 100) / this->_s2;
}

double Battlefield::PlayerStats::GetRatioSpecialOpKit() const
{
	if(this->_k3 == 0 or this->_s3 == 0)
	{
		return 0.0;
	}
	
	return static_cast<double>(this->_k3 * 100) / this->_s3;
}

double Battlefield::PlayerStats::GetRatioCombatEngineerKit() const
{
	if(this->_k4 == 0 or this->_s4 == 0)
	{
		return 0.0;
	}
	
	return static_cast<double>(this->_k4 * 100) / this->_s4;
}

double Battlefield::PlayerStats::GetRatioSupportKit() const
{
	if(this->_k5 == 0 or this->_s5 == 0)
	{
		return 0.0;
	}
	
	return static_cast<double>(this->_k5 * 100) / this->_s5;
}

bool Battlefield::PlayerStats::SetScore(int32_t score)
{
	this->_score = score;

	this->_calcRank();
	
	return true;
}

bool Battlefield::PlayerStats::SetRank(uint32_t ran)
{
	if(ran >= static_cast<uint8_t>(Ranks::Private) && ran <= static_cast<uint8_t>(Ranks::Five_Star_General))
	{
		this->_ran = ran;
		
		return true;
	}
	
	return false;
}

bool Battlefield::PlayerStats::SetRank(Ranks ran)
{
	this->_ran = static_cast<uint8_t>(ran);
	return true;
}

bool Battlefield::PlayerStats::SetPPH(uint32_t pph)
{
	this->_pph = pph;

	this->_calcRank();

	return true;
}

bool Battlefield::PlayerStats::SetKills(uint32_t kills)
{
	this->_kills = kills;
	return true;
}

bool Battlefield::PlayerStats::SetDeaths(uint32_t deaths)
{
	this->_deaths = deaths;
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

bool Battlefield::PlayerStats::SetVehiclesDestroyed(uint32_t vehicles)
{
	this->_vehicles = vehicles;
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

bool Battlefield::PlayerStats::SetDeathsAssualtKit(uint32_t deaths)
{
	this->_s1 = deaths;
	return true;
}

bool Battlefield::PlayerStats::SetKillsSniperKit(uint32_t kills)
{
	this->_k2 = kills;
	return true;
}

bool Battlefield::PlayerStats::SetDeathsSniperKit(uint32_t deaths)
{
	this->_s2 = deaths;
	return true;
}

bool Battlefield::PlayerStats::SetKillsSpecialOpKit(uint32_t kills)
{
	this->_k3 = kills;
	return true;
}

bool Battlefield::PlayerStats::SetDeathsSpecialOpKit(uint32_t deaths)
{
	this->_s3 = deaths;
	return true;
}

bool Battlefield::PlayerStats::SetKillsCombatEngineerKit(uint32_t kills)
{
	this->_k4 = kills;
	return true;
}

bool Battlefield::PlayerStats::SetDeathsCombatEngineerKit(uint32_t deaths)
{
	this->_s4 = deaths;
	return true;
}

bool Battlefield::PlayerStats::SetKillsSupportKit(uint32_t kills)
{
	this->_k5 = kills;
	return true;
}

bool Battlefield::PlayerStats::SetDeathsSupportKit(uint32_t deaths)
{
	this->_s5 = deaths;
	return true;
}

bool Battlefield::PlayerStats::SetMedals(uint32_t medals)
{
	if(medals <= static_cast<uint32_t>(Medals::All) && Util::countSetBits(medals) > Util::countSetBits(this->_medals))
	{
		this->_medals = medals;
		
		this->_calcRank();

		return true;
	}
	
	return false;
}

bool Battlefield::PlayerStats::SetMedals(Medals medals)
{
	return this->SetMedals(static_cast<uint32_t>(medals));
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

void Battlefield::PlayerStats::Update(const Battlefield::GameStatPlayer& gsplayer)
{
	// Calculate new pph
	this->_calcNewPPH(gsplayer.GetTime(), gsplayer.GetScore());

	this->SetBoatsDestroyed(          this->_bod      + gsplayer.GetBoatsDestroyed()          ); // bod
	this->SetHAVsDestroyed(           this->_havd     + gsplayer.GetHAVsDestroyed()           ); // havd
	this->SetHelicoptersDestroyed(    this->_hed      + gsplayer.GetHelicoptersDestroyed()    ); // hed
	this->SetKillsAssualtKit(         this->_k1       + gsplayer.GetKillsAssualtKit()         ); // k1
	this->SetKillsSniperKit(          this->_k2       + gsplayer.GetKillsSniperKit()          ); // k2
	this->SetKillsSpecialOpKit(       this->_k3       + gsplayer.GetKillsSpecialOpKit()       ); // k3
	this->SetKillsCombatEngineerKit(  this->_k4       + gsplayer.GetKillsCombatEngineerKit()  ); // k4
	this->SetKillsSupportKit(         this->_k5       + gsplayer.GetKillsSupportKit()         ); // k5
	this->SetKills(                   this->_kills    + gsplayer.GetKills()                   ); // kills
	this->SetDeaths(                  this->_deaths   + gsplayer.GetDeaths()                  ); // deaths
	this->SetLAVsDestroyed(           this->_lavd     + gsplayer.GetLAVsDestroyed()           ); // lavd
	this->SetMAVsDestroyed(           this->_mavd     + gsplayer.GetMAVsDestroyed()           ); // mavd
	this->SetTotalVictories(          this->_mv       + gsplayer.GetTotalVictories()          ); // mv
	this->SetTotalGameSessions(       this->_ngp      + gsplayer.GetTotalGameSessions()       ); // ngp
	this->SetDeathsAssualtKit(        this->_s1       + gsplayer.GetSpawnsAssualtKit()        ); // s1
	this->SetDeathsSniperKit(         this->_s2       + gsplayer.GetSpawnsSniperKit()         ); // s2
	this->SetDeathsSpecialOpKit(      this->_s3       + gsplayer.GetSpawnsSpecialOpKit()      ); // s3
	this->SetDeathsCombatEngineerKit( this->_s4       + gsplayer.GetSpawnsCombatEngineerKit() ); // s4
	this->SetDeathsSupportKit(        this->_s5       + gsplayer.GetSpawnsSupportKit()        ); // s5
	this->SetScore(                   this->_score    + gsplayer.GetScore()                   ); // score
	this->SetSuicides(                this->_suicides + gsplayer.GetSuicides()                ); // suicides
	this->SetTime(                    this->_time     + gsplayer.GetTime()                    ); // time
	this->SetTotalTopPlayer(          this->_ttb      + gsplayer.GetTotalTopPlayer()          ); // ttb

	this->SetVehiclesDestroyed(
		this->_lavd +
		this->_mavd +
		this->_havd +
		this->_hed +
		this->_bod
	);
	
	this->SetMedals(gsplayer.GetMedals()); // medals
}

void Battlefield::PlayerStats::_calcRank()
{
	int pph = this->_pph / 100;
	int total_medals = Util::countSetBits(this->_medals);
	int new_rank = 1;

	for(int i = 1; i < RankScores.size(); i++)
	{
		if(this->_score >= RankScores[i] && pph >= RankPph[i] && total_medals >= RankMedals[i])
			new_rank = i + 1;
		else
			break;
	}

	this->_ran = new_rank;
}

void Battlefield::PlayerStats::_calcNewPPH(uint32_t time, int32_t score)
{
	const int PPH_TIME_SPAN = 5;
	const int PPH_FACTOR = 100;
	const int SECONDS_PER_HOUR = 60 * 60;
	
	double total_hours = static_cast<double>(this->_time + time) / SECONDS_PER_HOUR;
	int32_t total_score = this->_score + score;

	uint32_t new_pph = 0;

	// In case we only played less then a one hour
	if(total_hours < 1.0)
	{
		new_pph = total_score * 100;
	}
	// In case we played less then 5 hours total
	else if(total_hours < PPH_TIME_SPAN)
	{
		// Calculate the new pph
		double new_pph_double = static_cast<double>(total_score) / total_hours;

		// Convert to unsigned 32 bit integer
		new_pph = static_cast<uint32_t>(new_pph_double * 100.0);
	}
	// In case more then 5 hours is played
	else
	{
		// Convert current pph to double
		double current_pph = static_cast<double>(this->_pph) / 100.0;
		
		// Calculate how much hours you played in current match
		double game_time_hours = static_cast<double>(time) / SECONDS_PER_HOUR;

		// gap_time_span = 5 hours - game_time_hours
		double gap_time_span = static_cast<double>(PPH_TIME_SPAN) - game_time_hours;

		// Calculate the total score for 5 hours
		double total_score = gap_time_span * current_pph;
		total_score += static_cast<double>(score);

		// Calculate the new pph
		double new_pph_double = total_score / PPH_TIME_SPAN;

		// Convert to unsigned 32 bit integer
		new_pph = static_cast<uint32_t>(new_pph_double * 100.0);
	}

	// Set new pph
	this->SetPPH(new_pph);
}

