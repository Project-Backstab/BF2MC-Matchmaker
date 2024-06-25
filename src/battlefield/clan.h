#ifndef BATTLEFIELD_CLAN_H
#define BATTLEFIELD_CLAN_H

#include <string>
#include <cstdint>
#include <map>
#include <unordered_map>

/**
 * @brief Forward declaration of the MYSQL_TIME structure.
 * 
 * This forward declaration allows the use of the MYSQL_TIME structure in contexts where only the structure name needs to be known,
 * such as declaring pointers or references to MYSQL_TIME objects, without the need to include the full definition of the structure.
 * 
 * MYSQL_TIME is typically used to represent date and time values in MySQL-related operations.
 */
struct MYSQL_TIME;

namespace Battlefield
{
	/**
	 * @brief Forward declaration of the Clan class.
	 * 
	 * This forward declaration allows the use of the Clan class in contexts where only the class name needs to be known,
	 * such as declaring pointers or references to Clan objects, without the need to include the full definition of the class.
	 * 
	 * Forward declarations are commonly used to reduce compile-time dependencies and improve compilation speed.
	 */
	class Clan;
	
	/**
	 * @brief Type definition for a vector of Clan objects.
	 * 
	 * This typedef defines a shorthand alias `Clans` for a vector containing instances of the Clan class.
	 * It can be used to conveniently refer to collections of Clan objects.
	 */
	typedef std::vector<Battlefield::Clan> Clans;

	/**
	 * @brief Type definition for a map of rank IDs to Clan objects.
	 * 
	 * This typedef defines a shorthand alias `RankClans` for a map where integer keys represent rank IDs
	 * and corresponding values are instances of the Clan class.
	 * It can be used to store and manipulate associations between rank IDs and Clan objects.
	 */
	typedef std::map<int, Battlefield::Clan> RankClans;

	/**
	 * @brief Represents a clan in the Battlefield game.
	 * 
	 * This class encapsulates data and behavior related to a clan in the Battlefield game.
	 */
	class Clan
	{
		public:
			/**
			 * @brief Represents the region associated with a clan.
			 * 
			 * This enum class defines different region associated with a clan.
			 * Each region corresponds to a specific geographical area.
			 */
			enum class Region : uint8_t
			{
				America   =  0x1, /**< The Americas region. */
				Europe    =  0x2, /**< The Europe region. */
				Asia      =  0x3, /**< The Asia region. */
				Unknown   =  0xf  /**< Unknown region. */
			};
			
			/**
			 * @brief Represents the ranks within a clan.
			 * 
			 * This enum class defines different ranks within a clan.
			 * Each rank corresponds to a specific position or role within the clan hierarchy.
			 */
			enum class Rank : uint8_t
			{
				Leader     = 0x0, /**< The leader of the clan. */
				Co_Leader  = 0x1, /**< A co-leader within the clan. */
				Member     = 0x2, /**< A regular member of the clan. */
				Unknown    = 0xf  /**< Unknown rank. */
			};

			typedef std::map<int, Battlefield::Clan::Rank> Ranks;
		
		private:
			int          _clanid     = -1;               /**< The unique identifier of the clan. */
			std::string  _name       = "";               /**< The name of the clan. */
			std::string  _tag        = "";               /**< The tag of the clan. */
			std::string  _homepage   = "";               /**< The homepage URL of the clan. */
			std::string  _info       = "";               /**< Additional information about the clan. */
			Region      _region      = Region::America;  /**< The region associated with the clan. */
			uint32_t     _score      = 0;                /**< The score of the clan. */
			uint32_t     _wins       = 0;                /**< The number of wins of the clan. */
			uint32_t     _losses     = 0;                /**< The number of losses of the clan. */
			uint32_t     _draws      = 0;                /**< The number of draws of the clan. */
			std::string  _created_at = "";               /**< The creation timestamp of the clan. */
			bool         _disable    = false;            /**< Disable option to not let player progress stats. */
			Ranks        _ranks;                         /**< The map of member IDs to ranks within the clan. */
		
