CC = gcc

TARGET = client.out server.out

all: $(TARGET)

client.out: client.c
	$(CC) $^ -o $@

server.out: server.c
	$(CC) $^ -o $@

clean:
	rm -f $(TARGET)