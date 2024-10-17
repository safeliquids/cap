#ifndef __POSITIONAL_INFO_H__
#define __POSITIONAL_INFO_H__

#include "helper_functions.h"
#include "types.h"
#include "typed_union.h"

#include <stdio.h>
#include <stdlib.h>

/**
 * Gets the argument's metavar.
 *
 * Returns a string representing the argument, e.g. in help messages. If 
 * available, this is the explicit value stored in mMetaVar. If it is not 
 * evailable, the argument name is returned.
 *
 * @param pi info about the positional argument
 * @return text representation of pi
 */
const char * cap_get_posit_metavar(const PositionalInfo * pi) {
    if (!pi) {
        return NULL;
    }
    if (pi -> mMetaVar) {
        return pi -> mMetaVar;
    }
    return pi -> mName;
}

/**
 * Factory for PositionalInfo objects.
 *
 * Allocates a new PositionalInfo object and initializes it using provided 
 * data. The caller is the owner of this new object and should dispose of it 
 * using cap_positional_info_destroy.
 *
 * @param name null-terminated name of the positional. This string is copied 
 *        into the new object.
 * @param meta_var optional null-terminated human-readable representation of
 *        the argument. If not NULL, a copy is created and stored in the new 
 *        object.
 * @param description optional null-terminated short description of the 
 *        argument for use in help messages. A copy is created and stored in 
 *        the new object.
 * @param type data type of this argument
 * @return a new PositionalInfo object
 */
PositionalInfo * cap_positional_info_make(
    const char * name, const char * meta_var, const char * description,
    DataType type) {
    PositionalInfo * info = (PositionalInfo *) malloc(sizeof(PositionalInfo));
    *info = (PositionalInfo) {
        .mName = copy_string(name),
	.mMetaVar = copy_string(meta_var),
	.mDescription = copy_string(description),
	.mType = type
    };
    return info;
}

/**
 * Desctuctor for PositionalInfo objects.
 *
 * Destroys and de-allocates a PositionalInfo object. Should only be called by 
 * the owner of that object.
 *
 * @param info object to destroy
 */
void cap_positional_info_destroy(PositionalInfo * info) {
    if (!info) {
        return;
    }
    delete_string_property(&(info -> mName));
    delete_string_property(&(info -> mMetaVar));
    delete_string_property(&(info -> mDescription));
    free(info);
}

/**
 * Display a positional info object.
 *
 * Prints a PositionalInfo to a given file. The format is similar to 
 * conventional help messages. 
 *
 * @file file to print into
 * @info object to display
 */
void cap_print_positional_info(
        FILE * file, const PositionalInfo * info) {
    fprintf(file, "%s\n", cap_get_posit_metavar(info));
    if (info -> mDescription) {
        fprintf(file, "\t%s\n", info -> mDescription);
    }
}

#endif

