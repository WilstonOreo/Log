CXX      = /usr/bin/g++
CFLAGS  = -Os -Wall 
LDFLAGS = -lboost_thread -lrt

PRG = logtest
SRC = $(PRG).cpp

$(PRG): ./log/Log.hpp $(SRC)
		$(CXX) -o $(PRG) $(CFLAGS) $(SRC) $(LDFLAGS)

.PHONY : clean
 	clean :
		rm $(PRG) $(PRG).o
