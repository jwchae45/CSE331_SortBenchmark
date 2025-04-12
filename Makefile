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

datagen-patterns:
	./datagen --seed=23138699 --bsize=32 --N=1M --path=./dataset_1M_dist_patterns/ --dist=uniform --pattern=almost
	./datagen --seed=23138698 --bsize=32 --N=1M --path=./dataset_1M_dist_patterns/ --dist=uniform --pattern=noise
	./datagen --seed=23138697 --bsize=32 --N=1M --path=./dataset_1M_dist_patterns/ --dist=uniform --pattern=reversed
	./datagen --seed=23138696 --bsize=32 --N=1M --path=./dataset_1M_dist_patterns/ --dist=uniform --pattern=sawtooth
	./datagen --seed=23138695 --bsize=32 --N=1M --path=./dataset_1M_dist_patterns/ --dist=uniform --pattern=bitonic
	./datagen --seed=23138694 --bsize=32 --N=1M --path=./dataset_1M_dist_patterns/ --dist=uniform --pattern=frontsorted
	./datagen --seed=23138693 --bsize=32 --N=1M --path=./dataset_1M_dist_patterns/ --dist=uniform --pattern=gap
	./datagen --seed=23138692 --bsize=32 --N=1M --path=./dataset_1M_dist_patterns/ --dist=normal  --pattern=almost
	./datagen --seed=23138691 --bsize=32 --N=1M --path=./dataset_1M_dist_patterns/ --dist=normal  --pattern=noise
	./datagen --seed=23138690 --bsize=32 --N=1M --path=./dataset_1M_dist_patterns/ --dist=normal  --pattern=reversed
	./datagen --seed=23138689 --bsize=32 --N=1M --path=./dataset_1M_dist_patterns/ --dist=normal  --pattern=sawtooth
	./datagen --seed=23138688 --bsize=32 --N=1M --path=./dataset_1M_dist_patterns/ --dist=normal  --pattern=bitonic
	./datagen --seed=23138687 --bsize=32 --N=1M --path=./dataset_1M_dist_patterns/ --dist=normal  --pattern=frontsorted
	./datagen --seed=23138686 --bsize=32 --N=1M --path=./dataset_1M_dist_patterns/ --dist=normal  --pattern=gap
	./datagen --seed=23138685 --bsize=32 --N=1M --path=./dataset_1M_dist_patterns/ --dist=bimodal --pattern=almost
	./datagen --seed=23138684 --bsize=32 --N=1M --path=./dataset_1M_dist_patterns/ --dist=bimodal --pattern=noise
	./datagen --seed=23138683 --bsize=32 --N=1M --path=./dataset_1M_dist_patterns/ --dist=bimodal --pattern=reversed
	./datagen --seed=23138682 --bsize=32 --N=1M --path=./dataset_1M_dist_patterns/ --dist=bimodal --pattern=sawtooth
	./datagen --seed=23138681 --bsize=32 --N=1M --path=./dataset_1M_dist_patterns/ --dist=bimodal --pattern=bitonic
	./datagen --seed=23138680 --bsize=32 --N=1M --path=./dataset_1M_dist_patterns/ --dist=bimodal --pattern=frontsorted
	./datagen --seed=23138679 --bsize=32 --N=1M --path=./dataset_1M_dist_patterns/ --dist=bimodal --pattern=gap

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

