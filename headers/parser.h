#ifndef __PARSER_H__
#define __PARSER_H__

/** 
 * @file 
 * @defgroup parser Argument Parser, its Configuration, and Usage
 * 
 * An `ArgumentParser` is the main interface of the library. It is an object
 * that can be configured with flags, positionals, program description and other
 * information. A configured object is then used to parse arguments from the
 * command line (the `argc` array). On successful parsing, a `ParsedArguments`
 * object is created which stores all parsed arguments. Functons related to the
 * `ArgumentParser` type are prefixed with `cap_parser_`.
 * 
 * ## Times
 * There are two important events related to an `ArgumentParser` object:
 * configuration-time and parse-time. Configuration-time takes place between the
 * creation of a parser and its use for parsing command line arguments. During
 * configuration-time, many properties of a parser can be added or changed.
 * These configurations are explained in more depth in the section
 * Configuration. If anything goes wrong with the parser (e.g. the user attempts
 * to create an invalid configuration), a configuration-time error occurs, which
 * usually forcefully exits the program.
 * 
 * Parse-time takes place when command line arguments (i.e. words stored in
 * `argv`) are given to a parser in order to create an output. No changes happen
 * to the parser object at this time. If any problems arise, such as not enough
 * arguments or not being able to parse a word, a parse-time error occurs. This
 * also usually exits the program.
 * 
 * ## Creation
 * There are two factory functions for parsers: `cap_parser_make_empty` and
 * `cap_parser_make_default`. The `default` function creates a parser with some
 * useful initial configuration, e.g. a help flag. Objects returned from these
 * functions are otherwise functionally identical. When configured, an
 * `ArgumentParser` stores dynamically allocated data, so it needs to be
 * properly disposed of when no longer needed. That is done using the
 * `cap_parser_destroy` function. `ParsedArguments` objects created by this
 * parser are independent of it and can be used even after the parser has been
 * destroyed.
 * 
 * ## Configuration
 * The two main ways of configuring a parser are creating flags and positional
 * arguments (or "positionals"). Other configurations are
 * - flag prefix characters,
 * - special flags,
 * - custom program name,
 * - program description,
 * - automatic or manual creation of help and usage messages, and
 * - enabling or disabling display of help and/or usage.
 * 
 * Do keep in mind that, in this document, "configuring a parser" is a general
 * expression used for any of the above listed changes to a parser. "Defining" a
 * flag is the same as "configuring" it, "configuring a flag in the parser", or
 * "configuring the parser with a flag". Similar terminology is used for other
 * configurations, such as positionals, descriptions, or a custom usage message.
 * 
 * ### Positional Arguments
 * Positional arguments are inputs to a program given as words on the command
 * line. Their meaning and interpretation are decided based on their order on
 * the command line, hence being positional. In this way, positionals are very
 * similar to arguments given to a function in a programming language such as C.
 * This order is decided by the programmer at configuration-time: positinoals
 * are expected on the command line in the same order as they were defined in
 * the program. Positionals are configured in a parser using the
 * `cap_parser_add_positional` function.
 * 
 * Positionals are defined using a string name, a data type, their
 * required-ness, and variadicity. The name is used to find its values in a
 * created `ParsedArguments`. The data type dictates how to interpret the word
 * on the command line (i.e. in `argv`) and is one of the primitive types
 * described in @ref typed_union with exception of the `DT_PRESENCE` type (see
 * later). Required-ness decides if the argument may be
 * missing on the command line - if it is required, it may not be missing.
 * Variadicity decides if the positional is allowed to accept multiple values.
 * (Normally, all positionals take exactly one value if they are not missing.)
 * 
 * Required-ness relates to an important concept: at parse-time, once the parser
 * decides to skip a positional that is is configured as optional, it also skips
 * parsing all other positionals that were configured after it. This means that
 * after configuring one optional positional argument, all following positionals
 * must be configured as optional as well. In other words, a required positional
 * may not be configured after an optional one. Attempting that creates a
 * configuration-time error.
 * 
 * Variadicity relates to another concept: at parse-time, once the parser
 * decides that the positional to parse next is variadic, all words that would
 * be positional values (i.e. they are not flags, see later) are parsed as
 * values for this positional. This means that, at configuration time, no more
 * positionals may be configured after a variadic one. Attempting that creates
 * a configuration-time error. NB that, a variadic argument can also be
 * required. 
 * 
 * Positionals can also have two optional (but useful) properties: a description
 * and a meta-var. They are both used in automatically created help messages.
 * The former is a short text explaining the meaning of the argument. The latter
 * is an intuitive name that represents the value of the argument in the usage
 * string.
 * 
 * ### Flags
 * Flags differ from positionals in that they are not identified by their
 * position on the command line. Instead, they are identified by a string
 * literal (the flag's name, or sometimes informally called "the flag") found on
 * the command line, and are sometimes followed by a single value. (In that case
 * this value is not considered a positional value, of course.) This means that,
 * flags can appear in any order and can even mix inbetween positionals, without
 * disrupting their (very important) order.
 * 
 * A flag is configured in a parser using the `cap_parser_add_flag` function.
 * That is done with its name, data type, minimum count and maximum count. 
 * The name is a string literal which must begin with a flag prefix character.
 * By default that is '-' (dash), but can be changed, see later. The name serves
 * two purposes:
 * 1. it identifies the flag in a `ParsedArguments` object, and
 * 2. it locates the flag and its value on the command line.
 * 
 * The data type identifies how the flag's value should be parsed and stored.
 * Available types are the ones defined in @ref typed_union . Contrary to
 * positionals, the `DT_PRESENCE` type is allowed here. It is used to define
 * flags that are not followed by a value. The flag's presence or absence *is*
 * the information.
 * 
 * The minimum and maximum count define how many times the flag can be present
 * on the command line. For example, setting the minimum to zero configures a
 * flag that may be omitted on the command line. Conversely, if a flag is
 * configured with a minimum count greater than zero, and it is not found at
 * parse-time, a parse-time error occurs. Similarly, a parse-time error occurs
 * if a flag is found more times than is its maximum count. The maximum count
 * can be set to `-1` to allow the flag to be parsed up to any number of times.
 * 
 * Flags also have two optional properties: a description and a meta-var. Their
 * meaning is exactly the same as for positionals. The only difference is that,
 * configuring a meta-var for a flag of the "presence type" has no effect; those
 * flags take no values, so they are displayed with no meta-variable in
 * automatically generated help messages.
 * 
 * It is also possible to create aliases for configured flags, e.g. to allow
 * long and short spelling of the flag. That is done using the
 * `cap_parser_add_flag_alias` function.
 * 
 * ### Flag Prefix Characters
 * By default '-' (dash), these characters identify a word as a flag name. At
 * parse-time, the parser treates all words it encounters as flags if they begin
 * with one of the flag prefix characters. That happens even if the word does
 * not match any configured flag names or aliases. In that case, a parse-time
 * error is created (with an error message mentioning an "unknown flag"). There
 * is a way to allow words that begin with a flag prefix character to be parsed
 * as positional values (e.g. negative numbers). It is called positional-only
 * mode and is described later, in the Special Flags section.
 * 
 * It is possible to change the set of flag prefix characters using the
 * `parser_set_flag_prefix` function. It is not allowed to do that after any
 * flags have been configured, however, so calling the function will create a
 * configuration-time error. This unfortunately includes the default help flag
 * ('-h') which is present in the default parser.
 * 
 * ### Special Flags
 * Now is a good time to mention some special flags. Namely, that is the
 * automatic help flag and the flag separator. At configuratino-time, these
 * behave largely the same as regular flags, but they are handled differently at
 * parse-time. The main configuration-time difference is that, to define or
 * change them, the user must call `cap_parser_set_help_flag` and
 * `cap_parser_set_flag_separator` respectively, instead of the usual
 * `cap_parser_add_flag` function.
 *
 * The automatic help flag ('-h' by default) is used to invoke help text at
 * parse-time. This can be either a automatically generated help message (the
 * default), or a custom help text configured by the user. The flag must indeed
 * be treated in this special way, so that a help message can be printed even if
 * the command line is "invalid". As mentioned above, the help flag is
 * configured using the `cap_parser_set_help_flag` function. Compared to
 * `cap_parser_add_flag`, this allows the user to replace the help flag with a
 * new configuration, (for instance if the default is not desirable). Aliases
 * can be configured exactly the same as for regular flags.
 * 
 * The flag separator ('--' by default) is a "command" that instructs the parser
 * to enter positional-only mode at parse-time. In this mode, all words that
 * have not yet been consumed are treated as positional arguments, even if they
 * begin with a flag prefix character (see above section Flag Prefix
 * Characters). This allows for positional argument values that the parser would
 * otherwise consider as "unknown flag", such as negative numbers. Again, it is
 * necessary to treat the flag separator in a special way, because it influences
 * parser logic at parse-time. The function `cap_parser_set_flag_separator` is
 * used to configure this special flag instead of the usual
 * `cap_parser_add_flag`. This, similarly to the help flag, allows the user to
 * re-configure or disable it. Creating aliases is also allowed.
 * 
 * ### Custom Program Name and Desctiption
 * The parser sometimes needs to display the program name, e.g. when a value
 * cannot be parsed as the desired type and an error message is in order. By
 * default, the parser extracts the program name from the first string in
 * `argv`. However, it is possible to override this and configure a program name
 * explicitly. This can be done using the `cap_parser_set_program_name` function.
 * 
 * Similarly, a description of the program can be configured. This is a text
 * displayed at the top of an automatically generated help message (see later).
 * It is also possible to configure an epilogue, a text do be displayed at the
 * very end of the help message. These texts are empty by default and can be
 * configured using `cap_parser_set_description` and `cap_parser_set_epilogue`
 * functions respectively.
 * 
 * ### Automatic and Manual Help Texts
 * When encountering a parse-time error, the parser is able to print a help message before exiting the program. By default, this message is automatically generated on-demand based on configured flags and positionals, but it can be explicitly changed to anything the user wants. That is done using `cap_parser_set_custom_help`. The automatic help looks similar to this.
 * ```
 * usage:
 *     prog.exe [ -f FORMAT ] INPUT_FILE [ OUTPUT_FILE ]
 * 
 * Prog is a simple tool for doing useful things.
 * 
 * Available Flags:
 * -f FORMAT
 *     A c-style format string
 * 
 * Positional Arguments:
 * INPUT_FILE
 *     Read input from this file
 * OUTPUT_FILE
 *     Write output to this file. If missing, print to standard output.
 * ```
 * 
 * TODO
 * 
 * ## Parsing
 * 
 * TODO
 * 
 */

