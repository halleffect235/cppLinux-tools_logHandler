/*
 Simple test program for the logHandler library

2025 September 6, version 1.1

J.P Langan
inquiries to softw-dev@dyn-eng.com

*/


#include "logHandler.h"



int main(void) {

    logHandler logs;

    //logs.initLogging((char *) "jpl", (char *) "/home/dev-software/", 15);

    logs.initLogging((char *) "jpl", (char *) "/home/dev-software/", 0x0000FFFF);
    logs.putMsgStndrd( 1, "this is a data record");
    logs.putMsgError(  1, "this is a data record");
    logs.putMsgRaw(    1, "this is a data record");
    logs.putMsgConfig( 1, "this is a data record");






    return(0);

}



