#ifndef __FLAG_INFO_H__
#define __FLAG_INFO_H__

#include "typed_union.h"

#include <stdio.h>

// ============================================================================
// === FLAG INFO ==============================================================
// ============================================================================

/**
 * Configuration of a flag in an `ArgumentParser`
 */
typedef struct {
    char * mName;
    char * mMetaVar;
    char * mDescription;
    DataType mType;
    int mMinCount;
    int mMaxCount;
} FlagInfo;

/**
 * Get a text representation of this flag's argument.
 *
 * Returns a metavar for this flag, according to its type. If available, the 
 * string is taken from the explicit value mMetaVar in fi. Else, the name of 
 * the flag's type is used. If the given flag has type DT_PRSENCE (meaning it
 * takes no value), returns NULL.
 * 
 * @param fi object to get the representation of
 * @return string representation of the flag's value
 */
const char * cap_get_flag_metavar(const FlagInfo * fi);

/**
 * Factory for FlagInfo objects.
 *
 * Allocates a new FlagInfo object and initializes it using given data. The 
 * caller is the owner of the new object and should dispose of it using
 *cap_flag_info_destroy.
 *
 * All strings given to this function are copied into the new object (if they
 * are not NULL).
 *
 * @param name null-terminated name of the flag
 * @param meta_var optional null-terminated representation of the flag's value.
 *        When creating DT_PRESENCE flags, this parameter should be NULL. This 
 *        identifier is used in help messages.
 * @param description optional null-terminated description of the flag to be 
 *        used in help messages. 
 * @param type data type of the flag's value
 * @param min_count minimum number of times the flag should be present. Must be
 *        at least zero.
 * @param max_count maximum number of times the flag should be present. Must
 *        not be less than min_count, or a negative number. If a negative 
 *        number is given, there is no upper limit.
 * @return new FlagInfo object
 */
FlagInfo * cap_flag_info_make(
    const char * name, const char * meta_var, const char * description,
    DataType type, int min_count, int max_count);

/**
 * Destructor for FlagInfo objects
 *
 * Destroys and de-allocates a flagInfo object. The caller should be the owner
 * of this object.
 *
 * @param info object to destroy
 */
void cap_flag_info_destroy(FlagInfo * info);

/**
 * Displays a FlagInfo object
 *
 * Prints a FlagInfo into a given file. The format is suitable for help 
 * messages.
 *
 * @param file file to display into
 * @flag object to display
 */
void cap_print_flag_info(FILE * file, const FlagInfo * flag);

#endif


