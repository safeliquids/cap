#ifndef __SLICER_UTILS_H__
#define __SLICER_UTILS_H__

#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>

#define LLR_LINE_BUFFER_LENGTH 1024u

char * copy_string(const char * s);

typedef struct SL {
    char * value;
    struct SL * tail;
} StringList;

StringList * sl_empty();

void sl_destroy(StringList * list);

bool sl_is_empty(const StringList * list);

void sl_insert_if_missing(StringList * strings, const char * string);

typedef enum {
    OK,
    FAIL,
    TOO_LONG,
    END,
    CLOSED
} ReaderStatus;

typedef struct {
    FILE * file;
    size_t line_number;
    size_t line_length;
    char line_buffer[LLR_LINE_BUFFER_LENGTH];
    ReaderStatus status;
} LimitedLineReader;

LimitedLineReader llr_open(const char * file_name);

void llr_close(LimitedLineReader * reader);

void llr_advance(LimitedLineReader * reader);

#endif
