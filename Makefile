.PHONY:all clean

export BASE_PATH = $(shell pwd)

export BASE_OBJ_PATH = $(BASE_PATH)/obj
export BASE_DEP_PATH = $(BASE_PATH)/dep

ALL_DEP_FILE = $(shell find $(BASE_PATH) -name \*.d)
ALL_OBJ_FILE = $(shell find $(BASE_PATH) -name \*.o)


export CFLAGS = -g $(addprefix -I $(BASE_PATH)/,$(SUB_DIR))
export CC = gcc
MAKE = make

SUB_DIR = stm serial test


all:stm 

stm :TRIGGER
	$(MAKE) -C $@

TRIGGER:

test.elf :TRIGGER
	$(CC)  $(shell find $(BASE_PATH) -name *.o)  -o $@

clean:
	-rm -rf $(BASE_OBJ_PATH)
	-rm -rf $(ALL_DEP_FILE)
	-rm -rf $(ALL_OBJ_FILE)

objtest:
	gcc $(CFLAGS) $(shell find $(BASE_PATH) -name *.c) -o test.elf
