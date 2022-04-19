/*
 * jpeg.h
 *
 *  Created on: 2021/09/05
 *      Author: MOND
 */

#ifndef JPEG_H_
#define JPEG_H_

#include <stdint.h>

#define JPEG_SOI_MARK       (0xD8FF)    
#define JPEG_EOI_MARK       (0xD9FF)
#define JPEG_DHT_MARK       (0xC4FF)
#define JPEG_DQT_MARK       (0xDBFF)

enum
{
	CAMERA_READ_IDLE,
	CAMERA_READ_EXEC,
	CAMERA_READ_DONE
};

extern const uint8_t huffmanTable[420];

#ifdef ENABLE_JPGDECODE
    #include "tjpgd/tjpgd.h"
    #define     N_BPP                   (3 - JD_FORMAT)
    #define     TJPGD_WORKSIZE          8000
    #define     TJPGD_BMP_MAX_HIGHT     120
    #define     TJPGD_BMP_MAX_WIDTH     160

    #define     LCD_WIDTH_FOR_DECODE    TJPGD_BMP_MAX_WIDTH

    extern uint8_t     TjpgdBmp[N_BPP*TJPGD_BMP_MAX_HIGHT*TJPGD_BMP_MAX_WIDTH];

    bool JpegDecodeProcess(uint8_t *jpgData, uint32_t jpgDataSize, uint8_t scale);
    void JpegDecodeBmpData(uint8_t *data, uint32_t *size);

#endif
    
    void JpegSwapForLcd(uint16_t *data);

#endif /* JPEG_H_ */
