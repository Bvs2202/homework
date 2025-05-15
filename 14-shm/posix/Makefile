CC = gcc

TARGET = client.out server.out

all: $(TARGET)

client.out: 14.1.client_posix.c
	$(CC) $^ -o $@

server.out: 14.1.server_posix.c
	$(CC) $^ -o $@

clean:
	rm -f $(TARGET)