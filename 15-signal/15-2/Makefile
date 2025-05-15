CC = gcc 

TARGET = send.out sigproc.out

all: $(TARGET)

send.out: send_SIGINT.c
	$(CC) $^ -o $@

sigproc.out: sigproc.c
	$(CC) $^ -o $@

clean:
	rm -f $(TARGET)