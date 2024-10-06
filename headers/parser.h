#ifndef __PARSER_H__
#define __PARSER_H__

#include "flag_info.h"
#include "parsed_arguments.h"
#include "positional_info.h"
#include "typed_union.h"

#include <stddef.h>
#include <stdbool.h>
#include <stdio.h>

// ============================================================================
// === PARSER: DEFINITION OF TYPES ============================================
// ============================================================================

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
ArgumentParser * cap_parser_make_empty();

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
ArgumentParser * cap_parser_make_default();

/**
 * Destroys an `ArgumentParser` object.
 * 
 * Destroys an `ArgumentParser` object and all data stored in it. 
 * `ParsedArgument` objects created using this parser are not owned by it 
 * and can be used after a parser is destroyed.
 * 
 * @param parser object to destroy. If it is `NULL`, nothing happens.
 */
void cap_parser_destroy(ArgumentParser * parser);

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
    ArgumentParser * parser, const char * prefix_chars);

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
    ArgumentParser * parser, const char * separator, const char * description);

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
void cap_parser_set_program_name(ArgumentParser * parser, const char * name);

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
    ArgumentParser * parser, const char * description);

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
void cap_parser_set_epilogue(ArgumentParser * parser, const char * epilogue);

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
void cap_parser_set_custom_help(ArgumentParser * parser, const char * help);

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
void cap_parser_set_custom_usage(ArgumentParser * parser, const char * usage);

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
void cap_parser_enable_help(ArgumentParser * parser, bool enable);

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
void cap_parser_enable_usage(ArgumentParser * parser, bool enable);

// ============================================================================
// === PARSER: ADDING FLAGS ===================================================
// ============================================================================

/**
 * Registers a flag in an argument parser.
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
 * @param metavar display name of the flag's value in help messages
 * @param description short description of the flag's meaning to display in 
 *        automatically generated help messages.
 */
void cap_parser_add_flag(
    ArgumentParser * parser, const char * flag, DataType type, 
    int min_count, int max_count, const char * metavar,
    const char * description);

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
    ArgumentParser * parser, const char * name, const char * description);

// ============================================================================
// === PARSER: ADDING POSITIONALS =============================================
// ============================================================================

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
 * All positional arguments are required. If any positional arguments are 
 * missing after parsing all command line words, a parse-time error is raised.
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
 * @param metavar display name for this argument in help messages
 * @param description short description of the argument's meaning to display 
 *        in automatically generated help messages
 */
void cap_parser_add_positional(
    ArgumentParser * parser, const char * name, DataType type, 
    const char * metavar, const char * description);

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
    const ArgumentParser * parser, const char * argv0);

/**
 * Prints a usage string.
 * 
 * Prints a usage string to `file` based on the flags and arguments configured
 * in `parser`.
 */
void cap_parser_print_usage(
    const ArgumentParser * parser, FILE * file, const char * argv0);

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
void cap_parser_print_help(const ArgumentParser * parser, FILE* file);

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
    ArgumentParser * parser, int argc, const char ** argv);

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
    ArgumentParser * parser, int argc, const char ** argv);

#endif
