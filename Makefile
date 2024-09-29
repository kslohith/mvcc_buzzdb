# Compiler and flags
CXX = g++
CXXFLAGS = -std=c++17 -Wall -Iinclude

# Directories
SRC_DIR = src
INCLUDE_DIR = include

# Source files and object files
SRCS = $(wildcard $(SRC_DIR)/*.cpp) main.cpp
OBJS = $(SRCS:.cpp=.o)

# Output binary name
TARGET = buzzdb

# Default target to build the project
all: $(TARGET)

# Rule to link object files into the final executable
$(TARGET): $(OBJS)
	$(CXX) $(OBJS) -o $@

# Rule to compile source files into object files
%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Clean up build artifacts
clean:
	rm -f $(OBJS) $(TARGET)

.PHONY: all clean