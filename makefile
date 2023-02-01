all: main.o functions.o
	g++ -Wall -Wextra main.o functions.o -o start
	./start;

main.o: main.cpp
	g++ -Wall -Wextra -c main.cpp -o main.o

functions.o: functions.cpp functions.h
	g++ -Wall -Wextra -c functions.cpp -o functions.o

clean:
	rm start functions.o main.o