		public:
			/**
			 * @brief The Elo score weight for clan battles.
			 * 
			 * This static constant represents the Elo score weight used for calculating the outcome
			 * of clan battles. It determines the impact of a draw on the Elo score, with multipliers
			 * applied for different victory levels.
			 */
			static const int ELO_WEIGHT = 25;

			/**
			 * @brief The maximum range for clan battles.
			 * 
			 * This static constant represents the maximum distance between clans allowed to fight each other.
			 * It defines the range within which clan battles can occur.
			 */
			static const int ELO_MAX_RANGE = 1000;

			typedef bool (Battlefield::Clan::*SetterFunc)(const std::string&);

			static std::unordered_map<std::string, SetterFunc> SetterMap;

		public:
			int          GetClanId() const     { return this->_clanid;     }
			std::string  GetName() const       { return this->_name;       }
			std::string  GetTag() const        { return this->_tag;        }
			std::string  GetHomepage() const   { return this->_homepage;   }
			std::string  GetInfo() const       { return this->_info;       }
			uint8_t      GetRegion() const     { return static_cast<uint8_t>(this->_region); }
			Region       GetRegionEnum() const { return this->_region;     }
			uint32_t     GetScore() const      { return this->_score;      }
			uint32_t     GetWins() const       { return this->_wins;       }
			uint32_t     GetLosses() const     { return this->_losses;     }
			uint32_t     GetDraws() const      { return this->_draws;      }
			std::string  GetCreatedAt() const  { return this->_created_at; }
			bool         IsDisabled() const    { return this->_disable;    }
			Ranks        GetRanks() const      { return this->_ranks;      }
			
			bool SetClanId(int clanid);
			bool SetClanId(const std::string& clanid);
			bool SetName(const std::string& name);
			bool SetTag(const std::string& tag);
			bool SetHomepage(const std::string& homepage);
			bool SetInfo(const std::string& info);
			bool SetRegion(Battlefield::Clan::Region region);
			bool SetRegion(uint8_t int_region);
			bool SetRegion(const std::string& region);
			bool SetScore(uint32_t score);
			bool SetWins(uint32_t wins);
			bool SetLosses(uint32_t losses);
			bool SetDraws(uint32_t draws);
			bool SetCreatedAt(MYSQL_TIME created_at);
			bool SetDisable(bool disable);
			bool SetDisable(uint8_t disable);
			void AddRank(int profileid, Rank rank);
			void AddRank(int profileid, uint8_t int_rank);
			Rank GetRank(int profileid) const;

			/**
			 * @brief Performs an example operation using the Clan class.
			 * 
			 * This member function demonstrates an example operation using the Clan class.
			 * It serves as a placeholder for illustrating the usage of the Clan class.
			 * 
			 * @note This is just an example function and does not perform any meaningful operation.
			 */
			void useExample();
			
			/*
				Static
			*/
			/**
			 * @brief Converts a string representation of a rank to its corresponding enum value.
			 * 
			 * This static member function converts a string representation of a rank to its corresponding
			 * enum value of the Ranks enumeration.
			 * 
			 * @param rank A string representing the rank.
			 * @return The enum value representing the rank.
			 */
			static Rank convertRank(const std::string& rank);

			/**
			 * @brief Converts an integer representation of a rank to its corresponding enum value.
			 * 
			 * This static member function converts an integer representation of a rank to its corresponding
			 * enum value of the Ranks enumeration.
			 * 
			 * @param int_rank An integer representing the rank.
			 * @return The enum value representing the rank.
			 */
			static Rank convertRank(uint8_t int_rank);

			/**
			 * @brief Converts a string representation of a region to its corresponding enum value.
			 * 
			 * This static member function converts a string representation of a region to its corresponding
			 * enum value of the Region enumeration.
			 * 
			 * @param str_region A string representing the region.
			 * @return The enum value representing the region.
			 */
			static Region convertRegion(const std::string& str_region);

			/**
			 * @brief Converts an integer representation of a region to its corresponding enum value.
			 * 
			 * This static member function converts an integer representation of a region to its corresponding
			 * enum value of the Region enumeration.
			 * 
			 * @param int_region An integer representing the region.
			 * @return The enum value representing the region.
			 */
			static Region convertRegion(uint8_t int_region);
	};
}

#endif // BATTLEFIELD_CLAN_H