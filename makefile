Proyecto: main.o proyecto.o
	g++ -std=c++0x main.o proyecto.o -o Proyecto -lncurses

main.o: proyecto.h
	g++ -std=c++0x -c -g main.cpp proyecto.cpp

proyecto.o: proyecto.h
	g++ -std=c++0x -c -g proyecto.cpp

clean:
	\rm *.o *~ Proyecto