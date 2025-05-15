CC = gcc 

TARGET = event_loop.out sig_send.out

all: $(TARGET)

event_loop.out: event_loop.c
	$(CC) $^ -o $@

sig_send.out: sig_send.c
	$(CC) $^ -o $@

clean:
	rm -f $(TARGET)