CC = gcc
AR = ar
RCS = rcs

TARGET = main
LIB = libcalc.a

SRCS = main.c add.c sub.c mul.c div.c

OBJS = $(SRCS:.c=.o)

all: $(TARGET)

$(TARGET): $(OBJS) $(LIB)
	$(CC) $(OBJS) -L. -lcalc -o $(TARGET)

$(LIB): $(OBJS)
	$(AR) $(RCS) $(LIB) $(OBJS)

%.o: %.c
	$(CC) -c $< -o $@

clean:
	rm -f $(OBJS) $(TARGET) $(LIB)

.PHONY: all clean
