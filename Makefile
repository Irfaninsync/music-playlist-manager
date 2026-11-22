# Makefile for Music Playlist Manager (DSA Project in C11)
CC = gcc
CFLAGS = -std=c11 -Wall -Wextra -pedantic -Iinclude
SRC = src/song.c \
      src/library.c \
      src/singly_linked_list.c \
      src/doubly_linked_list.c \
      src/circular_linked_list.c \
      src/stack.c \
      src/queue.c \
      src/priority_queue.c \
      src/hash_table.c \
      src/bst.c \
      src/graph.c \
      src/file_handler.c \
      src/interface.c \
      src/main.c

TARGET = music_manager.exe

all: $(TARGET)

$(TARGET): $(SRC)
	$(CC) $(CFLAGS) $(SRC) -o $(TARGET)

run: $(TARGET)
	./$(TARGET)

clean:
	rm -f $(TARGET) *.o

.PHONY: all run clean
