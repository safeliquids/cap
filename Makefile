# compiler programs
CC:=gcc
CCFLAGS:=-Wall -Wextra -Wformat-security -pedantic -std=c99 -g
LD:=gcc
LDFLAGS:=-std=c99 -pedantic -Wall -Wextra

# slicer
SLICER_DIR:=slicer_v2
SLICER:=slicer.exe

# source and header files
HEADERS_DIR:=headers
SOURCES_DIR:=sources
PARTS:=flag_info helper_functions parsed_arguments parser positional_info \
	typed_union
HEADERS:=$(patsubst %, $(HEADERS_DIR)/%.h, $(PARTS))
SOURCES:=$(patsubst %, $(SOURCES_DIR)/%.c, $(PARTS))

# test config
TEST_SRC_DIR:=test/src
TEST_INC_DIR:=test/include
TEST_OBJ_DIR:=test/obj
TEST_BIN_DIR:=test/bin

TEST_CAP_H:=$(TEST_INC_DIR)/cap_test.h
TEST_CAP_C:=$(TEST_SRC_DIR)/cap_test.c
TEST_CAP_O:=$(TEST_OBJ_DIR)/cap_test.o

TESTS:=typed_union pa_flags pa_positionals parser_flags_1 parser_flags_2 \
	parser_2 parser_3 parser_config_1 parser_help
TEST_TARGETS:=$(patsubst %,test.%,$(TESTS))
TEST_UNITS:=$(patsubst %,test_%,$(TESTS))
TEST_SOURCES:=$(patsubst %,$(TEST_SRC_DIR)/%.c,$(TEST_UNITS))
TEST_OBJS:=$(TEST_OBJ_DIR)/test.o $(TEST_CAP_O)
TEST_BINS:=$(patsubst %,$(TEST_BIN_DIR)/%.exe,$(TEST_UNITS))

# usefule information for cleaning
GENERATED:=cap.h cap.c $(SLICER) $(TEST_CAP_C) $(TEST_CAP_H) $(TEST_BINS) \
	$(TEST_OBJS)
GENERATED_DIRS:=$(TEST_BIN_DIR) $(TEST_OBJ_DIR)
COMMA:=,

all: cap.h cap.c

cap.h cap.c &: $(SLICER) public.symbols $(HEADERS) $(SOURCES)
	./$(SLICER) -H cap.h -C cap.c -S public.symbols $(HEADERS) $(SOURCES)

$(TEST_CAP_C) $(TEST_CAP_H) &: $(SLICER) $(HEADERS) $(SOURCES)
	./$(SLICER) -H $(TEST_CAP_H) -C $(TEST_CAP_C)  $(HEADERS) $(SOURCES)

$(SLICER): $(SLICER_DIR)/*.c $(SLICER_DIR)/*.h
	$(CC) $(CCFLAGS) -I $(SLICER_DIR) -o $@ $(SLICER_DIR)/*.c

test: $(TEST_TARGETS)

# this is a static pattern rule, those are pretty awesome!
$(TEST_TARGETS): test.%: $(TEST_BIN_DIR)/test_%.exe
	./$<

$(TEST_OBJ_DIR)/test.o: $(TEST_SRC_DIR)/test.c $(TEST_INC_DIR)/test.h | $(TEST_OBJ_DIR)
	$(CC) $(CCFLAGS) -I$(TEST_INC_DIR) -c -o $@ $<

$(TEST_CAP_O): $(TEST_CAP_C) $(TEST_CAP_H) | $(TEST_OBJ_DIR)
	$(CC) $(CCFLAGS) -i$(TEST_INC_DIR) -o $@ $<

$(TEST_BIN_DIR)/test_%.exe: $(TEST_SRC_DIR)/test_%.c $(TEST_OBJS) $(TEST_INC_DIR)/test.h | $(TEST_BIN_DIR)
	$(CC) $(CCFLAGS) -i$(TEST_INC_DIR) -o $@ $^

$(TEST_BIN_DIR):
	mkdir $@

$(TEST_OBJ_DIR):
	mkdir $@
	
clean:
ifeq ($(OS), Windows_NT)
	pwsh -c 'remove-item $(firstword $(GENERATED)) $(patsubst %,$(COMMA) %,$(wordlist 2, 1000, $(GENERATED))) || {};'
	pwsh -c 'remove-item $(firstword $(GENERATED_DIRS)) $(patsubst %,$(COMMA) %,$(wordlist 2, 1000, $(GENERATED_DIRS))) || {};'
else
	rm -f $(GENERATED)
	rm -f -d $(GENERATED_DIRS)
endif

.PHONY: all clean test $(TEST_TARGETS)
