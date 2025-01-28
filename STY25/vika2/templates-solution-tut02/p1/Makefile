CC=     gcc
LD=     ld
CFLAGS= -g -W -Wall -Werror -std=c99
TARGET= malloc malloc2
SRC=    malloc.c malloc2.c
OBJ=    $(SRC:.c=.o)

all:	$(TARGET)

malloc:	malloc.o
	$(CC) -o $@ $^

malloc2: malloc2.o
	$(CC) -o $@ $^

%.o: %.c
	$(CC) $(CFLAGS) -c $<

clean:
	rm -rf $(TARGET) $(TARGET)-sol *.o
