# This is jst a template... PPR can be built manually pretty easily

CC = gcc
AR = ar

.PHONY: all format clean
.SUFFIXES: .c .o

OBJS = src/core.o
OBJS += src/base/poll.o src/base/file.o src/base/socket.o src/base/arpa.o src/base/string.o src/base/dlfcn.o src/base/unistd.o src/base/stat.o src/base/dirent.o src/base/time.o
OBJS += src/base/mutex.o src/base/thread.o src/base/process.o
OBJS += src/hash/md5.o src/hash/sha256.o src/hash/blake2s.o
OBJS += src/misc/url.o src/misc/wildcard.o

all: libppr.a

format:
	clang-format --verbose -i `find src include -name "*.c" -or -name "*.h"`

.c.o:
	$(CC) -c -I include -o $@ $<

libppr.a: $(OBJS)
	$(AR) rcs $@ $(OBJS)

clean:
	rm -f *.a src/*.o src/*/*.o
