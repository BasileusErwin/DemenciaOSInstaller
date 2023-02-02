CC = g++

# compiler flags:
#  -g     - this flag adds debugging information to the executable file
#  -Wall  - this flag is used to turn on most compiler warnings
CFLAGS  = -g -Wall -lncurses -lparted -c

TARGET = demOSInstaller.cpp

all: $(TARGET)
	$(CC) $(CFLAGS) $(TARGET) -o demOSInstaller
 
no_install: $(TARGET)
	$(CC) -DNOINSTALL $(CFLAGS) $(TARGET) -o demOSInstall
