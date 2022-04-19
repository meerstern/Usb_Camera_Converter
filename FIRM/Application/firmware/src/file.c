

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include "system/debug/sys_debug.h"
#include "system/fs/sys_fs.h"
#include "file.h"
#include "jpeg.h"
#include "rtc_rv8803.h"

SYS_FS_HANDLE   fileHandle;
char Hellow[] = "Hellow World !!";
int N_Write;
uint8_t last_year, last_month, last_day, last_hour, last_min, last_sec, last_no;

//File Name 210918_181012_1.jpg
//Folder    210918

void getFileName(char *filename, bool isBitmap)
{
    
    static char fileNameOld[25]={'\0'};
        
//    GetRtcDate(&last_year, &last_month, &last_day);
//    GetRtcTime(&last_hour, &last_min, &last_sec);
    
    GetRtcDateTime(&last_year, &last_month, &last_day,
                    &last_hour, &last_min, &last_sec);

    if(isBitmap==true)
    {
        sprintf(filename, "%02d%02d%02d_%02d%02d%02d_%d.bmp",
            last_year, last_month, last_day, last_hour, last_min, last_sec, last_no); 
    }
    else
    {
        sprintf(filename, "%02d%02d%02d_%02d%02d%02d_%d.jpg",
            last_year, last_month, last_day, last_hour, last_min, last_sec, last_no); 
    }

    if(strcmp(filename, fileNameOld)==0)
        last_no=last_no+1;
    else
        last_no=0;
    strcpy(fileNameOld, filename);    
}

void sdMakeDir(char *dirname)
{
    SYS_FS_DirectoryMake(dirname);
}

void sdChangeDir(char *dirname)
{
    SYS_FS_DirectoryChange(dirname);
}


void checkDir()
{
    static char dirNameOld[25]={'\0'};    
    char dirName[25]={'\0'};
    
    sprintf(dirName, "%02d%02d%02d", last_year, last_month, last_day);
    
    if( strncmp(dirName, dirNameOld, 6) != 0)
    {
        sdChangeDir("/");
        sdMakeDir(dirName);
        sdChangeDir(dirName);
        strcpy(dirNameOld, dirName);
    }
    

}

void sdSetFileTime(char *filename)
{
    SYS_FS_TIME time;

    time.packedTime = 0;
    time.discreteTime.year = (2000 + last_year - 1980);  // Year
    time.discreteTime.month = last_month;            // Month
    time.discreteTime.day = last_day;               // Day
    time.discreteTime.hour = last_hour;             // hour
    time.discreteTime.minute = last_min;           // minutes
    time.discreteTime.second = last_sec;           // seconds

    SYS_FS_FileDirectoryTimeSet(filename, &time);
}

int sdMount()
{
    int res;
    res=SYS_FS_Mount("/dev/mmcblka1", "/mnt/myDrive1", FAT, 0, NULL);

    if(res!= 0)
    {
        //SYS_DEBUG_PRINT(SYS_ERROR_DEBUG, "ME:%d\r\n",res);
        return res;
    }
    else
    {
        SYS_DEBUG_PRINT(SYS_ERROR_WARNING, "->TF Mount OK\r\n");
    }
    return res;
}

void sdTest()
{
    fileHandle = SYS_FS_FileOpen("myTestData.txt", (SYS_FS_FILE_OPEN_APPEND_PLUS));            
    if(fileHandle == SYS_FS_HANDLE_INVALID)
    {
        SYS_DEBUG_PRINT(SYS_ERROR_WARNING, "FileOpen Error\r\n");
        return;
    }

    N_Write = SYS_FS_FileWrite(fileHandle, Hellow, sizeof(Hellow));
    if(N_Write != -1) //OK
    {
        SYS_DEBUG_PRINT(SYS_ERROR_WARNING, "FileWrite OK\r\n");
    }
    else//NG
    {
        SYS_DEBUG_PRINT(SYS_ERROR_WARNING, "FileWrite NG\r\n");
        return;
    }

    SYS_FS_FileClose(fileHandle);
    SYS_DEBUG_PRINT(SYS_ERROR_WARNING, "FileClosed\r\n");

    return;

}

