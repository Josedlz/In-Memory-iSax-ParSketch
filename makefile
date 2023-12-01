# Compiler
CXX = g++

# Compiler flags
CXXFLAGS = -Wall -Wextra -pedantic -std=c++11

# Directories
SRCDIR1 = Dataset
SRCDIR2 = knnSearchers
SRCDIR3 = TimeSeries
INCDIR = include
OBJDIR = bin/obj
BINDIR = bin

# Files
SRCS1 = $(wildcard $(SRCDIR1)/*.cpp)
SRCS2 = $(wildcard $(SRCDIR2)/*.cpp)
SRCS3 = $(wildcard $(SRCDIR3)/*.cpp)
OBJS = $(patsubst $(SRCDIR1)/%,$(OBJDIR)/%,$(SRCS1:.cpp=.o)) $(patsubst $(SRCDIR2)/%,$(OBJDIR)/%,$(SRCS2:.cpp=.o)) $(patsubst $(SRCDIR3)/%,$(OBJDIR)/%,$(SRCS3:.cpp=.o))
DEPS = $(OBJS:.o=.d)

# Targets
TARGET = SearchersProject

# Rules
all: $(BINDIR)/$(TARGET)

$(BINDIR)/$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $@ $^

$(OBJDIR)/%.o: $(SRCDIR1)/%.cpp $(SRCDIR2)/%.cpp $(SRCDIR3)/%.cpp
	mkdir -p $(OBJDIR)
	$(CXX) $(CXXFLAGS) -I$(INCDIR) -MMD -MP -c $< -o $@

-include $(DEPS)

clean:
	rm -f $(BINDIR)/$(TARGET) $(OBJS) $(DEPS)
