/*
 * bitmap.c
 *
 *  Created on: 2021/09/04
 *      Author: MOND
 */
#include "bitmap.h"
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#define 	BITMAP_FILE_HEADER_SIZE			14
#define 	BITMAP_INFO_HEADER_SIZE			40
#define 	BITMAP_BIT_FIELD_SIZE			16
#define 	BITMAP_TOTAL_HEADER_OFFSET		70
#define 	BITMAP_TOTAL_HEADER_MONO_OFFSET	62
#define 	BITMAP_HEADER_1ST				0x42//'B'
#define     BITMAP_HEADER_2ND				0x4D//'M'
#define     BITMAP_RESERVERD1_OFFSET		0x06
#define     BITMAP_RESERVERD2_OFFSET		0x08
#define 	BITMAP_FILE_OFFSET				0x02
#define 	BITMAP_WIDTH_OFFSET				0x04
#define 	BITMAP_HEIGHT_OFFSET			0x08
#define 	BITMAP_OFFSET					0x0A
#define 	BITMAP_PLANES_OFFSET			0x0C
#define 	BITMAP_BITCOUNT_OFFSET			0x0E
#define 	BITMAP_BITCOMPRESS_OFFSET		0x10
#define 	BITMAP_IMAGE_SIZE_OFFSET		0x14
#define 	BITMAP_XPIX_PER_METER_OFFSET	0x18
#define 	BITMAP_YPIX_PER_METER_OFFSET	0x1C
#define 	BITMAP_COLOR_INDEX_OFFSET		0x20
#define 	BITMAP_IMPORTANT_INDEX_OFFSET	0x24

static uint32_t staticWidth;
static uint32_t staticHeight;


// Calculate the number of bytes used to store a single image row.
static inline uint32_t getBytesPerRow(uint32_t width)
{
    if (width & 0x00000001)
        return ((width + 1) << 1);
    else
        return  (width<< 1);
}

static inline uint32_t getBytesPerRowMono(uint32_t width)
{
    if (width % 32== 0)
        return  4*(width/32);
    else
        return  4*(width/32+1);
}

static inline uint16_t convertRGB2RGB565(uint8_t r, uint8_t g, uint8_t b)
{
    return   ((uint16_t) (r >> 3) << 11)
    		| ((uint16_t) (g >> 2) << 5)
			|  (uint16_t) (b >> 3);
}

// Reads a little-endian unsigned int from the file.
static inline uint32_t readUint32(uint8_t* pSrc)
{
    uint32_t retval = 0x00000000;
    retval |= (uint32_t)*(pSrc + 3) << 24;
    retval |= (uint32_t)*(pSrc + 2) << 16;
    retval |= (uint32_t)*(pSrc + 1) <<  8;
    retval |= (uint32_t)*(pSrc    );
    return retval;
}
// Reads a little-endian unsigned int from the file.
static inline uint16_t readUint16(uint8_t* pSrc)
{
    uint16_t retval = 0x0000;
    retval |= (uint16_t)*(pSrc + 1) <<  8;
    retval |= (uint16_t)*pSrc;
    return retval;
}

// Writes a little-endian unsigned short int to the file.
static inline void writeUint32(uint32_t Src, uint8_t* pDst)
{
    *(pDst + 3) = (uint8_t)( ( Src & 0xff000000 ) >> 24 );
    *(pDst + 2) = (uint8_t)( ( Src & 0x00ff0000 ) >> 16 );
    *(pDst + 1) = (uint8_t)( ( Src & 0x0000ff00 ) >> 8  );
    *pDst       = (uint8_t)  ( Src & 0x000000ff )        ;
}
// Writes a little-endian unsigned short int to the file.
static inline void writeUint16(uint16_t Src, uint8_t* pDst)
{
    *(pDst + 1) = (uint8_t)( ( Src & 0xff00 ) >> 8 );
	*pDst       = (uint8_t)  ( Src & 0x00ff )       ;
}


uint32_t Bitmap565GetWidth(uint8_t* buffer)
{
    return readUint32(buffer + BITMAP_FILE_HEADER_SIZE + BITMAP_WIDTH_OFFSET);
}

