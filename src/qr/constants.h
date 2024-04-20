#ifndef QR_CONSTANTS_H
#define QR_CONSTANTS_H

#define REQUEST_CHALLENGE       0x01 /**< Request Challenge command. */
#define REQUEST_HEARTBEAT       0x03 /**< Request Heartbeat command. */
#define REQUEST_KEEPALIVE       0x08 /**< Request Keep Alive command. */
#define REQUEST_AVAILABLE       0x09 /**< Request Available command. */

#define RESPONSE_MAGIC_1        0xFE /**< Response Magic 1 byte. */
#define RESPONSE_MAGIC_2        0xFD /**< Response Magic 2 byte. */

#define RESPONSE_HEARTBEAT      0x01 /**< Response Heartbeat command. */
#define RESPONSE_AVAILABLE      0x09 /**< Response Available command. */
#define RESPONSE_KEEPALIVE      0x08 /**< Response Keep Alive command. */
#define RESPONSE_CHALLENGE      0x0A /**< Response Challenge command. */

#endif // QR_CONSTANTS_H
