run:
	rm -f program.exe
	g++ -g -Wall -std=c++11 application.cpp hashmap.cpp -o program.exe
	./program.exe

clean:
	rm -f program.exe

valgrind:
	valgrind --tool=memcheck --leak-check=yes ./program.exe
