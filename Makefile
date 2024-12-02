#
# File: Makefile
#

CC = g++
CFLAGS = -lsimlib -Wall -Wextra -pedantic -std=c++11 -Werror -pedantic-errors
SRC1 = main.cpp
SRC2 = experiment_1.cpp
SRC3 = experiment_2.cpp
SRC4 = experiment_3.cpp

all: main experiment1 experiment2 experiment3

main: main.cpp
	$(CC) $(SRC1) -o main $(CFLAGS)

experiment1: experiment_1.cpp
	$(CC) $(SRC2) -o experiment1 $(CFLAGS)

experiment2: experiment_2.cpp
	$(CC) $(SRC3) -o experiment2 $(CFLAGS)

experiment3: experiment_3.cpp
	$(CC) $(SRC4) -o experiment3 $(CFLAGS)

run: main experiment1 experiment2 experiment3
	./main
	./experiment1
	./experiment2
	./experiment3

clean:
	rm -f main experiment1 experiment2 experiment3