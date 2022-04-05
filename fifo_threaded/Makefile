# # the compiler: gcc for C program, define as g++ for C++
# CC = gcc

#   # compiler flags:
#   #  -g    adds debugging information to the executable file
#   #  -Wall turns on most, but not all, compiler warnings
# CFLAGS  = -Wall -pthread

# # the build target executable:
TARGET = server_fifo client_fifo
# OBJS = dense_mm.c  list.c 

# all: $(TARGET)

# $(TARGET): %: %.c
# 	$(CC) $(CFLAGS) ${OBJS} $< -o $@ 

# clean:
# 	$(RM) $(TARGET)

all:
	gcc -Wall -lrt -pthread -o server_fifo list.c dense_mm.c server_fifo.c
	gcc -Wall -lrt -o client_fifo client_fifo.c

clean:
	rm server_fifo client_fifo