benchmark-merge:
	./benchmark --iteration=10 --dataset=./dataset/int32_1K_uniform_random_1   --method=merge --verbose > ./result/int32_1K_uniform_random_1.merge
	./benchmark --iteration=10 --dataset=./dataset/int32_2K_uniform_random_1   --method=merge --verbose > ./result/int32_2K_uniform_random_1.merge
	./benchmark --iteration=10 --dataset=./dataset/int32_4K_uniform_random_1   --method=merge --verbose > ./result/int32_4K_uniform_random_1.merge
	./benchmark --iteration=10 --dataset=./dataset/int32_8K_uniform_random_1   --method=merge --verbose > ./result/int32_8K_uniform_random_1.merge
	./benchmark --iteration=10 --dataset=./dataset/int32_16K_uniform_random_1  --method=merge --verbose > ./result/int32_16K_uniform_random_1.merge
	./benchmark --iteration=10 --dataset=./dataset/int32_32K_uniform_random_1  --method=merge --verbose > ./result/int32_32K_uniform_random_1.merge
	./benchmark --iteration=10 --dataset=./dataset/int32_64K_uniform_random_1  --method=merge --verbose > ./result/int32_64K_uniform_random_1.merge
	./benchmark --iteration=10 --dataset=./dataset/int32_128K_uniform_random_1 --method=merge --verbose > ./result/int32_128K_uniform_random_1.merge
	./benchmark --iteration=10 --dataset=./dataset/int32_256K_uniform_random_1 --method=merge --verbose > ./result/int32_256K_uniform_random_1.merge
	./benchmark --iteration=10 --dataset=./dataset/int32_512K_uniform_random_1 --method=merge --verbose > ./result/int32_512K_uniform_random_1.merge
	./benchmark --iteration=10 --dataset=./dataset/int32_1M_uniform_random_1   --method=merge --verbose > ./result/int32_1M_uniform_random_1.merge

benchmark-heap:
	./benchmark --iteration=10 --dataset=./dataset/int32_1K_uniform_random_1   --method=heap --verbose > ./result/int32_1K_uniform_random_1.heap
	./benchmark --iteration=10 --dataset=./dataset/int32_2K_uniform_random_1   --method=heap --verbose > ./result/int32_2K_uniform_random_1.heap
	./benchmark --iteration=10 --dataset=./dataset/int32_4K_uniform_random_1   --method=heap --verbose > ./result/int32_4K_uniform_random_1.heap
	./benchmark --iteration=10 --dataset=./dataset/int32_8K_uniform_random_1   --method=heap --verbose > ./result/int32_8K_uniform_random_1.heap
	./benchmark --iteration=10 --dataset=./dataset/int32_16K_uniform_random_1  --method=heap --verbose > ./result/int32_16K_uniform_random_1.heap
	./benchmark --iteration=10 --dataset=./dataset/int32_32K_uniform_random_1  --method=heap --verbose > ./result/int32_32K_uniform_random_1.heap
	./benchmark --iteration=10 --dataset=./dataset/int32_64K_uniform_random_1  --method=heap --verbose > ./result/int32_64K_uniform_random_1.heap
	./benchmark --iteration=10 --dataset=./dataset/int32_128K_uniform_random_1 --method=heap --verbose > ./result/int32_128K_uniform_random_1.heap
	./benchmark --iteration=10 --dataset=./dataset/int32_256K_uniform_random_1 --method=heap --verbose > ./result/int32_256K_uniform_random_1.heap
	./benchmark --iteration=10 --dataset=./dataset/int32_512K_uniform_random_1 --method=heap --verbose > ./result/int32_512K_uniform_random_1.heap
	./benchmark --iteration=10 --dataset=./dataset/int32_1M_uniform_random_1   --method=heap --verbose > ./result/int32_1M_uniform_random_1.heap

