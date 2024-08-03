#include <settings.h>
#include <util.h>
#include <logger.h>

#include <dns/client.h>

/*
	To not let the executable run as admin you have to redirect port 53 to 5353.
	
	sudo iptables -A PREROUTING -t nat -i eth0 -p udp --dport 53 -j REDIRECT --to-port 5353
	sudo iptables -D PREROUTING -t nat -i eth0 -p udp --dport 53 -j REDIRECT --to-port 5353
*/

std::unordered_map<std::string, DNS::ConfigItem> DNS::Config = {
	// playstation.org
	{ "gate1.us.dnas.playstation.org",        { DNS::QType::A, DNS::QClass::IN, 60, {  67, 222, 156, 250 } } },
	{ "gate1.eu.dnas.playstation.org",        { DNS::QType::A, DNS::QClass::IN, 60, {  45,   7, 228, 197 } } },
	{ "gate1.jp.dnas.playstation.org",        { DNS::QType::A, DNS::QClass::IN, 60, { 153, 121,  44, 150 } } },
	
	// easports.com
	{ "www.easports.com",                     { DNS::QType::A, DNS::QClass::IN, 60, { 128, 140,   0,  23 } } },
	
	// gamespy.com
	{ "bfmc.gamespy.com",                     { DNS::QType::A, DNS::QClass::IN, 60, { 128, 140,   0,  23 } } },
	{ "gpcm.gamespy.com",                     { DNS::QType::A, DNS::QClass::IN, 60, { 128, 140,   0,  23 } } },
	{ "gpsp.gamespy.com",                     { DNS::QType::A, DNS::QClass::IN, 60, { 128, 140,   0,  23 } } },
	{ "gamestats.gamespy.com",                { DNS::QType::A, DNS::QClass::IN, 60, { 128, 140,   0,  23 } } },
	{ "stage-net.gamespy.com",                { DNS::QType::A, DNS::QClass::IN, 60, { 128, 140,   0,  23 } } },
	{ "adminpanel.gamespy.com",               { DNS::QType::A, DNS::QClass::IN, 60, { 128, 140,   0,  23 } } },
	{ "bfield1942ps2.available.gamespy.com",  { DNS::QType::A, DNS::QClass::IN, 60, { 128, 140,   0,  23 } } },
	{ "bfield1942ps2.ms7.gamespy.com",        { DNS::QType::A, DNS::QClass::IN, 60, { 128, 140,   0,  23 } } },
	{ "bfield1942ps2.master.gamespy.com",     { DNS::QType::A, DNS::QClass::IN, 60, { 128, 140,   0,  23 } } },
	{ "bfield1942ps2.ms7.gamespy.com",        { DNS::QType::A, DNS::QClass::IN, 60, { 128, 140,   0,  23 } } },
	{ "bfield1942ps2b.ms7.gamespy.com",       { DNS::QType::A, DNS::QClass::IN, 60, { 128, 140,   0,  23 } } },
	{ "bfield1942ps2b.master.gamespy.com",    { DNS::QType::A, DNS::QClass::IN, 60, { 128, 140,   0,  23 } } },
	{ "bfield1942ps2b.ms7.gamespy.com",       { DNS::QType::A, DNS::QClass::IN, 60, { 128, 140,   0,  23 } } },

	// bfmcspy.net
	{ "bfmc.bfmcspy.net",                     { DNS::QType::A, DNS::QClass::IN, 60, { 128, 140,   0,  23 } } },
	{ "gpcm.bfmcspy.net",                     { DNS::QType::A, DNS::QClass::IN, 60, { 128, 140,   0,  23 } } },
	{ "gamestats.bfmcspy.net",                { DNS::QType::A, DNS::QClass::IN, 60, { 128, 140,   0,  23 } } },
	{ "stage-net.bfmcspy.net",                { DNS::QType::A, DNS::QClass::IN, 60, { 128, 140,   0,  23 } } },
	{ "adminpanel.bfmcspy.net",               { DNS::QType::A, DNS::QClass::IN, 60, { 128, 140,   0,  23 } } },
	{ "bfield1942ps2.available.bfmcspy.net",  { DNS::QType::A, DNS::QClass::IN, 60, { 128, 140,   0,  23 } } },
	{ "bfield1942ps2.ms7.bfmcspy.net",        { DNS::QType::A, DNS::QClass::IN, 60, { 128, 140,   0,  23 } } },
	{ "bfield1942ps2.master.bfmcspy.net",     { DNS::QType::A, DNS::QClass::IN, 60, { 128, 140,   0,  23 } } },
	{ "bfield1942ps2.gamestats.bfmcspy.net",  { DNS::QType::A, DNS::QClass::IN, 60, { 128, 140,   0,  23 } } },
	{ "bfield1942ps2b.available.bfmcspy.net", { DNS::QType::A, DNS::QClass::IN, 60, { 128, 140,   0,  23 } } },
	{ "bfield1942ps2b.ms7.bfmcspy.net",       { DNS::QType::A, DNS::QClass::IN, 60, { 128, 140,   0,  23 } } },
	{ "bfield1942ps2b.master.bfmcspy.net",    { DNS::QType::A, DNS::QClass::IN, 60, { 128, 140,   0,  23 } } },
	{ "bfield1942ps2b.gamestats.bfmcspy.net", { DNS::QType::A, DNS::QClass::IN, 60, { 128, 140,   0,  23 } } },

	// evenbalance.com
	{ "bfmc1.evenbalance.com",                { DNS::QType::A, DNS::QClass::IN, 60, { 128, 140,   0,  23 } } },
	{ "bfmc2.evenbalance.com",                { DNS::QType::A, DNS::QClass::IN, 60, { 128, 140,   0,  23 } } },
	{ "bfmc3.evenbalance.com",                { DNS::QType::A, DNS::QClass::IN, 60, { 128, 140,   0,  23 } } },
	{ "bfmc4.evenbalance.com",                { DNS::QType::A, DNS::QClass::IN, 60, { 128, 140,   0,  23 } } },
	{ "bfmc5.evenbalance.com",                { DNS::QType::A, DNS::QClass::IN, 60, { 128, 140,   0,  23 } } },
	{ "bfmc6.evenbalance.com",                { DNS::QType::A, DNS::QClass::IN, 60, { 128, 140,   0,  23 } } },
	{ "bfmc7.evenbalance.com",                { DNS::QType::A, DNS::QClass::IN, 60, { 128, 140,   0,  23 } } },
	{ "bfmc8.evenbalance.com",                { DNS::QType::A, DNS::QClass::IN, 60, { 128, 140,   0,  23 } } },
	{ "bfmc9.evenbalance.com",                { DNS::QType::A, DNS::QClass::IN, 60, { 128, 140,   0,  23 } } },

	// Medal of honor 2010
	// ea.com
	{ "gosredirector.ea.com",                 { DNS::QType::A, DNS::QClass::IN, 60, { 207, 180, 233,  55 } } },
	{ "goscontent.ea.com",                    { DNS::QType::A, DNS::QClass::IN, 60, { 207, 180, 233,  55 } } },
	{ "easo.ea.com",                          { DNS::QType::A, DNS::QClass::IN, 60, { 207, 180, 233,  55 } } },
};

