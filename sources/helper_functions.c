#include "helper_functions.h"

#include <stdlib.h>
#include <string.h>

// ============================================================================
// === HELPER FUNCTIONS =======================================================
// ============================================================================

char * copy_string(const char * string) {
    if (!string) {
        return NULL;
    }
    const int len = strlen(string);
    char * copy = (char *) malloc((len + 1) * sizeof(char));
    memcpy(copy, string, len + 1);
    return copy;
}

void set_string_property(char ** property, const char * value) {
    if (!property) {
        return;
    }
    if (*property) {
        free(*property);
    }
    // copy_string returns NULL if its argument is NULL
    *property = copy_string(value);
}

void delete_string_property(char ** property) {
    set_string_property(property, NULL);
}
