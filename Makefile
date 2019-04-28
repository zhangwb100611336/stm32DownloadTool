.PHONY:all clean

export BASE_PATH = $(shell pwd)

export BASE_OBJ_PATH = $(BASE_PATH)/obj
export BASE_DEP_PATH = $(BASE_PATH)/.dep

ALL_DEP_FILE = $(shell find $(BASE_PATH) -name \*.d)
ALL_OBJ_FILE = $(shell find $(BASE_PATH) -name \*.o)


export CFLAGS = -g $(addprefix -I $(BASE_PATH)/,$(SUB_DIR))
export CC = gcc
MAKE = make

SUB_DIR = stm serial test

TARGET = test.elf

all:$(TARGET) 

$(TARGET):$(SUB_DIR)
	$(CC) $(shell find $(addprefix $(BASE_OBJ_PATH)/,$^) -name *.o) -o $@

$(SUB_DIR) :TRIGGER
	$(MAKE) -C $@

TRIGGER:

clean:
	-rm -rf $(BASE_OBJ_PATH)
	-rm -rf $(BASE_DEP_PATH)
	-rm -rf $(TARGET)

objtest:
	gcc $(CFLAGS) $(shell find $(BASE_PATH) -name *.c) -o test.elf