void saveBitmap(uint8_t *data, uint32_t datasize, char *filename)
{

    checkDir();
    
    fileHandle = SYS_FS_FileOpen(filename, (SYS_FS_FILE_OPEN_WRITE));            
    if(fileHandle == SYS_FS_HANDLE_INVALID)
    {
        SYS_DEBUG_PRINT(SYS_ERROR_WARNING, "FileOpen Error\r\n");
        return;
    }
    
    N_Write = SYS_FS_FileWrite(fileHandle, data, datasize);
    if(N_Write != -1) //OK
    {
        SYS_DEBUG_PRINT(SYS_ERROR_WARNING, "File:%s Size:%d\r\n",filename,datasize);
    }
    else//NG
    {
        SYS_DEBUG_PRINT(SYS_ERROR_WARNING, "FileWrite NG\r\n");
        return;
    }

    SYS_FS_FileClose(fileHandle);
    //SYS_DEBUG_PRINT(SYS_ERROR_WARNING, "FileClosed\r\n");
    
    if(N_Write != -1) //OK
    {
        //sdSetFileTime(filename);
    }
    
    return;

}

bool checkJpegFile(uint8_t *data, uint32_t *datasize)
{
    uint32_t i;
    bool hasDHT=false;
    bool hasSOS=false;
    uint32_t indexSOS=0;
    bool result=false;
    
    uint8_t *offdata=data;
    uint8_t off_old=0;
    for(i=0;i<*datasize;i++)
    {
        uint8_t df=*(offdata+i);
        if(off_old==0xFF && df==0xC4 && hasDHT==false)
            hasDHT=true;        
      
        if(off_old==0xFF && df==0xDA && hasSOS==false)
        {
            indexSOS=i-1;
            hasSOS=true;
        }
  
        off_old=df;
    }    
    
    if(hasSOS!=true)
    {
        SYS_DEBUG_PRINT(SYS_ERROR_WARNING, "No SOS Marking!\r\n");
        return result;
    }

    //hasDHT=true;
    if(hasDHT==false)
    {
        //SYS_DEBUG_PRINT(SYS_ERROR_WARNING, " Add DHT ");        
        memmove(&data[indexSOS+sizeof(huffmanTable)], &data[indexSOS],*datasize-indexSOS);
        memcpy(&data[indexSOS], huffmanTable,sizeof(huffmanTable));
        *datasize=*datasize+sizeof(huffmanTable);
    }
    result=true;
    return result;

}

bool saveJpeg(uint8_t *data, uint32_t datasize, char *filename)
{
    
    bool result=false;
    bool res=false;    
    
    res=checkJpegFile(data, &datasize);
    
    if(res==false)
        return res;
  
    checkDir();        
    fileHandle = SYS_FS_FileOpen(filename, (SYS_FS_FILE_OPEN_WRITE));            
    if(fileHandle == SYS_FS_HANDLE_INVALID)
    {
        SYS_DEBUG_PRINT(SYS_ERROR_WARNING, "FileOpen Error\r\n");
        SYS_FS_FileClose(fileHandle);
        return result;
    }
        
        
    N_Write = SYS_FS_FileWrite(fileHandle, data, datasize  );
    
         
    if(N_Write != -1) //OK
    {
        //SYS_DEBUG_PRINT(SYS_ERROR_WARNING, "FileWrite OK:%lu\r\n",datasize+206);
        SYS_DEBUG_PRINT(SYS_ERROR_WARNING, "File:%s Size:%d\r\n",filename,datasize);
    }
    else//NG
    {
        SYS_DEBUG_PRINT(SYS_ERROR_WARNING, "FileWrite NG\r\n");
        return result;
    }

    SYS_FS_FileClose(fileHandle);
    
    result=true;
    
    return result;

}