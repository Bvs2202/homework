CC = gcc

TARGET = client_udp.out client_tcp.out server.out

all: $(TARGET)

client_udp.out: client_udp.c
	$(CC) $^ -o $@

client_tcp.out: client_tcp.c
	$(CC) $^ -o $@

server.out: server.c
	$(CC) $^ -o $@

clean:
	rm -f $(TARGET)