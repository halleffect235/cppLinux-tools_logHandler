/*
 Simple test program for the logHandler library

2025 September 6, version 1.1

J.P Langan
inquiries to softw-dev@dyn-eng.com

*/


#include "logHandler.h"



int main(void) {

    logHandler logs;

    //logs.initLogging((char *) "jpl", (char *) "/home/dev-Softw", 15);

        // change directory root from "/home/dev-Softw/" to a directory on your hard drive the directory "~logging" will be created with subdirectories:
        //     msgConfig
        //     msgError
        //     msgRaw
        //     msgStrndrd

    logs.initLogging((char *) "yourProgramName", (char *) "/home/dev-Softw/", 0x0000FFFF);
    logs.putMsgStndrd( 1, "this is a data record");
    logs.putMsgError(  1, "this is a data record");
    logs.putMsgRaw(    1, "this is a data record");
    logs.putMsgConfig( 1, "this is a data record");






    return(0);

}



