CC = g++
CXXFLAGS = -Wall -Wextra -O2 -g -std=c++11 

obj = glhelper.o mesh.o main.o camera.o 

opengl: $(obj)
	$(CC) $^ -o $@ -lglut -lGLEW -lGL -pthread -L/usr/lib/nvidia-384

%.o: %.c
	$(CC)  -o $@ -c $<

clean:
	rm -f *.o 

mrproper: clean
	rm opengl
