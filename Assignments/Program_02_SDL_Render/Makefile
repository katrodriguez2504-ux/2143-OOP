CXX := g++
CXXFLAGS := -Wall -std=c++17 $(shell pkg-config --cflags sdl2)
LDFLAGS := $(shell pkg-config --libs sdl2)
TARGET := $(notdir $(CURDIR))
SRC := main.cpp
INCLUDES := -I./includes

all: $(TARGET)

$(TARGET): $(SRC)
	$(CXX) $(CXXFLAGS) $(INCLUDES) -o $@ $^ $(LDFLAGS)

run: $(TARGET)
	./$(TARGET)

clean:
	rm -f $(TARGET)
