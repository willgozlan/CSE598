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
	$(CC) ${OBJS_SERVER}$< -o $@ $(CFLAGS_SERVER)

client_fifo: %: %.c
	$(CC) $< -o $@ $(CFLAGS_CLIENT) 

clean:
	$(RM) $(TARGET)
