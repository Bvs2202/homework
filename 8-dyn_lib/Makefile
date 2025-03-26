CC = gcc

TARGET = main
LIB = libcalc.so

SRCS = main.c add.c sub.c mul.c div.c

OBJS = $(SRCS:.c=.o)

all: $(TARGET)

$(TARGET): $(OBJS) $(LIB)
	$(CC) $(OBJS) -L. -lcalc -o $(TARGET)

$(LIB): $(OBJS)
	$(CC) -shared $(OBJS) -o $(LIB)

%.o: %.c
	$(CC) -c $< -o $@

clean:
	rm -f $(OBJS) $(TARGET) $(LIB)

.PHONY: all clean
