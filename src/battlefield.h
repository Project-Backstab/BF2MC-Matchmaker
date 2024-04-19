#ifndef BATTLEFIELD_H
#define BATTLEFIELD_H

#include <vector>
#include <cstdint>

namespace Battlefield
{
	/*
		Enums
	*/
	/**
	 * @brief Represents various military ranks.
	 */
	enum class Ranks
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

	/**
	 * @brief Represents various military medals.
	 */
	enum class Medals
	{
		The_Service_Cross             = 1 << 0,
		The_Bronze_Star               = 1 << 1,
		Air_Force_Cross               = 1 << 2,
		The_Silver_Star               = 1 << 3,
		The_Service_Cross_First_Class = 1 << 4,
		The_Bronze_Star_First_Class   = 1 << 5,
		Air_Force_Cross_First_Class   = 1 << 6,
		Expert_Killing                = 1 << 7,
		Expert_Shooting               = 1 << 8,
		Expert_Demolition             = 1 << 9,
		Expert_Repair                 = 1 << 10,
		Expert_Healer                 = 1 << 11,
		Navy_Cross                    = 1 << 12,
		Legion_of_Merit               = 1 << 13,
		Legion_of_Merit_First_Class   = 1 << 14,
		All                           = (1 << 15) - 1,
	};
	
	/**
	 * @brief Represents various military ribbons.
	 * @note to-do: Start a server. Setup a account with stats that are close to gain a ribbon.
	 * Then progress one game to get the ribbon. Then watch "medals" value what bit is changed.
	 */
	enum class Ribbons
	{
		Ribbon_A                      = 1 << 22,
		Ribbon_B                      = 1 << 23,
		Ribbon_C                      = 1 << 24,
		Ribbon_D                      = 1 << 25,
		Ribbon_E                      = 1 << 26,
		Ribbon_F                      = 1 << 27,
		Ribbon_G                      = 1 << 28,
		Ribbon_H                      = 1 << 29
	};
	
	/**
	 * @brief Represents various game modes you can play in a game
	*/
	enum class GameType
	{
		Unknown         = -1,
		Conquest        = 1,
		CaptureTheFlag  = 2
	};

	/**
	 * @brief External vector storing rank scores.
	 * @note This vector contains the scores corresponding to each rank.
	 */
	extern std::vector<uint16_t> RankScores;

	/**
	 * @brief External vector storing rank performance per hour (PPH).
	 * @note This vector contains the performance per hour metrics corresponding to each rank.
	 */
	extern std::vector<uint16_t> RankPph;

	/**
	 * @brief External vector storing rank medals.
	 * @note This vector contains the number of medals corresponding to each rank.
	 */
	extern std::vector<uint16_t> RankMedals;
}

#endif // BATTLEFIELD_H