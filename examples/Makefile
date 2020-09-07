# the compiler to use
CC      = gcc
CFLAGS  = -march=native -g

#header
header_path = ../include/external/
INC         = -I$(header_path)

#path to libalm.so
lib_path = ../build/aocl-release/src/
LFLAGS   = -L$(lib_path) -lalm -lm

#src path
SOURCES = ./src/*.c

#exe file name
TARGET = ./test_libm

all: $(TARGET)

$(TARGET): $(SOURCE)
	$(CC) $(CFLAGS) $(INC) -o $(TARGET) $(SOURCES) $(LFLAGS)

clean:
	rm -rf $(TARGET)
