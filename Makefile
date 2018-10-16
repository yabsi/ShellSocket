all:
	make server && make client
server:
	gcc server.c -o server.o
client:
	gcc client.c -o client.o
clean:
	rm -f client.o server.o
