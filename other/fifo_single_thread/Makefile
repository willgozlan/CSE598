# the compiler: gcc for C program, define as g++ for C++
#CC = gcc

  # compiler flags:
  #  -g    adds debugging information to the executable file
  #  -Wall turns on most, but not all, compiler warnings
#CFLAGS  = -Wall

# the build target executable:
#TARGET = server client
#OBJS = dense_mm.c  

#all: $(TARGET)

#$(TARGET): %: %.c
#	$(CC) $(CFLAGS) ${OBJS} $< -o $@ 

#clean:
#	$(RM) $(TARGET)

all:
#	gcc -Wall -o server server.c
#	gcc -Wall dense_mm.c -o client client.c
	gcc -Wall -o server_fifo dense_mm.c server_fifo.c
	gcc -Wall -o client_fifo client_fifo.c

clean:
	rm server_fifo client_fifo
#	rm client server server_fifo client_fifo