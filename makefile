
all: test_logHandler logHandler.o test_logHandler.o

CXX = g++
CFLAGS = -c -Wall 


logHandler.o 		: logHandler.cpp logHandler.h
		 	   $(CXX) $(CFLAGS) logHandler.cpp 

test_logHandler.o	: test_logHandler.cpp logHandler.h
			   $(CXX) $(CFLAGS) test_logHandler.cpp

test_logHandler 	: test_logHandler.o logHandler.o
			   $(CXX) -o test_logHandler test_logHandler.o   logHandler.o