#include "data_type.h"
#include "flag_info.h"
#include "helper_functions.h"
#include "typed_union.h"
#include "parsed_arguments.h"
#include "positional_info.h"

#include <assert.h>
#include <stddef.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// ============================================================================
// === PARSER: DEFINITION OF TYPES ============================================
// ============================================================================

/**
 * @addtogroup parser
 * @{
 */

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
    char * mProgramName;
    char * mDescription;
    char * mEpilogue;
    char * mCustomHelp;
    char * mCustomUsage;

    bool mEnableHelp;
    bool mEnableUsage;

    FlagInfo ** mFlags;
    size_t mFlagCount;
    size_t mFlagAlloc;

    PositionalInfo ** mPositionals;
    size_t mPositionalCount;
    size_t mPositionalAlloc;

    char * mFlagPrefixChars;
    FlagInfo * mFlagSeparatorInfo;
    FlagInfo * mHelpFlagInfo;
} ArgumentParser;

/**
 * Identifies a parse-time error.
 * 
 * Identifies a parse-time error for the purpose of noexit parsing. The 
 * function `cap_parser_parse` constructs an error message depending on the 
 * error type. Each error can supply up to two other words to be insreted into 
 * the message, such as the name of a flag. If an error type expects any 
 * additional words, it is written in a comment block above it.
 */
typedef enum {
    /** 
     * No error.
     * 
     * This value is given when parsing was successful.
     */
    PER_NO_ERROR,
    /**
     * Help flag was encountered.
     */
    PER_HELP,
    /**
     * Some required positionals were omitted.
     */
    PER_NOT_ENOUGH_POSITIONALS,
    /**
     * Too many positionals were given.
     */
    PER_TOO_MANY_POSITIONALS,
    /**
     * Cannot parse a value given for a positional.
     * 
     * Cannot parse a value that was given for a positional argument, with respect to its type. Additional words are the name of the positional and the problematic value.
     */
    PER_CANNOT_PARSE_POSITIONAL,
    /**
     * An unknown flag was encountered.
     * 
     * The parser found an unknown flag. Additional word is the name of the unknown flag.
     */
    PER_UNKNOWN_FLAG,
    /**
     * No value was given for a flag.
     * 
     * A flag with a type other than `DT_PRESENCE` is missing a value. Additional word is the name of the flag.
     */
    PER_MISSING_FLAG_VALUE,
    /**
     * Cannot parse a value given to a flag.
     * 
     * Cannot parse a value that was given for a flag with respect to the flag's type. Additional words are the name of the flag and the problematic value.
     */
    PER_CANNOT_PARSE_FLAG,
    /**
     * A flag was not given enough times.
     * 
     * A flag was given less times than is required by the parser configuration. Additional word is the name of the flag.
     */
    PER_NOT_ENOUGH_FLAGS,

    /**
     * A flag was given too many times.
     * 
     * A flag was given more times than is required by the parser configuration. Additional word is the name of the flag.
     */
    PER_TOO_MANY_FLAGS
} ParsingError;

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
    
    /// first word to be inserted into an error message. 
    /// The nmeaning of this word depends on `mError`.
    const char * mFirstErrorWord;
    
    /// second word to be inserted into an error message.
    /// The nmeaning of this word depends on `mError`.
    const char * mSecondErrorWord;
   
    /// Type of a parsing error that occured.
    /// This value indicates to the caller of `cap_parser_parse_noexit` what 
    /// error message should be created, if any. It also indicates the meaning 
    /// of `mFirstErrorWord` and `mSecondErrorWord`. `PER_NO_ERROR` indicates 
    /// successful parsing.
    ParsingError mError;
} ParsingResult;

// ============================================================================
// === PARSER: DEFINITION OF PRIVATE TYPES ====================================
// ============================================================================

typedef enum {
    AFE_OK,
    AFE_MISSING_PARSER,
    AFE_MISSING_NAME,
    AFE_INVALID_PREFIX,
    AFE_DUPLICATE,
    AFE_MIN_COUNT_NEGATIVE,
    AFE_MAX_COUNT_VIOLATION,
    AFE_MAX_COUNT_ZERO
} AddFlagError;

typedef enum {
    AFAE_OK,
    AFAE_MISSING_PARSER,
    AFAE_MISSING_NAME,
    AFAE_MISSING_ALIAS,
    AFAE_INVALID_PREFIX,
    AFAE_FLAG_DOES_NOT_EXIST,
    AFAE_DUPLICATE_ALIAS
} AddFlagAliasError;

typedef enum {
    APE_ANYTHING_AFTER_VARIADIC,
    APE_DUPLICATE,
    APE_MISSING_NAME,
    APE_MISSING_PARSER,
    APE_NOT_IMPLEMENTED,
    APE_OK,
    APE_PRESENCE,
    APE_REQUIRED_AFTER_OPTIONAL,
} AddPositionalError;

typedef enum {
    OFPE_NO_ERROR,
    OFPE_UNKNOWN_FLAG,
    OFPE_MISSING_FLAG_VALUE,
    OFPE_CANNOT_PARSE_FLAG
} OneFlagParsingError;

typedef struct {
    const FlagInfo * mFlag;
    TypedUnion mValue;
    int mWordsConsumed;
    OneFlagParsingError mError;
} OneFlagParsingResult;

typedef enum {
    OPPE_NO_ERROR,
    OPPE_TOO_MANY,
    OPPE_CANNOT_PARSE
} OnePositionalParsingError;

typedef struct {
    const PositionalInfo * mPositional;
    TypedUnion mValue;
    int mWordsConsumed;
    OnePositionalParsingError mError;
} OnePositionalParsingResult;

typedef enum {
    TOO_MANY,
    TOO_FEW,
    GOOD
} BoundsCheckingResult;

typedef struct {
    const FlagInfo * mFlag;
    BoundsCheckingResult mCount;
} FlagCountCheckResult;

// ============================================================================
// === PARSER: DECLARATION OF PRIVATE FUNCTIONS ===============================
// ============================================================================

static bool _cap_parse_double(const char * word, double * value);
static bool _cap_parse_int(const char * word, int * value);
static bool _cap_parse_word_as_type(
    const char * word, DataType type, TypedUnion * uninitialized_tu);
static FlagInfo * _cap_parser_find_flag(
    const ArgumentParser * parser, const char * flag);
static bool _cap_flag_matches_name_or_alias(
    const FlagInfo * flag_info, const char * name_or_alias);
static const char * _cap_get_shortest_flag_name(const FlagInfo * flag_info);

static OnePositionalParsingResult _cap_parser_parse_one_positional(
    const ArgumentParser * parser, const char * arg, 
    size_t positional_index);
static OneFlagParsingResult _cap_parser_parse_one_flag(
    const ArgumentParser * parser, int argc, const char * const * argv,
    int index); 
static void _cap_parser_parse_flags_and_positionals(
    const ArgumentParser * parser, int argc, const char * const * argv,
    ParsingResult * result);

