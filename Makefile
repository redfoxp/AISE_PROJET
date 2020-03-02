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
OPTI_FLAG = -O2

CFLAGS = $(ERROR_FLAGS) $(OPTI_FLAG)

all: clean lib test

run :
	LD_LIBRARY_PATH=$(BUILDLIBDIR) $(BUILDTESTDIR)/test

lib : $(LIBSRCDIR)/$(LIBNAME).$(SRCEXT)
	cp -r include ./build/
	mkdir -p $(BUILDLIBDIR)
	$(CC) -c -fPIC $(CFLAGS) $(INCLUDE) -o $(BUILDLIBDIR)/$(LIBNAME).o $(LIBSRCDIR)/$(LIBNAME).$(SRCEXT)
	$(CC) -shared -o $(BUILDLIBDIR)/libmy_malloc.so $(BUILDLIBDIR)/my_malloc.o
	$(AR) rcs $(BUILDLIBDIR)/libmy_malloc.a $(BUILDLIBDIR)/my_malloc.o

test : lib
	mkdir -p $(BUILDTESTDIR)
	$(CC) $(INCLUDE) $(LIB) $(CFLAGS) $(TEST) -lmy_malloc -o $(BUILDTESTDIR)/test
	LD_LIBRARY_PATH=$(BUILDLIBDIR)

atom:
	atom $(LIBSRCDIR)/$(LIBNAME).$(SRCEXT)	$(TEST)
	atom include/*.h

clean:
	@echo " Cleaning...";
	@echo " $(RM) -r $(BUILDDIR) $(TARGET)"; $(RM) -r $(BUILDLIBDIR) $(BUILDTESTDIR)

.PHONY: clean
