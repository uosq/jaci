CC       := gcc
CXX      := g++

COMMON_FLAGS := -g -O0

CFLAGS   := -fPIC $(COMMON_FLAGS) -DFUNCHOOK_USE_DISTORM -DDISASM_DISTORM

CXXFLAGS := -fPIC -shared --std=c++20 $(COMMON_FLAGS) -fno-omit-frame-pointer -fasynchronous-unwind-tables -DDEBUG

LDFLAGS  := -lSDL2 -lvulkan

SRC_DIR        := src
THIRDPARTY_DIR := thirdparty
BUILD_DIR      := build

TARGET := $(BUILD_DIR)/libjaci.so

PROJECT_SRCS := $(shell find $(SRC_DIR) -name "*.cpp")
PROJECT_SRCS += $(shell find $(THIRDPARTY_DIR) -name "*.cpp")

THIRDPARTY_SRCS := \
	$(THIRDPARTY_DIR)/libsigscan/libsigscan.c \
    $(THIRDPARTY_DIR)/funchook/funchook.c \
    $(THIRDPARTY_DIR)/funchook/funchook_unix.c \
    $(THIRDPARTY_DIR)/funchook/funchook_x86.c \
    $(THIRDPARTY_DIR)/funchook/disasm_distorm.c \
    $(THIRDPARTY_DIR)/distorm/decoder.c \
    $(THIRDPARTY_DIR)/distorm/distorm.c \
    $(THIRDPARTY_DIR)/distorm/instructions.c \
    $(THIRDPARTY_DIR)/distorm/insts.c \
    $(THIRDPARTY_DIR)/distorm/mnemonics.c \
    $(THIRDPARTY_DIR)/distorm/operands.c \
    $(THIRDPARTY_DIR)/distorm/prefix.c \
    $(THIRDPARTY_DIR)/distorm/textdefs.c

OBJS := $(patsubst %.cpp,$(BUILD_DIR)/%.o,$(PROJECT_SRCS))
OBJS += $(patsubst %.c,$(BUILD_DIR)/%.o,$(THIRDPARTY_SRCS))

all: $(TARGET)

$(TARGET): $(OBJS)
	@echo "Linking started..."
	@mkdir -p $(BUILD_DIR)
	$(CXX) $(CXXFLAGS) $(OBJS) -o $(TARGET) $(LDFLAGS)
	@echo "Compilation and linking successfully ended!"

$(BUILD_DIR)/%.o: %.cpp
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(BUILD_DIR)/%.o: %.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -rf $(BUILD_DIR)

.PHONY: all clean