static FlagCountCheckResult _cap_parser_check_flag_counts(
    const ArgumentParser * parser, const ParsedArguments * parsed_arguments);
static void _cap_parser_check_flag_and_positional_counts(
    const ArgumentParser * parser, ParsingResult * result);

// ============================================================================
// === PARSER: DECLARATION OF PUBLIC FUNCTIONS ================================
// ============================================================================

void cap_parser_set_help_flag(
        ArgumentParser * parser, const char * name, const char * description);
void cap_parser_set_flag_separator(
    ArgumentParser * parser, const char * separator, const char * description);
void cap_parser_enable_help(ArgumentParser * parser, bool enable);
void cap_parser_enable_usage(ArgumentParser * parser, bool enable);

// ============================================================================
// === PARSER: CREATION AND DESTRUCTION =======================================
// ============================================================================

/**
 * Creates a new empty parser.
 * 
 * Creates a new parser with no configured flags or positional arguments. 
 * The caller becomes the owner of this new object and should dispose of it 
 * using `cap_parser_destroy` when it is no longer needed.
 * 
 * @return new object
 * 
 * @see cap_parser_destroy
 * @see cap_parser_make_default
 */
ArgumentParser * cap_parser_make_empty() {
    ArgumentParser * p = (ArgumentParser *) malloc(sizeof(ArgumentParser));
    *p = (ArgumentParser) {
        .mProgramName = NULL,
        .mDescription = NULL,
        .mEpilogue = NULL,
        .mCustomHelp = NULL,
        .mCustomUsage = NULL,
        .mEnableHelp = false,
        .mEnableUsage = false,

        .mFlags = NULL,
        .mFlagCount = 0u,
        .mFlagAlloc = 0u,

        .mPositionals = NULL,
        .mPositionalCount = 0u,
        .mPositionalAlloc = 0u,
        
        .mFlagPrefixChars = copy_string("-"),
        .mFlagSeparatorInfo = NULL,
        .mHelpFlagInfo = NULL
    };
    return p;
}

/**
 * Creates a new default parser.
 * 
 * Creates a new parser with default configuration. It differs from an empty 
 * parser in that it contains the automatic help flag "-h" and the flag 
 * separator "--" that switches the parser to positional-only mode.
 * 
 * @see cap_parser_make_empty
 * @see cap_parser_destroy
 */
ArgumentParser * cap_parser_make_default() {
    ArgumentParser * parser = cap_parser_make_empty();
    cap_parser_set_help_flag(parser, "-h", NULL);
    cap_parser_set_flag_separator(parser, "--", NULL);
    cap_parser_enable_help(parser, true);
    cap_parser_enable_usage(parser, true);
    return parser;
}

/**
 * Destroys an `ArgumentParser` object.
 * 
 * Destroys an `ArgumentParser` object and all data stored in it. 
 * `ParsedArgument` objects created using this parser are not owned by it 
 * and can be used after a parser is destroyed.
 * 
 * @param parser object to destroy. If it is `NULL`, nothing happens.
 */
void cap_parser_destroy(ArgumentParser * parser) {
    if (!parser) return;
    if (parser -> mProgramName) {
        free(parser -> mProgramName);
        parser -> mProgramName = NULL;
    }
    delete_string_property(&(parser -> mDescription));
    delete_string_property(&(parser -> mEpilogue));
    delete_string_property(&(parser -> mCustomHelp));
    for (size_t i = 0; i < parser -> mFlagCount; ++i) {
        cap_flag_info_destroy(parser -> mFlags[i]);
    }
    for (size_t i = 0; i < parser -> mPositionalCount; ++i) {
        cap_positional_info_destroy(parser -> mPositionals[i]);
    }
    free(parser -> mFlags);
    free(parser -> mPositionals);
    parser -> mFlags = NULL;
    parser -> mPositionals = NULL;
    parser -> mFlagCount = parser -> mFlagAlloc = 0u;
    parser -> mPositionalCount = parser -> mPositionalAlloc = 0;

    delete_string_property(&(parser -> mFlagPrefixChars));

    if (parser -> mHelpFlagInfo) {
        cap_flag_info_destroy(parser -> mHelpFlagInfo);
        parser -> mHelpFlagInfo = NULL;
    }
    if (parser -> mFlagSeparatorInfo) {
        cap_flag_info_destroy(parser -> mFlagSeparatorInfo);
        parser -> mFlagSeparatorInfo = NULL;
    }

    free(parser);
}

// ============================================================================
// === PARSER: GENERAL CONFIGURATION ==========================================
// ============================================================================

/**
 * Sets characters to be considered flag-prefix characters.
 * 
 * Configures a set of characters to be considered flag-prefixes by the parser.
 * Any command line word that begins with one of these characters is considered 
 * a flag unless parsing in positional-only mode, see below. By default, 
 * '-' (dash) is considered the only prefix character.
 * 
 * If `parser` is `NULL`, nothing happens. If `prefix_chars` is `NULL` or
 * empty, the program exits with an error.
 * 
 * Note that, this configuration must be done before adding any flags, because 
 * added flags are required to begin with a flag prefix character. If any flags 
 * are present when calling this function, the program exits with an error.
 * 
 * @param parser parser object to configure
 * @param prefix_chars null-terminated string of characters that should be 
 *        considered flag prefixes.
 */
void cap_parser_set_flag_prefix(
        ArgumentParser * parser, const char * prefix_chars) {
    if (!parser) return;
    if (!prefix_chars || strlen(prefix_chars) == 0) {
        fprintf(stderr, "cap: missing flag prefix characters\n");
        exit(-1);
    }
    if (parser -> mFlagCount || parser -> mHelpFlagInfo) {
        fprintf(
            stderr, "cap: cannot set flag prefix characters when flags "
            "already exist\n");
        exit(-1);
    }
    set_string_property(&(parser -> mFlagPrefixChars), prefix_chars);
    return;
}

/**
 * Sets a flag separator.
 * 
 * Configures a parser to recognize a flag separator, or to not have one. A 
 * flag separator is a special flag that, when found on the command line, puts 
 * the parser in positional only mode. In this mode all command line words are 
 * used as positionals even if they begin with a flag prefix character.
 * By default the flag separator is '--' (double dash). 
 * 
 * If the given string is empty or a flag already exists with the same name, 
 * the program exits with an error. This string is copied into the parser and
 * the caller remains the owner of the given pointer.
 * 
 * The `description` parameter allows to set a short description of this 
 * separator which appears in help messages. If `NULL` is given, a default
 * description is used. If the description should be blank, an empty string 
 * should be passed to this function.
 * 
 * @param parser parser object to configure
 * @param separator null-terminated string containing the new flag separator.
 *        If `NULL` is given, the flag separator is explicitly disabled.
 * @param description short description of this symbol for use in help mesages.
 */
void cap_parser_set_flag_separator(
        ArgumentParser * parser, const char * separator, 
        const char * description) {
    static const char * const DEFAULT_FLAG_SEPARATOR_DESCRIPTION 
        = "Treat all following command line arguments as positionals";

    if (!parser) return;
    if (separator && strlen(separator) == 0) {
        fprintf(stderr, "cap: missing flag separator\n");
        exit(-1);
    }
    if (parser -> mFlagSeparatorInfo) {
        cap_flag_info_destroy(parser -> mFlagSeparatorInfo);
        parser -> mFlagSeparatorInfo = NULL;
    }
    if (_cap_parser_find_flag(parser, separator)) {
        fprintf(
            stderr, "cap: cannot set '%s' as flag separator - this flag"
            " already exists\n", separator);
        exit(-1);
    }
    if (!separator) {
        return;
    }
    FlagInfo * separator_info = cap_flag_info_make(
        separator, NULL, 
	description ? description : DEFAULT_FLAG_SEPARATOR_DESCRIPTION,
       	DT_PRESENCE, 0, -1);
    parser -> mFlagSeparatorInfo = separator_info;
}

/**
 * Sets the name of the program.
 * 
 * Sets the display name of the program for use in help and usage messages.
 * By default, the program name is constructed from the first command line 
 * argument.
 * 
 * @param parser parser object to configure
 * @param name null-terminated name of the program. If NULL, the parser is 
 *        reset to default behaviour (described above).
 */
void cap_parser_set_program_name(ArgumentParser * parser, const char * name) {
    if (!parser) {
        return;
    }
    set_string_property(&(parser -> mProgramName), name);
}

/**
 * Sets the program's description.
 * 
 * Sets the description of the program that is displayed at the beginning of 
 * the automatically generated help message. By default no description is 
 * printed. A previously configured description can be removed using this 
 * function by giving `NULL` as the second parameter.
 * 
 * If `description` is not `NULL`, a copy of it is created and `parser` becomes
 * the owner of that copy.
 * 
 * @param parser parser object to configure
 * @param description null-terminated description of the program, or NULL
 */