benchmark-quick:
	./benchmark --iteration=10 --dataset=./dataset/int32_1K_uniform_random_1   --method=quick --verbose > ./result/int32_1K_uniform_random_1.quick
	./benchmark --iteration=10 --dataset=./dataset/int32_2K_uniform_random_1   --method=quick --verbose > ./result/int32_2K_uniform_random_1.quick
	./benchmark --iteration=10 --dataset=./dataset/int32_4K_uniform_random_1   --method=quick --verbose > ./result/int32_4K_uniform_random_1.quick
	./benchmark --iteration=10 --dataset=./dataset/int32_8K_uniform_random_1   --method=quick --verbose > ./result/int32_8K_uniform_random_1.quick
	./benchmark --iteration=10 --dataset=./dataset/int32_16K_uniform_random_1  --method=quick --verbose > ./result/int32_16K_uniform_random_1.quick
	./benchmark --iteration=10 --dataset=./dataset/int32_32K_uniform_random_1  --method=quick --verbose > ./result/int32_32K_uniform_random_1.quick
	./benchmark --iteration=10 --dataset=./dataset/int32_64K_uniform_random_1  --method=quick --verbose > ./result/int32_64K_uniform_random_1.quick
	./benchmark --iteration=10 --dataset=./dataset/int32_128K_uniform_random_1 --method=quick --verbose > ./result/int32_128K_uniform_random_1.quick
	./benchmark --iteration=10 --dataset=./dataset/int32_256K_uniform_random_1 --method=quick --verbose > ./result/int32_256K_uniform_random_1.quick
	./benchmark --iteration=10 --dataset=./dataset/int32_512K_uniform_random_1 --method=quick --verbose > ./result/int32_512K_uniform_random_1.quick
	./benchmark --iteration=10 --dataset=./dataset/int32_1M_uniform_random_1   --method=quick --verbose > ./result/int32_1M_uniform_random_1.quick

benchmark-library:
	./benchmark --iteration=10 --dataset=./dataset/int32_1K_uniform_random_1   --method=library --verbose > ./result/int32_1K_uniform_random_1.library
	./benchmark --iteration=10 --dataset=./dataset/int32_2K_uniform_random_1   --method=library --verbose > ./result/int32_2K_uniform_random_1.library
	./benchmark --iteration=10 --dataset=./dataset/int32_4K_uniform_random_1   --method=library --verbose > ./result/int32_4K_uniform_random_1.library
	./benchmark --iteration=10 --dataset=./dataset/int32_8K_uniform_random_1   --method=library --verbose > ./result/int32_8K_uniform_random_1.library
	./benchmark --iteration=10 --dataset=./dataset/int32_16K_uniform_random_1  --method=library --verbose > ./result/int32_16K_uniform_random_1.library
	./benchmark --iteration=10 --dataset=./dataset/int32_32K_uniform_random_1  --method=library --verbose > ./result/int32_32K_uniform_random_1.library
	./benchmark --iteration=10 --dataset=./dataset/int32_64K_uniform_random_1  --method=library --verbose > ./result/int32_64K_uniform_random_1.library
	./benchmark --iteration=10 --dataset=./dataset/int32_128K_uniform_random_1 --method=library --verbose > ./result/int32_128K_uniform_random_1.library
	./benchmark --iteration=10 --dataset=./dataset/int32_256K_uniform_random_1 --method=library --verbose > ./result/int32_256K_uniform_random_1.library
	./benchmark --iteration=10 --dataset=./dataset/int32_512K_uniform_random_1 --method=library --verbose > ./result/int32_512K_uniform_random_1.library
	./benchmark --iteration=10 --dataset=./dataset/int32_1M_uniform_random_1   --method=library --verbose > ./result/int32_1M_uniform_random_1.library

benchmark-tim:
	./benchmark --iteration=10 --dataset=./dataset/int32_1K_uniform_random_1   --method=tim --verbose > ./result/int32_1K_uniform_random_1.tim
	./benchmark --iteration=10 --dataset=./dataset/int32_2K_uniform_random_1   --method=tim --verbose > ./result/int32_2K_uniform_random_1.tim
	./benchmark --iteration=10 --dataset=./dataset/int32_4K_uniform_random_1   --method=tim --verbose > ./result/int32_4K_uniform_random_1.tim
	./benchmark --iteration=10 --dataset=./dataset/int32_8K_uniform_random_1   --method=tim --verbose > ./result/int32_8K_uniform_random_1.tim
	./benchmark --iteration=10 --dataset=./dataset/int32_16K_uniform_random_1  --method=tim --verbose > ./result/int32_16K_uniform_random_1.tim
	./benchmark --iteration=10 --dataset=./dataset/int32_32K_uniform_random_1  --method=tim --verbose > ./result/int32_32K_uniform_random_1.tim
	./benchmark --iteration=10 --dataset=./dataset/int32_64K_uniform_random_1  --method=tim --verbose > ./result/int32_64K_uniform_random_1.tim
	./benchmark --iteration=10 --dataset=./dataset/int32_128K_uniform_random_1 --method=tim --verbose > ./result/int32_128K_uniform_random_1.tim
	./benchmark --iteration=10 --dataset=./dataset/int32_256K_uniform_random_1 --method=tim --verbose > ./result/int32_256K_uniform_random_1.tim
	./benchmark --iteration=10 --dataset=./dataset/int32_512K_uniform_random_1 --method=tim --verbose > ./result/int32_512K_uniform_random_1.tim
	./benchmark --iteration=10 --dataset=./dataset/int32_1M_uniform_random_1   --method=tim --verbose > ./result/int32_1M_uniform_random_1.tim

