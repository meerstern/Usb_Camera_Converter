/*
 * bitmap.h
 *
 *  Created on: 2021/09/04
 *      Author: MOND
 */

#ifndef BITMAP_H_
#define BITMAP_H_

#include <stdint.h>
#include "configuration.h"


#define Color565(red, green, blue)    (uint16_t) ((((uint16_t)(red) & 0xF8) << 8) | (((uint16_t)(green) & 0xFC) << 3) | (((uint16_t)(blue) & 0xF8) >> 3))


#define	BITMAP_COLOR_RED(_COLOR_)		(uint8_t)((_COLOR_)>>16)
#define	BITMAP_COLOR_GREEN(_COLOR_)		(uint8_t)((_COLOR_)>>8)
#define	BITMAP_COLOR_BLUE(_COLOR_)		(uint8_t)((_COLOR_)>>0)
#define BITMAP_COLOR_RGB_SET(_COLOR_)  	COLOR_R(_COLOR_),COLOR_G(_COLOR_),COLOR_B(_COLOR_)


#define  BITMAP_MAROON                     0x800000
#define  BITMAP_DARK_RED                   0x8B0000
#define  BITMAP_BROWN                      0xA52A2A
#define  BITMAP_FIREBRICK                  0xB22222
#define  BITMAP_CRIMSON                    0xDC143C
#define  BITMAP_RED                        0xFF0000
#define  BITMAP_TOMATO                     0xFF6347
#define  BITMAP_CORAL                      0xFF7F50
#define  BITMAP_INDIAN_RED                 0xCD5C5C
#define  BITMAP_LIGHT_CORAL                0xF08080
#define  BITMAP_DARK_SALMON                0xE9967A
#define  BITMAP_SALMON                     0xFA8072
#define  BITMAP_LIGHT_SALMON               0xFFA07A
#define  BITMAP_ORANGE_RED                 0xFF4500
#define  BITMAP_DARK_ORANGE                0xFF8C00
#define  BITMAP_ORANGE                     0xFFA500
#define  BITMAP_GOLD                       0xFFD700
#define  BITMAP_DARK_GOLDEN_ROD            0xB8860B
#define  BITMAP_GOLDEN_ROD                 0xDAA520
#define  BITMAP_PALE_GOLDEN_ROD            0xEEE8AA
#define  BITMAP_DARK_KHAKI                 0xBDB76B
#define  BITMAP_KHAKI                      0xF0E68C
#define  BITMAP_OLIVE                      0x808000
#define  BITMAP_YELLOW                     0xFFFF00
#define  BITMAP_YELLOW_GREEN               0x9ACD32
#define  BITMAP_DARK_OLIVE_GREEN           0x556B2F
#define  BITMAP_OLIVE_DRAB                 0x6B8E23
#define  BITMAP_LAWN_GREEN                 0x7CFC00
#define  BITMAP_CHART_REUSE                0x7FFF00
#define  BITMAP_GREEN_YELLOW               0xADFF2F
#define  BITMAP_DARK_GREEN                 0x006400
#define  BITMAP_GREEN                      0x00FF00
#define  BITMAP_FOREST_GREEN               0x228B22
#define  BITMAP_LIME                       0x00FF00
#define  BITMAP_LIME_GREEN                 0x32CD32
#define  BITMAP_LIGHT_GREEN                0x90EE90
#define  BITMAP_PALE_GREEN                 0x98FB98
#define  BITMAP_DARK_SEA_GREEN             0x8FBC8F
#define  BITMAP_MEDIUM_SPRING_GREEN        0x00FA9A
#define  BITMAP_SPRING_GREEN               0x00FF7F
#define  BITMAP_SEA_GREEN                  0x2E8B57
#define  BITMAP_MEDIUM_AQUA_MARINE         0x66CDAA
#define  BITMAP_MEDIUM_SEA_GREEN           0x3CB371
#define  BITMAP_LIGHT_SEA_GREEN            0x20B2AA
#define  BITMAP_DARK_SLATE_GRAY            0x2F4F4F
#define  BITMAP_TEAL                       0x008080
#define  BITMAP_DARK_CYAN                  0x008B8B
#define  BITMAP_AQUA                       0x00FFFF
#define  BITMAP_CYAN                       0x00FFFF
#define  BITMAP_LIGHT_CYAN                 0xE0FFFF
#define  BITMAP_DARK_TURQUOISE             0x00CED1
#define  BITMAP_TURQUOISE                  0x40E0D0
#define  BITMAP_MEDIUM_TURQUOISE           0x48D1CC
#define  BITMAP_PALE_TURQUOISE             0xAFEEEE
#define  BITMAP_AQUA_MARINE                0x7FFFD4
#define  BITMAP_POWDER_BLUE                0xB0E0E6
#define  BITMAP_CADET_BLUE                 0x5F9EA0
#define  BITMAP_STEEL_BLUE                 0x4682B4
#define  BITMAP_CORN_FLOWER_BLUE           0x6495ED
#define  BITMAP_DEEP_SKY_BLUE              0x00BFFF
#define  BITMAP_DODGER_BLUE                0x1E90FF
#define  BITMAP_LIGHT_BLUE                 0xADD8E6
#define  BITMAP_SKY_BLUE                   0x87CEEB
#define  BITMAP_LIGHT_SKY_BLUE             0x87CEFA
#define  BITMAP_MIDNIGHT_BLUE              0x191970
#define  BITMAP_NAVY                       0x000080
#define  BITMAP_DARK_BLUE                  0x00008B
#define  BITMAP_MEDIUM_BLUE                0x0000CD
#define  BITMAP_BLUE                       0x0000FF
#define  BITMAP_ROYAL_BLUE                 0x4169E1
#define  BITMAP_BLUE_VIOLET                0x8A2BE2
#define  BITMAP_INDIGO                     0x4B0082
#define  BITMAP_DARK_SLATE_BLUE            0x483D8B
#define  BITMAP_SLATE_BLUE                 0x6A5ACD
#define  BITMAP_MEDIUM_SLATE_BLUE          0x7B68EE
#define  BITMAP_MEDIUM_PURPLE              0x9370DB
#define  BITMAP_DARK_MAGENTA               0x8B008B
#define  BITMAP_DARK_VIOLET                0x9400D3
#define  BITMAP_DARK_ORCHID                0x9932CC
#define  BITMAP_MEDIUM_ORCHID              0xBA55D3
#define  BITMAP_PURPLE                     0x800080
#define  BITMAP_THISTLE                    0xD8BFD8
#define  BITMAP_PLUM                       0xDDA0DD
#define  BITMAP_VIOLET                     0xEE82EE
#define  BITMAP_MAGENTA                    0xFF00FF
#define  BITMAP_ORCHID                     0xDA70D6
#define  BITMAP_MEDIUM_VIOLET_RED          0xC71585
#define  BITMAP_PALE_VIOLET_RED            0xDB7093
#define  BITMAP_DEEP_PINK                  0xFF1493
#define  BITMAP_HOT_PINK                   0xFF69B4
#define  BITMAP_LIGHT_PINK                 0xFFB6C1
#define  BITMAP_PINK                       0xFFC0CB
#define  BITMAP_ANTIQUE_WHITE              0xFAEBD7
#define  BITMAP_BEIGE                      0xF5F5DC
#define  BITMAP_BISQUE                     0xFFE4C4
#define  BITMAP_BLANCHED_ALMOND            0xFFEBCD
#define  BITMAP_WHEAT                      0xF5DEB3
#define  BITMAP_CORN_SILK                  0xFFF8DC
#define  BITMAP_LEMON_CHIFFON              0xFFFACD
#define  BITMAP_LIGHT_GOLDEN_ROD_YELLOW    0xFAFAD2
#define  BITMAP_LIGHT_YELLOW               0xFFFFE0
#define  BITMAP_SADDLE_BROWN               0x8B4513
#define  BITMAP_SIENNA                     0xA0522D
#define  BITMAP_CHOCOLATE                  0xD2691E
#define  BITMAP_PERU                       0xCD853F
#define  BITMAP_SANDY_BROWN                0xF4A460
#define  BITMAP_BURLY_WOOD                 0xDEB887
#define  BITMAP_TAN                        0xD2B48C
#define  BITMAP_ROSY_BROWN                 0xBC8F8F
#define  BITMAP_MOCCASIN                   0xFFE4B5
#define  BITMAP_NAVAJO_WHITE               0xFFDEAD
#define  BITMAP_PEACH_PUFF                 0xFFDAB9
#define  BITMAP_MISTY_ROSE                 0xFFE4E1
#define  BITMAP_LAVENDER_BLUSH             0xFFF0F5
#define  BITMAP_LINEN                      0xFAF0E6
#define  BITMAP_OLD_LACE                   0xFDF5E6
#define  BITMAP_PAPAYA_WHIP                0xFFEFD5
#define  BITMAP_SEA_SHELL                  0xFFF5EE
#define  BITMAP_MINT_CREAM                 0xF5FFFA
#define  BITMAP_SLATE_GRAY                 0x708090
#define  BITMAP_LIGHT_SLATE_GRAY           0x778899
#define  BITMAP_LIGHT_STEEL_BLUE           0xB0C4DE
#define  BITMAP_LAVENDER                   0xE6E6FA
#define  BITMAP_FLORAL_WHITE               0xFFFAF0
#define  BITMAP_ALICE_BLUE                 0xF0F8FF
#define  BITMAP_GHOST_WHITE                0xF8F8FF
#define  BITMAP_HONEYDEW                   0xF0FFF0
#define  BITMAP_IVORY                      0xFFFFF0
#define  BITMAP_AZURE                      0xF0FFFF
#define  BITMAP_SNOW                       0xFFFAFA
#define  BITMAP_BLACK                      0x000000
#define  BITMAP_DIM_GRAY                   0x696969
#define  BITMAP_GRAY                       0x808080
#define  BITMAP_DARK_GRAY                  0xA9A9A9
#define  BITMAP_SILVER                     0xC0C0C0
#define  BITMAP_LIGHT_GRAY                 0xD3D3D3
#define  BITMAP_GAINSBORO                  0xDCDCDC
#define  BITMAP_WHITE_SMOKE                0xF5F5F5
#define  BITMAP_WHITE                      0xFFFFFF


