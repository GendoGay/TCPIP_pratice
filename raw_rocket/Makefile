CC ?= gcc
CFLAGS_commom ?= -Wall -std=gun99
CFLAGS_opt = -O0

EXEC = raw_socket

all: $(EXEC)

raw_socket: raw_socket.o
	$(CC) $(CFLAGS_common) -o $@ $@.o

.PHONY: clean
clean:
	$(RM) $(EXEC) *.o
