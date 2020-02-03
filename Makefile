CC = gcc
TARGET := bin/main
SRCDIR := src
BUILDDIR := build

SRCEXT := c
SOURCES := $(shell find $(SRCDIR) -type f -name *.$(SRCEXT))
OBJECTS := $(patsubst $(SRCDIR)/%,$(BUILDDIR)/%,$(SOURCES:.$(SRCEXT)=.o))
INCLUDE := -I include
LIB := -L lib

ERROR_CFLAGS = -Wall
OPTI_FLAG = -Ofast

CFLAGS = $(ERROR_FLAGS) $(OPTI_FLAG)

$(TARGET): $(OBJECTS)
	@echo " Linking..."
	@echo " $(CC) $^ -o $(TARGET)"; $(CC) $^ -o $(TARGET) $(LIB) $(INCLUDE)

$(BUILDDIR)/%.o: $(SRCDIR)/%.$(SRCEXT)
	@mkdir -p $(BUILDDIR)
	@echo " $(CC) $(CFLAGS) -c -o $@ $<"; $(CC) $(CFLAGS) $(LIB) $(INCLUDE) -c -o $@ $<
	@echo " $(CC) $^ -o $(TARGET)"; $(CC) $^ -o $(TARGET) $(LIB) $(INCLUDE)

$(BUILDDIR)/%.o: $(SRCDIR)/%.$(SRCEXT)
	@mkdir -p $(BUILDDIR)
	@echo " $(CC) $(CFLAGS) -c -o $@ $<"; $(CC) $(CFLAGS) $(LIB) $(INCLUDE) -c -o $@ $<

run:
	rm -Rf out.txt
	./$(TARGET)

atom:
	atom ./src/*.$(SRCEXT)
	atom ./include/*.h

clean:
	@echo " Cleaning...";
	@echo " $(RM) -r $(BUILDDIR) $(TARGET)"; $(RM) -r $(BUILDDIR) $(TARGET)

.PHONY: clean
