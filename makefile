all: main.o functions.o error.o build_tree.o differentiate_tree.o node_functions.o output_tree.o
	g++ bin/main.o bin/functions.o bin/error.o bin/build_tree.o bin/differentiate_tree.o bin/node_functions.o bin/output_tree.o -o bin/start
	bin/./start

dump: main_dump.o functions.o error.o build_tree.o differentiate_tree.o node_functions.o output_tree.o
	g++ bin/main_dump.o bin/functions.o bin/error.o bin/build_tree.o bin/differentiate_tree.o bin/node_functions.o bin/output_tree.o -o bin/start
	bin/./start
	dot -Tpng tree.gv -o tree.png
	dot -Tpng diff_tree.gv -o diff_tree.png

bin:
	mkdir bin

main_dump.o: src/main.cpp bin
	g++ -Wall -Wextra -D DUMP -I includes/ -c src/main.cpp -o bin/main_dump.o

main.o: src/main.cpp bin
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
	g++ -Wall -Wextra -Wno-format-zero-length -I includes/ -c src/output_tree.cpp -o bin/output_tree.o

clean:
	rm bin/start bin/functions.o bin/main.o bin/build_tree.o bin/differentiate_tree.o bin/node_functions.o bin/output_tree.o bin/error.o

clean_dump:
	rm bin/start bin/functions.o bin/main_dump.o bin/build_tree.o bin/differentiate_tree.o bin/node_functions.o bin/output_tree.o bin/error.o diff_tree.gv diff_tree.png tree.gv tree.png