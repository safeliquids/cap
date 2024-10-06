#include "helper_functions.h"
#include "positional_info.h"
#include "typed_union.h"

#include <stdio.h>
#include <stdlib.h>

const char * cap_get_posit_metavar(const PositionalInfo * pi) {
    if (!pi) {
        return NULL;
    }
    if (pi -> mMetaVar) {
        return pi -> mMetaVar;
    }
    return pi -> mName;
}

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

void cap_positional_info_destroy(PositionalInfo * info) {
    if (!info) {
        return;
    }
    delete_string_property(&(info -> mName));
    delete_string_property(&(info -> mMetaVar));
    delete_string_property(&(info -> mDescription));
    free(info);
}

void cap_print_positional_info(
        FILE * file, const PositionalInfo * info) {
    fprintf(file, "\t%s", cap_get_posit_metavar(info));
    if (info -> mDescription) {
        fprintf(file, "\t%s", info -> mDescription);
    }
    fputc('\n', file);
}
