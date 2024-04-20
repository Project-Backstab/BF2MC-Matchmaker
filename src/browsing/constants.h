#ifndef BROWSING_CONSTANTS_H
#define BROWSING_CONSTANTS_H

#define REQUEST_SERVER_LIST             0 /**< Request type for server list. */
#define REQUEST_SERVER_INFO             1 /**< Request type for server information. */
#define REQUEST_SEND_MESSAGE            2 /**< Request type for sending a message. */
#define REQUEST_KEEPALIVE               3 /**< Request type for keep-alive. */
#define REQUEST_MAPLOOP                 4 /**< Request type for map loop. */
#define REQUEST_PLAYERSEARCH            5 /**< Request type for player search. */

#define CHALLENGE_CRYPT_LEN            10 /**< Length of challenge crypt. */
#define CHALLENGE_SERVER_LEN           25 /**< Length of server challenge. */
#define CHALLENGE_CLIENT_LEN            8 /**< Length of client challenge. */
#define CHALLENGE_HEADER_LEN          (1 + CHALLENGE_CRYPT_LEN + 1 + CHALLENGE_SERVER_LEN) /**< Length of challenge header. */

#define FLAG_UNSOLICITED_UDP            1 /**< Flag for unsolicited UDP. */
#define FLAG_PRIVATE_IP                 2 /**< Flag for private IP. */
#define FLAG_CONNECT_NEGOTIATE          4 /**< Flag for connect negotiate. */
#define FLAG_ICMP_IP                    8 /**< Flag for ICMP IP. */
#define FLAG_NONSTANDARD_PORT          16 /**< Flag for non-standard port. */
#define FLAG_NONSTANDARD_PRIVATE_PORT  32 /**< Flag for non-standard private port. */
#define FLAG_HAS_KEYS                  64 /**< Flag for having keys. */
#define FLAG_HAS_FULL_RULES           128 /**< Flag for having full rules. */

//#define SECRET_KEY                    "HpWx9z"
#define SECRET_KEY                    "Hpch92"
#define SECRET_KEY_LEN                6

#endif // BROWSING_CONSTANTS_H
