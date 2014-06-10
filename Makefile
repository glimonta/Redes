COPTS = -Wall -Wextra -std=gnu11 -pthread -ggdb
.PHONY: all clean

all: svr_s svr_c

svr_s: cola.o server.o evento.o; gcc $(COPTS) -o $@ $^

svr_c: client.o evento.o; gcc $(COPTS) -o $@ $<

%.o: %.c; gcc $(COPTS) -c $<

clean: ; rm -f ./*.o svr_s svr_c
