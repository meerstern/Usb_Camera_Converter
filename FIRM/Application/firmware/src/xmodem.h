

#ifndef XMODEM_H
#define XMODEM_H

enum
{ 
    XMODEM_STATE_INIT,
    XMODEM_STATE_NACK,
    XMODEM_STATE_SOH,
    XMODEM_STATE_ACK,
    XMODEM_STATE_END,
    XMODEM_STATE_TIMEOUT
};


void XmodemInit(ssize_t sendSize, uint8_t *sendBuff);
void XmodemSendProcess(ssize_t consoleBuffSize, uint8_t *consoleBuff);
uint8_t XmodemGetState();


#endif