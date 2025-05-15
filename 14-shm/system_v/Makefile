CC = gcc

TARGET = client.out server.out

all: $(TARGET)

client.out: 14.1.client_sys_v.c
	$(CC) $< -o $@

server.out: 14.1.server_sys_v.c
	$(CC) $< -o $@

clean:
	rm -f $(TARGET)
