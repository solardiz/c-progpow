#
# Copyright (c) 2019 Solar Designer <solar at openwall.com>
#
# Redistribution and use in source and binary forms, with or without
# modification, are permitted.
#
# There's ABSOLUTELY NO WARRANTY, express or implied.
#

CC = gcc
LD = $(CC)
RM = rm -f
CFLAGS = -std=c99 -Wall -O2
LDFLAGS = -s -lm

PROJ = progpow-test
OBJS = progpow-test.o \
	progpow.o \
	libethash/internal.o libethash/sha3.o libethash/io.o libethash/io_posix.o

all: $(PROJ)

progpow-test: $(OBJS)
	$(LD) $(LDFLAGS) $(OBJS) -o progpow-test

.c.o:
	$(CC) -c $(CFLAGS) $< -o $@

clean:
	$(RM) $(PROJ)
	$(RM) $(OBJS)
