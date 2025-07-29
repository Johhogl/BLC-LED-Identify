# Makefile
CXX = g++

CXXFLAGS = -std=c++17 $(shell pkg-config --cflags opencv4)
LDFLAGS  = $(shell pkg-config --libs opencv4)

# Targets
findLEDs: findLEDs.cpp
	$(CXX) $(CXXFLAGS) findLEDs.cpp -o build/findLEDs $(LDFLAGS)

# Clean target
clean:
	rm -f *.o *.a *~ 
	rm -f build/*
