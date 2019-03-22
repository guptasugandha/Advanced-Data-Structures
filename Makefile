CXX      := -c++
CXXFLAGS := -pedantic-errors -Wall -Wextra -Werror
LDFLAGS  := -L/usr/lib -lstdc++ -lm
BUILD    := ./build
OBJ_DIR  := $(BUILD)/objects
TARGET   := ./keywordcounter
INCLUDE  := -Iinclude
SRC      :=                      \
   $(wildcard *.cpp)         \

OBJECTS := $(SRC:%.cpp=$(OBJ_DIR)/%.o)

all: build $(TARGET)

$(OBJ_DIR)/%.o: %.cpp
	@mkdir -p $(@D)
	$(CXX) $(CXXFLAGS) $(INCLUDE) -o $@ -c $<

$(TARGET): $(OBJECTS)
	@mkdir -p $(@D)
	$(CXX) $(CXXFLAGS) $(INCLUDE) $(LDFLAGS) -o $(TARGET) $(OBJECTS)

.PHONY: all build clean debug release

build:
	@mkdir -p $(OBJ_DIR)

debug: CXXFLAGS += -DDEBUG -g
debug: all

release: CXXFLAGS += -O2
release: all

clean:
	-@rm -rvf $(OBJ_DIR)/*
