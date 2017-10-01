SHELL = /bin/sh
CC = gcc
STD = -std=c11

FLAGS = -I /usr/include/modbus

LDIR = /usr/local/lib
LIBS = -lmodbus

%: %.c
	$(CC) $(STD) $(FLAGS) $^ -o $@ -L$(LDIR) $(LIBS)

TARGETS = $(shell echo *.c | sed -e 's/\.c//g')

all: $(TARGETS)

clean:
	rm $(TARGETS)
