CC ?= gcc
CFLAGS_commom ?= -Wall -g
CFLAGS_opt = -O0

EXEC = raw_socket udp_cap tcp_cap

all: $(EXEC)

raw_socket: raw_socket.o
	$(CC) $(CFLAGS_common) -o $@ $@.o

udp_cap: udp_cap.o
	$(CC) $(CFLAGS_common) -o $@ $@.o

tdp_cap: tdp_cap.o
	$(CC) $(CFLAGS_common) -o $@ $@.o

.PHONY: clean
clean:
	$(RM) $(EXEC) *.o
