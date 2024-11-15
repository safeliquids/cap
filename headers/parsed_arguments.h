#ifndef __PARSED_ARGUMENTS_H__
#define __PARSED_ARGUMENTS_H__

/**
 * @file
 * @defgroup parsed_arguments ParsedArguments: Storage of Parsed Flags and Arguments
 * 
 * `ParsedArguments` is the result of parsing arguments given on the command
 * line. An object of this type is usually returned from a configured
 * `ArgumentParser` after successful parsing. It should contain information
 * about all configured flags and arguments that were found on the command line.
 * The library offers several functions for creating, deleting, modifying and
 * querying these objects. They are prefixed with `cap_pa_`.
 * 
 * `ParsedArguments` objects are able to store arguments of two kinds: flags and
 * positional arguments (also called positionals). Each stored argument has a
 * string name and a set of values. The name is most commonly used to query the
 * `ParsedArguments` for the given argument (e.g. if it is present). Values are
 * stored as `TypedUnion` objects, and pointers to those objects can be returned
 * from a search function call on the `ParsedArguments`, e.g. `cap_pa_get_flag`.
 * This way, `ParsedArguments` implements a pair of primitive multi-sets of
 * `string --> TypedUnion` (one such mapping for flags and one for positionals.)
 * 
 * An empty `ParsedArguments` object can be created using `cap_pa_make_empty()`.
 * This is normally done at parse-time by the parser so users do not need to
 * create it manually. Objects should be disposed of using `cap_pa_destroy` when
 * no longer needed. This call invalidates all information obtained from the
 * object, e.g. pointers to `TypedUnion`s that were stored as argument values.
 */

#include "named_values.h"
#include "named_values_array.h"
#include "typed_union.h"

#include <assert.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>

/**
 * @addtogroup parsed_arguments
 * @{
 */

// ============================================================================
// === PARSED ARGUMENTS =======================================================
// ============================================================================

/**
 * Stores all information about command line arguments after successful
 * parsing.
 * 
 * This structure is the main result of parsing command line arguments by a
 * configured parser. All flags and positional arguments are stored with their
 * values as `TypedUnion` objects. Objects of this type should not be created 
 * directly. The function `cap_parser_parse` should be used instead. Similarly, 
 * information about flags and positionals should be obtained using functions 
 * such as `cap_pa_has_flag` or `cap_pa_get_positional`. 
 * 
 * Once created using the `cap_parser_parse` or `cap_pa_make_empty` functions,
 * the caller owns this object and should dispose of it using `cap_pa_destroy`
 * once it is no longer needed. When created by `cap_parser_parse`, the pointer
 * returned by this function must also be freed. (this is an unfortunate
 * technical limitation.) `cap_pa_destroy` also deletes all data contained in
 * it, including names of flags, and `TypedUnion`s and strings contained
 * in them. If any data obtained from a `ParsedArguments` should be usable 
 * after destroying the object, it must be properly copied.
 * 
 * @see ParsedFlag
 * @see ParsedPositional
 * @see cap_pa_make_empty
 * @see cap_pa_destroy
 * @see cap_pa_has_flag
 * @see cap_pa_flag_count
 * @see cap_pa_get_flag
 * @see cap_pa_get_flag_i
 * @see cap_pa_has_positional
 * @see cap_pa_get_positional
 */
typedef struct {
    /// Information about individual parsed flags
    NamedValuesArray * mFlags;
    /// Information about individual positional arguments
    NamedValuesArray * mPositionals;
} ParsedArguments;

// ============================================================================
// === DECLARATION OF PRIVATE FUNCTIONS =======================================
// ============================================================================

NamedValues * _cap_pa_get_flag(const ParsedArguments * args, const char * flag);
NamedValues * _cap_pa_get_positional(
    const ParsedArguments * args, const char * name);
