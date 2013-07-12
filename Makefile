SHELL = /bin/sh
CC    = gcc
FLAGS        = -std=gnu99 -Iinclude -Wall -pedantic
CFLAGS       = -fPIC -g -shared 
LDFLAGS	     = -shared -pthread -lcheck
DEBUGFLAGS   = -O0 -D _DEBUG -fprofile-arcs -ftest-coverage
RELEASEFLAGS = -O2 -D NDEBUG -combine -fwhole-program
TEST_LDFLAGS = -lcheck -lllist -Llib

OBJDIR	= lib
TARGET  = $(OBJDIR)/libllist.so
TEST_TARGET = starttest
SOURCES = $(shell echo src/*.c)
HEADERS = $(shell echo inc/*.h)
TEST_SOURCES = $(shell echo tests/*.c)
TEST_OBJECTS = $(TEST_SOURCES:.c=.o)
OBJECTS = $(SOURCES:.c=.o)

PREFIX = $(DESTDIR)/usr/local
BINDIR = $(PREFIX)/bin

all: $(TARGET) tests

$(TARGET): $(OBJECTS)
	mkdir -p $(OBJDIR)
	$(CC) $(FLAGS) $(CFLAGS) $(DEBUGFLAGS) -o $(TARGET) $(OBJECTS)

tests: $(TEST_OBJECTS)
	$(CC) $(FLAGS)  -o $(TEST_TARGET) $(TEST_OBJECTS) $(TEST_LDFLAGS)

clean:
	rm -rf $(TEST_OBJECTS) $(OBJECTS) *~ $(TARGET) $(TEST_TARGET)