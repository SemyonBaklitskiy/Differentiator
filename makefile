all: main.o functions.o error.o build_tree.o differentiate_tree.o node_functions.o output_tree.o
	g++ -Wall -Wextra bin/main.o bin/functions.o bin/error.o bin/build_tree.o bin/differentiate_tree.o bin/node_functions.o bin/output_tree.o -o bin/start
	bin/./start;

main.o: src/main.cpp
	g++ -Wall -Wextra -I includes/ -c src/main.cpp -o bin/main.o

functions.o: src/functions.cpp includes/functions.h
	g++ -Wall -Wextra -I includes/ -c src/functions.cpp -o bin/functions.o

error.o: src/error.cpp includes/error.h
	g++ -Wall -Wextra -I includes/ -c src/error.cpp -o bin/error.o

build_tree.o: src/build_tree.cpp includes/build_tree.h
	g++ -Wall -Wextra -I includes/ -c src/build_tree.cpp -o bin/build_tree.o

differentiate_tree.o: src/differentiate_tree.cpp includes/differentiate_tree.h
	g++ -Wall -Wextra -I includes/ -c src/differentiate_tree.cpp -o bin/differentiate_tree.o

node_functions.o: src/node_functions.cpp includes/node_functions.h
	g++ -Wall -Wextra -I includes/ -c src/node_functions.cpp -o bin/node_functions.o

output_tree.o: src/output_tree.cpp includes/output_tree.h
	g++ -Wall -Wextra -I includes/ -c src/output_tree.cpp -o bin/output_tree.o

clean:
	rm bin/start bin/functions.o bin/main.o bin/build_tree.o bin/differentiate_tree.o bin/node_functions.o bin/output_tree.o bin/error.o