void cap_parser_set_description(
        ArgumentParser * parser, const char * description) {
    if (!parser) {
        return;
    }
    set_string_property(&(parser -> mDescription), description);
}

/**
 * Sets the epilogue of the program's help.
 * 
 * Sets an epilogue of the program's description which is displayed at the end 
 * of the automatically generated help message. By default no epilogue is 
 * printed and a previously configured epilogue can be removed by giving `NULL` 
 * as the second parameter.
 * 
 * If `epilogue` is not `NULL`, a copy of it is created and `parser` becomes 
 * the owner of that copy.
 * 
 * @param parser object to configure
 * @param epilogue epilogue of the help message, or `NULL`
 */
void cap_parser_set_epilogue(ArgumentParser * parser, const char * epilogue) {
    if (!parser) {
        return;
    }
    set_string_property(&(parser -> mEpilogue), epilogue);
}

/**
 * Sets a custom help message.
 * 
 * Sets a custom help message to displays instead of an automatically generated 
 * one. The given text is displayed verbatim whenever a help message is 
 * printed. This configuration overrides the configuration done using 
 * `cap_parser_set_description`, `cap_parser_set_epilogue` and setting metavars 
 * for flags and arguments for the purposes of displaying a help message. Those
 * configurations are not removed by calling this function, however.
 * 
 * Unlike `cap_parser_set_description`, `cap_parser_set_epilogue`, 
 * or `cap_parser_set_program_name`, if `help` is `NULL`, the parser is 
 * configured to automatically generate help messages. An empty help can be
 * configured by passing an empty string to this function.
 * 
 * If `help` is not `NULL`, a copy of it is created and `parser` becomes the 
 * owner of that copy.
 * 
 * @param parser object to configure
 * @param help custom help message, or `NULL`
 */
void cap_parser_set_custom_help(ArgumentParser * parser, const char * help) {
    if (!parser) {
        return;
    }
    set_string_property(&(parser -> mCustomHelp), help);
}

/**
 * Sets a custom usage string.
 * 
 * Sets a custom usage message to display instead of an automatically generated 
 * one. The given text is displayed verbatim whenever a help message is to be 
 * printed. If `usage` is `NULL`, the parser reverts to generating usage 
 * automatically. 
 * 
 * An empty string should be used if usage should be blank. 
 * Alternately, usage can be suppressed by calling `cap_parser_enable_usage`.
 * If usage was previously disabled using `cap_parser_enable_usage(false)`,
 * it does not get re-enabled by calling this function.
 * 
 * @param parser object to configure
 * @param usage null-terminated verbatim usgage string, or `NULL` to revert to 
 *        automatic.
 */
void cap_parser_set_custom_usage(ArgumentParser * parser, const char * usage) {
    if (!parser) {
        return;
    }
    set_string_property(&(parser -> mCustomUsage), usage);
}

/**
 * Enables or disable displaying help.
 * 
 * Temporarily enables or disables displaying help messages. When disabling 
 * help by setting `enable` to `false`, information such as program description 
 * is not removed. Note that, changing any help configuration when displaying 
 * help is disabled does not re-enable it. Help must be manually re-enabled 
 * using this function.  
 * 
 * @param parser object to configure
 * @param enable `true` if help should be displayed, `false` if not
 */
void cap_parser_enable_help(ArgumentParser * parser, bool enable) {
    if (!parser) return;
    parser -> mEnableHelp = enable;
}

/**
 * Enables or disable displaying usage.
 * 
 * Temporarily enables or disables displaying usage messages. When disabling 
 * usage by setting `enable` to `false`, information such as custom usage 
 * is not removed. Note that, changing any related configuration when displaying 
 * usage is disabled does not re-enable it. Usage must be manually re-enabled 
 * using this function.  
 * 
 * @param parser object to configure
 * @param enable `true` if usage should be displayed, `false` if not
 */
void cap_parser_enable_usage(ArgumentParser * parser, bool enable) {
    if (!parser) {
        return;
    }
    parser -> mEnableUsage = enable;
}

// ============================================================================
// === PARSER: ADDING FLAGS ===================================================
// ============================================================================

/**
 * Registers a flag in an argument parser.
 * 
 * Behaves the same as cap_parser_add_flag but returns an error code instead of
 * exiting when an error is encountered.
 * 
 * Registers a new flag in `parser` under the name `flag`. The flag's name must 
 * be unique and must be different from the flag separator (if one is 
 * configured). Do note that, 
 * the `cap_parser_make_default` factory returns a parser containg the default 
 * flag separator and the default help falg '-h'. Additionally, the flag name 
 * must begin with a flag prefix character, either the default '-' or one of 
 * the characters previously configured using  `cap_parser_set_flag_prefix`.
 * 
 * The `type` parameter specifies the type of the value stored in this flag. 
 * The type `DT_PRESENCE` should be used if a flag should not store any 
 * explicit values (i.e. the flag's presence or absence is the only information 
 * associated with it). For all other types the next word on the command line 
 * is used as the flag's value. If it cannot be parsed at parse-time, the 
 * program exits with an error message.
 * 
 * The `min_count` and `max_count` parameters specify the minimum and maximum 
 * times the flag can be present on the command line. If a flag is found less 
 * than `min_count` times or more than `max_count` times, the program exits 
 * with an error at parse-time. This pair of parameters can be used to create 
 * required flags, flags that may be given up to once, and similar constructs.
 * If `max_count` is negative, the flag can be read up to any number of times.
 * 
 * The `metavar` parameter specifies how the value of the flag should be
 * displayed in usage and help messages. If `NULL` is given, the flag's data 
 * type is used instead. The `description` parameter provides a short 
 * explanation of the flag that will be displayd in help messages automatically 
 * generated by the  parser. If `NULL` is given, no  description is displayed.
 * 
 * @param parser object to create the flag in
 * @param flag null-terminated name of the new flag, including flag prefix 
 *        charactes (e.g. '-')
 * @param type data type of the flag's values
 * @param min_count minimum number of times the flag must be given on the 
 *        command line. Must be at least zero.
 * @param max_count maximum number of times the flag may be given on the 
 *        command line. Must be either negative or at least `min_count`.
 * @param metavar display name of the flag's value in help messages
 * @param description short description of the flag's meaning to display in 
 *        automatically generated help messages.
 * @return error code representing the kind of error that occurs, or AFE_OK if 
 *        a flag was added successfully
 */
AddFlagError cap_parser_add_flag_noexit(
    ArgumentParser * parser, const char * flag, DataType type, 
    int min_count, int max_count, const char * metavar,
    const char * description) 
{
    const char * const flag_prefix = parser -> mFlagPrefixChars;
    if (!parser) {
        return AFE_MISSING_PARSER;
    }
    if (!flag || !strlen(flag)) {
        return AFE_MISSING_NAME;
    }
    if (!strchr(flag_prefix, *flag)) {
        return AFE_INVALID_PREFIX;
    }
    // this also checks agains the help flag and flag separator if they exist
    if (_cap_parser_find_flag(parser, flag)) {
        return AFE_DUPLICATE;
    }
    if (min_count < 0) {
        return AFE_MIN_COUNT_NEGATIVE;
    }
    if (max_count >= 0 && max_count < min_count) {
        return AFE_MAX_COUNT_VIOLATION;
    }
    if (min_count == 0 && max_count == 0) {
        return AFE_MAX_COUNT_ZERO;
    }
    if (parser -> mFlagCount >= parser -> mFlagAlloc) {
        size_t alloc_size = parser -> mFlagAlloc;
        alloc_size = alloc_size ? alloc_size * 2 : 1;
        parser -> mFlagAlloc = alloc_size;
        parser -> mFlags = (FlagInfo **) realloc(
            parser -> mFlags, alloc_size * sizeof(FlagInfo *));
    }
    FlagInfo * new_flag = cap_flag_info_make(
        flag, metavar, description, type, min_count, max_count);
    parser -> mFlags[parser -> mFlagCount++] = new_flag;

    return AFE_OK;
}

