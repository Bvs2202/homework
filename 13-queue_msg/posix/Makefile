CC = gcc

TARGET = client_posix.out server_posix.out

all: $(TARGET)

client_posix.out: 13.1.client_posix.c
	$(CC) $< -o $@

server_posix.out: 13.1.server_posix.c
	$(CC) $< -o $@

clean:
	rm -f $(TARGET)