uint32_t Bitmap565GetWidth(uint8_t* buffer);
uint32_t Bitmap565GetHeight(uint8_t* buffer);
uint32_t Bitmap565GetFileSize(uint8_t* buffer);
uint32_t Bitmap565GetImageSize(uint8_t* buffer);
uint32_t Bitmap565GetOffset(uint8_t* buffer);


void Bitmap565InitBuf(uint8_t *buffer, uint32_t bufferSize, uint32_t width, uint32_t height);
void __inline Bitmap565SetPixel(uint8_t* buffer, uint32_t x, uint32_t y, uint16_t color);
void Bitmap565SetPixelRGB(uint8_t* buffer, uint32_t x, uint32_t y, uint8_t r, uint8_t g, uint8_t b);
void Bitmap565GetPixel(uint8_t* buffer, uint32_t x, uint32_t y, uint16_t* color);
void Bitmap565GetPixelRGB(uint8_t* buffer, uint32_t x, uint32_t y, uint8_t* r, uint8_t* g, uint8_t* b);
void Bitmap565DrawRectRGB(uint8_t* buffer, uint32_t x0, uint32_t y0, uint32_t x1, uint32_t y1, uint8_t r, uint8_t g, uint8_t b);
void Bitmap565FillRGB(uint8_t* buffer, uint8_t r, uint8_t g, uint8_t b);




#endif /* BITMAP_H_ */