/**
 * Registers a flag in an argument parser.
 * 
 * Behaves the same as cap_parser_add_flag_noexit but prints an error message 
 * and exits the program if an error is encountered.
 * 
 * Registers a new flag in `parser` under the name `flag`. The flag's name must 
 * be unique and the program exits with an error if a duplicate flag name is 
 * given. Flag names may not be identical to the flag separator. Do note that, 
 * the `cap_parser_make_default` factory returns a parser containg the default 
 * flag separator and the default help falg '-h'. Additionally, the flag name 
 * must begin with a flag prefix character, either the default '-' or one of 
 * the characters previously configured using  `cap_parser_set_flag_prefix`.
 * 
 * The `type` parameter specifies the type of the value stored in this flag. 
 * The type `DT_PRESENCE` should be used if a flag should not store any 
 * explicit values (i.e. the flag's presence or absence is the only information 
 * associated with it). For all other types the next word on the command line 
 * is used as the flag's value. If it cannot be parsed at parse-time, the 
 * program exits with an error message.
 * 
 * The `min_count` and `max_count` parameters specify the minimum and maximum 
 * times the flag can be present on the command line. If a flag is found less 
 * than `min_count` times or more than `max_count` times, the program exits 
 * with an error at parse-time. This pair of parameters can be used to create 
 * required flags, flags that may be given up to once, and similar constructs.
 * If `max_count` is negative, the flag can be read up to any number of times.
 * 
 * The `metavar` parameter specifies how the value of the flag should be
 * displayed in usage and help messages. If `NULL` is given, the flag's data 
 * type is used instead. The `description` parameter provides a short 
 * explanation of the flag that will be displayd in help messages automatically 
 * generated by the  parser. If `NULL` is given, no  description is displayed.
 * 
 * @param parser object to create the flag in
 * @param flag null-terminated name of the new flag, including flag prefix 
 *        charactes (e.g. '-')
 * @param type data type of the flag's values
 * @param min_count minimum number of times the flag must be given on the 
 *        command line. Must be at least zero.
 * @param max_count maximum number of times the flag may be given on the 
 *        command line. Must be either negative or at least `min_count`.
 * @param meta_var display name of the flag's value in help messages
 * @param description short description of the flag's meaning to display in 
 *        automatically generated help messages.
 */
void cap_parser_add_flag(
    ArgumentParser * parser, const char * flag, DataType type, 
    int min_count, int max_count, const char * meta_var,
    const char * description) 
{
    AddFlagError error = cap_parser_add_flag_noexit(
        parser, flag, type, min_count, max_count, meta_var, description);
    switch (error) {
        case AFE_OK:
            return;
        case AFE_MISSING_PARSER:
            fprintf(stderr, "cap: missing parser\n");
            break;
        case AFE_MISSING_NAME:
            fprintf(stderr, "cap: missing flag name");
            break;
        case AFE_INVALID_PREFIX:
            fprintf(
                stderr, "cap: invalid flag name - must begin with one"
                " of \"%s\"\n", parser -> mFlagPrefixChars);
            break;
        case AFE_DUPLICATE:
            fprintf(stderr, "cap: duplicate flag definition %s\n", flag);
            break;
        case AFE_MIN_COUNT_NEGATIVE:
            fprintf(
                stderr, "cap: min_count requirement must not be"
                " negative\n");
            break;
        case AFE_MAX_COUNT_VIOLATION:
            fprintf(
                stderr, "cap: max_count requirement must not be less than"
                " min_count\n");
            break;
        case AFE_MAX_COUNT_ZERO:
            fprintf(
                stderr, "cap: min_count and max_count cannot be both zero\n");
            break;
        default:
            assert(false && "unreachable in cap_parser_add_flag_noexit");
    }
    exit(-1);
}

/**
 * Creates an alias for an existing flag.
 * 
 * Behaves the same as cap_parser_add_flag_alias but returns an error code 
 * instead of exiting when an error is encountered.
 * 
 * Adds a new flag to the parser which functions as an alias for an existing 
 * flag. When parsing, this alias is parsed the same way as the original flag,
 * and any value parsed for the alias is stored with the original flag's name
 * (not under the alias.)
 * 
 * @param parser object to configure
 * @param name original flag name. Must be an existing flag.
 * @param alias alias for 'name'. Must be a new, unique flag name.
 */
AddFlagAliasError cap_parser_add_flag_alias_noexit(
    ArgumentParser * parser, const char * name, const char * alias) 
{
    if (!parser) {
        return AFAE_MISSING_PARSER;
    }
    if (!name || !strlen(name)) {
        return AFAE_MISSING_NAME;
    }
    if (!alias || !strlen(alias)) {
        return AFAE_MISSING_ALIAS;
    }
    if (!strchr(parser -> mFlagPrefixChars, *alias)) {
        return AFAE_INVALID_PREFIX;
    }
    FlagInfo * fi = _cap_parser_find_flag(parser, name);
    if (!fi) {
        return AFAE_FLAG_DOES_NOT_EXIST;
    }
    if (_cap_parser_find_flag(parser, alias)) {
        return AFAE_DUPLICATE_ALIAS;
    }

    // register the alias
    if (fi -> mAliasCount >= fi -> mAliasAlloc) {
        fi -> mAliasAlloc = fi -> mAliasAlloc ? 2 * fi -> mAliasAlloc : 1u;
        fi -> mAliases = (char **) realloc(fi -> mAliases, fi -> mAliasAlloc * sizeof(char *));
    }
    fi -> mAliases[fi -> mAliasCount++] = copy_string(alias);
    return AFAE_OK;
}

/**
 * Creates an alias for an existing flag.
 * 
 * Adds a new flag to the parser which functions as an alias for an existing 
 * flag. When parsing, this alias is parsed the same way as the original flag,
 * and any value parsed for the alias is stored with the original flag's name
 * (not under the alias.)
 * 
 * @param parser object to configure
 * @param name original flag name. Must be an existing flag.
 * @param alias alias for 'name'. Must be a new, unique flag name.
 */
void cap_parser_add_flag_alias(
    ArgumentParser * parser, const char * name, const char * alias) 
{
    AddFlagAliasError error = cap_parser_add_flag_alias_noexit(
        parser, name, alias);
    switch (error) {
        case AFAE_OK:
            return;
        case AFAE_MISSING_PARSER:
            fprintf(stderr, "cap: missing parser\n");
            break;
        case AFAE_MISSING_NAME:
            fprintf(stderr, "cap: missing flag name\n");
            break;
        case AFAE_MISSING_ALIAS:
            fprintf(stderr, "cap: missing flag alias\n");
            break;
        case AFAE_INVALID_PREFIX:
            fprintf(stderr, "cap: invalid flag alias prefix: must be one of"
            " '%s'", parser -> mFlagPrefixChars);
            break;
        case AFAE_FLAG_DOES_NOT_EXIST:
            fprintf(
                stderr, "cap: flag '%s' does not exist, cannot set alias"
                " for it\n", name);
            break;
        case AFAE_DUPLICATE_ALIAS:
            fprintf(
                stderr, "cap: cannot set alias '%s', this flag already"
                " exists\n", alias);
            break;
        default:
            assert(false && "unreachable in cap_parser_add_flag");
    }
    exit(-1);
}

/**
 * Sets a flag for displaying help.
 * 
 * Sets a flag that makes the program immediately display help information 
 * and exit. By default, this flag is "-h", but can be overriden using this 
 * function.
 * 
 * If `name` is `NULL` and a help flag is already configured, it is removed. 
 * If a duplicate or otherwise invalid `name` is given, the program exits with
 * an error.
 * 
 * If `description` is `NULL`, a default is used. If the help flag should have
 * no description, an empty string should be used instead.
 * 
 * @param parser object to configure
 * @param name name of the custom help flag
 * @param description description of the help flag. If it is `NULL`, a default 
 *        is used.
 */
void cap_parser_set_help_flag(
        ArgumentParser * parser, const char * name, const char * description) {
    static const char * const DEFAULT_HELP_DESCRIPTION 
        = "Display this help message and exit";

    if (!parser) {
        return;
    }
    if (parser -> mHelpFlagInfo) {
        // name is identical -> there's nothing to do
        if (name && !strcmp(name, parser -> mHelpFlagInfo -> mName)) {
            return;
        }
        // now we un-configure the existing help flag
        cap_flag_info_destroy(parser -> mHelpFlagInfo);
        parser -> mHelpFlagInfo = NULL;
    }
    
    // at this moment, the help flag is definitely not configured: either it 
    // did not exist, or it just got removed.

    // if the goal was to remove the help flag, we end here
    if (!name) {
        return;
    }

    if (_cap_parser_find_flag(parser, name)) {
        fprintf(
            stderr, "cap: cannot add help flag '%s' because an identical flag"
            " already exists\n", name);
        exit(-1);
    }
    if (*name == '\0' || !strchr(parser -> mFlagPrefixChars, *name)) {
        fprintf(stderr, "cap: invalid flag name '%s'\n", name);
        exit(-1);
    }
    FlagInfo * fi = cap_flag_info_make(
        name, NULL, description ? description : DEFAULT_HELP_DESCRIPTION,
       	DT_PRESENCE, 0, 1);
    parser -> mHelpFlagInfo = fi;
}

// ============================================================================
// === PARSER: ADDING POSITIONALS =============================================
// ============================================================================

