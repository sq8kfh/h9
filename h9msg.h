#ifndef _H9MSG_H_
#define _H9MSG_H_

#include <stdint.h>

#define H9MSG_PRIORITY_BIT_LENGTH 1
#define H9MSG_TYPE_BIT_LENGTH 5
#define H9MSG_SEQNUM_BIT_LENGTH 5
#define H9MSG_DESTINATION_ID_BIT_LENGTH 9
#define H9MSG_SOURCE_ID_BIT_LENGTH 9

#define H9MSG_BROADCAST_ID 0x1ff

#define H9MSG_PRIORITY_HIGH 0
#define H9MSG_PRIORITY_LOW 1

#define H9MSG_TYPE_GROUP_MASK 24

/* 00.... BOOTLOADER + NOP*/
#define H9MSG_TYPE_GROUP_0 0

#define H9MSG_TYPE_NOP 0
#define H9MSG_TYPE_PAGE_START 1
#define H9MSG_TYPE_QUIT_BOOTLOADER 2
#define H9MSG_TYPE_PAGE_FILL 3
#define H9MSG_TYPE_ENTER_INTO_BOOTLOADER 4
#define H9MSG_TYPE_PAGE_FILL_NEXT 5
#define H9MSG_TYPE_PAGE_WRITED 6
#define H9MSG_TYPE_PAGE_FILL_BREAK 7

/* 01.... */
#define H9MSG_TYPE_GROUP_1 8

#define H9MSG_TYPE_REG_EXTERNALLY_CHANGED 8
#define H9MSG_TYPE_REG_INTERNALLY_CHANGED 9
#define H9MSG_TYPE_REG_VALUE_BROADCAST 10
#define H9MSG_TYPE_REG_VALUE 11
#define H9MSG_TYPE_NODE_HEARTBEAT 12
#define H9MSG_TYPE_NODE_ERROR 13
#define H9MSG_TYPE_U14 14
#define H9MSG_TYPE_U15 15

/* 10.... */
#define H9MSG_TYPE_GROUP_2 16

#define H9MSG_TYPE_SET_REG 16
#define H9MSG_TYPE_GET_REG 17
#define H9MSG_TYPE_NODE_INFO 18
#define H9MSG_TYPE_NODE_RESET 19
#define H9MSG_TYPE_NODE_UPGRADE 20
#define H9MSG_TYPE_U21 21
#define H9MSG_TYPE_U22 22
#define H9MSG_TYPE_U23 23


/* 11.... BROADCAST */
#define H9MSG_TYPE_GROUP_3 24

#define H9MSG_TYPE_DISCOVERY 24
#define H9MSG_TYPE_NODE_TURNED_ON 25
#define H9MSG_TYPE_POWER_OFF 26
#define H9MSG_TYPE_U27 27
#define H9MSG_TYPE_U28 28
#define H9MSG_TYPE_U29 29
#define H9MSG_TYPE_U30 30
#define H9MSG_TYPE_U31 31

#define H9MSG_TYPE_COUNT (1 << H9MSG_TYPE_BIT_LENGTH)

// 31 30 29 28 27 26 25 24 23 22 21 20 19 18 17 16 15 14 13 12 11 10 09 08 07 06 05 04 03 02 01 00
// -- -- -- pp ty ty ty ty ty se se se se se ds ds ds ds ds ds ds ds ds so so so so so so so so so

struct h9msg {
    char *endpoint;
    uint8_t priority :H9MSG_PRIORITY_BIT_LENGTH;
    uint8_t type :H9MSG_TYPE_BIT_LENGTH;
    uint8_t seqnum: H9MSG_SEQNUM_BIT_LENGTH;
    uint16_t destination_id :H9MSG_DESTINATION_ID_BIT_LENGTH;
    uint16_t source_id :H9MSG_SOURCE_ID_BIT_LENGTH;
    uint8_t dlc;
    uint8_t data[8];
};

typedef struct h9msg h9msg_t;

h9msg_t *h9msg_init(void);
void h9msg_free(h9msg_t * msg);
h9msg_t *h9msg_copy(const h9msg_t * msg);
char *h9msg_replace_endpoint(h9msg_t * msg, const char *name);
const char *h9msg_type_name(uint8_t type);

#endif /* _H9MSG_H_ */
