CXX=g++
CXXFLAGS= -g3 -gdwarf
#CXXFLAGS= -g3 -gdwarf -D__GLIBCXX_USE_CXX11_ABI=0
CXXFLAGS+=-I./include/ -I./include/srvr/ -I./include/clnt/ -Wall -Wextra -Werror -std=c++20 -DDEBUG -g -O0
LDFLAGS=-g
SRVR_OBJS=./src/srvr/rediscc.o ./src/resp.o ./src/srvr/srvr.o ./src/commands.o ./src/srvr/commands_srvr.o ./src/srvr/memdb.o ./src/srvr/timer_wheel.o
CLNT_OBJS=./src/clnt/rediscccli.o ./src/resp.o ./src/commands.o ./src/clnt/commands_clnt.o ./src/clnt/clnt.o

rediscc_srvr: $(SRVR_OBJS)
	$(CXX) $(CXXFLAGS) -o rediscc_srvr	$^	$(LDFLAGS)

rediscc_clnt: $(CLNT_OBJS)
	$(CXX) $(CXXFLAGS) -o rediscc_clnt	$^	$(LDFLAGS)

.PHONY: all

all: rediscc_clnt rediscc_srvr

.PHONY: clean

clean:  
	rm -f  /tmp/core* core ./src/*.o ./src/srvr/*.o ./src/clnt/*.o ./rediscc_srvr ./rediscc_clnt
