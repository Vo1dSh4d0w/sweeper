PKG_CFLAGS=$(shell pkg-config --cflags ncurses)
PKG_LDFLAGS=$(shell pkg-config --libs ncurses)
SWEEPER_VERSION=$(shell cat ./version)

CFLAGS?=-Wall -O2
CFLAGS+= $(PKG_CFLAGS) -DSWEEPER_VERSION=\"$(SWEEPER_VERSION)\"
LDFLAGS?=
LDFLAGS+= $(PKG_LDFLAGS)

OBJS=\
src/color.o \
src/config.o \
src/config_menu.o \
src/files.o \
src/input_handler.o \
src/main.o \
src/menu.o \
src/status_bar.o \
src/win_helpers.o

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
