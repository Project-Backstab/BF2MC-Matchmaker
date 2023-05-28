#ifndef BATTLEFIELD_CLAN_H
#define BATTLEFIELD_CLAN_H

#include <string>
#include <cstdint>
#include <map>
#include <urlrequest.h>

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
				Unknown_Region = -1,
				America        =  1,
				Europe         =  2,
				Asia           =  3
			};
			
			enum Roles
			{
				Unknown_Role = -1,
				Leader       =  0,
				Co_Leader    =  1,
				Member       =  2
			};
		
		private:
			int                  _clanid      = -1;
			std::string          _name        = "";
			std::string          _tag         = "";
			std::string          _homepage    = "";
			std::string          _info        = "";
			Regions              _region      = America;
			std::string          _date        = "";
	        uint32_t             _rating      = 0;
			uint32_t             _wins        = 0;
			uint32_t             _losses      = 0;
			uint32_t	         _draws       = 0;
			std::map<int, Roles> _roles;
		
		public:
			void useExample();
			
			int                         GetClanId() const   { return this->_clanid;   }
			const std::string           GetName() const     { return this->_name;     }
			const std::string           GetTag() const      { return this->_tag;      }
			const std::string           GetHomepage() const { return this->_homepage; }
			const std::string           GetInfo() const     { return this->_info;     }
			Regions                     GetRegion() const   { return this->_region;   }
			const std::map<int, Roles>  GetRoles() const    { return this->_roles;    }
			
			bool SetClanId(int clanid);
			bool SetClanId(const std::string& clanid);
			bool SetName(const std::string &name);
			bool SetTag(const std::string &tag);
			bool SetHomepage(const std::string &homepage);
			bool SetInfo(const std::string &info);
			bool SetRegion(Battlefield::Clan::Regions region);
			bool SetRegion(std::string region);
			bool SetDate(const std::string &date);
			bool SetStats(uint32_t rating, uint32_t wins, uint32_t losses, uint32_t draws);
			void AddRole(int profileid, Roles role);
			Roles GetRole(int profileid) const;
			
			/*
				Request responses
			*/
			std::string responseGetClanInfo();
			std::string responseGetClanMembers();
			void updateInformation(const UrlRequest::UrlVariables &url_variables, bool is_update = false);
	};
}

#endif // BATTLEFIELD_CLAN_H