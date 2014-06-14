COPTS = -Wall -Wextra -std=gnu11 -pthread -ggdb -D_GNU_SOURCE
LDOPTS = -lrt -DCURL_STATICLIB -lz
.PHONY: all clean

all: svr_s svr_c

svr_s: cola.o server.o evento.o curl/lib/.libs/libcurl.a; gcc $(COPTS) -o $@ $^ $(LDOPTS)

svr_c: client.o evento.o; gcc $(COPTS) -o $@ $^ $(LDOPTS)

%.o: %.c; gcc $(COPTS) -c $<

curl/lib/.libs/libcurl.a: ; (cd curl && ./buildconf && ./configure && make)

clean: ; rm -f ./*.o svr_s svr_c
