CXX = g++
CXXFLAGS = -O3 -flto -Wall -Wextra
TARGET = zero
SOURCE = main.cc
OBJECTS = main.o

all: $(TARGET)

$(TARGET): $(OBJECTS)
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(OBJECTS)

%.o: %.cc
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -f $(TARGET) $(OBJECTS)