uint32_t Bitmap565GetHeight(uint8_t* buffer)
{
    return readUint32(buffer + BITMAP_FILE_HEADER_SIZE + BITMAP_HEIGHT_OFFSET);
}

uint32_t Bitmap565GetFileSize(uint8_t* buffer)
{
    return readUint32(buffer + BITMAP_FILE_OFFSET);
}

uint32_t Bitmap565GetImageSize(uint8_t* buffer)
{
    return readUint32(buffer + BITMAP_FILE_HEADER_SIZE + BITMAP_IMAGE_SIZE_OFFSET);
}

uint32_t Bitmap565GetOffset(uint8_t* buffer)
{
    return readUint32(buffer + BITMAP_OFFSET);
}


void Bitmap565InitBuf(uint8_t *buffer, uint32_t bufferSize, uint32_t width, uint32_t height)
{

    uint32_t bytesPerRow = getBytesPerRow(width);
    uint32_t imageSize = bytesPerRow * height;
    uint32_t dataSize = BITMAP_TOTAL_HEADER_OFFSET + imageSize;
    uint32_t i;
    // Check Null
	if(buffer==NULL)
		return;

	// Check Size
    if(dataSize > bufferSize)
    	return;

    // Initialize Data
    for( i = 0; i < dataSize; i++)
        *(buffer + i) = 0;

    // Set Header Value
    uint8_t *header= buffer;
    
    staticWidth=width;
    staticHeight=height;

    *(header  +  0) = BITMAP_HEADER_1ST;     // 'B' : Magic number
    *(header  +  1) = BITMAP_HEADER_2ND;     // 'M' : Magic number
    writeUint32( dataSize        			, header + BITMAP_FILE_OFFSET);   		// File Size
    writeUint16( 0                			, header + BITMAP_RESERVERD1_OFFSET);   // Reserved1
    writeUint16( 0                			, header + BITMAP_RESERVERD2_OFFSET);   // Reserved2
    writeUint32( BITMAP_TOTAL_HEADER_OFFSET , header + BITMAP_OFFSET);   			// Offset
    header += BITMAP_FILE_HEADER_SIZE;

    // Info Header
    writeUint32( BITMAP_INFO_HEADER_SIZE + BITMAP_BIT_FIELD_SIZE, header + 0x00);   // HeaderSize
    writeUint32( width           , header + BITMAP_WIDTH_OFFSET);    				// width
    writeUint32( height          , header + BITMAP_HEIGHT_OFFSET); 	 				// height
    writeUint16( 1               , header + BITMAP_PLANES_OFFSET);	 				// planes
    writeUint16( 16              , header + BITMAP_BITCOUNT_OFFSET); 				// Bit count
    writeUint32( 3               , header + BITMAP_BITCOMPRESS_OFFSET); 			// Bit compression
    writeUint32( imageSize       , header + BITMAP_IMAGE_SIZE_OFFSET); 				// Image size
    writeUint32( 0               , header + BITMAP_XPIX_PER_METER_OFFSET);  		// X pixels per meter
    writeUint32( 0               , header + BITMAP_YPIX_PER_METER_OFFSET);  		// Y pixels per meter
    writeUint32( 0               , header + BITMAP_COLOR_INDEX_OFFSET);  			// Color index
    writeUint32( 0               , header + BITMAP_IMPORTANT_INDEX_OFFSET);  		 // Important index
    header += BITMAP_INFO_HEADER_SIZE;

    // Bit Field
    writeUint32( 0x0000F800      , header + 0x00);  				 // red
    writeUint32( 0x000007E0      , header + 0x04);  				 // green
    writeUint32( 0x0000001F      , header + 0x08);  				 // blue
    writeUint32( 0x00000000      , header + 0x0C);  				 // reserved

}

