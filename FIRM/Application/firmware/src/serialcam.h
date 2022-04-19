#ifndef SERIALCAM_H
#define SERIALCAM_H



extern const uint8_t reset_cmd[4];
extern const uint8_t reset_ack[4];
extern const uint8_t capture_cmd[5];
extern const uint8_t capture_ack[5];
extern const uint8_t read_len_cmd[5];
extern const uint8_t read_len_ack[9];
extern const uint8_t read_data_cmd[16];//0x0A or0xFF
extern const uint8_t read_data_ack[11];
















#endif