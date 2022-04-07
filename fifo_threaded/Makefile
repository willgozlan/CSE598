# the compiler: gcc for C program
CC = gcc

# compiler flags, for server and client:
CFLAGS_SERVER  = -Wall -lrt -pthread 
CFLAGS_CLIENT  = -Wall -lrt

# the build target executable:
TARGET = server_fifo client_fifo

# required objects for server
OBJS_SERVER =  list.c dense_mm.c  

all: $(TARGET)

server_fifo: %: %.c
	$(CC) $(CFLAGS_SERVER) ${OBJS_SERVER} $< -o $@ 

client_fifo: %: %.c
	$(CC) $(CFLAGS_CLIENT) $< -o $@ 

clean:
	$(RM) $(TARGET)
