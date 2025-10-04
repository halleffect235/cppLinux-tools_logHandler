/*
 ------------------------------------------------------------------------------------------------------------------------------
Class logHandler

    This software was developed and is maitained by :

        J.P. Langan
        13186 15th Street South
        Afton, MN 55001

        Support inquiries to softw-dev@dyn-eng.com

Copyright September 6, 2025 - jpl

-------------------------------------------------------------------------------------------------------------------------------

Operating Systems Supported:

    POP-OS                      tested
    Oracle versions 3 - 9.5     tested
    Debian                      believed to work but not tested
    Ubuntu                      beleived to work but not tested

-------------------------------------------------------------------------------------------------------------------------------

This class when initialized creates a directory structure for sorted program logging
The base directory is "~logging" with four child directories
    msgStndrd       - 15 unique record type indicators, 0000 000F
    msgError        - 15 unique record type indicators, 0000 00F0
    msgRaw          - 15 unique record type indicators, 0000 0F00
    msgConfig       - 15 unique record type indicators, 0000 F000

    msgSpares       - FFFF 0000 Spares unused

Each sub directory will have files beginning with the program name, and year/month/day format for linux
    directory sorting, file name examples

        myProgram_msgStndrd-20250905.log
        myProgram_msgRaw-20250905.log
        myProgram_msgConfig-20250905.log
        myProgram_msgError-20250906.log


 Each file has records beginning with a time stamp followed by a text string from the program creating the record
    including timestamp, nano-seconds of record, debugLevel of record, record data from operating program,
    file contents are sequential in time, content examples

    07:29:38:275823099|001|   New Debug Level 0000000Fh

 ------------------------------------------------------------------------------------------------------------------------------
Key Variables

    ProgramName:    size, 50 characters, program name of calling program for directed data, up to 64 characters
    Path:           path for directory structure of logged data, paths may be up to 256 bytes
    Debug Level:    debug level in four bit chunks for both file direction and some screening for data into log files

 ------------------------------------------------------------------------------------------------------------------------------
Debug Level, each debug level is four bits of int_32

    0000000F - Any bits, data directed to "path/!logging/msgStndrd/progname
    000000F0 - Any bits, data directed to "path/!logging/msgError/progname
    00000F00 - any bits, data directed to "path/!logging/MsgRaw/progname
    0000F000 - any bits, data directed to "path/!logging/MsgConfig/progname
    FFFF0000 - any bits, reserved for future applications
-------------------------------------------------------------------------------------------------------------------------------
Revision History

    2025-09-04 - jpl    : assigned version 1.1
                        : file created and uploaded from working project
                        : Added cpp header file, created linkable library

-------------------------------------------------------------------------------------------------------------------------------
 */

#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include <stddef.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <time.h>

#include <ctype.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <dirent.h>
#include <libgen.h>
#include <sys/stat.h>           // For mkdir
#include <iostream>

#include "logHandler.h"

int     sl_debugLevel = 0x0F;               // let's turn on default standard debug for now, unless told otherwise
char    sl_progNM[Max_ProgNM_Size];         //
char    sl_path[Max_Path_Size];             //
char    sl_logfileNM[Max_LogfileNM_Size];   // program name + path

//
// prototypes for files in this class


logHandler::logHandler(void) {

    strcpy(sl_progNM, "");
    strcpy(sl_path, "");
    strcpy(sl_logfileNM, "");

    sl_debugLevel = 0x0F;
}


logHandler::~logHandler(void) {

}


void logHandler::setDebugLevel(int dbl) {
    sl_debugLevel = dbl;
}

int logHandler::setProgramName(char *NM) {

    if(NM == NULL) {return(-1);}                                        // called with no string return error

    if(strlen(NM) == 0) {return(-2);}                                   // program name is blank get out with error
    if(strlen(NM) > Max_ProgNM_Size) {NM[Max_ProgNM_Size] = 0;}         // program name too long truncate and use it
    strcpy(sl_progNM, NM);
    return(0);
}

void logHandler::putMsgStndrd(int debugLevel, const char *format, ...) {

    va_list argList;

    if(! ((debugLevel & 0x0F) && sl_debugLevel)) {return;}

    va_start(argList, format);
    writeMsg(debugLevel, (char *) "msgStndrd", format, argList);
    va_end(argList);
}


void logHandler::putMsgError(int debugLevel, const char *format, ...) {

    va_list argList;

    if(! (((debugLevel << 4) & 0xF0) && sl_debugLevel)) {return;}

    va_start(argList, format);
    writeMsg(debugLevel, (char *) "msgError", format, argList);
    va_end(argList);
}


void logHandler::putMsgRaw(int debugLevel, const char *format, ...) {

    va_list argList;

    if(! (((debugLevel << 8) & 0xF00) && sl_debugLevel)) {return;}

    va_start(argList, format);
    writeMsg(debugLevel, (char *) "msgRaw", format, argList);
    va_end(argList);
}


void logHandler::putMsgConfig(int debugLevel, const char *format, ...) {

    va_list argList;

    if(! (((debugLevel << 12) & 0xF000) && sl_debugLevel)) {return;}

    va_start(argList, format);
    writeMsg(debugLevel, (char *) "msgConfig", format, argList);
    va_end(argList);
}

