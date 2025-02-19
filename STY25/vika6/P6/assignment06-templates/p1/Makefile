CC=     gcc
LD=     ld
CFLAGS= -g -W -Wall -Werror -std=c17
TARGET= malloc
SRC=    malloc.c testlib.c main.c
OBJ=    $(SRC:.c=.o)

all:
	@echo "Nothing to do here"

showsrc:
	@echo trace-info.txt sim-paging.py simresult.png mempattern-1.png mempattern-2.png

zip:
	cd ..; ./makezip.py
