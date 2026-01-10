SOURCE_DIR := src
BUILD_DIR := build

SOURCE := $(wildcard $(SOURCE_DIR)/*.cpp)
BUILD_NAME := laf.exe
OUTPUT := $(BUILD_DIR)/$(BUILD_NAME)
CXX := g++
CXXFLAGS := -Wall -Wextra -std=c++17
INCLUDES := -I$(SOURCE_DIR)/include

all: $(OUTPUT)

$(OUTPUT): $(SOURCE)
	$(CXX) -o $(OUTPUT) $(SOURCE) $(CXXFLAGS) $(INCLUDES)
