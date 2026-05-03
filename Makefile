CXX = g++
CXXFLAGS = -std=c++20 -Wall -Wextra -I./src
LDFLAGS = -lsfml-graphics -lsfml-window -lsfml-system

SRCS = src/main.cpp src/SolverODE.cpp src/EulerODE.cpp src/RK4.cpp src/PlotVisualizer.cpp src/BucketVisualizer.cpp
OBJS = $(SRCS:.cpp=.o)
TARGET = model.exe

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $@ $^ $(LDFLAGS)

src/%.o: src/%.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS) $(TARGET)
