#include "flag_info.h"
#include "helper_functions.h"
#include "typed_union.h"

#include <stdio.h>
#include <stdlib.h>

const char * cap_get_flag_metavar(const FlagInfo * fi) {
    if (!fi || fi -> mType == DT_PRESENCE) {
        return NULL;
    }
    if (fi -> mMetaVar) {
        return fi -> mMetaVar;
    }
    return cap_type_metavar(fi -> mType);
}

FlagInfo * cap_flag_info_make(
    const char * name, const char * meta_var, const char * description,
    DataType type, int min_count, int max_count)
{
    FlagInfo * info = (FlagInfo *) malloc(sizeof(FlagInfo));
    *info = (FlagInfo) {
        .mName = copy_string(name),
        .mMetaVar = copy_string(meta_var),
	.mDescription = copy_string(description),
        .mType = type,
        .mMinCount = min_count,
        .mMaxCount = max_count	
    };
    return info;
}

void cap_flag_info_destroy(FlagInfo * info) {
    if (!info) {
        return;
    }
    delete_string_property(&(info -> mName));
    delete_string_property(&(info -> mMetaVar));
    delete_string_property(&(info -> mDescription));
    free(info);
}

void cap_print_flag_info(FILE * file, const FlagInfo * flag) {
    fprintf(file, "\t%s", flag -> mName);
    if (flag -> mType != DT_PRESENCE) {
        fprintf(file, " %s", cap_get_flag_metavar(flag));
    }
    if (flag -> mDescription) {
        fprintf(file, "\t%s", flag -> mDescription);
    }
    fputc('\n', file);
}
