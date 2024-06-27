CC = gcc
CFLAGS = -Wall -std=c99
LDFLAGS = -lraylib

SOURCES = interface.c

EXECUTABLE = game

all: $(EXECUTABLE)

$(EXECUTABLE): $(SOURCES)
    $(CC) $(CFLAGS) $(SOURCES) -o $(EXECUTABLE) $(LDFLAGS)

clean:
    rm -f $(EXECUTABLE)