/**
 * Configures a new positional argument.
 * 
 * Configures a new positional argument in `parser` with the name `name`. If a 
 * positional argument with the same name already exists, returns an 
 * appropriate error code.
 * 
 * The data type of the argument's value is given by the `type` parameter. If 
 * the type `DT_PRESENCE` is used, an error code is returned. The 
 * positional argument's type is used to parse a value at parse-time. If the 
 * word given on the command line cannot be parsed as that type, parsing 
 * terminates with an error.
 * 
 * If `required` is false, this argument will be configured as optional. 
 * Required positionals may not be configured after any number of optional 
 * ones. At parse-time, optional positionals may be missing on the command 
 * line.
 * 
 * If `variadic` is true, this argument can take multiple values. After one 
 * variadic argument, no other arguments may be configured. Note that, 
 * a variadic argument can also be required. At parse-time, all available 
 * command line words that are not flags are used as values for the variadic 
 * argument. If it is also required, at least one such word must be found.
 * 
 * The `metavar` parameter specifies the display name of this argument in help 
 * and usage messages. If `NULL` is given, the argument's `name` is used 
 * instead. The `description` parameter specifies a short explanation of the 
 * argument that will be displayed in help messages automatically generated by 
 * the parser. If `NULL` is given, no description is displayed.
 * 
 * @param parser object to configure
 * @param name name of the new argument
 * @param type data type of the new argument
 * @param required required-ness of the argument, see above for constraints
 * @param variadic variadicity of the argument, see above for the constraints
 * @param metavar display name for this argument in help messages
 * @param description short description of the argument's meaning to display 
 *        in automatically generated help messages
 * @return TODO
 */
AddPositionalError cap_parser_add_positional_noexit(
    ArgumentParser * parser, const char * name, DataType type, bool required, 
    bool variadic, const char * metavar, const char * description)
{
    if (!parser) {
        return APE_MISSING_PARSER;
    }
    if (!name || strlen(name) == 0) {
        return APE_MISSING_NAME;
    }
    if (type == DT_PRESENCE) {
        return APE_PRESENCE;
    }
    for (size_t i = 0; i < parser -> mPositionalCount; ++i) {
        const PositionalInfo * pi = parser -> mPositionals[i];
        if (!strcmp(pi -> mName, name)) {
            return APE_DUPLICATE;
        }
    }
    if (parser -> mPositionalCount >= 1u) {
        const PositionalInfo * last
            = parser -> mPositionals[parser -> mPositionalCount - 1u];
        if (last -> mVariadic) {
            return APE_ANYTHING_AFTER_VARIADIC;
        }
        if (required && !last -> mRequired) {
            return APE_REQUIRED_AFTER_OPTIONAL;
        }
    }
    if (parser -> mPositionalCount >= parser -> mPositionalAlloc) {
        size_t alloc_size = parser -> mPositionalAlloc;
        alloc_size = alloc_size ? alloc_size * 2 : 1;
        parser -> mPositionalAlloc = alloc_size;
        parser -> mPositionals = (PositionalInfo **) realloc(
            parser -> mPositionals, alloc_size * sizeof(PositionalInfo *));
    }
    PositionalInfo * new_positional = cap_positional_info_make(
	name, metavar, description, type, required, variadic);
    parser -> mPositionals[parser -> mPositionalCount++] = new_positional;

    return APE_OK;
}

/**
 * Configures a new positional argument.
 * 
 * Configures a new positional argument in `parser` with the name `name`. If a 
 * positional argument with the same name already exists, the program exits 
 * with an error.
 * 
 * The data type of the argument's value is given by the `type` parameter. If 
 * the type `DT_PRESENCE` is used, the program exits with an error. The 
 * positional argument's type is used to parse a value at parse-time. If the 
 * word given on the command line cannot be parsed as that type, parsing 
 * terminates with an error.
 * 
 * If `required` is false, this argument will be configured as optional. 
 * Required positionals may not be configured after any number of optional 
 * ones. At parse-time, optional positionals may be missing on the command 
 * line.
 * 
 * If `variadic` is true, this argument can take multiple values. After one 
 * variadic argument, no other arguments may be configured. Note that, 
 * a variadic argument can also be required. At parse-time, all available 
 * command line words that are not flags are used as values for the variadic 
 * argument. If it is also required, at least one such word must be found.
 * 
 * The `metavar` parameter specifies the display name of this argument in help 
 * and usage messages. If `NULL` is given, the argument's `name` is used 
 * instead. The `description` parameter specifies a short explanation of the 
 * argument that will be displayed in help messages automatically generated by 
 * the parser. If `NULL` is given, no description is displayed.
 * 
 * @param parser object to configure
 * @param name name of the new argument
 * @param type data type of the new argument
 * @param required if the argument is required
 * @param variadic variadicity of the argument, see above for the constraints
 * @param metavar display name for this argument in help messages
 * @param description short description of the argument's meaning to display 
 *        in automatically generated help messages
 */
void cap_parser_add_positional(
    ArgumentParser * parser, const char * name, DataType type, 
    bool required, bool variadic, const char * metavar,
    const char * description)
{
    AddPositionalError error = cap_parser_add_positional_noexit(
        parser, name, type, required, variadic, metavar, description);
    switch (error) {
        case APE_ANYTHING_AFTER_VARIADIC:
            fprintf(stderr, "cap: cannot add positional after variadic\n");
            break;
        case APE_DUPLICATE:
            fprintf(stderr, "cap: duplicate positional argument %s\n", name);
            break;
        case APE_MISSING_NAME:
            fprintf(stderr, "cap: invalid argument name\n");
            break;
        case APE_MISSING_PARSER:
            fprintf(stderr, "cap: missing parser\n");
            break;
        case APE_OK:
            return;
        case APE_PRESENCE:
            fprintf(
                stderr, "cap: data type DT_PRESENCE is invalid for positional"
                " arguments\n");
            break;
        case APE_REQUIRED_AFTER_OPTIONAL:
            fprintf(
                stderr, "cap: cannot add required positional after"
                " optional\n");
            break;
        default:
            assert(false && "unreachable in cap_parser_add_positional");
    }
    exit(-1);
}

// ============================================================================
// === PARSER: HELP ===========================================================
// ============================================================================

/**
 * Retrieves the program name for this parser.
 * 
 * Obtains the custom program name configured in `parser` or guesses the 
 * executable's name from `argv0` if not name has been configured. If `parser`
 * has no program name, the part of `argv0' after the right-most path separator
 * is used. On Windows, both '\' and '/' are considered path separators.
 * 
 * @param parser parser object to consult
 * @param argv0 first word on the command line
 * @return configured or estimated program name
 */
const char * cap_parser_get_program_name(
        const ArgumentParser * parser, const char * argv0) {
    if (parser -> mProgramName) {
        return parser -> mProgramName;
    }
    const char * program_name = argv0;
    const char * slash = strrchr(argv0, '/');
    if (slash) {
        program_name = slash + 1;
    }
#ifdef _WIN32
    const char * backslash = strrchr(argv0, '\\');
    if (backslash && backslash > slash) {
        program_name = backslash + 1;
    }
#endif
    return program_name;
}

/**
 * Prints a usage string.
 * 
 * Prints a usage string to `file` based on the flags and arguments configured
 * in `parser`. If a flag has aliases, the shortest available name is picked.
 */
void cap_parser_print_usage(
        const ArgumentParser * parser, FILE * file,
        const char * argv0) {
    if (!parser || !file) {
        return;
    }

    if (!parser -> mEnableUsage) {
        return;
    }
    if (parser -> mCustomUsage) {
        fprintf(file, "%s\n", parser -> mCustomUsage);
        return;
    }

    fprintf(file, "usage:\n");
    fprintf(file, "\t");
    fprintf(file, "%s", cap_parser_get_program_name(parser, argv0));

    if (parser -> mHelpFlagInfo) {
        fprintf(file, " [%s]", _cap_get_shortest_flag_name(parser -> mHelpFlagInfo));
    }

    for (size_t i = 0; i < parser -> mFlagCount; ++i) {
        const FlagInfo * fi = parser -> mFlags[i];
        fputc(' ', file);
        if (fi -> mMinCount == 0) {
            fputc('[', file);
        }
        fprintf(file, "%s", _cap_get_shortest_flag_name(fi));
        if (fi -> mType != DT_PRESENCE) {
            fprintf(file, " %s", cap_get_flag_metavar(fi));
        }
        if (fi -> mMinCount == 0) {
            fputc(']', file);
        }
    }

    if (parser -> mPositionalCount > 0u && parser -> mFlagSeparatorInfo) {
        fprintf(file, " [%s]", _cap_get_shortest_flag_name(parser -> mFlagSeparatorInfo));
    }
    size_t optionals = 0u;
    for (size_t i = 0; i < parser -> mPositionalCount; ++i) {
        const PositionalInfo * pi = parser -> mPositionals[i];
        fputc(' ', file);
        if (!pi -> mRequired) {
            fputc('[', file);
            ++optionals;
        }
        if (pi -> mMetaVar) {
            fprintf(file, "%s", pi -> mMetaVar);
            continue;
        }
        fprintf(file, "<%s>", pi -> mName);
    }
    for (size_t i = 0; i < optionals; ++i) {
        fputc(']', file);
    }
    fputc('\n', file);
}