//
// create directories along path as necessary
//

void logHandler::createPath(void) {

    int i;
    int sLen;

    char tmpStr[Max_Path_Size];
    char chompPath[Max_Path_Size];
    sLen = strlen(sl_path);

    strcpy(tmpStr, sl_path);

    //
    //
    // parse each "/" create directory as necessary

    for(i = 0; i < sLen; i++) {

        if(tmpStr[i] == '/' && i > 0) {
            strncpy(chompPath, tmpStr, i);
            chompPath[i] = 0;
            mkdir (chompPath, 0755);
            //printf (chompPath);
        }

    }

    //
    // now we have root logging directory, we need to add, the following directoies
    //  msgError
    //  msgStndrd
    //  msgConfig
    //  msgRaw

    //
    //
    // create the depository directories for each of the four file type groupings

    strcpy(tmpStr, sl_path);
    mkdir(strcat(tmpStr, "msgStndrd"), 0755);
    strcpy(tmpStr, sl_path);
    mkdir(strcat(tmpStr, "msgError"), 0755);
    strcpy(tmpStr, sl_path);
    mkdir(strcat(tmpStr, "msgConfig"), 0755);
    strcpy(tmpStr, sl_path);
    mkdir(strcat(tmpStr, "msgRaw"), 0755);

}


//
//
// initialize the logging system with the program name, base path for logging directory file structure and the debug level
//

int logHandler::initLogging(char *progNM, char *path, int debugLevel) {

    int  sLen;
    char tmp[Max_Path_Size];

//
// test the path for validity, length, and presence

    if(path == NULL ) {return(-2);}                                         // validate path pointer not NULL
    if(strlen(path) > Max_Path_Size || strlen(path) <= 0) {return(-2);}     // validate path is within reasonable range

    strcpy(tmp, path);

    sLen = strlen(tmp); if(tmp[sLen - 1] != '/') {strcat(tmp, "/");}       // make certain last character of path is a "/"
    strcat(tmp, Main_Logging_Dir_Name);
    sLen = strlen(tmp); if(tmp[sLen - 1] != '/') {strcat(tmp, "/");}       // make certain last character of path is a "/"

    //
    // out base path is now complete in tmp character array

    strcpy(sl_path, tmp);                                                   // copy our completed path for creation if it doesn't exist

    createPath();                                                           // create our full path, as necessary

    //
    // here we test our path, and make certain program has access and file priviledges

//
// Test the program name of the calling program for validity, length and persence

    if(progNM != NULL) {
        if(strlen(progNM) == 0) {return(-1);}                               // program name is blank get out with error
        if(strlen(progNM) > Max_ProgNM_Size) {progNM[Max_ProgNM_Size] = 0;} // program name too long truncate and use it
        strcpy(sl_progNM, progNM);

    } else {
        return(-1);                                     // program name is null, return error quickly
    }

    if(debugLevel >= 0) {                               // update debug level, if less than zero x00000000F is default
        sl_debugLevel = debugLevel;
        putMsgStndrd(1, "New Debug Level %.8Xh", sl_debugLevel);
    }

    return(0);
}

//
// message handler to write the programs data record, create time stamp, check record lengths to prevent overflow
//  of data, and verify basics, open file for appending data, before sending to the file stream.
//
void logHandler::writeMsg(int debugLevel, char *recType, const char *format, va_list argList) {

    char recToWrite[20 * 1024];
    char tpRecBuff[10240];
    char tmp[30];
    FILE *debug_fp;

    recToWrite[0] = 0;
    tmp[0] = 0;
    sl_logfileNM[0] = 0;

    int totalSz = 0;

    time_t now;
    struct tm *timePtr;
    struct timespec msgTime;
    now = time(NULL);
    timePtr = localtime(&now);
    clock_gettime(CLOCK_REALTIME, &msgTime);

    //
    // create our filename for this message

    sprintf(tmp, "-%.4d%.2d%.2d", timePtr->tm_year + 1900, timePtr->tm_mon + 1, timePtr->tm_mday);

    totalSz = strlen(tmp);
    totalSz += strlen(recType) * 2;
    totalSz += strlen(sl_progNM);
    totalSz += strlen(sl_path);
    if(totalSz < Max_LogfileNM_Size) {sprintf(sl_logfileNM, "%s%s/%s_%s%s.log", sl_path, recType, sl_progNM, recType, tmp);}
    else {return;}

    debug_fp = fopen(sl_logfileNM, "a+");

    //
    // check if file exists, es, ok to write

    if(debug_fp != NULL) {

        sprintf(recToWrite, "%.2d:%.2d:%.2d:%.9lu|%.2d|   ", timePtr->tm_hour, timePtr->tm_min, timePtr->tm_sec, msgTime.tv_nsec, debugLevel);

        vsnprintf(tpRecBuff, sizeof(tpRecBuff), format, argList);

        fprintf(debug_fp, "%s", recToWrite);
        fprintf(debug_fp, "%s\n", tpRecBuff);       // write data to file stream
        fflush(debug_fp);                           // flush the buffers to the data file
        fclose(debug_fp);                           // close the file and return to calling program as quickly as possible
    }
}






