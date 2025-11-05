#!/usr/bin/make -f

#-------------------------------------------------------------------------------
# Project layout
#-------------------------------------------------------------------------------
SRC_DIR := src
INC_DIR := include
BUILD_DIR := build

CC := gcc
CXX := g++

SRCS := $(wildcard $(SRC_DIR)/*.cpp)
HDRS := $(wildcard $(INC_DIR)/*.h)

SVR_SRCS := $(wildcard $(SRC_DIR)/Server*.cpp)
CLNT_SRCS := $(wildcard $(SRC_DIR)/Client*.cpp)
CMN_SRCS := $(filter-out $(SVR_SRCS) $(CLNT_SRCS), $(SRCS))

SVR_OBJS := $(SVR_SRCS:$(SRC_DIR)/%.cpp=$(BUILD_DIR)/%.o)
CLNT_OBJS := $(CLNT_SRCS:$(SRC_DIR)/%.cpp=$(BUILD_DIR)/%.o)
CMN_OBJS := $(CMN_SRCS:$(SRC_DIR)/%.cpp=$(BUILD_DIR)/%.o)

CXXFLAGS := -Wall -std=c++11 -I$(INC_DIR)
LDFLAGS := -pthread
TARGET := server client

all: $(TARGET)

debug: CXXFLAGS += -ggdb -DDEBUG
debug: $(TARGET)

server: $(SVR_OBJS) $(CMN_OBJS)
	$(CXX) $(LDFLAGS) -o $@ $^

client: $(CLNT_OBJS) $(CMN_OBJS)
	$(CXX) $(LDFLAGS) -o $@ $^

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.cpp $(HDRS) | $(BUILD_DIR)
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

clean:
	rm -rf $(BUILD_DIR) $(TARGET)

.PHONY: all debug clean
