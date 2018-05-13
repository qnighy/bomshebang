#!/usr/bin/make -f

CC = gcc
CFLAGS = -O2 -Wall -Wextra -g
INSTALL ?= install

all: bomshebang

clean:
	$(RM) bomshebang

install:
	$(INSTALL) -d /usr/local/bin
	$(INSTALL) -t /usr/local/bin bomshebang
	update-binfmts --install bomshebang /usr/local/bin/bomshebang --magic '\xEF\xBB\xBF#!'

uninstall:
	update-binfmts --remove bomshebang /usr/local/bin/bomshebang
	$(RM) /usr/local/bin/bomshebang

.PHONY: all clean install uninstall

bomshebang: bomshebang.o
	$(CC) $(CFLAGS) $(LDFLAGS) $^ $(LDLIBS) -o $@

.c.o:
	$(CC) $(CFLAGS) $(CPPFLAGS) -c $< -o $@
