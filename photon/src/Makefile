# C Makefile

CC = gcc
CXXFLAGS = -g -std=c++11 -Wall -I/home/oem/eigen
INCLUDE = 
LDFLAGS = 
LIBS = -lSDL2 -lstdc++ -lm
TARGET = rt
OBJS = main.o Canvas.o DimensionalTree.o Geometry.o Light.o Random.o Scene.o SceneObject.o Visualizer.o 

.PHONY: clean

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $(OBJS) $(LDFLAGS) $(LIBS)

clean:
	rm -rf $(OBJS) $(TARGET) *~

.c.o:
	$(CC) $(CFLAGS) $(INCLUDE) -c $<