benchmark-cocktail:
	./benchmark --iteration=10 --dataset=./dataset/int32_1K_uniform_random_1   --method=cocktail --verbose > ./result/int32_1K_uniform_random_1.cocktail
	./benchmark --iteration=10 --dataset=./dataset/int32_2K_uniform_random_1   --method=cocktail --verbose > ./result/int32_2K_uniform_random_1.cocktail
	./benchmark --iteration=10 --dataset=./dataset/int32_4K_uniform_random_1   --method=cocktail --verbose > ./result/int32_4K_uniform_random_1.cocktail
	./benchmark --iteration=10 --dataset=./dataset/int32_8K_uniform_random_1   --method=cocktail --verbose > ./result/int32_8K_uniform_random_1.cocktail
	./benchmark --iteration=10 --dataset=./dataset/int32_16K_uniform_random_1  --method=cocktail --verbose > ./result/int32_16K_uniform_random_1.cocktail
	./benchmark --iteration=10 --dataset=./dataset/int32_32K_uniform_random_1  --method=cocktail --verbose > ./result/int32_32K_uniform_random_1.cocktail
	./benchmark --iteration=10 --dataset=./dataset/int32_64K_uniform_random_1  --method=cocktail --verbose > ./result/int32_64K_uniform_random_1.cocktail
	./benchmark --iteration=10 --dataset=./dataset/int32_128K_uniform_random_1 --method=cocktail --verbose > ./result/int32_128K_uniform_random_1.cocktail
	./benchmark --iteration=10 --dataset=./dataset/int32_256K_uniform_random_1 --method=cocktail --verbose > ./result/int32_256K_uniform_random_1.cocktail
	./benchmark --iteration=10 --dataset=./dataset/int32_512K_uniform_random_1 --method=cocktail --verbose > ./result/int32_512K_uniform_random_1.cocktail
	./benchmark --iteration=10 --dataset=./dataset/int32_1M_uniform_random_1   --method=cocktail --verbose > ./result/int32_1M_uniform_random_1.cocktail

benchmark-shaker:
	./benchmark --iteration=10 --dataset=./dataset/int32_1K_uniform_random_1   --method=shaker --verbose > ./result/int32_1K_uniform_random_1.shaker
	./benchmark --iteration=10 --dataset=./dataset/int32_2K_uniform_random_1   --method=shaker --verbose > ./result/int32_2K_uniform_random_1.shaker
	./benchmark --iteration=10 --dataset=./dataset/int32_4K_uniform_random_1   --method=shaker --verbose > ./result/int32_4K_uniform_random_1.shaker
	./benchmark --iteration=10 --dataset=./dataset/int32_8K_uniform_random_1   --method=shaker --verbose > ./result/int32_8K_uniform_random_1.shaker
	./benchmark --iteration=10 --dataset=./dataset/int32_16K_uniform_random_1  --method=shaker --verbose > ./result/int32_16K_uniform_random_1.shaker
	./benchmark --iteration=10 --dataset=./dataset/int32_32K_uniform_random_1  --method=shaker --verbose > ./result/int32_32K_uniform_random_1.shaker
	./benchmark --iteration=10 --dataset=./dataset/int32_64K_uniform_random_1  --method=shaker --verbose > ./result/int32_64K_uniform_random_1.shaker
	./benchmark --iteration=10 --dataset=./dataset/int32_128K_uniform_random_1 --method=shaker --verbose > ./result/int32_128K_uniform_random_1.shaker
	./benchmark --iteration=10 --dataset=./dataset/int32_256K_uniform_random_1 --method=shaker --verbose > ./result/int32_256K_uniform_random_1.shaker
	./benchmark --iteration=10 --dataset=./dataset/int32_512K_uniform_random_1 --method=shaker --verbose > ./result/int32_512K_uniform_random_1.shaker
	./benchmark --iteration=10 --dataset=./dataset/int32_1M_uniform_random_1   --method=shaker --verbose > ./result/int32_1M_uniform_random_1.shaker

