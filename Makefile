CXX = g++
CXXFLAGS = -std=c++11 -fPIC -O2 -Wall
LDFLAGS = -shared

# Source files
SOURCES = src/tcp_tahoe.cpp src/stop_and_wait.cpp src/prims_algorithm.cpp src/crc.cpp
OBJECTS = $(SOURCES:.cpp=.o)

# Target library
TARGET = libnetwork_protocols.so

# Default target
all: $(TARGET)

# Build shared library
$(TARGET): $(OBJECTS)
	$(CXX) $(LDFLAGS) -o $@ $^

# Compile source files
%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Clean build files
clean:
	rm -f $(OBJECTS) $(TARGET)

# Install library
install: $(TARGET)
	cp $(TARGET) /usr/local/lib/
	ldconfig

# Uninstall library
uninstall:
	rm -f /usr/local/lib/$(TARGET)
	ldconfig

.PHONY: all clean install uninstall 