void _cap_named_values_destroy(NamedValues * nv);
void _cap_named_values_array_destroy(NamedValuesArray * nva);

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
        .mFlags = cap_nva_make_empty(),
        .mPositionals = cap_nva_make_empty(),
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
    if (args -> mFlags) {
        cap_nva_destroy(args -> mFlags);
        args -> mFlags = NULL;
    }
    if (args -> mPositionals) {
        cap_nva_destroy(args -> mPositionals);
        args -> mPositionals = NULL;
    }
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
    const NamedValues * pf = _cap_pa_get_flag(args, flag);
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
    const NamedValues * pf = _cap_pa_get_flag(args, flag);
    return cap_nv_get_value(pf);
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
    const NamedValues * pf = _cap_pa_get_flag(args, flag);
    return cap_nv_get_value_i(pf, index);
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
    cap_nva_append_value(args -> mFlags, flag, value);
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
 * Get the number of values stored for a positional.
 * 
 * Checks if `args` contains a positional argument with the name `name`. If
 * `args` or `name` are `NULL`, or if `name` is not contained in `args`, 
 * returns zero. Else returns the number of values stored for `name`.
 * 
 * @param args object to search
 * @param name null-terminated name of the argument
 * @return number of values stored for `name`
 */
size_t cap_pa_positional_count(
    const ParsedArguments * args, const char * name)
{
    const NamedValues * pp = _cap_pa_get_positional(args, name);
    return cap_nv_value_count(pp);
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
    const ParsedArguments * args, const char * name) 
{
    const NamedValues * pp =  _cap_pa_get_positional(args, name);
    return cap_nv_get_value(pp);
}

/**
 * Retrieves a positional argument with the given name at the given index.
 * 
 * Checks if `args` contains a positional argument named `name`. If it is 
 * present and there are at least `index + 1` values stored for it returns 
 * the value at position `index` (starting at zero). Returns `NULL` if that 
 * argument does not exist or there are not enough values. Also returns `NULL`
 * if `args` or `name` are `NULL`. Do note that, if a valid pointer to 
 * a `TypedUnion` object is returned, `args` remains the owner of that object.
 * 
 * @param args object to search
 * @param name null-terminated name of the positional argument
 * @param index index of the requested value
 * @return pointer to the argument's value at `index`, or `NULL` if it is
 *         not found.
 */
const TypedUnion * cap_pa_get_positional_i(
    const ParsedArguments * args, const char * name, size_t index) 
{
    const NamedValues * pp =  _cap_pa_get_positional(args, name);
    return cap_nv_get_value_i(pp, index);
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
    ParsedArguments * args, const char * name, const TypedUnion value)
{
    if (!args || !name) {
        return;
    }
    cap_nva_set_value(args -> mPositionals, name, value);
}

/**
 * Adds a value for a positional
 * 
 * Adds a new value for a stored positional argument in `args`. If no such
 * positional exists, it gets also created.
 * 
 * In this process, `args` becomes the owner of `value`. Only newly created 
 * `TypedUnion` objects should be passed to this functions. Passing a (shallow)
 * copy of a `TypedUnion` with dynamic memory (such as the string type) that is
 * already owned by a `ParsedArguments` will lead to double-free faults.
 * 
 * @param args object to add a value to
 * @param name null-terminated name of the positional
 * @param value value to store for this positional
 */
void cap_pa_append_positional(
    ParsedArguments * args, const char * name, const TypedUnion value)
{
    if (!args || !name) {
        return;
    }
    cap_nva_append_value(args -> mPositionals, name, value);
}

// ============================================================================
// === IMPLEMENTATION OF PRIVATE FUNCTIONS ====================================
// ============================================================================

NamedValues * _cap_pa_get_flag(
    const ParsedArguments * args, const char * flag)
{
    if (!args) {
        return NULL;
    }
    return cap_nva_get(args -> mFlags, flag);
}

NamedValues * _cap_pa_get_positional(
    const ParsedArguments * args, const char * name)
{
    if (!args) {
        return NULL;
    }
    return cap_nva_get(args -> mPositionals, name);
}

/**
 * @}
 */

#endif
