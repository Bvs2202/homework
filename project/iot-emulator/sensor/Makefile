CC      = gcc
CFLAGS = -Wall -O2

SRCS    = src/sensor_main.c  \
           src/config.c      \
           src/net_utils.c   \
           src/payload.c	 \
		   src/sscanj.c		 \
		   src/snprintj.c	 \
		   src/str_util.c    \
		   ../common/crypto/encrypt.c

OBJS    := $(SRCS:.c=.o)
TARGET  := sensor.out

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $^

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS) $(TARGET)
