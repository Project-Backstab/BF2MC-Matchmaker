#ifndef BATTLEFIELD_CLAN_H
#define BATTLEFIELD_CLAN_H

#include <string>
#include <cstdint>
#include <map>

/*
	Forward declarations
*/
struct MYSQL_TIME;

namespace Battlefield
{
	/*
		Clan
	*/
	class Clan
	{
		public:
			enum Regions
			{
				America        =  0x1,
				Europe         =  0x2,
				Asia           =  0x3,
				Unknown_Region =  0xf,
			};
			
			enum Ranks
			{
				Leader         = 0x0,
				Co_Leader      = 0x1,
				Member         = 0x2,
				Unknown_Rank   = 0xf,
			};
		
		private:
			int          _clanid     = -1;
			std::string  _name       = "";
			std::string  _tag        = "";
			std::string  _homepage   = "";
			std::string  _info       = "";
			Regions      _region     = America;
			uint32_t     _rating     = 0;
			uint32_t     _wins       = 0;
			uint32_t     _losses     = 0;
			uint32_t     _draws      = 0;
			std::string  _created_at = "";
			
			std::map<int, Ranks> _ranks;
		
		public:
			void useExample();
			
			int                         GetClanId() const    { return this->_clanid;     }
			std::string                 GetName() const      { return this->_name;       }
			std::string                 GetTag() const       { return this->_tag;        }
			std::string                 GetHomepage() const  { return this->_homepage;   }
			std::string                 GetInfo() const      { return this->_info;       }
			Regions                     GetRegion() const    { return this->_region;     }
			uint32_t                    GetRating() const    { return this->_rating;     }
			uint32_t                    GetWins() const      { return this->_wins;       }
			uint32_t                    GetLosses() const    { return this->_losses;     }
			uint32_t                    GetDraws() const     { return this->_draws;      }
			std::string                 GetCreatedAt() const { return this->_created_at; }
			
			const std::map<int, Ranks>  GetRanks() const     { return this->_ranks;    }
			
			bool SetClanId(int clanid);
			bool SetClanId(const std::string& clanid);
			bool SetName(const std::string& name);
			bool SetTag(const std::string& tag);
			bool SetHomepage(const std::string& homepage);
			bool SetInfo(const std::string& info);
			bool SetRegion(Battlefield::Clan::Regions region);
			bool SetRegion(uint8_t int_region);
			bool SetRegion(const std::string& region);
			bool SetStats(uint32_t rating, uint32_t wins, uint32_t losses, uint32_t draws);
			bool SetCreatedAt(MYSQL_TIME created_at);
			
			void AddRank(int profileid, Ranks rank);
			void AddRank(int profileid, uint8_t int_rank);
			Ranks GetRank(int profileid) const;
			
			/*
				Static
			*/
			static Ranks convertRank(const std::string& rank);
			static Ranks convertRank(uint8_t int_rank);
			static Regions convertRegion(const std::string& str_region);
			static Regions convertRegion(uint8_t int_region);
	};
}

#endif // BATTLEFIELD_CLAN_H