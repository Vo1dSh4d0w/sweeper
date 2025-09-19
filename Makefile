PKG_CFLAGS=$(shell pkg-config --cflags ncurses)
PKG_LDFLAGS=$(shell pkg-config --libs ncurses)

CLFAGS?=-Wall -O2
CFLAGS+= $(PKG_CFLAGS)
LDFLAGS?=
LDFLAGS+= $(PKG_LDFLAGS)

OBJS=\
src/color.o \
src/input_handler.o \
src/main.o \
src/menu.o

.SUFFIXES: .o .c

all: out/sweeper

out/sweeper: $(OBJS)
	mkdir -p out
	$(CC) $(OBJS) -o $@ $(LDFLAGS)

.c.o:
	$(CC) -c $< -o $@ $(CFLAGS)

clean:
	rm -rf out
	rm $(OBJS)
