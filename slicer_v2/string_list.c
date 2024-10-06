#include "string_list.h"

#include <string.h>
#include <stdlib.h>

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

void add_str_if_not_present(StringList * strings, const char * string) {
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
