CXX := g++
CXXFLAGS := -m32 -Wall -O2 -rdynamic
LDFLAGS := -m32

TARGET := steam
SRC := steam.cpp

all: $(TARGET)

$(TARGET): $(SRC)
	$(CXX) $(CXXFLAGS) $(SRC) -o $(TARGET) $(LDFLAGS)

clean:
	rm -f $(TARGET)
