CC      := gcc

SRCS    := src/sensor_main.c \
           src/config.c      \
           src/net_utils.c   \
           src/payload.c

OBJS    := $(SRCS:.c=.o)
TARGET  := sensor.out

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) -o $@ $^

%.o: %.c
	$(CC) -c $< -o $@

clean:
	rm -f $(OBJS) $(TARGET)
