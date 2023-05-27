#ifndef BATTLEFIELD_CLAN_H
#define BATTLEFIELD_CLAN_H

namespace Battlefield
{
	/*
		Clan
	*/
	class Clan
	{
		enum Regions
		{
			America = 0x1,
			Europe  = 0x2,
			Asia    = 0x3,
		};
		
		private:
			int          _clanid = -1;
			std::string  _name;
			std::string  _tag;
			std::string  _homepage;
			std::string  _info;
			Regions      _region;
			std::string  _date;
	        uint32_t     _rating;
			uint32_t     _wins;
			uint32_t     _losses;
			uint32_t	 _draws;
			uint32_t     _membercount;
		
		public:
			void useExample();
			
			bool SetClanId(int clanid);
			bool SetName(const std::string &name);
			bool SetTag(const std::string &tag);
			bool SetHomepage(const std::string &homepage);
			bool SetInfo(const std::string &info);
			bool SetRegion(Battlefield::Clan::Regions region);
			bool SetDate(const std::string &date);
			bool SetStats(uint32_t rating, uint32_t wins, uint32_t losses, uint32_t draws);
			bool SetTotalMembers(uint32_t membercount);
			
			/*
				Request responses
			*/
			std::string responseGetClanInfo();
	};
}

#endif // BATTLEFIELD_CLAN_H