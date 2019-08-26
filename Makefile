CC=g++ -o chs

OGL=-lGLEW -lGL
SDL=-lSDL2

F=main.cpp obj_loader.cpp mesh.cpp

make: main.cpp
	$(CC) $(F) $(OGL) $(SDL)
