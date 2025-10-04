

#ifndef LOG_HANDLER
#define LOG_HANDLER


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

#define Max_Path_Size		448
#define Max_ProgNM_Size  	 64
#define Max_LogfileNM_Size	576
#define Main_Logging_Dir_Name	"~logging/"			// main logging directory for all logging directory structure, 
													// added to base path


class logHandler {

	public :

		logHandler(void);
		~logHandler(void);
	
		void setDebugLevel(int);
		int  setProgramName(char *);
		void putMsgStndrd(int, const char *format, ...);
		void putMsgRaw(int, const char *format, ...);
		void putMsgError(int, const char *format, ...);
		void putMsgConfig(int, const char *format, ...);

		int initLogging(char *, char *, int);


	private :

		//
		// following is key to the logging, these are determined at initialization
		//

		int     sl_debugLevel = 0x0F;           	// let's turn on default standard debug for now, unless told otherwise
		char    sl_progNM[Max_ProgNM_Size];     	//
		char    sl_path[Max_Path_Size];         	//
		char    sl_logfileNM[Max_LogfileNM_Size];	// program name + path
		
		void writeMsg(int, char *, const char *, va_list);
		void createPath(void);
};

#endif
