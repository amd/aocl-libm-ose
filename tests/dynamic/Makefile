# the compiler to use
CC = gcc
CFLAGS  = -Wall -g

#header
INC=-I./inc

#files to link:
LFLAGS = -lm -mavx2 -ldl

SOURCES = ./src/*.c

#exe file name
TARGET = ./exe

all: $(TARGET)

$(TARGET): $(SOURCE)
	$(CC) $(CFLAGS) -o $(TARGET) $(SOURCES) $(INC) $(LFLAGS)

clean:
	rm -rf $(TARGET)
