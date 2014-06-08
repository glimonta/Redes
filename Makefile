COPTS = -Wall -Wextra
.PHONY: all clean

all: svr_s svr_c

svr_s: server.o; gcc $(COPTS) -o $@ $<

svr_c: client.o; gcc $(COPTS) -o $@ $<

%.o: %.c; gcc -c $<

clean: ; rm -f ./*.o svr_s svr_c
