CC = g++

# compiler flags:
#  -g     - this flag adds debugging information to the executable file
#  -Wall  - this flag is used to turn on most compiler warnings
CFLAGS  = -g -Wall

TARGET = installer2hdd.cpp

all: $(TARGET)
	$(CC) $(CFLAGS) $(TARGET) -o installer2hdd