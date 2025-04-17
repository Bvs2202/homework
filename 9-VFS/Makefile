CC = gcc

LDFLAGS = -lncurses

TARGET = 9.1.out 9.2.out

all: $(TARGET)

9.1.out: 9.1.c
	$(CC) $< -o $@

9.2.out: 9.2.c
	$(CC) $< -o $@ $(LDFLAGS)

.PHONY: clean

clean:
	rm -f $(TARGET)
