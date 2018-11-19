#fisier folosit pentru compilarea serverului&clientului 

all:
	gcc server.c -o server -lpthread
	gcc client.c -o client
clean:
	rm -f *~client server
