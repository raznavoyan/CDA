# Compiler to use
CXX = g++

# Compiler flags: 
# -Wall enables all warnings
# -std=c++17 enforces C++17 standard
CXXFLAGS = -Wall -std=c++17

# Linker flags: link against ncurses for terminal UI
LDFLAGS = -lncurses

# Source files used to build the project
SRC = interface.cpp \
      test.cpp \
      measurement_manager.cpp \
      data_manager.cpp \
      PyPlotter.cpp

# Header files (not strictly required by make but listed for clarity)
HDR = interface.hpp \
      measurement_manager.hpp \
      data_manager.hpp \
      PyPlotter.hpp

# Default target: build the main executable
all: test_interface

# Rule to build the executable
test_interface: $(SRC) $(HDR)
	$(CXX) $(CXXFLAGS) $(SRC) -o test_interface $(LDFLAGS)

# Clean up build artifacts
clean:
	rm -f test_interface

