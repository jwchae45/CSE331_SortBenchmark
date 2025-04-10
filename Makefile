CXX := g++
CXXFLAGS := -std=c++20 -Wall -Wextra -I./include -I. -O2
LDFLAGS :=

SRC_DIR := ./src
AFX_DIR := ./include
BUILD_DIR := build
BINS := benchmark datagen

COMMON_SRCS := $(wildcard $(AFX_DIR)/*.cpp)

BENCHMARK_SRC := $(SRC_DIR)/benchmark.cpp
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
	$(MAKE) datagen
	$(MAKE) datagen-clean
	./datagen --seed=2313860 --bsize=32 --dist=uniform --iter=1K
	./datagen --seed=2313861 --bsize=32 --dist=uniform --iter=10K
	./datagen --seed=2313862 --bsize=32 --dist=uniform --iter=100K
	./datagen --seed=2313863 --bsize=32 --dist=uniform --iter=1M
	./datagen --seed=2313864 --bsize=64 --dist=uniform --iter=1K
	./datagen --seed=2313865 --bsize=64 --dist=uniform --iter=10K
	./datagen --seed=2313866 --bsize=64 --dist=uniform --iter=100K
	./datagen --seed=2313867 --bsize=64 --dist=uniform --iter=1M

datagen-clean:
	rm -rf ./dataset/*

.PHONY: all clean debug release benchmark datagen datagen-run datagen-clean