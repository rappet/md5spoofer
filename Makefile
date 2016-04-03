CC?=gcc
CFLAGS=-O2 -s -Wall
LDFLAGS=-s

OBJ=main.o
LIB=-lcrypto
BIN=hashspoofer

all: $(OBJ)
	$(CC) $(LDFLAGS) $(OBJ) -o $(BIN) $(LIB)

%.o: %.c
	clang-format -i $<
	$(CC) $(CFLAGS) $< -c

.PHONY: clean
clean:
	rm -rf $(BIN) $(OBJ)
