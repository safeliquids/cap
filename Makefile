CC:=gcc
CCFLAGS:=-Wall -Wextra -pedantic -std=c99 -g -I.
LD:=gcc
LDFLAGS:=

INC_DIR:=headers
H:=types.h helper_functions.h typed_union.h parsed_arguments.h parser.h
HEADERS:=$(patsubst %,$(INC_DIR)/%,$H)

TEST_SRC_DIR:=test/src
TEST_INC_DIR:=test/include
TEST_OBJ_DIR:=test/obj
TEST_BIN_DIR:=test/bin
TESTS:=compilation typed_union pa_flags pa_positionals parser_flags_1 \
	   parser_flags_2 parser_2
TEST_TARGETS:=$(patsubst %,test.%,$(TESTS))
TEST_UNITS:=$(patsubst %,test_%,$(TESTS))
TEST_SOURCES:=$(patsubst %,$(TEST_SRC_DIR)/%.c,$(TEST_UNITS))
TEST_OBJS:=$(TEST_OBJ_DIR)/test.o
TEST_BINS:=$(patsubst %,$(TEST_BIN_DIR)/%.exe,$(TEST_UNITS))

GENERATED:=cap.h slicer.exe $(TEST_BINS) $(TEST_BIN_DIR) $(TEST_OBJS) $(TEST_OBJ_DIR)
COMMA:=,

all: cap.h

cap.h: slicer.exe $(HEADERS)
	./slicer.exe -o $@ $(HEADERS)

slicer.exe: slicer.c
	$(CC) $(CCFLAGS) -o $@ $^

test: $(TEST_TARGETS)

# this is a static pattern rule, those are pretty awesome!
$(TEST_TARGETS): test.%: $(TEST_BIN_DIR)/test_%.exe
	./$<
	
$(TEST_BIN_DIR)/%.exe: $(TEST_SRC_DIR)/%.c $(TEST_OBJ_DIR)/test.o cap.h | $(TEST_BIN_DIR)
	$(CC) $(CCFLAGS) -I$(TEST_INC_DIR) -o $@ $(wordlist 1, 2, $^)

$(TEST_OBJ_DIR)/test.o: $(TEST_SRC_DIR)/test.c $(TEST_INC_DIR)/test.h | $(TEST_OBJ_DIR)
	$(CC) $(CCFLAGS) -I$(TEST_INC_DIR) -c -o $@ $<

$(TEST_BIN_DIR):
	mkdir $@

$(TEST_OBJ_DIR):
	mkdir $@
	
clean:
ifeq ($(OS), Windows_NT)
	pwsh -c 'remove-item $(firstword $(GENERATED)) $(patsubst %,$(COMMA) %,$(wordlist 2, 1000, $(GENERATED))) || {};'
else
	rm $(GENERATED)
endif
.PHONY: all clean test $(TEST_TARGETS)