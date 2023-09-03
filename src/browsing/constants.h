#ifndef BROWSING_CONSTANTS_H
#define BROWSING_CONSTANTS_H

//message types for outgoing requests
#define REQUEST_SERVER_LIST             0
#define REQUEST_SERVER_INFO             1
#define REQUEST_SEND_MESSAGE            2
#define REQUEST_KEEPALIVE               3
#define REQUEST_MAPLOOP                 4
#define REQUEST_PLAYERSEARCH            5

#define CHALLENGE_CRYPT_LEN            10
#define CHALLENGE_SERVER_LEN           25
#define CHALLENGE_CLIENT_LEN            8
#define CHALLENGE_CRYPT_SERVER_LEN    (1 + CHALLENGE_CRYPT_LEN + 1 + CHALLENGE_SERVER_LEN)
#define CHALLENGE_HEADER_LEN          (CHALLENGE_CRYPT_SERVER_LEN + CHALLENGE_CLIENT_LEN)

#define FLAG_UNSOLICITED_UDP            1
#define FLAG_PRIVATE_IP                 2
#define FLAG_CONNECT_NEGOTIATE          4
#define FLAG_ICMP_IP                    8
#define FLAG_NONSTANDARD_PORT          16
#define FLAG_NONSTANDARD_PRIVATE_PORT  32
#define FLAG_HAS_KEYS                  64
#define FLAG_HAS_FULL_RULES           128

//#define SECRET_KEY                    "HpWx9z"
#define SECRET_KEY                    "Hpch92"
#define SECRET_KEY_LEN                6

#endif // BROWSING_CONSTANTS_H
