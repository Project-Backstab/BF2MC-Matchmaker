#ifndef DNS_CLIENT_H
#define DNS_CLIENT_H

#include <vector>
#include <unordered_map>

#include <net/socket.h>

namespace DNS
{
	enum class QType : uint16_t
	{
		A        = 1,    // IPv4 address
		NS       = 2,    // Name server
		MD       = 3,    // Obsolete, use MX
		MF       = 4,    // Obsolete, use MX
		CNAME    = 5,    // Canonical name
		SOA      = 6,    // Start of authority
		MB       = 7,    // Mailbox domain name
		MG       = 8,    // Mail group member
		MR       = 9,    // Mail rename domain name
		NULL_R   = 10,   // Null resource record
		WKS      = 11,   // Well-known service
		PTR      = 12,   // Domain name pointer
		HINFO    = 13,   // Host information
		MINFO    = 14,   // Mailbox information
		MX       = 15,   // Mail exchange
		TXT      = 16,   // Text strings
		AAAA     = 28,   // IPv6 address
		SRV      = 33,   // Service location
		AXFR     = 252,  // Transfer of an entire zone
		MAILB    = 253,  // Mailbox-related records (MB, MG, MR)
		MAILA    = 254,  // Mail agent RRs (Obsolete - see MX)
		ANY      = 255   // All records
	};

	enum class QClass : uint16_t
	{
		IN     = 1,    // Internet
		CS     = 2,    // CSNET (obsolete)
		CH     = 3,    // Chaos
		HS     = 4,    // Hesiod
		ANY    = 255   // All classes
	};

	struct ConfigItem
	{
		QType qtype;
		QClass qclass;
		uint32_t ttl;

		uint8_t ip[4];
	};

	extern std::unordered_map<std::string, ConfigItem> Config;

	/**
	 * @brief Represents a client for DNS.
	 * 
	 * This class extends the Net::Socket class to handle client
	 * connections and requests for DNS.
	 */
	class Client : public Net::Socket
	{
		public:
			/**
			 * @brief Constructor for DNS Client.
			 *
			 * @param socket The socket ID.
			 * @param address The client's address.
			 */
			Client(int socket, struct sockaddr_in address);

			/**
			 * @brief Destructor for DNS Client.
			 */
			~Client();

			void Send(const std::vector<unsigned char>& request, const DNS::ConfigItem& config_item);

			/*
				Events
			*/
			/**
			 * @brief Event handler for incoming requests.
			 * @param request The incoming request data.
			 */
			void onRequest(const std::vector<unsigned char>& request);
			
			bool IsQTypeValid(uint16_t qtype);
			bool IsQClassValid(uint16_t qclass);
		
		private:
			/**
			 * @brief Logs a transaction with direction and response.
			 * @param direction The direction of the transaction ("<--" or "-->").
			 * @param response The response message to log.
			 */
			void _LogTransaction(const std::string& direction, const std::string& response) const;
	};
}

#endif // DNS_CLIENT_H
