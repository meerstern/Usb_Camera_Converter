
#ifndef FILE_H_
#define FILE_H_

#include <stdbool.h>

void getFileName(char *filename, bool isBitmap);
int  sdMount();
void sdTest();
void saveBitmap(uint8_t *data, uint32_t datasize, char *filename);
bool saveJpeg();
bool checkJpegFile(uint8_t *data, uint32_t *datasize);


#endif