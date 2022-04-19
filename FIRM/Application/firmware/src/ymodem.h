
#ifndef YMODEM_H
#define YMODEM_H


#define YMODEM_PACKET_HEADER_SIZE      ((uint32_t)3)
#define YMODEM_PACKET_DATA_INDEX       ((uint32_t)3)
#define YMODEM_PACKET_START_INDEX      ((uint32_t)0)
#define YMODEM_PACKET_NUMBER_INDEX     ((uint32_t)1)
#define YMODEM_PACKET_CNUMBER_INDEX    ((uint32_t)2)
#define YMODEM_PACKET_TRAILER_SIZE     ((uint32_t)2)
#define YMODEM_PACKET_OVERHEAD_SIZE    (YMODEM_PACKET_HEADER_SIZE + YMODEM_PACKET_TRAILER_SIZE)
#define YMODEM_PACKET_SIZE             ((uint32_t)128)
#define YMODEM_PACKET_1K_SIZE          ((uint32_t)1024)
#define YMODEM_FILE_NAME_LENGTH        ((uint32_t)64)
#define YMODEM_FILE_SIZE_LENGTH        ((uint32_t)16)


enum
{ 
    YMODEM_STATE_INIT,///0
    YMODEM_STATE_HEADER,
    YMODEM_STATE_ACK1,//2
    YMODEM_STATE_BODY,
    YMODEM_STATE_ACK2,//4
    YMODEM_STATE_EOT,
    YMODEM_STATE_SOH,//6
    YMODEM_STATE_ACK3,
    YMODEM_STATE_END,//7
    YMODEM_STATE_ABRT,
    YMODEM_STATE_ERR,//8
    YMODEM_STATE_TIMEOUT
};


void YmodemSendInit(ssize_t sendSize, uint8_t *sendBuff, uint8_t *sendName);
void YmodemSendProcess(ssize_t consoleBuffSize, uint8_t *consoleBuff);
uint8_t YmodemGetState();
void YmodemRecieveInit(uint8_t *buff, ssize_t buffMaxSize, uint8_t *recieveName, uint32_t *recieveSize);
void YmodemRecieceProcess(ssize_t consoleBuffSize, uint8_t *consoleBuff);

#endif