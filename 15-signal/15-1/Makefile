CC = gcc

TARGET = sig_rec.out sig_send.out

all: $(TARGET)

sig_rec.out: sig_rec.c
	$(CC) $^ -o $@

sig_send.out: sig_send.c
	$(CC) $^ -o $@

clean:
	rm -f $(TARGET)