DNS::Client::Client(int socket, struct sockaddr_in address)
{
	this->_socket = socket;
	this->_address = address;
}

DNS::Client::~Client()
{
	
}

void DNS::Client::Send(const std::vector<unsigned char>& request, const DNS::ConfigItem& config_item)
{
	std::vector<unsigned char> response;

	// Copy over request
	response.assign(request.begin(), request.end());

	// Flags
	response[2] = 0x81;
	response[3] = 0x80;

	// Answers
	response[6] = 0x00;
	response[7] = 0x01;

	// Domain Name reference:
	response.push_back(0xC0); // indicates a pointer
	response.push_back(0x0C); // the offset where the domain name starts.

	// Type of the response (QType)
	response.push_back(static_cast<uint8_t>(static_cast<uint16_t>(config_item.qtype) << 8));
	response.push_back(static_cast<uint8_t>(config_item.qtype));

	// Class of the response (QClass)
	response.push_back(static_cast<uint8_t>(static_cast<uint16_t>(config_item.qclass) << 8));
	response.push_back(static_cast<uint8_t>(config_item.qclass));

	// Time to live (ttl)
	response.push_back(static_cast<uint8_t>(static_cast<uint32_t>(config_item.ttl) << 24));
	response.push_back(static_cast<uint8_t>(static_cast<uint32_t>(config_item.ttl) << 16));
	response.push_back(static_cast<uint8_t>(static_cast<uint32_t>(config_item.ttl) << 8));
	response.push_back(static_cast<uint8_t>(config_item.ttl));

	// Data length
	response.push_back(0x00);
	response.push_back(0x04);

	// Address
	response.push_back(config_item.ip[0]);
	response.push_back(config_item.ip[1]);
	response.push_back(config_item.ip[2]);
	response.push_back(config_item.ip[3]);

	this->UDPSend(response);
	this->_LogTransaction("<--", Util::Buffer::ToString(response));
}

