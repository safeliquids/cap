#ifndef __PARSED_ARGUMENTS_H__
#define __PARSED_ARGUMENTS_H__

#include <stdbool.h>
#include <stddef.h>

#include "typed_union.h"

// ============================================================================
// === PARSED ARGUMENTS: TYPE DEFINITIONS =====================================
// ============================================================================

/**
 * Stores information about a flag in a `ParsedArguments` object.
 * 
 * Stores the values associated with a single flag in a `ParsedArguments`
 * object. Objects of this type should never be directly created or accessed
 * by the user.
 * 
 * @see ParsedArguments
 * @see TypedUnion
 */
typedef struct {
    /// Name of the flag The object should be considered the 'owner' of 
    /// this string.
    char * mName;
    /// Number of values stored for this flag
    size_t mValueCount;
    /// Number of values that can currently be stored in `mValues`
    size_t mValueAlloc;
    /// Array of stored values. This object should be considered the owner 
    /// of these values.
    TypedUnion * mValues;   
} ParsedFlag;

/**
 * Stores information about a positional argument in a `ParsedArguments` object.
 * 
 * Stores a value for a positional argument. Unlike flags, positionals may only
 * have one `TypedUnion` value.
 * 
 * @see ParsedArguments
 * @see TypedUnion
 */
typedef struct {
    /// Name of the argument. The object should be considered the owner of 
    /// this string.
    char * mName;
    /// Stored value of the positional. The object should be considered the 
    /// owner of this TypedUnion value. 
    TypedUnion mValue;
} ParsedPositional;

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
    /// Number of differend flags stored
    size_t mFlagCount;
    /// Number of flags that can currently be stored in mFlags
    size_t mFlagAlloc;
    /// Information about individual parsed flags
    ParsedFlag * mFlags;

    /// Number of positional arguments
    size_t mPositionalCount;
    /// Number of arguments that can currently be stored in mPositionals
    size_t mPositionalAlloc;
    /// Information about individual positional arguments
    ParsedPositional * mPositionals;
} ParsedArguments;

// ============================================================================
// === PARSED ARGUMENTS: CREATION AND DELETION ================================
// ============================================================================

/**
 * Creates an empty `ParsedArguments` object
 * 
 * Creates a `ParsedArguments` containing no flags or positionals.
 */
ParsedArguments * cap_pa_make_empty();

/**
 * Destroys a `ParsedArguments` object
 * 
 * Destroys a `ParsedArguments` and all data contained in it. All pointers 
 * obtained from the given object become invalid.
 * 
 * @param args object to destroy. If it is `NULL`, this function does nothing.
 */
void cap_pa_destroy(ParsedArguments * args);

// ============================================================================
// === PARSED ARGUMENTS: ACCESS TO PARSED FLAGS ===============================
// ============================================================================

/**
 * Checks if a flag is present.
 * 
 * @param args `ParsedArguments` object to search
 * @param flag null-terminated string name of the flag, including any leading 
 *        flag prefix characters (such as '-').
 * @return `true` if `flag` is present
 */
bool cap_pa_has_flag(const ParsedArguments * args, const char * flag);

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
size_t cap_pa_flag_count(const ParsedArguments * args, const char * flag);

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
    const ParsedArguments * args, const char * flag);

/**
 * Retrieves a value stored in the given flag, at the given position.
 * 
 * Returns a pointer to the value stored for the given flag located at the 
 * given position. If there are too few values, returns `NULL` instead. If 
 * a valid pointer to a `TypedUnion`  object is returned, `args` remains the 
 * owner of that object.
 * 
 * @param args `ParsedArguments` object to search
 * @param flag name of the falg to look up including any flag prefix 
 *        characters (such as '-').
 * @param index position of the value
 * @return pointer to the value for this flag indicated by `index`, or `NULL`.
 */
const TypedUnion * cap_pa_get_flag_i(
    const ParsedArguments * args, const char * flag, size_t index);

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
    ParsedArguments * args, const char * flag, TypedUnion value);

// ============================================================================
// === PARSED ARGUMENTS: ACCESS TO PARSED POSITIONAL ARGUMENTS ================
// ============================================================================

/**
 * Checks if a positinal argument with this name exists.
 * 
 * Checks if `args` contains a positional argument with the name `name`. If 
 * `args` or `name` are `NULL`, always returns `false`.
 * 
 * @param args object to search
 * @param name null-terminated argument name
 * @return `true` if a positional argument `name` exists
 */
bool cap_pa_has_positional(const ParsedArguments * args, const char * name);

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
    const ParsedArguments * args, const char * name);

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
 * @param name null-terminated name of the positional argument to set a new 
 *        value for
 * @param value the new value. `args` becomes the owner of this object.
 */
void cap_pa_set_positional(
    ParsedArguments * args, const char * name, const TypedUnion value);

#endif
