CC = gcc

LDFLAGS = -lncurses

TARGET = server.out client.out

all: $(TARGET)

client.out: client.c
	$(CC) $^ -o $@ $(LDFLAGS)

server.out: server.c
	$(CC) $^ -o $@

clean:
	rm -f $(TARGET)