/**
 * Prints a help message.
 * 
 * Prints a help message to `file`. This message is either set explicitly using
 * `cap_parser_set_custom_help`, or generated based on flag/argument 
 * configuration of `parser`.
 * 
 * @param parser parser to generate or extract the help message from
 * @param file write the message here
 * 
 * @see cap_parser_set_custom_help
 * @see cap_parser_set_description
 * @see cap_parser_set_epilogue
 * @see cap_parser_add_flag
 * @see cap_parser_add_positional
 */
void cap_parser_print_help(const ArgumentParser * parser, FILE* file) {
    if (!parser || !file) {
        return;
    }
    if (!parser -> mEnableHelp) {
        return;
    }
    if (parser -> mCustomHelp) {
        fprintf(file, "%s\n", parser -> mCustomHelp);
        return;
    }
    if (parser -> mDescription) {
        fprintf(file, "%s\n", parser -> mDescription);
    }
    if (parser -> mFlagCount || parser -> mHelpFlagInfo || parser -> mFlagSeparatorInfo) {
        fprintf(file, "\nAvailable flags:\n");
    }
    if (parser -> mHelpFlagInfo) {
        cap_print_flag_info(file, parser -> mHelpFlagInfo);
    }
    if (parser -> mFlagSeparatorInfo) {
        cap_print_flag_info(file, parser -> mFlagSeparatorInfo);
    }
    for (size_t i = 0; i < parser -> mFlagCount; ++i) {
        const FlagInfo * fi = parser -> mFlags[i];
        cap_print_flag_info(file, fi);
    }

    if (parser -> mPositionalCount) {
        fprintf(file, "\nPositional Arguments:\n");
    }
    for (size_t i = 0; i < parser -> mPositionalCount; ++i) {
        const PositionalInfo * pi = parser -> mPositionals[i];
	cap_print_positional_info(file, pi);
    }

    if (parser -> mEpilogue) {
        fprintf(file, "\n%s\n", parser -> mEpilogue);
    }
}

// ============================================================================
// === PARSER: PARSING ARGUMENTS ==============================================
// ============================================================================

/**
 * Parse comand line arguments without exiting when an error is encountered.
 * 
 * Parses given command line words using the given parser. When a parsing error 
 * occurs, parsing terminates and an appropriate result is returned. This 
 * function should not be called directly by the user, `cap_parser_parse`
 * should be used instead.
 * 
 * @param parser parser object to use
 * @param argc number of command line arguments
 * @param argv array of command line arguments
 * @return result of the parsing, containing a `ParsedArguments` if parsing was 
 *         successful.
 */
ParsingResult cap_parser_parse_noexit(
        ArgumentParser * parser, int argc, const char ** argv) {
    ParsedArguments * parsed_arguments = cap_pa_make_empty();
    ParsingResult result = (ParsingResult) {
        .mArguments = parsed_arguments,
        .mFirstErrorWord = NULL,
        .mSecondErrorWord = NULL,
        .mError = PER_NO_ERROR
    };

    _cap_parser_parse_flags_and_positionals(parser, argc, argv, &result);   
    if (result.mError != PER_NO_ERROR) {
	goto fail;
    }
    
    _cap_parser_check_flag_and_positional_counts(parser, &result);
    if (result.mError != PER_NO_ERROR) {
	goto fail;
    }

    return result;

fail:
    cap_pa_destroy(parsed_arguments);
    result.mArguments = NULL;
    return result;
}

/**
 * Parses command line arguments.
 * 
 * Parses command line words using a given parser. If a parsing error occurs, 
 * the program exits with an error message.
 * 
 * On successful parsing returns a pointer to a `ParsedArguments` object 
 * containing all parsed flags and positional arguments. The caller is the owner
 * of this object - it can be used even after the parser is destroyed using 
 * `cap_parser_destroy` and needs to be destroyed using `cap_pa_destroy` and a 
 * subsequent call to `free`.
 * 
 * @param parser parser object to use
 * @param argc number of command line words
 * @param argv array of command line words
 * @return pointer to a new `ParsedArguments` object containing information on 
 *         parsed flags and positional arguments.
 */
ParsedArguments * cap_parser_parse(
        ArgumentParser * parser, int argc, const char ** argv) {
    ParsingResult result = cap_parser_parse_noexit(parser, argc, argv);
    if (result.mError == PER_NO_ERROR) {
        return result.mArguments;
    }
    if (result.mError == PER_HELP) {
        cap_parser_print_usage(parser, stdout, *argv);
        putchar('\n');
        cap_parser_print_help(parser, stdout);
        exit(0);
    }
    fprintf(stderr, "%s: ", cap_parser_get_program_name(parser, *argv));
    switch (result.mError) {
        case PER_NOT_ENOUGH_POSITIONALS:
            fprintf(stderr, "not enough arguments");
            break;
        case PER_TOO_MANY_POSITIONALS:
            fprintf(stderr, "too many arguments");
            break;
        case PER_CANNOT_PARSE_POSITIONAL:
            fprintf(
                stderr, "cannot parse value '%s' for argument '%s'",
	       	result.mSecondErrorWord, result.mFirstErrorWord);
            break;
        case PER_UNKNOWN_FLAG:
            fprintf(stderr, "unknown flag '%s'", result.mFirstErrorWord);
            break;
        case PER_MISSING_FLAG_VALUE:
            fprintf(
                stderr, "missing value for flag '%s'", result.mFirstErrorWord);
            break;
        case PER_CANNOT_PARSE_FLAG:
            fprintf(
                stderr, "cannot parse value '%s' for flag '%s'",
	       	result.mSecondErrorWord, result.mFirstErrorWord);
            break;
        case PER_NOT_ENOUGH_FLAGS:
            fprintf(
                stderr, "not enough instances of flag '%s'", 
		result.mFirstErrorWord);
            break;
        case PER_TOO_MANY_FLAGS:
            fprintf(
                stderr, "too many instances of flag '%s'", 
		result.mFirstErrorWord);
            break;
        case PER_HELP:
        case PER_NO_ERROR:
        default:
            assert(false && "unreachable in parsing error checking");

    }
    fprintf(stderr, "\n\n");
    cap_parser_print_usage(parser, stderr, *argv);
    exit(-1);
}

// ============================================================================
// === PARSER: IMPLEMENTATION OF PRIVATE FUNCTIONS ============================
// ============================================================================

static bool _cap_parse_double(const char * word, double * value) {
    double v;
    int c;
    long long int n;
    c = sscanf(word, "%lf%lln", &v, &n);
    if (c != 1 || (unsigned long long int) n != strlen(word)) {
        return false;
    }
    *value = v;
    return true;
}

static bool _cap_parse_int(const char * word, int * value) {
    int v, c;
    long long int n;
    c = sscanf(word, "%i%lln", &v, &n);
    if (c != 1 || (unsigned long long int) n != strlen(word)) {
        return false;
    }
    *value = v;
    return true;
}

static bool _cap_parse_word_as_type(
        const char * word, DataType type, TypedUnion * uninitialized_tu) {
    switch (type) {
        case DT_DOUBLE: {
            double v;
            if (_cap_parse_double(word, &v)) {
                *uninitialized_tu = cap_tu_make_double(v);
                return true;
            }
            break;
        }
        case DT_INT: {
            int v;
            if (_cap_parse_int(word, &v)) {
                *uninitialized_tu = cap_tu_make_int(v);
                return true;
            }
            break;
        }
        case DT_STRING: {
            *uninitialized_tu = cap_tu_make_string(word);
            return true;
        }
        default:
            break;
    }
    return false;
}

static FlagInfo * _cap_parser_find_flag(
        const ArgumentParser * parser, const char * flag) {
    if (parser -> mHelpFlagInfo 
    	    && _cap_flag_matches_name_or_alias(parser -> mHelpFlagInfo, flag)) {
        return parser -> mHelpFlagInfo;
    }
    if (parser -> mFlagSeparatorInfo 
	        && _cap_flag_matches_name_or_alias(
                parser -> mFlagSeparatorInfo, flag)) {
        return parser -> mFlagSeparatorInfo;
    }
    for (size_t i = 0; i < parser -> mFlagCount; ++i) {
        FlagInfo * fi = parser -> mFlags[i];
        if (_cap_flag_matches_name_or_alias(fi, flag)) {
            return fi;
        }
    }
    return NULL;
}

