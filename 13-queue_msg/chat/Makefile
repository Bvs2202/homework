CC = gcc

LDFLAGS = -lncurses

TARGET = client.out server.out

all: $(TARGET)

client.out: client.c
	$(CC) -o $@ $^ $(LDFLAGS)

server.out: server.c
	$(CC) -o $@ $^

clean:
	rm -f *$(TARGET)