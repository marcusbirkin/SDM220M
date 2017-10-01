SHELL = /bin/sh
CC = gcc

FLAGS = -I /usr/include/modbus

LDIR = /usr/local/lib
LIBS = -lmodbus

%: %.c
	$(CC) $(FLAGS) $^ -o $@ -L$(LDIR) $(LIBS)

TARGETS = $(shell echo *.c | sed -e 's/\.c//g')

all: $(TARGETS)

clean:
	rm $(TARGETS)
