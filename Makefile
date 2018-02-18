# Copyright (C) Omar Thor <omarthoro@gmail.com> - All Rights Reserved
# Unauthorized copying of this file, via any medium is strictly prohibited
# Proprietary and confidential
#
# Written by Omar Thor <omarthoro@gmail.com>, 2017


BUILD_DIR := build
HEADERS_DIRS := include
TEST_SRCS_DIRS := test

HEADERS := $(shell find $(HEADERS_DIR) -name *.hpp)

TEST_SRCS := $(shell find $(TEST_SRCS_DIRS) -name *.cpp)
TESTS := $(TEST_SRCS:%.cpp=$(BUILD_DIR)/%.bin)

DEPS := $(TESTS:=.d)

LDFLAGS := -pthread -lm -lboost_system -lboost_filesystem
TEST_LDFLAGS := $(LDFLAGS) -lboost_unit_test_framework

INC_DIRS := include /usr/include/eigen3
INC_FLAGS := $(addprefix -I,$(INC_DIRS))
INC_TEST_FLAGS := $(INC_FLAGS)

CCPFLAGS_MODE_HARDENER =  -fstack-protector-all
CCPFLAGS_MODE_DEBUG :=  -g -Og $(CCPFLAGS_MODE_HARDENER)
#CCPFLAGS_MODE_RELEASE :=  -O3

CPPFLAGS := -fdiagnostics-color=auto -std=c++17 -Wall -Wpedantic -fopenmp -pipe -MMD -MP $(CCPFLAGS_MODE_DEBUG)
CCPOBJFLAGS := $(CPPFLAGS) $(INC_FLAGS)
CCPTESTFLAGS := $(CPPFLAGS) $(TEST_LDFLAGS) $(CPPFLAGS) $(INC_TEST_FLAGS) -DBOOST_TEST_DYN_LINK

MKDIR_P ?= mkdir -p

.PHONY: clean all resources

all: resources $(OBJS) $(TESTS)

resources:
	bash -c ./resources/fetch.sh

# c++ source
$(BUILD_DIR)/src/%.cpp.o: src/%.cpp  $(HEADERS)
	$(MKDIR_P) $(dir $@)
	$(CXX) $(CCPOBJFLAGS) $(CXXFLAGS) $(LDFLAGS) -c $< -o $@

## c++ test source
$(BUILD_DIR)/test/%.bin : test/%.cpp $(HEADERS)
	$(MKDIR_P) $(dir $@)
	$(CXX) $(CCPTESTFLAGS) $(CXXFLAGS) $(TEST_LDFLAGS) $(OBJS) $< -o $@

doc: Doxyfile
	doxygen

clean:
	$(RM) -r $(BUILD_DIR) doc

-include $(DEPS)



