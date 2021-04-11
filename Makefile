CC = g++
STD = -std=c++17 -Werror -Wall
INC = -I./
CXXFLAGS = ${STD} ${INC}
EXEC = main
SRC = main.cpp
SRCSINGLE = main_single.cpp
EXECSINGLE = main_single

debug:
	$(CC) $(CXXFLAGS) -g -o $(EXEC) $(SRC)
release:
	$(CC) $(CXXFLAGS) -o $(EXEC) -DRELEASE $(SRC)
oj:
	$(CC) $(CXXFLAGS) -o $(EXEC) -DONLINE_JUDGE $(SRC)
ojsingle:
	$(CC) $(CXXFLAGS) -g -o $(EXECSINGLE) $(SRCSINGLE)