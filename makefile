all: main.o functions.o error.o
	g++ -Wall -Wextra main.o functions.o error.o -o start
	./start;

main.o: main.cpp
	g++ -Wall -Wextra -c main.cpp -o main.o

functions.o: functions.cpp functions.h
	g++ -Wall -Wextra -c functions.cpp -o functions.o

error.o: error.cpp error.h
	g++ -Wall -Wextra -c error.cpp -o error.o

clean:
	rm start functions.o main.o error.o