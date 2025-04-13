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

RANDOM_SEED := 20231386
ITERATION := 10
METHOD := merge heap bubble insertion selection quick library tim cocktail shaker comb tournament introsort
DISTRIBUTION := uniform normal bimodal

N := 1K 2K 4K 8K 16K 32K 64K 128K 256K 512K 1M
DATASET_N_UNIFORM_RANDOM := ./dataset_N_uniform_random
DATASET_N_UNIFORM_RANDOM_FILES := $(basename $(notdir $(wildcard $(DATASET_N_UNIFORM_RANDOM)/*.unsorted)))

PATTERN := random almost noise reversed sawtooth bitonic frontsorted gap
DATASET_1M_DIST_PATTERN := ./dataset_1M_dist_pattern
DATASET_1M_DIST_PATTERN_FILES := $(basename $(notdir $(wildcard $(DATASET_1M_DIST_PATTERN)/*.unsorted)))

datagen-n-uniform-random:
	@mkdir -p $(DATASET_N_UNIFORM_RANDOM)
	@$(foreach x, $(N), ./datagen --seed=$(RANDOM_SEED) --N=$(x) --path=$(DATASET_N_UNIFORM_RANDOM);)
	@python3 plot.py --path=$(DATASET_N_UNIFORM_RANDOM)

datagen-1m-dist-pattern:
	@mkdir -p $(DATASET_1M_DIST_PATTERN)
	@$(foreach dist, $(DISTRIBUTION), \
		$(foreach pattern, $(PATTERN), \
			./datagen --seed=$(RANDOM_SEED) --N=1M --path=$(DATASET_1M_DIST_PATTERN) --dist=$(dist) --pattern=$(pattern);))
	@python3 plot.py --path=$(DATASET_1M_DIST_PATTERN)

benchmark-n-uniform-random:
	@mkdir -p $(DATASET_N_UNIFORM_RANDOM)/result
	@$(foreach method, $(METHOD), \
		$(foreach filename, $(DATASET_N_UNIFORM_RANDOM_FILES), \
			./benchmark --iteration=$(ITERATION) --dataset=$(DATASET_N_UNIFORM_RANDOM)/$(filename) --method=$(method) --verbose > $(DATASET_N_UNIFORM_RANDOM)/result/$(filename).$(method);))

benchmark-1m-dist-pattern:
	@mkdir -p $(DATASET_1M_DIST_PATTERN)/result
	@$(foreach method, $(METHOD), \
		$(foreach filename, $(DATASET_1M_DIST_PATTERN_FILES), \
			./benchmark --iteration=$(ITERATION) --dataset=$(DATASET_1M_DIST_PATTERN)/$(filename) --method=$(method) --verbose > $(DATASET_1M_DIST_PATTERN)/result/$(filename).$(method);))

benchmark-clean:
	echo "timestamp,sorting method,N,data bits,distribution,order,iteration,mean elapsed time (ms),#(array accesses) / iteration,#(comparisons) / iteration" > benchmark_result.csv

.PHONY: all clean debug release benchmark datagen datagen-n-uniform-random benchmark-n-uniform-random datagen-1m-dist-pattern benchmark-1m-dist-pattern