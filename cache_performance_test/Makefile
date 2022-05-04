CC = gcc

CFLAGS  = -Wall 


TARGET = dense_mm cache_optimal_dense_mm


all: $(TARGET)


$(TARGET): %: %.c
	$(CC) $< -o $@ $(CFLAGS) 

clean:
	$(RM) $(TARGET)