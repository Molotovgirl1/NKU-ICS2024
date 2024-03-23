NAME = nemu
INC_DIR += ./include
BUILD_DIR ?= ./build
OBJ_DIR ?= $(BUILD_DIR)/obj
BINARY ?= $(BUILD_DIR)/$(NAME)
COUNT_LINE_NOW := $ ( shell find . - name "*.h" -or - name "*.c" | xargs cat | wc -l )
COUNT_ADD := $ ( shell expr $ ( COUNT_LINE_NOW ) - 2817)

include Makefile.git

.DEFAULT_GOAL = app

# Compilation flags
CC = gcc
LD = gcc
INCLUDES  = $(addprefix -I, $(INC_DIR))
CFLAGS   += -O2 -MMD -Wall -Werror -ggdb $(INCLUDES)

# Files to be compiled
SRCS = $(shell find src/ -name "*.c")
OBJS = $(SRCS:src/%.c=$(OBJ_DIR)/%.o)

# Compilation patterns
$(OBJ_DIR)/%.o: src/%.c
	@echo + CC $<
	@mkdir -p $(dir $@)
	@$(CC) $(CFLAGS) -c -o $@ $<

# Depencies
-include $(OBJS:.o=.d)

# Some convinient rules

.PHONY: app run submit clean count
app: $(BINARY)

ARGS ?= -l $(BUILD_DIR)/nemu-log.txt

# Command to execute NEMU
NEMU_EXEC := $(BINARY) $(ARGS)

$(BINARY): $(OBJS)
	@echo + LD $@
	@$(LD) -O2 -o $@ $^ -lSDL2 -lreadline

run: $(BINARY)
	$(call git_commit, "run")
	$(NEMU_EXEC)

gdb: $(BINARY)
	$(call git_commit, "gdb")
	gdb -s $(BINARY) --args $(NEMU_EXEC)

iiclean: 
	rm -rf $(BUILD_DIR)
	
count:
	@echo "  $( COUNT_LINE_NOW )lines in nemu now !"
	@echo "  $( COUNT_ADD ) lines added now !"
	
