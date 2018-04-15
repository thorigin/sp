# Copyright (C) Omar Thor <omarthoro@gmail.com> - All Rights Reserved
# Unauthorized copying of this file, via any medium is strictly prohibited
# Proprietary and confidential
#
# Written by Omar Thor <omarthoro@gmail.com>, 2017


BUILD_DIR := build
HEADERS_DIRS := include
TEST_SRCS_DIRS := test
EXAMPLE_SRCS_DIRS := examples

HEADERS := $(shell find $(HEADERS_DIR) -name *.hpp)

TEST_SRCS := $(shell find $(TEST_SRCS_DIRS) -name *.cpp)
TESTS := $(TEST_SRCS:%.cpp=$(BUILD_DIR)/%.bin)

EXAMPLES_SRCS := $(shell find $(EXAMPLE_SRCS_DIRS) -name *.cpp)
EXAMPLES := $(EXAMPLES_SRCS:%.cpp=$(BUILD_DIR)/%.bin)

DEPS := $(TESTS:=.d)

LDFLAGS := -pthread -lm -lboost_system -lboost_filesystem
TEST_LDFLAGS := $(LDFLAGS) -lboost_unit_test_framework -DBOOST_TEST_DYN_LINK

INC_DIRS := include /usr/include/eigen3
INC_FLAGS := $(addprefix -I,$(INC_DIRS))
INC_TEST_FLAGS := $(INC_FLAGS)

# Example optimizations
CPPFLAGS_MODE_EXAMPLES :=  -O3 -flto -march=native -mavx -DNDEBUG
# Test flags
CPPFLAGS_MODE_DEBUG := -g -O0

CPPFLAGS := -fdiagnostics-color=auto -std=c++17 -Wall -Wpedantic -fopenmp -pipe -MMD -MP
CCPTESTFLAGS := $(CPPFLAGS) $(CPPFLAGS_MODE_DEBUG) $(TEST_LDFLAGS) $(INC_TEST_FLAGS)
CPPEXAMPLEFLAGS := $(CPPFLAGS) $(CPPFLAGS_MODE_EXAMPLES) $(LDFLAGS) $(INC_TEST_FLAGS)

MKDIR_P ?= mkdir -p

.PHONY: clean all resources

all: resources $(TESTS) $(EXAMPLES)

resources:
	bash -c ./resources/fetch.sh

# Examples
$(BUILD_DIR)/examples/%.bin: examples/%.cpp $(HEADERS)
	@$(MKDIR_P) $(dir $@)
	$(CXX) $(CPPEXAMPLEFLAGS) $< -o $@

# Tests
$(BUILD_DIR)/test/%.bin : test/%.cpp $(HEADERS)
	@$(MKDIR_P) $(dir $@)
	$(CXX) $(CCPTESTFLAGS) $(OBJS) $< -o $@

doc: Doxyfile
	doxygen

clean:
	$(RM) -r $(BUILD_DIR) doc

-include $(DEPS)
