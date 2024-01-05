CXX=g++
CXXFLAGS= -g3 -gdwarf
CXXFLAGS+=-I./include/ -Wall -Wextra -Werror -std=c++20 -DDEBUG -g -O0
LDFLAGS=-g
DEPS=./include/rediscc.h ./include/resp.h ./include/srvr.h ./include/commands.h ./include/clnt.h
SRVR_OBJS=./src/srvr/rediscc.o ./src/resp.o ./src/srvr/srvr.o ./src/commands.o
CLNT_OBJS=./src/clnt/rediscccli.o ./src/resp.o ./src/commands.o ./src/clnt/clnt.o

%.o: ./src/%.cpp $(DEPS)
	$(CXX) $(CXXFLAGS) $(LDFLAGS)-c -o ./src/$@ $< $(CCFLAGS)
./srvr/%.o: ./src/srvr/%.cpp $(DEPS)
	$(CXX) $(CXXFLAGS) $(LDFLAGS) -c -o ./src/srvr/$@ $< $(CCFLAGS)
./clnt/%.o: ./src/clnt/%.cpp $(DEPS)
	$(CXX) $(CXXFLAGS) $(LDFLAGS) -c -o ./src/clnt/$@ $< $(CCFLAGS)

rediscc_srvr: $(SRVR_OBJS)
	$(CXX) $(CXXFLAGS) -o rediscc_srvr	$^	$(CXXFLAGS) $(LDFLAGS)

rediscc_clnt: $(CLNT_OBJS)
	$(CXX) $(CXXFLAGS) -o rediscc_clnt	$^	$(CXXFLAGS) $(LDFLAGS)

.PHONY: all

all: rediscc_clnt rediscc_srvr

.PHONY: clean

clean:  
	rm -f  ./src/*.o ./src/srvr/*.o ./src/clnt/*.o ./rediscc_srvr ./rediscc_clnt
