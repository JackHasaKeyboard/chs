CC=g++ -o chs

OGL=-lGLEW -lGL
SDL=-lSDL2

F=main.cpp

make: main.cpp
	$(CC) $(F) $(OGL) $(SDL)
