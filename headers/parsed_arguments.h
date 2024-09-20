#ifndef __PARSED_ARGUMENTS_H__
#define __PARSED_ARGUMENTS_H__

#include <assert.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>

#include "types.h"
#include "typed_union.h"

// ============================================================================
// === DECLARATION OF PRIVATE FUNCTIONS =======================================
// ============================================================================

ParsedFlag * _cap_pa_get_flag(const ParsedArguments * args, const char * flag);
ParsedPositional * _cap_pa_get_positional(
    const ParsedArguments * args, const char * name);

// ============================================================================
// === FACTORY FUNCTION =======================================================
// ============================================================================

/**
 * Creates an empty `ParsedArguments` object
 * 
 * Creates a `ParsedArguments` containing no flags or positionals.
 */
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

// ============================================================================
// === DISPOSAL ===============================================================
// ============================================================================

/**
 * Destroys a `ParsedArguments` object
 * 
 * Destroys a `ParsedArguments` and all data contained in it. All pointers 
 * obtained from the given object become invalid.
 * 
 * @param args object to destroy. If it is `NULL`, this function does nothing.
 */
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
// === ACCESS TO PARSED FLAGS =================================================
// ============================================================================

/**
 * Checks if a flag is present.
 * 
 * @param args `ParsedArguments` object to search
 * @param flag null-terminated string name of the flag, including any leading 
 *        flag prefix characters (such as '-').
 * @return `true` if `flag` is present
 */
bool cap_pa_has_flag(const ParsedArguments * args, const char * flag) {
    return (bool) _cap_pa_get_flag(args, flag);
}

/**
 * Returns the number of times a flag was given.
 * 
 * If `flag` is not present in `args`, returns zero. If a `ParsedArgument` is 
 * only modified using library functions (such as `cap_pa_add_flag`), it is not 
 * possible to create a flag with no values. Therefore, `cap_pa_flag_count` 
 * returning zero is equivalent to `cap_pa_has_flag` returning false.
 * 
 * @param args `ParsedArguments` object to search
 * @param flag null-terminated string representing the flag name including any
 *        leading flag prefix characters (such as '-').
 * @return number of values stored for the given flag
 */
size_t cap_pa_flag_count(const ParsedArguments * args, const char * flag) {
    const ParsedFlag * pf = _cap_pa_get_flag(args, flag);
    if (!pf) return 0u;
    return pf -> mValueCount;
}

/**
 * Retrieves a value stored in the given flag, or `NULL` if it is absent.
 * 
 * Returns a pointer to the first value stored for the given flag. If the flag 
 * is absent, returns `NULL` instead. If a valid pointer to a `TypedUnion` 
 * object is returned, `args` remains the owner of that object.
 * 
 * @param args `ParsedArguments` object to search
 * @param flag name of the falg to look up including any flag prefix characters 
 *        (such as '-').
 * @return pointer to the first value for this flag, or `NULL`.
 */
const TypedUnion * cap_pa_get_flag(
        const ParsedArguments * args, const char * flag) {
    const ParsedFlag * pf = _cap_pa_get_flag(args, flag);
    if (!pf) {
        return NULL;
    }

    return pf -> mValues;
}

/**
 * Retrieves a value stored in the given flag, at the given position.
 * 
 * Returns a pointer to the value stored for the given flag located at the 
 * given position. If there are too few values, returns `NULL` instead. If 
 * a valid pointer to a `TypedUnion`  object is returned, `args` remains the 
 * owner of that object.
 * 
 * @param args `ParsedArguments` object to search
 * @param flag name of the falg to look up including any flag prefix characters 
 *        (such as '-').
 * @param index position of the value
 * @return pointer to the value for this flag indicated by `index`, or `NULL`.
 */
const TypedUnion * cap_pa_get_flag_i(
        const ParsedArguments * args, const char * flag, size_t index) {
    const ParsedFlag * pf = _cap_pa_get_flag(args, flag);
    if (!pf || index >= pf -> mValueCount) {
        return NULL;
    }
    return pf -> mValues + index;
}
/**
 * Inserts a new value for the given flag.
 * 
 * Appends a new value for the given flag to its list of values. `args` becomes 
 * the owner of `value`. If the flag is not yet present, it is created. In this 
 * process, the flag name is copied and `args` becomes the owner of this copy.
 * 
 * @param args `ParsedArguments` object to add the flag into. If it is `NULL`,
 *        the function does nothing.
 * @param flag null-terminated name of the flag in question including any flag 
 *        prefix characters. If it is `NULL`, the function does nothing.
 * @param value value to store for the flag
 */
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

        int flag_length = strlen(flag);
        char * flag_copy = (char *) malloc((flag_length + 1) * sizeof(char));
        memcpy(flag_copy, flag, flag_length + 1);
        
        ParsedFlag new_flag = (ParsedFlag) {
            .mName = flag_copy,
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
// === ACCESS TO PARSED POSITIONAL ARGUMENTS ==================================
// ============================================================================

/**
 * Checks if a positinal argument with this name exists.
 * 
 * Checks if `args` contains a positional argument with the name `name`. If `args` or `name` are `NULL`, always returns `false`.
 * 
 * @param args object to search
 * @param name null-terminated argument name
 * @return `true` if a positional argument `name` exists
 */
bool cap_pa_has_positional(const ParsedArguments * args, const char * name) {
    return (bool) _cap_pa_get_positional(args, name);
}

/**
 * Retrieves a positional argument with the given name.
 * 
 * Checks if `args` contains a positional argument named `name` and, if it is 
 * present, returns a pointer to its value. Returns `NULL` if no such 
 * positional argument exists or if `args` or `name` are `NULL`. Do note that, 
 * if a valid pointer to a `TypedUnion` object is returned, `args` remains the 
 * owner of that object.
 * 
 * @param args object to search
 * @param name null-terminated name of the positional argument
 * @returns pointer to the argument's value, or `NULL` if it is not found.
 */
const TypedUnion * cap_pa_get_positional(
        const ParsedArguments * args, const char * name) {
    const ParsedPositional * pp =  _cap_pa_get_positional(args, name);
    if (!pp) {
        return NULL;
    }
    return &(pp -> mValue);
}

/**
 * Sets a value for the given argument.
 * 
 * Finds a positional argument with the given name and replaces its value with 
 * `value`.
 * In this process, `args` becomes the owner of `value`. Only newly created 
 * `TypedUnion` objects should be passed to this functions. Passing a (shallow) 
 * copy of a `TypedUnion` with dynamic memory (such as the string type) that is 
 * already owned by a `ParsedArguments` will lead to double-free faults.
 * 
 * If no positional argument with the name `name` exists in `args`, it is 
 * created. A copy of `name` is made when creating the argument in `args`.
 * 
 * @param args object to set the new value in
 * @param name null-terminated name of the positional argument to set a new value for
 * @param value the new value. `args` becomes the owner of this object.
 */
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

    int name_length = strlen(name);
    char * name_copy = (char *) malloc((name_length + 1) * sizeof(char));
    memcpy(name_copy, name, name_length + 1);

    ParsedPositional new_positional = (ParsedPositional) {
        .mName = name_copy,
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

#endif