static bool _cap_flag_matches_name_or_alias(
    const FlagInfo * flag_info, const char * name_or_alias) 
{
    if (!strcmp(name_or_alias, flag_info -> mName)) {
        return true;
    }
    for (size_t j = 0; j < flag_info -> mAliasCount; ++j) {
        if (!strcmp(name_or_alias, flag_info -> mAliases[j])) {
            return true;
        }
    }
    return false;
}

static const char * _cap_get_shortest_flag_name(const FlagInfo * flag_info) {
    const char * shortest = flag_info -> mName;
    size_t shortest_length = strlen(shortest);
    for (size_t i = 0; i < flag_info -> mAliasCount; ++i) {
        size_t len = strlen(flag_info -> mAliases[i]);
        if (len >= shortest_length) {
            continue;
        }
        shortest = flag_info -> mAliases[i];
        shortest_length = len;
    }
    return shortest;
}

static OnePositionalParsingResult _cap_parser_parse_one_positional(
        const ArgumentParser * parser, const char * arg, 
        size_t positional_index) {
    OnePositionalParsingResult res;
    res.mPositional = NULL;
    res.mWordsConsumed = 0;
    res.mError = OPPE_NO_ERROR;
     
    if (positional_index >= parser -> mPositionalCount) {
        res.mError = OPPE_TOO_MANY;
        return res;
    }
    const PositionalInfo * posit_info 
        = parser -> mPositionals[positional_index];
    res.mPositional = posit_info;
    if (!_cap_parse_word_as_type(arg, posit_info -> mType, &(res.mValue))) {
        res.mError = OPPE_CANNOT_PARSE;
        return res;
    }

    res.mWordsConsumed = 1;
    return res;
}

static OneFlagParsingResult _cap_parser_parse_one_flag(
        const ArgumentParser * parser, int argc, const char * const * argv,
        int index) {
    OneFlagParsingResult result;
    result.mWordsConsumed = 0;
    result.mError = OFPE_NO_ERROR;

    const char * arg = argv[index];

    // 1. is this a flag that exists?
    const FlagInfo * flag_info = _cap_parser_find_flag(parser, arg);
    result.mFlag = flag_info;
    if (!flag_info) {  // no such flag was found
        result.mError = OFPE_UNKNOWN_FLAG;
       	return result;
    }
    // skip arg because it is being consumed right now
    ++index;
    ++result.mWordsConsumed;
   
    // 3. check data type and try to parse it
    if (flag_info -> mType == DT_PRESENCE) {
	result.mValue = cap_tu_make_presence();
	return result;
    }
    // parse the next argument according to dtype
    if (index >= argc) {
        result.mError = OFPE_MISSING_FLAG_VALUE;
        return result;
    }
    const char * value_arg = argv[index];
    // This is a bit messy but it should work.
    // Generally, it is bad practice to use uninitialized objects.
    // What's even funnier is, I made factory functions for typed unions
    // but this code isn't directly using them lol.
    if (!_cap_parse_word_as_type(
            value_arg, flag_info -> mType, &(result.mValue))) {
        result.mError = OFPE_CANNOT_PARSE_FLAG;
        return result;
    }
    // very important! must skip extra the word that was consumed here
    ++index;
    ++result.mWordsConsumed;
    return result;
} 

static void _cap_parser_parse_flags_and_positionals(
        const ArgumentParser * parser, int argc, const char * const * argv,
        ParsingResult * result) {
    size_t positional_index = 0;
    int index = 1;
    bool positional_only = false;

    while (index < argc) {
        const char * arg = argv[index];

        if (positional_only || !strlen(arg) 
		        || !strchr(parser -> mFlagPrefixChars, arg[0])) {
            // positional
            OnePositionalParsingResult one_posit_res = 
                _cap_parser_parse_one_positional(parser, arg, positional_index);
            const PositionalInfo * posit_info = one_posit_res.mPositional;
            switch (one_posit_res.mError) {
                case OPPE_NO_ERROR:
                    break;
                case OPPE_TOO_MANY:
                    result -> mError = PER_TOO_MANY_POSITIONALS;
                    return;
                case OPPE_CANNOT_PARSE:
                    result -> mError = PER_CANNOT_PARSE_POSITIONAL;
                    result -> mFirstErrorWord = posit_info -> mName;
                    result -> mSecondErrorWord = arg;
                    return;
                default:
                    assert(
                        false && "unreachable in "
                        "_cap_parser_parse_flags_and_positionals");
            }
            cap_pa_append_positional(
                result -> mArguments, posit_info -> mName, 
                one_posit_res.mValue);
            if (!posit_info -> mVariadic) {
                // if the current argument is variadic, do not advance
                // positional_index. That way more words can be consumed by
                // this.
                ++positional_index;
            }
            index += one_posit_res.mWordsConsumed;
            continue;
        }
	
        // try to parse a flag
	OneFlagParsingResult one_flag_res = _cap_parser_parse_one_flag(
            parser, argc, argv, index);
	const FlagInfo * parsed_flag = one_flag_res.mFlag;
	switch (one_flag_res.mError) {
	    case OFPE_NO_ERROR:
	       	break;
	    case OFPE_UNKNOWN_FLAG:
            result -> mError = PER_UNKNOWN_FLAG;
            result -> mFirstErrorWord = arg;
            return;
	    case OFPE_MISSING_FLAG_VALUE:
            result -> mError = PER_MISSING_FLAG_VALUE;
            result -> mFirstErrorWord = arg;
            return;
	    case OFPE_CANNOT_PARSE_FLAG:
            result -> mError = PER_CANNOT_PARSE_FLAG;
            result -> mFirstErrorWord = arg;
            result -> mSecondErrorWord 
                = argv[index + one_flag_res.mWordsConsumed];
            return;
	    default:
            assert(false && "unreachable in cap_parser_parse_noexit");
	}
	index += one_flag_res.mWordsConsumed;
        if (parsed_flag == parser -> mFlagSeparatorInfo) {
            // switch to positional-only mode
            positional_only = true;
            continue;
        }
        if (parsed_flag == parser -> mHelpFlagInfo) {
            result -> mError = PER_HELP;
            return;
        }
	// normal flag -> add its value to parsed_arguments
	cap_pa_add_flag(
	    result -> mArguments, parsed_flag -> mName, one_flag_res.mValue); 
    }
}

static FlagCountCheckResult _cap_parser_check_flag_counts(
        const ArgumentParser * parser,
       	const ParsedArguments * parsed_arguments) {
    
    // check min and max count requirements for flags
    for (size_t i = 0; i < parser -> mFlagCount; ++i) {
        const FlagInfo * flag_info = parser -> mFlags[i];
        size_t real_count = cap_pa_flag_count(
            parsed_arguments, flag_info -> mName);
        if (real_count < (unsigned int) flag_info -> mMinCount) {
	    return (FlagCountCheckResult) {
		.mFlag = flag_info,
		.mCount = TOO_FEW
	    };
        }
        if (flag_info -> mMaxCount >= 0
	       	&& real_count > (unsigned int) flag_info -> mMaxCount) {
            return (FlagCountCheckResult) {
	       .mFlag = flag_info,
               .mCount = TOO_MANY
            };		   
        }
    }
    return (FlagCountCheckResult) {
	.mFlag = NULL,
	.mCount = GOOD
    };
}

static void _cap_parser_check_flag_and_positional_counts(
        const ArgumentParser * parser, ParsingResult * result) {
    // positional argument presence is checked here
    //
    // if p_count is at least the number of positionals configured in the
    // parser, it is all good (NB that a case of too-many-arguments is caught
    // when actually parsing)
    //
    // if p_count is less, find the PositionalInfo of the first argument that
    // was not parsed. If it is required, fail.
    const size_t p_count = cap_nva_length(result -> mArguments -> mPositionals);
    if (p_count < parser -> mPositionalCount) {
        const PositionalInfo * first_not_parsed = parser -> mPositionals[p_count];
        if (first_not_parsed -> mRequired) {
            result -> mError = PER_NOT_ENOUGH_POSITIONALS;
            return;
        }
    }
    // required flag counts are checked using another function because we also
    // want to know the name of the flag
    FlagCountCheckResult count_check = _cap_parser_check_flag_counts(
        parser, result -> mArguments);
    switch (count_check.mCount) {
	case GOOD:
	    break;
	case TOO_FEW:
            result -> mError = PER_NOT_ENOUGH_FLAGS;
	    result -> mFirstErrorWord = count_check.mFlag -> mName;
	    return; 
	case TOO_MANY:
	    result -> mError = PER_TOO_MANY_FLAGS;
	    result -> mFirstErrorWord = count_check.mFlag -> mName;
	    return;
	default:
	    assert(false && "unreachable in cap_parser_parse_noexit");
    }
}

/**
 * @}
 */

#endif
