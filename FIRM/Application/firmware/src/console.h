

#ifndef CONSOLE_H_
#define CONSOLE_H_

#include <stdbool.h>


extern SYS_CONSOLE_HANDLE  consoleHandle;
extern ssize_t             consoleReadSize;
extern char                consoleBuffer[100];

void ConsoleReadTask();
void ConsoleDispInit();
void ConsoleDispInfo();
void ConsoleCheckInterval(bool *trig);
void ConsoleDispHelp();
void ConsoleCheckAlternateInterface();
void ConsoleCheckDataFrameInterval();
void ConsoleDispStreamSetting();

#endif