void DNS::Client::onRequest(const std::vector<unsigned char>& request)
{
	this->_LogTransaction("-->", Util::Buffer::ToString(request));

	if (request.size() < 12)
	{
		Logger::warning("Not enough data for DNS header", Server::DNS);
		return;
	}

	// DNS header
	uint16_t transaction_id = (static_cast<uint16_t>(request[0]) << 8) | static_cast<uint16_t>(request[1]);
	uint16_t flags = (static_cast<uint16_t>(request[2]) << 8) | static_cast<uint16_t>(request[3]);
	uint16_t questions = (static_cast<uint16_t>(request[4]) << 8) | static_cast<uint16_t>(request[5]);
	uint16_t answers = (static_cast<uint16_t>(request[6]) << 8) | static_cast<uint16_t>(request[7]);
	uint16_t authority = (static_cast<uint16_t>(request[8]) << 8) | static_cast<uint16_t>(request[9]);
	uint16_t additional = (static_cast<uint16_t>(request[10]) << 8) | static_cast<uint16_t>(request[11]);
	
	// DNS question
	size_t offset = 12;

	std::string qname = "";
	uint16_t qtype;
	uint16_t qclass;

	while(request.size() > offset && request[offset] != 0x00)
	{
		// Get string length
		uint8_t length = static_cast<uint8_t>(request[offset]);
		
		offset += 1;
		
		// Is there enough request data for the domain name?
		if(request.size() < offset + length)
		{
			Logger::warning("Not enough data for QName", Server::DNS);
			break;
		}

		// Add dot between subdomains
		if(qname != "")
		{
			qname += ".";
		}

		// Add new domain name
		qname.append(request.begin() + offset, request.begin() + offset + length);
		
		offset += length;
	}
	offset += 1;

	// Is there enough request data for QTyp and QClass?
	if(request.size() < offset + 4)
	{
		Logger::warning("Not enough data for Qtype and QClass", Server::DNS);
		return;
	}

	// Get QType
	qtype = (static_cast<uint16_t>(request[offset]) << 8) | static_cast<uint16_t>(request[offset + 1]);
	offset += 2;

	if(!this->IsQTypeValid(qtype))
	{
		Logger::warning("Unknown qtype value detected.", Server::DNS);
		return;
	}
	
	// Get QClass
	qclass = (static_cast<uint16_t>(request[offset]) << 8) | static_cast<uint16_t>(request[offset + 1]);

	if(!this->IsQClassValid(qclass))
	{
		Logger::warning("Unknown qclass value detected.", Server::DNS);
		return;
	}

	// Get config for domain name
	auto it = DNS::Config.find(qname);
	if(it != DNS::Config.end())
	{
		this->Send(request, it->second);
	}
	else
	{
		Logger::warning("Domain name not detected: \"" + qname + "\"", Server::DNS);
	}

	// Debug
	//Logger::info("transaction_id = " + std::to_string(transaction_id));
	//Logger::info("flags = "          + std::to_string(flags));
	//Logger::info("questions = "      + std::to_string(questions));
	//Logger::info("answers = "        + std::to_string(answers));
	//Logger::info("authority = "      + std::to_string(authority));
	//Logger::info("additional = "     + std::to_string(additional));
	//Logger::info("qname = " + qname);
	//Logger::info("qtype = " + std::to_string(qtype));
	//Logger::info("qclass = " + std::to_string(qclass));
}

bool DNS::Client::IsQTypeValid(uint16_t qtype)
{
	return
		( qtype >= static_cast<uint16_t>(QType::A) && qtype <= static_cast<uint16_t>(QType::TXT)) ||
    	  qtype == static_cast<uint16_t>(QType::AAAA) ||
		  qtype == static_cast<uint16_t>(QType::SRV) ||
		( qtype >= static_cast<uint16_t>(QType::AXFR) && qtype <= static_cast<uint16_t>(QType::ANY));
}

bool DNS::Client::IsQClassValid(uint16_t qclass)
{
	return
		( qclass >= static_cast<uint16_t>(QClass::IN) && qclass <= static_cast<uint16_t>(QClass::HS)) ||
    	  qclass == static_cast<uint16_t>(QClass::ANY);
}

//	Private functions

void DNS::Client::_LogTransaction(const std::string& direction, const std::string& response) const
{
	std::shared_lock<std::shared_mutex> guard(g_settings_mutex); // settings lock (read)

	if ((g_logger_mode & Logger::Mode::Development) == 0)
	{
		return;
	}

	bool show_console = (g_settings["dns"]["show_requests"].asBool() && direction == "-->") ||
						(g_settings["dns"]["show_responses"].asBool() && direction == "<--");

	Logger::info(this->GetAddress() + " " + direction + " " + response,
			Server::Type::DNS, show_console);
}

