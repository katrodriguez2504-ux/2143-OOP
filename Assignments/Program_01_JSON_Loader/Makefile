CXX := g++
CXXFLAGS := -Wall -std=c++17
TARGET := $(notdir $(CURDIR))
SRC := main.cpp
INCLUDES := -I./includes

all: $(TARGET)

$(TARGET): $(SRC)
	$(CXX) $(CXXFLAGS) $(INCLUDES) -o $@ $^

run: $(TARGET)
	./$(TARGET)

clean:
	rm -f $(TARGET)