benchmark-comb:
	./benchmark --iteration=10 --dataset=./dataset/int32_1K_uniform_random_1   --method=comb --verbose > ./result/int32_1K_uniform_random_1.comb
	./benchmark --iteration=10 --dataset=./dataset/int32_2K_uniform_random_1   --method=comb --verbose > ./result/int32_2K_uniform_random_1.comb
	./benchmark --iteration=10 --dataset=./dataset/int32_4K_uniform_random_1   --method=comb --verbose > ./result/int32_4K_uniform_random_1.comb
	./benchmark --iteration=10 --dataset=./dataset/int32_8K_uniform_random_1   --method=comb --verbose > ./result/int32_8K_uniform_random_1.comb
	./benchmark --iteration=10 --dataset=./dataset/int32_16K_uniform_random_1  --method=comb --verbose > ./result/int32_16K_uniform_random_1.comb
	./benchmark --iteration=10 --dataset=./dataset/int32_32K_uniform_random_1  --method=comb --verbose > ./result/int32_32K_uniform_random_1.comb
	./benchmark --iteration=10 --dataset=./dataset/int32_64K_uniform_random_1  --method=comb --verbose > ./result/int32_64K_uniform_random_1.comb
	./benchmark --iteration=10 --dataset=./dataset/int32_128K_uniform_random_1 --method=comb --verbose > ./result/int32_128K_uniform_random_1.comb
	./benchmark --iteration=10 --dataset=./dataset/int32_256K_uniform_random_1 --method=comb --verbose > ./result/int32_256K_uniform_random_1.comb
	./benchmark --iteration=10 --dataset=./dataset/int32_512K_uniform_random_1 --method=comb --verbose > ./result/int32_512K_uniform_random_1.comb
	./benchmark --iteration=10 --dataset=./dataset/int32_1M_uniform_random_1   --method=comb --verbose > ./result/int32_1M_uniform_random_1.comb

benchmark-tournament:
	./benchmark --iteration=10 --dataset=./dataset/int32_1K_uniform_random_1   --method=tournament --verbose > ./result/int32_1K_uniform_random_1.tournament
	./benchmark --iteration=10 --dataset=./dataset/int32_2K_uniform_random_1   --method=tournament --verbose > ./result/int32_2K_uniform_random_1.tournament
	./benchmark --iteration=10 --dataset=./dataset/int32_4K_uniform_random_1   --method=tournament --verbose > ./result/int32_4K_uniform_random_1.tournament
	./benchmark --iteration=10 --dataset=./dataset/int32_8K_uniform_random_1   --method=tournament --verbose > ./result/int32_8K_uniform_random_1.tournament
	./benchmark --iteration=10 --dataset=./dataset/int32_16K_uniform_random_1  --method=tournament --verbose > ./result/int32_16K_uniform_random_1.tournament
	./benchmark --iteration=10 --dataset=./dataset/int32_32K_uniform_random_1  --method=tournament --verbose > ./result/int32_32K_uniform_random_1.tournament
	./benchmark --iteration=10 --dataset=./dataset/int32_64K_uniform_random_1  --method=tournament --verbose > ./result/int32_64K_uniform_random_1.tournament
	./benchmark --iteration=10 --dataset=./dataset/int32_128K_uniform_random_1 --method=tournament --verbose > ./result/int32_128K_uniform_random_1.tournament
	./benchmark --iteration=10 --dataset=./dataset/int32_256K_uniform_random_1 --method=tournament --verbose > ./result/int32_256K_uniform_random_1.tournament
	./benchmark --iteration=10 --dataset=./dataset/int32_512K_uniform_random_1 --method=tournament --verbose > ./result/int32_512K_uniform_random_1.tournament
	./benchmark --iteration=10 --dataset=./dataset/int32_1M_uniform_random_1   --method=tournament --verbose > ./result/int32_1M_uniform_random_1.tournament

