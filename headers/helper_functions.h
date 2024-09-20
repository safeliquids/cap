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
 * using `free` when it is no longer needed.
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

#endif
