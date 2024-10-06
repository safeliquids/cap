#include "slicer_utils.h"

#include <stdbool.h>
#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

char * copy_string(const char * s) {
    if (!s) {
        return NULL;
    }
    size_t len = strlen(s) + 1;
    char * res = (char *) malloc(len * sizeof(char));
    memcpy(res, s, len);
    return res;
}

StringList * sl_empty() {
    StringList * list = (StringList *) malloc(sizeof(StringList));
    list -> value = NULL;
    list -> tail = NULL;
    return list;
}

void sl_destroy(StringList * list) {
    while (list) {
        if (list -> value) {
            free(list -> value);
        }
        StringList * new_list = list -> tail;
        free(list);
        list = new_list;
    }
}

bool sl_is_empty(const StringList * list) {
    return !(bool)list -> tail;
}

void sl_insert_if_missing(StringList * strings, const char * string) {
    for ( ; strings -> tail; strings = strings -> tail) {
        if (!strcmp(strings -> value, string)) {
            return;
        }
    }
    StringList * new_sentinel = (StringList *) malloc(sizeof(StringList));
    *new_sentinel = (StringList) {NULL, NULL};

    size_t len = strlen(string) + 1;
    strings -> value = (char *) malloc(len * sizeof(char));
    memcpy(strings -> value, string, len);
    strings -> tail = new_sentinel;
}

LimitedLineReader llr_open(const char * file_name) {
    LimitedLineReader reader = (LimitedLineReader) {
        .file = NULL,
        .line_number = 0u,
        .line_length = 0u,
        .line_buffer = {0}
    };
    FILE * f = fopen(file_name, "r");
    if (!f) {
        reader.status = FAIL;
    }
    else {
        reader.file = f;
        reader.status = OK;
    }
    return reader;
}

void llr_close(LimitedLineReader * reader) {
    if (!reader -> file) {
        return;
    }
    fclose(reader -> file);
    reader -> file = NULL;
    reader -> line_number = 0;
    reader -> line_length = 0;
    reader -> line_buffer[0] = 0;
    reader -> status = CLOSED;
}

void llr_advance(LimitedLineReader * reader) {
    if (!reader -> file) {
        return;
    }
    if (reader -> status == CLOSED || reader -> status == FAIL || reader -> status == END) {
        return;
    }
    char * s = fgets(reader -> line_buffer, LLR_LINE_BUFFER_LENGTH, reader -> file);
    int eof = feof(reader -> file);
    if (!s && eof) {
        reader -> status = END;
        return;
    }
    if (!s) {
        reader -> status = FAIL;
        return;
    }
    ++reader -> line_number;
    reader -> line_length = strlen(reader -> line_buffer);
    if (reader -> line_buffer[reader -> line_length - 1] != '\n') {
        if (reader -> line_length >= LLR_LINE_BUFFER_LENGTH - 1) {
            reader -> status = TOO_LONG;
            return;
        }
        reader -> line_buffer[reader -> line_length] = '\n';
        reader -> line_buffer[reader -> line_length + 1] = 0;
        reader -> line_length += 1;
    }
    reader -> status = OK;
}