benchmark-introsort:
	./benchmark --iteration=10 --dataset=./dataset/int32_1K_uniform_random_1   --method=introsort --verbose > ./result/int32_1K_uniform_random_1.introsort
	./benchmark --iteration=10 --dataset=./dataset/int32_2K_uniform_random_1   --method=introsort --verbose > ./result/int32_2K_uniform_random_1.introsort
	./benchmark --iteration=10 --dataset=./dataset/int32_4K_uniform_random_1   --method=introsort --verbose > ./result/int32_4K_uniform_random_1.introsort
	./benchmark --iteration=10 --dataset=./dataset/int32_8K_uniform_random_1   --method=introsort --verbose > ./result/int32_8K_uniform_random_1.introsort
	./benchmark --iteration=10 --dataset=./dataset/int32_16K_uniform_random_1  --method=introsort --verbose > ./result/int32_16K_uniform_random_1.introsort
	./benchmark --iteration=10 --dataset=./dataset/int32_32K_uniform_random_1  --method=introsort --verbose > ./result/int32_32K_uniform_random_1.introsort
	./benchmark --iteration=10 --dataset=./dataset/int32_64K_uniform_random_1  --method=introsort --verbose > ./result/int32_64K_uniform_random_1.introsort
	./benchmark --iteration=10 --dataset=./dataset/int32_128K_uniform_random_1 --method=introsort --verbose > ./result/int32_128K_uniform_random_1.introsort
	./benchmark --iteration=10 --dataset=./dataset/int32_256K_uniform_random_1 --method=introsort --verbose > ./result/int32_256K_uniform_random_1.introsort
	./benchmark --iteration=10 --dataset=./dataset/int32_512K_uniform_random_1 --method=introsort --verbose > ./result/int32_512K_uniform_random_1.introsort
	./benchmark --iteration=10 --dataset=./dataset/int32_1M_uniform_random_1   --method=introsort --verbose > ./result/int32_1M_uniform_random_1.introsort

benchmark-run:
	$(MAKE) benchmark-bubble
	$(MAKE) benchmark-selection
	$(MAKE) benchmark-insertion
	$(MAKE) benchmark-merge
	$(MAKE) benchmark-heap
	$(MAKE) benchmark-quick
	$(MAKE) benchmark-library
	$(MAKE) benchmark-tim
	$(MAKE) benchmark-cocktail
	$(MAKE) benchmark-shaker
	$(MAKE) benchmark-comb
	$(MAKE) benchmark-tournament
	$(MAKE) benchmark-introsort

benchmark-clean:
	rm -rf ./result/*
	echo "timestamp,sorting method,N,data bits,distribution,order,iteration,mean elapsed time (ms),#(array accesses) / iteration,#(comparisons) / iteration" > benchmark_result.csv

.PHONY: all clean debug release benchmark datagen datagen-clean datagen-run benchmark-clean benchmark-run benchmark-bubble benchmark-selection benchmark-insertion benchmark-merge benchmark-heap benchmark-quick benchmark-library benchmark-tim benchmark-cocktail benchmark-shaker benchmark-comb benchmark-tournament benchmark-introsort