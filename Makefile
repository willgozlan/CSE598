all:
	gcc server.c -o server -Wall
	gcc client.c -o client -Wall
	gcc example_process.c -o example_process -Wall

clean:
	rm server client example_process