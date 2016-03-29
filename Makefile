CC ?= gcc
CFLAGS_commom ?= -Wall -std=gun99
CFLAGS_opt = -O0

EXEC = client server

all: $(EXEC)

client: client.o
	$(CC) $(CFLAGS_common) -o $@ $@.o

server: server.o
	$(CC) -o $@ $@.o

.PHONY: clean
clean:
	$(RM) $(EXEC) *.o
