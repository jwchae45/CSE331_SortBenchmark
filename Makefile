CXX := g++
CXXFLAGS := -std=c++20 -Wall -Wextra -I./afx -I. -O2
LDFLAGS :=

SRC_DIR := .
AFX_DIR := ./afx
BUILD_DIR := build
BINS := benchmark datagen

COMMON_SRCS := $(wildcard $(AFX_DIR)/*.cpp)

BENCHMARK_SRC := $(SRC_DIR)/main.cpp
DATAGEN_SRC := $(SRC_DIR)/datagen.cpp

BENCHMARK_OBJS := $(patsubst %.cpp, $(BUILD_DIR)/%.o, $(notdir $(BENCHMARK_SRC) $(COMMON_SRCS)))
DATAGEN_OBJS := $(patsubst %.cpp, $(BUILD_DIR)/%.o, $(notdir $(DATAGEN_SRC) $(COMMON_SRCS)))

all: $(BINS)

benchmark: $(BENCHMARK_OBJS)
	$(CXX) $(CXXFLAGS) $^ -o $@ $(LDFLAGS)

datagen: $(DATAGEN_OBJS)
	$(CXX) $(CXXFLAGS) $^ -o $@ $(LDFLAGS)

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.cpp | $(BUILD_DIR)
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(BUILD_DIR)/%.o: $(AFX_DIR)/%.cpp | $(BUILD_DIR)
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

clean:
	rm -rf $(BUILD_DIR) $(BINS)

debug: CXXFLAGS += -g -O0
debug: clean all

release: CXXFLAGS += -O3
release: clean all

datagen-run:
	./datagen --bsize=32 --dist=uniform --iter=1K
	./datagen --bsize=32 --dist=uniform --iter=10K
	./datagen --bsize=32 --dist=uniform --iter=100K
	./datagen --bsize=32 --dist=uniform --iter=1M
	./datagen --bsize=64 --dist=uniform --iter=1K
	./datagen --bsize=64 --dist=uniform --iter=10K
	./datagen --bsize=64 --dist=uniform --iter=100K
	./datagen --bsize=64 --dist=uniform --iter=1M

datagen-clean:
	rm -rf ./dataset/*

.PHONY: all clean debug release benchmark datagen datagen-run datagen-clean