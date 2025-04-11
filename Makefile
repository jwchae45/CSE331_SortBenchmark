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
	$(MAKE) datagen-clean
	./datagen --seed=23138600 --bsize=32 --dist=uniform --N=1K
	./datagen --seed=23138602 --bsize=32 --dist=uniform --N=4K
	./datagen --seed=23138604 --bsize=32 --dist=uniform --N=16K
	./datagen --seed=23138602 --bsize=32 --dist=uniform --N=64K
	./datagen --seed=23138604 --bsize=32 --dist=uniform --N=256K
	./datagen --seed=23138605 --bsize=32 --dist=uniform --N=1M

datagen-clean:
	rm -rf ./dataset/*

benchmark-test:
	./benchmark --iteration=10 --dataset=./dataset/int32_1K_uniform_random_1   --method=bubble --verbose > ./result/int32_1K_uniform_random_1.bubble
	./benchmark --iteration=10 --dataset=./dataset/int32_4K_uniform_random_1   --method=bubble --verbose > ./result/int32_4K_uniform_random_1.bubble
	./benchmark --iteration=10 --dataset=./dataset/int32_16K_uniform_random_1  --method=bubble --verbose > ./result/int32_16K_uniform_random_1.bubble
	./benchmark --iteration=10 --dataset=./dataset/int32_64K_uniform_random_1  --method=bubble --verbose > ./result/int32_64K_uniform_random_1.bubble
	./benchmark --iteration=10 --dataset=./dataset/int32_256K_uniform_random_1 --method=bubble --verbose > ./result/int32_256K_uniform_random_1.bubble
	./benchmark --iteration=10 --dataset=./dataset/int32_1M_uniform_random_1   --method=bubble --verbose > ./result/int32_1M_uniform_random_1.bubble

benchmark-clean:
	echo "timestamp,sorting method,N,data bits,distribution,order,iteration,mean elapsed time (ms),#(array accesses) / iteration,#(comparisons) / iteration" > benchmark_result.csv

.PHONY: all clean debug release benchmark datagen datagen-run datagen-clean benchmark-test benchmark-clean