void __inline Bitmap565SetPixel(uint8_t* buffer, uint32_t x, uint32_t y, uint16_t color)
{
//    uint32_t width  = Bitmap565GetWidth (buffer);
//    uint32_t height = Bitmap565GetHeight (buffer);
     

    if(buffer == NULL || x >= staticWidth || y >= staticHeight)
        return;

    uint32_t bytes_per_row = getBytesPerRow(staticWidth);

    writeUint16( color, buffer + BITMAP_TOTAL_HEADER_OFFSET + bytes_per_row * (staticHeight - y - 1) + (x << 1));
}

void Bitmap565SetPixelRGB(uint8_t* buffer, uint32_t x, uint32_t y, uint8_t r, uint8_t g, uint8_t b)
{
    uint32_t width  = Bitmap565GetWidth (buffer);
    uint32_t height = Bitmap565GetHeight (buffer);

    if(buffer == NULL || x >= width || y >= height)
        return;

    uint32_t bytes_per_row = getBytesPerRow(width);
    uint16_t color = convertRGB2RGB565(r, g, b);

    writeUint16( color, buffer + BITMAP_TOTAL_HEADER_OFFSET + bytes_per_row * (height - y - 1) + (x << 1));
}


void Bitmap565GetPixel(uint8_t* buffer, uint32_t x, uint32_t y, uint16_t* color)
{
    uint32_t width  = Bitmap565GetWidth(buffer);
    uint32_t height = Bitmap565GetHeight(buffer);

    if(buffer == NULL || x >= width || y >= height)
        return;

    uint32_t bytes_per_row = getBytesPerRow(width);
    *color = readUint16(buffer + BITMAP_TOTAL_HEADER_OFFSET + bytes_per_row * (height - y - 1) + (x << 1));
}

void Bitmap565GetPixelRGB(uint8_t* buffer, uint32_t x, uint32_t y, uint8_t* r, uint8_t* g, uint8_t* b)
{
    uint32_t width  = Bitmap565GetWidth(buffer);
    uint32_t height = Bitmap565GetHeight(buffer);

    if(buffer == NULL || x >= width || y >= height)
        return;

    uint32_t bytes_per_row = getBytesPerRow(width);
    uint16_t color = readUint16(buffer + BITMAP_TOTAL_HEADER_OFFSET + bytes_per_row * (height - y - 1) + (x << 1));
    *r = (uint8_t)(color >> 11) << 3;
    *g = (uint8_t)(color >>  5) << 2;
    *b = (uint8_t) color        << 3;
}


void Bitmap565DrawRectRGB(uint8_t* buffer, uint32_t x0, uint32_t y0, uint32_t x1, uint32_t y1,
        uint8_t r, uint8_t g, uint8_t b)
{
    uint32_t width  = Bitmap565GetWidth(buffer);
    uint32_t height = Bitmap565GetHeight(buffer);

    if (buffer == NULL || x0 >= width || x1 >= width || y0 >= height || y1 >= height)
        return;

    uint32_t swap;
    if(x0 > x1)
    {
        swap = x0;
        x0 = x1;
        x1 = swap;
    }
    if (y0 > y1)
    {
        swap = y0;
        x0 = y1;
        y1 = swap;
    }

    uint32_t bytesPerRow = getBytesPerRow(width);
    uint16_t color = convertRGB2RGB565(r, g, b);

    uint8_t* buffer_data = buffer + BITMAP_TOTAL_HEADER_OFFSET;

    int32_t y_addr_start = bytesPerRow*(height - y0 - 1);
    int32_t y_addr_end   = bytesPerRow*(height - y1 - 1);
    int32_t x_addr_start = x0 * 2;
    int32_t x_addr_end   = x1 * 2;
    int32_t y_addr;
    int32_t x_addr;
    for( y_addr = y_addr_start; y_addr >= y_addr_end; y_addr -= bytesPerRow)
    {
        for( x_addr = x_addr_start; x_addr <= x_addr_end; x_addr += 2)
        	writeUint16( color, buffer_data + y_addr + x_addr);
    }
}

void Bitmap565FillRGB(uint8_t* buffer, uint8_t r, uint8_t g, uint8_t b)
{
    if (buffer == NULL)
        return;

    Bitmap565DrawRectRGB(buffer, 0, 0, Bitmap565GetWidth(buffer)-1, Bitmap565GetHeight(buffer)-1, r, g, b);
}

