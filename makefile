# Compiler
CC = gcc

# Compiler flags
CFLAGS = -Wall -ansi -pedantic -g

# Target executable
TARGET = assembler

# Directories
SRCDIR = src
OBJDIR = obj

# Source files (Include test_preprocessor.c)
SRC = $(wildcard $(SRCDIR)/**/*.c)
SRC := $(filter-out src/assembler/test_preprocessor.c, $(SRC))

# Object files
OBJ = $(patsubst $(SRCDIR)/%.c,$(OBJDIR)/%.o,$(SRC))

# Main target
$(TARGET): $(OBJ)
	$(CC) $(CFLAGS) $^ -o $@

# Rule to compile object files
$(OBJDIR)/%.o: $(SRCDIR)/%.c
	@mkdir -p $(@D)
	$(CC) $(CFLAGS) -c $< -o $@

# Clean target
clean:
	rm -rf $(OBJDIR) $(TARGET)

.PHONY: clean
