CFLAGS=-Wall -Wextra -std=c23 -Oz -D UNICODE -nostartfiles -nodefaultlibs -nolibc -nostdlib
CC=gcc

LDFLAGS=--strip-all -static --subsystem console --entry WinMain
LDLIBS=-lkernel32 -luser32

SOURCES=$(wildcard *.c)
OBJS=$(patsubst %.c,objs/%.obj,$(SOURCES))

.DEFAULT_GOAL=all

.NOTPARALLEL :
.PHONY : clean mkdirs

mkdirs:
	@ mkdir -p objs
	@ mkdir -p bin

objs/%.obj : %.c
	@ echo "Compiling '$<' -> '$@'"
	@ $(CC) $(CFLAGS) -c -o $@ $<

bin/listvolumes.exe : $(OBJS)
	@ echo Linking $@
	@ ld $(LDFLAGS) -o $@ $(filter-out objs/tests.obj,$(OBJS)) $(LDLIBS)

bin/tests.exe : $(OBJS)
	@ echo Linking $@
	@ ld $(LDFLAGS) -o $@ $(filter-out objs/main.obj,$(OBJS)) $(LDLIBS)

all : mkdirs bin/tests.exe bin/listvolumes.exe

clean :
	rm -fr objs
	rm -fr bin
