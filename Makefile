.PHONY: all tests runtests clean install 


OS =

LLIST_OPTS   = 
CFLAGS       = -g -Wall -pedantic -std=gnu99 -Iinclude
EXTRA_FLAGS  = -fPIC -shared -fprofile-arcs -ftest-coverage
LIBFLAGS	 = -fPIC -shared -fprofile-arcs
LDFLAGS	     = -shared
DEBUGFLAGS   = -O0 -D _DEBUG
RELEASEFLAGS = -O2 -D NDEBUG -combine -fwhole-program
TEST_LDFLAGS = -lcheck -lpthread -lllist -lm -lrt

ifeq ($(OS),ubuntu16.04)
   TEST_LDFLAGS += -lsubunit
endif

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

all: $(TARGET) 

$(TARGET): $(OBJECTS)
	mkdir -p $(OBJDIR)
	$(CC) $(FLAGS) $(LIBFLAGS) $(DEBUGFLAGS) -o $(TARGET) $(OBJECTS)

tests: $(TEST_OBJECTS)
	$(CC) $(FLAGS) -o $(TEST_TARGET) $(TEST_OBJECTS) $(TEST_LDFLAGS) -L$(OBJDIR)/

# Need a special rule to compile the lib to allow EXTRA_FLAGS
$(OBJECTS): $(SOURCES) $(HEADERS)
	@echo [Compiling]: $<
	$(CC) $(CFLAGS) $(LLIST_OPTS) $(EXTRA_FLAGS) -o $@ -c $<

clean:
	rm -rf $(TEST_OBJECTS) $(OBJECTS) *.gcda *.gcov *.gcno *~ $(TARGET) $(TEST_TARGET)
	
runtests: tests
	@echo Invoking Test:
	@echo --------------
	@./$(TEST_TARGET)
	@echo Test Coverage:
	@echo --------------
	@gcov -osrc llist.c

install:
	@echo Installing libllist:
	@echo --------------------
	cp $(TARGET) /usr/local/lib
	cp inc/llist.h /usr/local/include
