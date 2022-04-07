# the compiler: gcc for C program, define as g++ for C++
CC = gcc

  # compiler flags:
  #  -g    adds debugging information to the executable file
  #  -Wall turns on most, but not all, compiler warnings
CFLAGS  = -Wall

# the build target executable:
TARGET = server client

all: $(TARGET)

$(TARGET): %: %.c
	$(CC) $(CFLAGS) $< -o $@ 

clean:
	$(RM) $(TARGET)