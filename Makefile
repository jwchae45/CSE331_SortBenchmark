CXX := g++
CXXFLAGS := -std=c++20 -Wall -Wextra -I./include -O2
LDFLAGS :=

SRC_DIR := ./src
AFX_DIR := ./include
BUILD_DIR := build

SRC_FILES := $(wildcard $(SRC_DIR)/*.cpp)
AFX_FILES := $(wildcard $(AFX_DIR)/*.cpp)

BINS := $(basename $(notdir $(SRC_FILES)))
OBJS := $(patsubst %.cpp, $(BUILD_DIR)/%.o, $(notdir $(SRC_FILES) $(AFX_FILES)))

all: $(BINS)

$(BINS): %: $(BUILD_DIR)/%.o $(patsubst %.cpp, $(BUILD_DIR)/%.o, $(notdir $(AFX_FILES)))
	$(CXX) $(CXXFLAGS) $^ -o $@ $(LDFLAGS)

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.cpp | $(BUILD_DIR)
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(BUILD_DIR)/%.o: $(AFX_DIR)/%.cpp | $(BUILD_DIR)
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

clean:
	rm -rf $(BUILD_DIR) $(BINS)

debug: override CXXFLAGS := -std=c++20 -Wall -Wextra -I./include -O0 -g
debug: clean all

release: override CXXFLAGS := -std=c++20 -Wall -Wextra -I./include -O3 -fno-rtti
release: override LDFLAGS := -flto
release: clean all

datagen-run:
	$(MAKE) datagen-clean
	./datagen --seed=23138600 --bsize=32 --dist=uniform --N=1K
	./datagen --seed=23138601 --bsize=32 --dist=uniform --N=2K
	./datagen --seed=23138602 --bsize=32 --dist=uniform --N=4K
	./datagen --seed=23138603 --bsize=32 --dist=uniform --N=8K
	./datagen --seed=23138604 --bsize=32 --dist=uniform --N=16K
	./datagen --seed=23138605 --bsize=32 --dist=uniform --N=32K
	./datagen --seed=23138606 --bsize=32 --dist=uniform --N=64K
	./datagen --seed=23138607 --bsize=32 --dist=uniform --N=128K
	./datagen --seed=23138608 --bsize=32 --dist=uniform --N=256K
	./datagen --seed=23138609 --bsize=32 --dist=uniform --N=512K
	./datagen --seed=23138610 --bsize=32 --dist=uniform --N=1M

datagen-clean:
	rm -rf ./dataset/*

benchmark-bubble:
	./benchmark --iteration=10 --dataset=./dataset/int32_1K_uniform_random_1   --method=bubble --verbose > ./result/int32_1K_uniform_random_1.bubble
	./benchmark --iteration=10 --dataset=./dataset/int32_2K_uniform_random_1   --method=bubble --verbose > ./result/int32_2K_uniform_random_1.bubble
	./benchmark --iteration=10 --dataset=./dataset/int32_4K_uniform_random_1   --method=bubble --verbose > ./result/int32_4K_uniform_random_1.bubble
	./benchmark --iteration=10 --dataset=./dataset/int32_8K_uniform_random_1   --method=bubble --verbose > ./result/int32_8K_uniform_random_1.bubble
	./benchmark --iteration=10 --dataset=./dataset/int32_16K_uniform_random_1  --method=bubble --verbose > ./result/int32_16K_uniform_random_1.bubble
	./benchmark --iteration=10 --dataset=./dataset/int32_32K_uniform_random_1  --method=bubble --verbose > ./result/int32_32K_uniform_random_1.bubble
	./benchmark --iteration=10 --dataset=./dataset/int32_64K_uniform_random_1  --method=bubble --verbose > ./result/int32_64K_uniform_random_1.bubble
	./benchmark --iteration=10 --dataset=./dataset/int32_128K_uniform_random_1 --method=bubble --verbose > ./result/int32_128K_uniform_random_1.bubble
	./benchmark --iteration=10 --dataset=./dataset/int32_256K_uniform_random_1 --method=bubble --verbose > ./result/int32_256K_uniform_random_1.bubble
	./benchmark --iteration=10 --dataset=./dataset/int32_512K_uniform_random_1 --method=bubble --verbose > ./result/int32_512K_uniform_random_1.bubble
	./benchmark --iteration=10 --dataset=./dataset/int32_1M_uniform_random_1   --method=bubble --verbose > ./result/int32_1M_uniform_random_1.bubble

benchmark-selection:
	./benchmark --iteration=10 --dataset=./dataset/int32_1K_uniform_random_1   --method=selection --verbose > ./result/int32_1K_uniform_random_1.selection
	./benchmark --iteration=10 --dataset=./dataset/int32_2K_uniform_random_1   --method=selection --verbose > ./result/int32_2K_uniform_random_1.selection
	./benchmark --iteration=10 --dataset=./dataset/int32_4K_uniform_random_1   --method=selection --verbose > ./result/int32_4K_uniform_random_1.selection
	./benchmark --iteration=10 --dataset=./dataset/int32_8K_uniform_random_1   --method=selection --verbose > ./result/int32_8K_uniform_random_1.selection
	./benchmark --iteration=10 --dataset=./dataset/int32_16K_uniform_random_1  --method=selection --verbose > ./result/int32_16K_uniform_random_1.selection
	./benchmark --iteration=10 --dataset=./dataset/int32_32K_uniform_random_1  --method=selection --verbose > ./result/int32_32K_uniform_random_1.selection
	./benchmark --iteration=10 --dataset=./dataset/int32_64K_uniform_random_1  --method=selection --verbose > ./result/int32_64K_uniform_random_1.selection
	./benchmark --iteration=10 --dataset=./dataset/int32_128K_uniform_random_1 --method=selection --verbose > ./result/int32_128K_uniform_random_1.selection
	./benchmark --iteration=10 --dataset=./dataset/int32_256K_uniform_random_1 --method=selection --verbose > ./result/int32_256K_uniform_random_1.selection
	./benchmark --iteration=10 --dataset=./dataset/int32_512K_uniform_random_1 --method=selection --verbose > ./result/int32_512K_uniform_random_1.selection
	./benchmark --iteration=10 --dataset=./dataset/int32_1M_uniform_random_1   --method=selection --verbose > ./result/int32_1M_uniform_random_1.selection

benchmark-insertion:
	./benchmark --iteration=10 --dataset=./dataset/int32_1K_uniform_random_1   --method=insertion --verbose > ./result/int32_1K_uniform_random_1.insertion
	./benchmark --iteration=10 --dataset=./dataset/int32_2K_uniform_random_1   --method=insertion --verbose > ./result/int32_2K_uniform_random_1.insertion
	./benchmark --iteration=10 --dataset=./dataset/int32_4K_uniform_random_1   --method=insertion --verbose > ./result/int32_4K_uniform_random_1.insertion
	./benchmark --iteration=10 --dataset=./dataset/int32_8K_uniform_random_1   --method=insertion --verbose > ./result/int32_8K_uniform_random_1.insertion
	./benchmark --iteration=10 --dataset=./dataset/int32_16K_uniform_random_1  --method=insertion --verbose > ./result/int32_16K_uniform_random_1.insertion
	./benchmark --iteration=10 --dataset=./dataset/int32_32K_uniform_random_1  --method=insertion --verbose > ./result/int32_32K_uniform_random_1.insertion
	./benchmark --iteration=10 --dataset=./dataset/int32_64K_uniform_random_1  --method=insertion --verbose > ./result/int32_64K_uniform_random_1.insertion
	./benchmark --iteration=10 --dataset=./dataset/int32_128K_uniform_random_1 --method=insertion --verbose > ./result/int32_128K_uniform_random_1.insertion
	./benchmark --iteration=10 --dataset=./dataset/int32_256K_uniform_random_1 --method=insertion --verbose > ./result/int32_256K_uniform_random_1.insertion
	./benchmark --iteration=10 --dataset=./dataset/int32_512K_uniform_random_1 --method=insertion --verbose > ./result/int32_512K_uniform_random_1.insertion
	./benchmark --iteration=10 --dataset=./dataset/int32_1M_uniform_random_1   --method=insertion --verbose > ./result/int32_1M_uniform_random_1.insertion

benchmark-run:
	$(MAKE) benchmark-bubble
	$(MAKE) benchmark-selection
	$(MAKE) benchmark-insertion

benchmark-clean:
	rm -rf ./result/*
	echo "timestamp,sorting method,N,data bits,distribution,order,iteration,mean elapsed time (ms),#(array accesses) / iteration,#(comparisons) / iteration" > benchmark_result.csv

.PHONY: all clean debug release benchmark datagen datagen-clean datagen-run benchmark-clean benchmark-run benchmark-bubble benchmark-selection benchmark-insertion