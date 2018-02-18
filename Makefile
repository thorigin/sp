# Copyright (C) Omar Thor <omarthoro@gmail.com> - All Rights Reserved
# Unauthorized copying of this file, via any medium is strictly prohibited
# Proprietary and confidential
#
# Written by Omar Thor <omarthoro@gmail.com>, 2017


BUILD_DIR := build
SRC_DIRS := src
HEADERS_DIRS := include
TEST_SRCS_DIRS := test

HEADERS := $(shell find $(HEADERS_DIR) -name *.hpp)
SRCS := $(shell find $(SRC_DIRS) -not -name main.cpp -and \( -name *.cpp -or -name *.c -or -name *.s \))
OBJS := $(SRCS:%=$(BUILD_DIR)/%.o)

TEST_SRCS := $(shell find $(TEST_SRCS_DIRS) -name *.cpp)
TESTS := $(TEST_SRCS:%.cpp=$(BUILD_DIR)/%.bin)

DEPS := $(OBJS:.o=.d) $(TESTS:=.d)

LDFLAGS := -pthread -lm
TEST_LDFLAGS := $(LDFLAGS) $(LDFLAGS) -lboost_unit_test_framework

INC_DIRS := include
INC_FLAGS := $(addprefix -I,$(INC_DIRS))
INC_TEST_FLAGS := $(INC_FLAGS)

CPPFLAGS := -fdiagnostics-color=always -std=c++17 -Wall -Wpedantic -fopenmp -g3 -MMD -MP
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



