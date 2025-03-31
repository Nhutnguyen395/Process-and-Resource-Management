CC:=gcc
CFLAGS:=-g
SRC_DIR:=src
BIN_DIR:=bin
INCLUDE_DIR:=include

all: setup shell

shell: src/helpers.c src/shell.c
	$(CC) -I $(INCLUDE_DIR) $(CFLAGS) $(SRC_DIR)/helpers.c $(SRC_DIR)/shell.c -o $(BIN_DIR)/shell.exe

setup:
	-mkdir $(BIN_DIR)

clean:
	-rmdir /s /q $(BIN_DIR)

.PHONY: clean all setup