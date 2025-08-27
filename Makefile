TARGET = main
CXX = g++
SOURCES = main.cpp window.cpp renderpipeline.cpp
OBJS = $(SOURCES:.cpp=.o)

CXXFLAGS = $(shell sdl2-config --cflags) -std=c++17
LDFLAGS = $(shell sdl2-config --libs) -lGL -lGLEW -lSDL2_image -lvulkan

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) $^ -o $@ $(LDFLAGS)

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS) $(TARGET)
