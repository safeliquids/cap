#ifndef __TYPES_H__
#define __TYPES_H__

#include <stdbool.h>
#include <stddef.h>

// ============================================================================
// === TYPE DEFINITIONS =======================================================
// ============================================================================

/**
 * Identifies data type of parsed values
 * 
 * For the purposes of the `cap` library, every value has a data type. That
 * type is, however, not known at compile-time (because parsers are created by
 * the user at run-time). Values are stored as Typed Unions,
 * and their type is represented by DataType.
 * @see TypedUnion
 */
typedef enum {
    /// integer value, corresponds to the `int` type
    DT_INT,
    /// real value, corresponds to the `double` type
    DT_DOUBLE,
    /// character string value, corresponds to a null-terminated string
    DT_STRING,
    /// used for flags that do not store any information other than their 
    /// presence or absence
    DT_PRESENCE  
} DataType;

/**
 * Elementary storage of values whose type is not known at compile-time.
 * 
 * For the purposes of the `cap` library, every value has a data type. That
 * type is, however, not known at compile-time (because parsers are created by
 * the user at run-time). Values are stored in a union type and marked with
 * their type using the DataType enum.
 * @see DataType.
 * 
 * The type of a TypedUnion should always be inspected using functions such as
 * `cap_tu_is_double()`, and values from them should only be extracted using
 * functions such as `cap_tu_as_double()`, and after checking the type.
 * @see cap_tu_is_double
 * @see cap_tu_is_int
 * @see cap_tu_is_presence
 * @see cap_tu_is_string
 * @see cap_tu_as_double
 * @see cap_tu_as_int
 * @see cap_tu_as_string
 * 
 * `TypedUnion` objects should always be created using factory functions such
 * as `cap_tu_make_double()`.
 * @see cap_tu_make_double
 * @see cap_tu_make_int
 * @see cap_tu_make_presence
 * @see cap_tu_make_string
 */
typedef struct {
    /// type of the stored value
    DataType mType;
    union {
        /// stores the value for DT_INT type
        int asInt;
        /// stores the value for DT_DOUBLE type
        double asDouble;
        /// stores the value for DT_STRING type
        char * asString;
    } mValue;
} TypedUnion;

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

/**
 * Configuration of a flag in an `ArgumentParser`
 */
typedef struct {
    char * mName;
    DataType mType;
    int mMinCount;
    int mMaxCount;
} FlagInfo;

/**
 * Configuration of a positional argument in an `ArgumentParser`
 */
typedef struct {
    char * mName;
    DataType mType;
} PositionalInfo;

/**
 * Main object for parsing given command line arguments.
 * 
 * This object contains all information about configured flags and positional 
 * arguments, including their types, names, and (for flags) how many times they 
 * may be present. Once configured, this object is passed to the
 * `cap_parser_parse` function to parse given command line arguments. That
 * produces a `ParsedArguments` object, or exits the program if an error 
 * occurs, e.g. if a given argument cannot be converted to the required type.
 * It is safe to reuse a configured parser for multiple arrays of arguments.
 * 
 * Objects of this type should be created using the factory function
 * `cap_parser_make_empty`. Configuration is done using functions such as 
 * `cap_parser_add_flag` and `cap_parser_add_positional`. If an error occurs
 * when configuring the parser (e.g. the same flag name is used in multiple
 * calls to `cap_parser_add_flag`) the program exits with an error message.
 * 
 * This object can be disposed of using `cap_parser_destroy` when it is no 
 * longer needed. Any produced `ParsedArguments` objects are not affected by 
 * this and can be used even after the parser is destroyed.
 * 
 * @see cap_parser_make_empty
 * @see cap_parser_destroy
 * @see cap_parser_add_flag
 * @see cap_parser_add_positional
 * @see cap_parser_parse
 */
typedef struct {
    FlagInfo * mFlags;
    size_t mFlagCount;
    size_t mFlagAlloc;

    PositionalInfo * mPositionals;
    size_t mPositionalCount;
    size_t mPositionalAlloc;

    char * mFlagPrefixChars;
    char * mFlagSeparator;
    bool mUseFlagSeparator;
} ArgumentParser;

/**
 * Result of argument parsing.
 * 
 * Represents the result of processing command line arguments using
 * a configured parser. This object is returned by the
 * `cap_parser_parse_noexit` function, which does not exit the running program 
 * when an error is encountered. The success, result, and error message for
 * that parsing operation are stored in this object.
 * 
 * @note `cap_parser_parse_noexit` and by extension `ParsingResult` exist
 * mainly for unit testing purposes. Users should primarily use
 * `cap_parser_parse`.
 * 
 * @see cap_parser_parser_noexit
 * @see cap_parser_parse
 */
typedef struct {
    /// Result of argument parsing, or `NULL` if an error occured
    ParsedArguments * mArguments;
    /// `printf`-style format string to use for displaying an error message. 
    /// Up to two string parameters can be inserted into this error message
    /// using the `mFirstErrorWord` and `mSecondErrorWord` members. The `%s`
    /// conversion specifier must be used. This format string is ignored if
    /// parsing was successful.
    const char * mErrorMessageFormat;  
    /// first word to be inserted into an error message. 
    /// If it is `NULL`, it is ignored, and the `mSecondErrorWord` is also ignored.
    const char * mFirstErrorWord;
    /// second word to be inserted into an error message. 
    /// If it is `NULL`, it is ignored.
    const char * mSecondErrorWord;
    /// success flag of the parsing.
    /// If `mSuccess` is `true`, `mArguments` must point to a valid 
    /// `ParsedArguments` object. If it is `false`, `mArguments` must be
    /// `NULL`.
    bool mSuccess;
} ParsingResult;

#endif