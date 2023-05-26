#ifndef BATTLEFIELD_H
#define BATTLEFIELD_H

#include <string>

namespace Battlefield
{
	enum PlayerStatsRank
	{
		Private                     = 1,
		Private_1st_Class           = 2,
		Corporal                    = 3,
		Sergeant                    = 4,
		Sergeant_1st_Class          = 5,
		Master_Sergeant             = 6,
		Sgt_Major                   = 7,
		Command_Sgt_Major           = 8,
		Warrant_Officer             = 9,
		Chief_Warrant_Officer       = 10,
		Second_Lieutenant           = 11,
		First_Lieutenant            = 12,
		Captian                     = 13,
		Major                       = 14,
		Lieutenant_Colonel          = 15,
		Colonel                     = 16,
		Brigadier_General           = 17,
		Major_General               = 18,
		Lieutenant_General          = 19,
		Five_Star_General           = 20,
	};
	
	enum PlayerStatsMedals
	{
		Service_Cross               = 1 << 0,
		The_Bronse_Star             = 1 << 1,
		Air_Forse_Cross             = 1 << 2,
		The_Silver_star             = 1 << 3,
		The_Service_Cross_1st_Class = 1 << 4,
		The_Bronse_Star_1st_Class   = 1 << 5,
		Air_Force_Cross_1st_Class   = 1 << 6,
		Expert_Killing              = 1 << 7,
		Expert_Shooting             = 1 << 8,
		Expert_Demolition           = 1 << 9,
		Expert_Repair               = 1 << 10,
		Expert_Healer               = 1 << 11,
		Navy_Cross                  = 1 << 12,
		Legion_of_Merit             = 1 << 13,
		Legion_of_Merit_1st_Class   = 1 << 14,
		All_Medals                  = (1 << 14) - 1,
	};
}

#endif // BATTLEFIELD_H