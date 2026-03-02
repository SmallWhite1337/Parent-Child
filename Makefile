CC = gcc

DEBUG_FLAGS = -g -ggdb -std=c11 -pedantic -W -Wall -Wextra
RELEASE_FLAGS = -std=c11 -pedantic -W -Wall -Wextra -Werror

SRC_DIR = src
DEBUG_DIR = build/debug
RELEASE_DIR = build/release

ifeq ($(MODE),release)
    CFLAGS = $(RELEASE_FLAGS)
    OUT_DIR = $(RELEASE_DIR)
else
    CFLAGS = $(DEBUG_FLAGS)
    OUT_DIR = $(DEBUG_DIR)
endif

.PHONY: all clean

all: $(OUT_DIR)/parent $(OUT_DIR)/child

$(OUT_DIR)/parent: $(SRC_DIR)/parent.c Makefile
	$(CC) $(CFLAGS) $< -o $@

$(OUT_DIR)/child: $(SRC_DIR)/child.c Makefile
	$(CC) $(CFLAGS) $< -o $@

clean:
	rm -f $(DEBUG_DIR)/* $(RELEASE_DIR)/*