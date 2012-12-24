CFLAGS=-Ilib

all: ms

ms: main.o
	cd lib && $(MAKE)
	$(CC) -o ms main.o lib/httpd.a


clean:
	@rm *.o ms

.PHONY: all clean
