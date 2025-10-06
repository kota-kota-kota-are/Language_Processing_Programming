# Makefile for MPPL Token Counter (tc)
# Author: omzn
# Date: 2025-02-26

# Compiler and flags
CC = gcc
CFLAGS = -Wall -Wextra -g
TARGET = tc
OBJS = main.o scan.o

# Default target
.PHONY: all
all: $(TARGET)

# Link object files to create executable
$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJS)

# Compile main.c
main.o: main.c scan.h
	$(CC) $(CFLAGS) -c main.c

# Compile scan.c
scan.o: scan.c scan.h
	$(CC) $(CFLAGS) -c scan.c

# Clean up generated files
.PHONY: clean
clean:
	rm -f $(TARGET) $(OBJS) *~

# Run official tests using lpptest
.PHONY: test
test:
	@echo "Running lpptest for assignment 01..."
	lpptest 01test
	@echo "Test results available in test_results/ directory"

# Run manual tests (for environments without lpptest)
.PHONY: test-manual
test-manual: $(TARGET)
	@echo "Running manual tests..."
	@if [ -d test-sample ]; then \
		for file in test-sample/sample1[12].mpl; do \
			echo "Testing $$file:"; \
			./$(TARGET) $$file; \
			echo "---"; \
		done \
	else \
		echo "test-sample directory not found"; \
	fi

# Show help
.PHONY: help
help:
	@echo "Available targets:"
	@echo "  all         - Build the executable (default)"
	@echo "  clean       - Remove generated files"
	@echo "  test        - Run lpptest for assignment 01"
	@echo "  test-manual - Run manual tests (if lpptest unavailable)"
	@echo "  help        - Show this help message"
