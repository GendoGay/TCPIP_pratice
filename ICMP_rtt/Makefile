CC ?= gcc
CFLAGS ?= -Wall -g

EXEC = ICMP_rtt

all: $(EXEC)

ICMP_rtt: ICMP_rtt.o
	$(CC) $(CFLAGS) -o $@ $@.o

.PHONY: clean
clean:
	$(RM) $(EXEC) *.o
