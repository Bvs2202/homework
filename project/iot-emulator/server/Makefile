INCLUDEDIRS := -I./include -I../common -I../common/crypto
SRCDIR := src
CRYPTODIR := ../common/crypto
OBJDIR := obj
CC := gcc
CFLAGS := -Wall -Wextra -O2 $(INCLUDEDIRS)

SERVER_SRCS := $(wildcard $(SRCDIR)/*.c)
CRYPTO_SRCS := $(wildcard $(CRYPTODIR)/*.c)
SRCS := $(SERVER_SRCS) $(CRYPTO_SRCS)

OBJS := $(patsubst $(SRCDIR)/%.c, $(OBJDIR)/%.o, $(SERVER_SRCS)) \
        $(patsubst $(CRYPTODIR)/%.c, $(OBJDIR)/%.o, $(CRYPTO_SRCS))

TARGET := server

all: $(OBJDIR) $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $^

$(OBJDIR)/%.o: $(SRCDIR)/%.c
	$(CC) $(CFLAGS) -c $< -o $@

$(OBJDIR)/%.o: $(CRYPTODIR)/%.c
	$(CC) $(CFLAGS) -c $< -o $@

$(OBJDIR):
	mkdir -p $(OBJDIR)

clean:
	rm -rf $(OBJDIR) $(TARGET)

.PHONY: all clean