##### global settings #####

.PHONY: nemu entry testcase kernel run run-c gdb test submit clean

CC := gcc
LD := ld
CFLAGS := -MMD -Wall -Werror -c

LIB_COMMON_DIR := lib-common
LIBC_INC_DIR := $(LIB_COMMON_DIR)/uclibc/include
LIBC_LIB_DIR := $(LIB_COMMON_DIR)/uclibc/lib
LIBC := $(LIBC_LIB_DIR)/libc.a
FLOAT_DIR := $(LIB_COMMON_DIR)/FLOAT
FLOAT_SRC := $(FLOAT_DIR)/FLOAT.c
FLOAT_VFPRINTF_SRC := $(FLOAT_DIR)/FLOAT_vfprintf.c
FLOAT_OBJ := obj/$(FLOAT_DIR)/FLOAT.o
FLOAT_VFPRINTF_OBJ := obj/$(FLOAT_DIR)/FLOAT_vfprintf.o
FLOAT := obj/$(FLOAT_DIR)/FLOAT.a

include config/Makefile.git
include config/Makefile.build

all: nemu


##### rules for building the project #####

include nemu/Makefile.part
include testcase/Makefile.part
include kernel/Makefile.part
include game/Makefile.part

# 编译 FLOAT 库
FLOAT_CFLAGS := -m32 -O2 -fno-builtin -fno-stack-protector -U_FORTIFY_SOURCE -D_FORTIFY_SOURCE=0 -I$(LIB_COMMON_DIR)

$(FLOAT_OBJ): $(FLOAT_SRC)
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) $(FLOAT_CFLAGS) $< -o $@

$(FLOAT_VFPRINTF_OBJ): $(FLOAT_VFPRINTF_SRC)
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) $(FLOAT_CFLAGS) $< -o $@

$(FLOAT): $(FLOAT_OBJ) $(FLOAT_VFPRINTF_OBJ)
	ar rcs $@ $^

nemu: $(nemu_BIN)
testcase: $(testcase_BIN)
kernel: $(kernel_BIN)
game: $(game_BIN)


##### rules for cleaning the project #####

clean-nemu:
	-rm -rf obj/nemu 2> /dev/null

clean-testcase:
	-rm -rf obj/testcase 2> /dev/null

clean-kernel:
	-rm -rf obj/kernel 2> /dev/null

clean-game:
	-rm -rf obj/game 2> /dev/null

clean-float:
	-rm -rf obj/$(FLOAT_DIR) 2> /dev/null

clean: clean-cpp clean-float
	-rm -rf obj 2> /dev/null
	-rm -f *log.txt entry $(FLOAT) 2> /dev/null


##### some convinient rules #####

USERPROG := obj/testcase/add
ENTRY := $(kernel_BIN)
entry: $(ENTRY)
	objcopy -S -O binary $(ENTRY) entry

run: $(nemu_BIN) $(USERPROG) entry
ifeq ($(AUTO),1)
	printf "c\nq\n" | $(nemu_BIN) $(USERPROG)
else
	$(nemu_BIN) $(USERPROG)
endif

run-c: $(nemu_BIN) $(USERPROG) entry
	printf "c\nq\n" | $(nemu_BIN) $(USERPROG)

gdb: $(nemu_BIN) $(USERPROG) entry
	gdb -s $(nemu_BIN) --args $(nemu_BIN) $(USERPROG)

test: $(nemu_BIN) $(testcase_BIN) entry
	bash test.sh $(testcase_BIN)

submit: clean
	cd .. && zip -r $(STU_ID).zip $(shell pwd | grep -o '[^/]*$$')
