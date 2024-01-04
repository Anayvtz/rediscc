CC=g++
CFLAGS=-I./include/ -Wall -Wextra -Werror -std=c++17
DEPS=./include/rediscc.h ./include/resp.h

%.o: ./src/%.cpp $(DEPS)
	$(CC) $(CFLAGS) -c -o ./src/$@ $< $(CFLAGS)

rediscc: ./src/rediscc.o ./src/resp.o
	$(CC) $(CFLAGS) -o rediscc ./src/rediscc.o ./src/resp.o

.PHONY: clean

clean:  
	rm -f  ./src/*.o ./rediscc
