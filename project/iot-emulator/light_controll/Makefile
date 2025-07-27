CC = gcc
CFLAGS = -Wall -Werror -O2 -I./include -I../../common
SRC = src/controll_light.c src/light_utils.c          \
	  src/snprintj.c src/socket_utils.c               \
	  src/sscanj.c src/str_util.c                     \
	  ../common/crypto/encrypt.c
OBJ = $(SRC:.c=.o)

TARGET = bin/TEST

.PHONY: all clean dirs

all: dirs $(TARGET)

dirs:
	@mkdir -p ./bin

$(TARGET): $(OBJ)
	$(CC) $(CFLAGS) -o $@ $(OBJ)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -rf $(OBJ) $(TARGET) bin
