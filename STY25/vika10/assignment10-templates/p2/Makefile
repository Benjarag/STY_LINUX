CC=     gcc
LD=     ld
CFLAGS= -g -W -Wall -Werror -std=c17
TARGET= copy
SRC=    copy.c testlib.c main.c
OBJ=    $(SRC:.c=.o)

copy: $(OBJ)
	$(CC) -o $@ $(OBJ)

.PHONY: $(TARGET)-solution.o
$(TARGET)-sol: $(subst $(TARGET),$(TARGET)-solution,$(OBJ))
	$(CC) -o $@ $^

$(TARGET)-solution.o:
	@test -f $(TARGET)-solution.c \
	|| { echo "Please download solution." >&2; false; }
	$(CC) $(CFLAGS) -c $(TARGET)-solution.c

%.o: %.c
	$(CC) $(CFLAGS) -c $<

clean:
	rm -rf $(TARGET) $(TARGET)-sol *.o


showsrc:
	@echo $(TARGET).c

sol:	copy-sol

zip:
	cd ..; ./makezip.py