void BitmapMonoInitBuf(uint8_t *buffer, uint32_t bufferSize, uint32_t width, uint32_t height)
{

    uint32_t bytesPerRow = getBytesPerRowMono(width);
    uint32_t imageSize = bytesPerRow * height;
    uint32_t dataSize = BITMAP_TOTAL_HEADER_MONO_OFFSET + imageSize;
    uint32_t i;
    
    // Check Null
	if(buffer==NULL)
		return;

	// Check Size
    if(dataSize > bufferSize)
    	return;

    // Initialize Data
    for( i = 0; i < dataSize; i++)
        *(buffer + i) = 0;

    // Set Header Value
    uint8_t *header= buffer;

    *(header  +  0) = BITMAP_HEADER_1ST;     // 'B' : Magic number
    *(header  +  1) = BITMAP_HEADER_2ND;     // 'M' : Magic number
    writeUint32( dataSize        				 , header + BITMAP_FILE_OFFSET);   		// File Size
    writeUint16( 0                				 , header + BITMAP_RESERVERD1_OFFSET);   // Reserved1
    writeUint16( 0                				 , header + BITMAP_RESERVERD2_OFFSET);   // Reserved2
    writeUint32( BITMAP_TOTAL_HEADER_MONO_OFFSET , header + BITMAP_OFFSET);   			// Offset
    header += BITMAP_FILE_HEADER_SIZE;

    // Info Header
    writeUint32( BITMAP_INFO_HEADER_SIZE , header + 0x00);   // HeaderSize
    writeUint32( width           , header + BITMAP_WIDTH_OFFSET);    				// width
    writeUint32( height          , header + BITMAP_HEIGHT_OFFSET); 	 				// height
    writeUint16( 1               , header + BITMAP_PLANES_OFFSET);	 				// planes
    writeUint16( 1               , header + BITMAP_BITCOUNT_OFFSET); 				// Bit count
    writeUint32( 0               , header + BITMAP_BITCOMPRESS_OFFSET); 			// Bit compression
    writeUint32( imageSize       , header + BITMAP_IMAGE_SIZE_OFFSET); 				// Image size
    writeUint32( 0               , header + BITMAP_XPIX_PER_METER_OFFSET);  		// X pixels per meter
    writeUint32( 0               , header + BITMAP_YPIX_PER_METER_OFFSET);  		// Y pixels per meter
    writeUint32( 0               , header + BITMAP_COLOR_INDEX_OFFSET);  			// Color index
    writeUint32( 0               , header + BITMAP_IMPORTANT_INDEX_OFFSET);  		 // Important index
    header += BITMAP_INFO_HEADER_SIZE;

    // Bit Field
    writeUint32( 0x00000000      , header + 0x00);
    writeUint32( 0x00FFFFFF      , header + 0x04);
}

uint32_t BitmapMonoGetFileSize(uint8_t* buffer)
{
    return readUint32(buffer + BITMAP_FILE_OFFSET);
}

void BitmapMonoSetPixel(uint8_t* buffer, uint32_t x, uint32_t y, bool bit)
{
    uint32_t width  = Bitmap565GetWidth (buffer);
    uint32_t height = Bitmap565GetHeight (buffer);
    uint32_t bytesPerRow = getBytesPerRowMono(width);

    if(buffer == NULL || x >= width || y >= height)
        return;

    uint32_t offset=BITMAP_TOTAL_HEADER_MONO_OFFSET+ bytesPerRow * (height - y - 1) + (x>>3);
    uint8_t val= *(buffer+offset);
    uint8_t pos=7-(x%8);

    if(bit==false)
    {
    	val = val | (0x01<<pos);
    }
    else
    {
    	val = val & ~(0x01<<pos);
    }

    *(buffer+offset)=val;


}

