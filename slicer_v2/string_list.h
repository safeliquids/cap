#ifndef __STRING_LIST_H__
#define __STRING_LIST_H__

#include <stdbool.h>

typedef struct SL {
    char * value;
    struct SL * tail;
} StringList;

StringList * sl_empty();

void sl_destroy(StringList * list);

bool sl_is_empty(const StringList * list);

void add_str_if_not_present(StringList * strings, const char * string);

#endif
