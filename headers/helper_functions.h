#ifndef __HELPER_FUNCTIONS_H__
#define __HELPER_FUNCTIONS_H__

#include <stdlib.h>
#include <string.h>

// ============================================================================
// === HELPER FUNCTIONS =======================================================
// ============================================================================

/**
 * Creates a copy of a string
 * 
 * Copies the given null-terminated string into newly allocated memory. 
 * The caller becomes the owner of that memoroy and should deallocate it
 * using `free` when it is no longer needed. If `NULL` is given, `NULL` is 
 * also returned.
 * 
 * @param string original null-terminated string
 * @return pointer to the copy of the given string 
 */
char * copy_string(const char * string) {
    if (!string) {
        return NULL;
    }
    const int len = strlen(string);
    char * copy = (char *) malloc((len + 1) * sizeof(char));
    memcpy(copy, string, len + 1);
    return copy;
}

/**
 * Replaces a string with a new value.
 * 
 * Replaces a string stored in `*property` with a copy of `value`. If 
 * `*property` is a string already, it is first deleted using 
 * `delete_string_property`. The caller must be the owner of the original 
 * string. The original must have been allocated using `malloc`.
 * 
 * The new value must be either `NULL`, or a null-terminated string. If it is 
 * not `NULL`, a copy is created and stored. The owner of `property` becomes 
 * the owner of the copy.
 * 
 * @param property pointer to a string that should be repalced
 * @param value null-terminated string, or `NULL`
 */
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

/**
 * Deletes a string stored in `*property`
 * 
 * Deallocates a string and replaces it with `NULL`. The string in question 
 * must be owned by the caller and it must have been previously allocated 
 * using `malloc`.
 * 
 * @param property pointer to a string that should be deleted
 */
void delete_string_property(char ** property) {
    set_string_property(property, NULL);
}

/**
 * Get an string representation of type.
 *
 * Returns a null-terminated string representing the given data type. If 
 * DT_PRESENCE is given, returns NULL instead. This should be used in help and
 * usage messages.
 *
 * @param type data type to display
 * @return string representation of the type, or NULL if type is  DT_PRESENCE 
 */
static const char * cap_type_metavar(DataType type) {
    const char * type_metavar;
    switch (type) {
        case DT_DOUBLE:
            type_metavar = "DOUBLE";
            break;
        case DT_INT:
            type_metavar = "INT";
            break;
        case DT_STRING:
            type_metavar = "STRING";
            break;
        case DT_PRESENCE:
        default:
            type_metavar = NULL;
    }
    return type_metavar;
}

#endif