void BitmapMonoDrawRect(uint8_t* buffer, uint32_t x0, uint32_t y0, uint32_t x1, uint32_t y1, bool bit)
{
    uint32_t width  = Bitmap565GetWidth(buffer);
    uint32_t height = Bitmap565GetHeight(buffer);

    if (buffer == NULL || x0 >= width || x1 >= width || y0 >= height || y1 >= height)
        return;

    uint32_t swap;
    if(x0 > x1)
    {
        swap = x0;
        x0 = x1;
        x1 = swap;
    }
    if (y0 > y1)
    {
        swap = y0;
        x0 = y1;
        y1 = swap;
    }

    uint32_t x,y;
    for( x = 0 ; x < width ; x++ )
    {
    	for( y = 0 ; y < height ; y++ )
    	{
    		if( x0<= x && x<=x1 && y0 <= y && y<= y1 )
    			BitmapMonoSetPixel(buffer, x, y, bit);

    	}

    }

}

void BitmapMonoFill(uint8_t* buffer, bool bit)
{
    if (buffer == NULL)
        return;

    BitmapMonoDrawRect(buffer, 0, 0, Bitmap565GetWidth(buffer)-1, Bitmap565GetHeight(buffer)-1, bit);
}


void BitmapRGB565toMono(uint8_t *rgb565Buff, uint8_t *monoBuff)
{
    uint32_t widthRGB  = Bitmap565GetWidth(rgb565Buff);
    uint32_t heightRGB = Bitmap565GetHeight(rgb565Buff);

    uint32_t widthMono  = Bitmap565GetWidth(monoBuff);
    uint32_t heightMono = Bitmap565GetHeight(monoBuff);

    if (rgb565Buff == NULL || monoBuff == NULL || widthRGB != widthMono || heightRGB != heightMono )
        return;

    uint8_t r,g,b;
    uint16_t gray_16;
    uint8_t gray_8;
    uint16_t hist[256]={0};
    uint32_t x,y;
    //Get Grayscale histgram
    for( x=0; x<widthRGB; x++ )
    {
    	 for( y=0; y<heightRGB; y++ )
		{
    		 Bitmap565GetPixelRGB(rgb565Buff, x, y, &r, &g, &b);
    		 gray_16 = (0.2126 * (float)r) + (0.7152 * (float)g/2.0f) + (0.0722 * (float)b);

    		 if( gray_16>255 )
    			 gray_8 = 255;
    		 else
    			 gray_8 = gray_16;

    		 hist[gray_8]++;
		}
    }

    //Get Threshold
    uint16_t th = 0;
    float max = 0.0f;
    uint16_t index, sub;
    for( index=0; index<256; index++ )
    {
    	uint32_t classA_num = 0;
    	uint32_t classB_num = 0;
    	uint32_t classA_sum = 0;
    	uint32_t classB_sum = 0;
    	float classA_ave = 0;
    	float classB_ave = 0;
    	float var = 0.0f;

    	for( sub=0; sub<=index; sub++)
    	{
    		classA_num += hist[sub];
    		classA_sum += sub*hist[sub];
    	}

    	for( sub=index+1; sub<256; sub++)
    	{
    		classB_num += hist[sub];
    		classB_sum += sub*hist[sub];
    	}

    	if( classA_num != 0 )
    		classA_ave = (float)classA_sum/(float)classA_num;
    	if( classB_num != 0 )
    		classB_ave = (float)classB_sum/(float)classB_num;

    	var=classA_num*classB_num*(classA_ave-classB_ave)*(classA_ave-classB_ave);

    	if( var > max )
    	{
    		max = var;
    		th = index;
    	}
    }
    
    //Set Binary Value with Threshold
    for( x=0; x<widthRGB; x++ )
    {
    	 for( y=0; y<heightRGB; y++ )
		{
    		 Bitmap565GetPixelRGB(rgb565Buff, x, y, &r, &g, &b);
    		 uint16_t gray_16 = (0.2126 * (float)r) + (0.7152 * (float)g/2.0f) + (0.0722 * (float)b);
    		 bool bit=0;
    		 if( gray_16 > th )
    			 bit = 0;
    		 else
    			 bit = 1;

    		 BitmapMonoSetPixel(monoBuff, x, y, bit);
		}
    }

}