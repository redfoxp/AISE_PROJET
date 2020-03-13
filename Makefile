CC = gcc
AR = ar
LIBNAME := my_malloc
LIBSRCDIR := ./src/lib
TEST := ./src/test/test.c
BUILDTESTDIR := ./build/test
BUILDLIBDIR := ./build/lib


SRCEXT := c
INCLUDE := -I ./include
LIB := -L ./build/lib

ERROR_CFLAGS = -Wall
OPTI_FLAG = -O0

CFLAGS = $(ERROR_FLAGS) $(OPTI_FLAG)

all: clean lib test run

run :
	LD_LIBRARY_PATH=$(BUILDLIBDIR) $(BUILDTESTDIR)/test

lib :
	cp -r include ./build/
	mkdir -p $(BUILDLIBDIR)
	$(CC)  -lpthread -shared -fPIC $(CFLAGS) $(INCLUDE) -o $(BUILDLIBDIR)/libmy_malloc.so $(LIBSRCDIR)/my_malloc.$(SRCEXT)

test : lib
	mkdir -p $(BUILDTESTDIR)
	$(CC) $(INCLUDE) $(LIB) $(CFLAGS) $(TEST) -lmy_malloc -o $(BUILDTESTDIR)/test


atom:
	atom $(LIBSRCDIR)/*.$(SRCEXT)	$(TEST)
	atom include/*.h

clean:
	@echo " Cleaning...";
	@echo " $(RM) -Rf $(BUILDLIBDIR) $(BUILDTESTDIR) "; $(RM) -Rf $(BUILDLIBDIR) $(BUILDTESTDIR)

.PHONY: clean
