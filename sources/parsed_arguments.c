#include "helper_functions.h"
#include "parsed_arguments.h"
#include "typed_union.h"

#include <assert.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>

// ============================================================================
// === PARSED ARGUMENTS: DECLARATION OF PRIVATE FUNCTIONS =====================
// ============================================================================

ParsedFlag * _cap_pa_get_flag(const ParsedArguments * args, const char * flag);
ParsedPositional * _cap_pa_get_positional(
    const ParsedArguments * args, const char * name);

// ============================================================================
// === PARSED ARGUMENTS: CREATION AND DELETION ================================
// ============================================================================

ParsedArguments * cap_pa_make_empty() {
    ParsedArguments * pa = (ParsedArguments *) malloc(sizeof(ParsedArguments));
    *pa = (ParsedArguments) {
        .mFlagCount = 0,
        .mFlagAlloc = 0,
        .mFlags = NULL,
        .mPositionalCount = 0,
        .mPositionalAlloc = 0,
        .mPositionals = NULL
    };
    return pa;
}

void cap_pa_destroy(ParsedArguments * args) {
    if (!args) return;
    for (size_t i = 0; i < args -> mFlagCount; ++i) {
        ParsedFlag * pf = args -> mFlags + i;
        free(pf -> mName);
        pf -> mName = NULL;
        for (size_t j = 0; j < pf -> mValueCount; ++j) {
            cap_tu_destroy(pf -> mValues + j);
        }
        free(pf -> mValues);
        pf -> mValues = NULL;
        pf -> mValueAlloc = pf -> mValueCount = 0u;
    }
    args -> mFlags = NULL;
    args -> mFlagCount = args -> mFlagAlloc = 0u;

    for (size_t i = 0; i < args -> mPositionalCount; ++i) {
        ParsedPositional * pp = args -> mPositionals + i;
        free(pp -> mName);
        pp -> mName = NULL;
        cap_tu_destroy(&(pp -> mValue));
    }
    args -> mPositionals = NULL;
    args -> mPositionalCount = args -> mPositionalAlloc = 0u;
    free(args);
}

// ============================================================================
// ===  PARSED ARGUMENTS: ACCESS TO PARSED FLAGS ==============================
// ============================================================================

bool cap_pa_has_flag(const ParsedArguments * args, const char * flag) {
    return (bool) _cap_pa_get_flag(args, flag);
}

size_t cap_pa_flag_count(const ParsedArguments * args, const char * flag) {
    const ParsedFlag * pf = _cap_pa_get_flag(args, flag);
    if (!pf) return 0u;
    return pf -> mValueCount;
}

const TypedUnion * cap_pa_get_flag(
        const ParsedArguments * args, const char * flag) {
    const ParsedFlag * pf = _cap_pa_get_flag(args, flag);
    if (!pf) {
        return NULL;
    }

    return pf -> mValues;
}

const TypedUnion * cap_pa_get_flag_i(
        const ParsedArguments * args, const char * flag, size_t index) {
    const ParsedFlag * pf = _cap_pa_get_flag(args, flag);
    if (!pf || index >= pf -> mValueCount) {
        return NULL;
    }
    return pf -> mValues + index;
}

void cap_pa_add_flag(
        ParsedArguments * args, const char * flag, TypedUnion value) {
    if (!args || !flag) return;
    ParsedFlag * pf = _cap_pa_get_flag(args, flag);
    if (!pf) {
        // create a new flag in args
        if (args -> mFlagAlloc == 0u) {
            args -> mFlagAlloc = 4;
            args -> mFlags = (ParsedFlag *) malloc(4 * sizeof(ParsedFlag));
        }
        if (args -> mFlagCount >= args -> mFlagAlloc) {
            args -> mFlagAlloc *= 2;
            args -> mFlags = (ParsedFlag *) realloc(
                args -> mFlags, (args -> mFlagAlloc) * sizeof(ParsedFlag));
        }
        
        ParsedFlag new_flag = (ParsedFlag) {
            .mName = copy_string(flag),
            .mValueCount = 0u,
            .mValueAlloc = 1u,
            .mValues = (TypedUnion *) malloc(sizeof(TypedUnion))
        };

        args -> mFlags[args -> mFlagCount] = new_flag;
        pf = args -> mFlags + args -> mFlagCount;
        ++(args -> mFlagCount);
    }
    // now pf definitely is not NULL and points to the flag where the new value 
    // should be added
    if (pf -> mValueCount >= pf -> mValueAlloc) {
        pf -> mValueAlloc *= 2;
        pf -> mValues = (TypedUnion *) realloc(
            pf -> mValues, (pf -> mValueAlloc) * sizeof(TypedUnion));
    }
    pf -> mValues[pf -> mValueCount++] = value;
}

// ============================================================================
// === PARSED ARGUMENTS: ACCESS TO PARSED POSITIONAL ARGUMENTS ================
// ============================================================================

bool cap_pa_has_positional(const ParsedArguments * args, const char * name) {
    return (bool) _cap_pa_get_positional(args, name);
}

const TypedUnion * cap_pa_get_positional(
        const ParsedArguments * args, const char * name) {
    const ParsedPositional * pp =  _cap_pa_get_positional(args, name);
    if (!pp) {
        return NULL;
    }
    return &(pp -> mValue);
}

void cap_pa_set_positional(
        ParsedArguments * args, const char * name, const TypedUnion value) {
    ParsedPositional * pp = _cap_pa_get_positional(args, name);

    if (pp) {
        cap_tu_destroy(&(pp -> mValue));
        pp -> mValue = value;
        return;
    }
    
    if (args -> mPositionalAlloc == 0u) {
        args -> mPositionalAlloc = 4u;
        args -> mPositionals = (ParsedPositional *) malloc(
            4 * sizeof(ParsedPositional));
    }
    if (args -> mPositionalCount >= args -> mPositionalAlloc) {
        args -> mPositionalAlloc *= 2;
        args -> mPositionals = (ParsedPositional *) realloc(
            args -> mPositionals,
            args -> mPositionalAlloc * sizeof(ParsedPositional));
    }

    ParsedPositional new_positional = (ParsedPositional) {
        .mName = copy_string(name),
        .mValue = value
    };

    args -> mPositionals[(args -> mPositionalCount)++] = new_positional;
}

// ============================================================================
// === IMPLEMENTATION OF PRIVATE FUNCTIONS ====================================
// ============================================================================

ParsedFlag * _cap_pa_get_flag(
        const ParsedArguments * args, const char * flag) {
    if (!args || !flag) {
        return NULL;
    }
    for (size_t i = 0; i < args -> mFlagCount; ++i) {
        ParsedFlag * pf = args -> mFlags + i;
        if (strcmp(pf -> mName, flag) == 0) {
            return pf;
        }
    }
    return NULL;
}

ParsedPositional * _cap_pa_get_positional(
        const ParsedArguments * args, const char * name) {
    if (!args || !name) {
        return NULL;
    }
    for (size_t i = 0; i < args -> mPositionalCount; ++i)  {
        ParsedPositional * pp = args -> mPositionals + i;
        if (strcmp(pp -> mName, name) == 0) {
            return pp;
        }
    }
    return NULL;
}
