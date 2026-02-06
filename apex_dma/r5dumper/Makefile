# Makefile for a C++ program linked with X11 and Xtst libraries

# Compiler
CXX = g++

# Source files
SRCS = Main.cpp

# Libraries to link
#LIBS = -lX11 -lXtst

TARGET = r5dumper

# Default target
all:
	@$(MAKE) clean
	@$(MAKE) $(TARGET)
	@echo -e "\033[1musage: ./$(TARGET)\033[0m"

# Clean up object files and the executable
clean:
	rm -f $(TARGET)

# Compilation and linking
$(TARGET): $(SRCS)
	$(CXX) $^ -o $@ $(LIBS)
