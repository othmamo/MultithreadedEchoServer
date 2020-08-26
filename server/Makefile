# Makefile

CC = gcc -pthread -fsanitize=address
CPPFLAGS =
CFLAGS = -Wall -Wextra -std=c99 -Og
LDFLAGS =
LDLIBS =

SRC = shared_queue.c main.c
OBJ = ${SRC:.c=.o}
DEP = ${SRC:.c=.d}

all: main

main: ${OBJ}

clean:
	rm -f ${OBJ}
	rm -f ${DEP}
	rm -f main

-include ${DEP}

# END
