CC      ?= gcc
CFLAGS  += -Wall -O2 $(shell pkg-config --cflags libxml-2.0)
LDLIBS  += -lwebsockets $(shell pkg-config --libs libxml-2.0)

SRC     = main.c apis.c web_socket.c
OBJ     = $(SRC:.c=.o)
BIN     = ws_server

# DESTDIR is provided by Yocto during the install phase
bindir  ?= /usr/bin
sysconfdir ?= /etc

.PHONY: all clean install

all: $(BIN)

$(BIN): $(OBJ)
	$(CC) $(CFLAGS) $(LDFLAGS) -o $(BIN) $(OBJ) $(LDLIBS)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

# Standard GNU install layout (No sudo, uses DESTDIR)
install: $(BIN)
	install -d $(DESTDIR)$(bindir)
	install -m 0755 $(BIN) $(DESTDIR)$(bindir)

clean:
	rm -f $(BIN) $(OBJ)
