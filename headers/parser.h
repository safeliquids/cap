#ifndef __PARSER_H__
#define __PARSER_H__

#include "helper_functions.h"
#include "types.h"
#include "typed_union.h"
#include "parsed_arguments.h"

#include <assert.h>
#include <stddef.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// ============================================================================
// === PARSER: DECLARATION OF PRIVATE FUNCTIONS ===============================
// ============================================================================

bool _cap_parse_double(const char * word, double * value);
bool _cap_parse_int(const char * word, int * value);
bool _cap_parse_word_as_type(
    const char * word, DataType type, TypedUnion * uninitialized_tu);
void _cap_set_string_property(char ** property, const char * value);
void _cap_delete_string_property(char ** property);
const char * _cap_get_flag_metavar(const FlagInfo * fi);
const char * _cap_get_posit_metavar(const PositionalInfo * pi);
const char * _cap_type_metavar(DataType type);

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
 */
ArgumentParser * cap_parser_make_empty() {
    ArgumentParser * p = (ArgumentParser *) malloc(sizeof(ArgumentParser));
    *p = (ArgumentParser) {
        .mProgramName = NULL,
        .mDescription = NULL,
        .mEpilogue = NULL,
        .mCustomHelp = NULL,
        .mFlags = NULL,
        .mFlagCount = 0u,
        .mFlagAlloc = 0u,
        .mPositionals = NULL,
        .mPositionalCount = 0u,
        .mPositionalAlloc = 0u,
        .mFlagPrefixChars = copy_string("-"),
        .mFlagSeparator = copy_string("--")
    };
    return p;
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
    _cap_delete_string_property(&(parser -> mDescription));
    _cap_delete_string_property(&(parser -> mEpilogue));
    _cap_delete_string_property(&(parser -> mCustomHelp));
    for (size_t i = 0; i < parser -> mFlagCount; ++i) {
        FlagInfo * fi = parser -> mFlags + i;
        free(fi -> mName);
        fi -> mName = NULL;
        if (fi -> mMetaVar) {
            free(fi -> mMetaVar);
            fi -> mMetaVar = NULL;
        }
        _cap_delete_string_property(&(fi -> mDescription));
    }
    for (size_t i = 0; i < parser -> mPositionalCount; ++i) {
        PositionalInfo * pi = parser -> mPositionals + i;
        free(pi -> mName);
        pi -> mName = NULL;
        if (pi -> mMetaVar) {
            free(pi -> mMetaVar);
            pi -> mMetaVar = NULL;
        }
        _cap_delete_string_property(&(pi -> mDescription));
    }
    free(parser -> mFlags);
    free(parser -> mPositionals);
    parser -> mFlags = NULL;
    parser -> mPositionals = NULL;
    parser -> mFlagCount = parser -> mFlagAlloc = 0u;
    parser -> mPositionalCount = parser -> mPositionalAlloc = 0;

    free(parser -> mFlagPrefixChars);
    free(parser -> mFlagSeparator);

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
 * This also automatically configures a flag called a flag separator. When the
 * flag separator is found on the command line, the parser enters positional 
 * only mode. In this mode all command line words are used as positionals even 
 * if they begin with a flag prefix character. The automatic value created for 
 * the flag separator is the first character given to this function repeated 
 * twice (e.g. by default '--'). The flag separator can be afterwards 
 * explicitly configured or disabled using the `cap_parser_set_flag_separator` 
 * function.
 * 
 * If `parser` is `NULL`, nothing happens. If `prefix_chars` is `NULL` or
 * empty, the program exits with an error.
 * 
 * Note that, this configuration must be done before adding any flags, because 
 * added flags are required to begin with a flag prefix character. If any flags 
 * are present when calling this function, the program exits with an error.
 * 
 * @param parser parser object to configure
 * @param prefix_chars null-terminated string of characters that should be considered flag prefixes.
 */
void cap_parser_set_flag_prefix(
        ArgumentParser * parser, const char * prefix_chars) {
    if (!parser) return;
    if (!prefix_chars || strlen(prefix_chars) == 0) {
        fprintf(stderr, "cap: missing flag prefix characters\n");
        exit(-1);
    }
    if (parser -> mFlagCount) {
        fprintf(
            stderr, "cap: cannot set flag prefix characters when flags "
            "already exist\n");
        exit(-1);
    }
    free(parser -> mFlagPrefixChars);
    parser -> mFlagPrefixChars = copy_string(prefix_chars);
    if (parser -> mFlagSeparator) {
        free(parser -> mFlagSeparator);
    }
    char * s = (char *) malloc(3 * sizeof(char));
    s[0] = s[1] = *prefix_chars;
    s[2] = 0;
    parser -> mFlagSeparator = s;
    return;
}

/**
 * Sets a flag separator.
 * 
 * Configures a parser to recognize a flag separator, or to not have one. A 
 * flag separator is a special flag that, when found on the command line, puts 
 * the parser in positional only mode. In this mode all command line words are 
 * used as positionals even if they begin with a flag prefix character.
 * 
 * By default the flag separator is '--' (double dash). It is further 
 * automatically set using a call to `cap_parser_set_flag_prefix` as the first 
 * flag prefix character doubled. `cap_parser_set_flag_separator` can be used 
 * to explicitly choose a different flag separator. Note that, calling 
 * `cap_parser_set_flag_prefix` after `cap_parser_set_flag_separator` replaces 
 * the previously configured value.
 * 
 * @param parser parser object to configure
 * @param separator null-terminated string containing the new flag separator.
 *        If `NULL` is given, the flag separator is explicitly disabled.
 *        If an empty string is given, the program exits with an error.
 *        This string is copied into the parser and the caller remains the owner of the given pointer.
 */
void cap_parser_set_flag_separator(
        ArgumentParser * parser, const char * separator) {
    if (!parser) return;
    if (!separator) {
        if (parser -> mFlagSeparator) {
            free(parser -> mFlagSeparator);
        }
        return;
    }
    if (strlen(separator) == 0) {
        fprintf(stderr, "cap: missing flag separator\n");
        exit(-1);
    }
    if (parser -> mFlagSeparator) {
        free(parser -> mFlagSeparator);
    }
    parser -> mFlagSeparator = copy_string(separator);
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
    if (parser -> mProgramName) {
        free(parser -> mProgramName);
        parser -> mProgramName = NULL;
    }
    if (!name) {
        return;
    }
    parser -> mProgramName = copy_string(name);
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
void cap_parser_set_description(ArgumentParser * parser, const char * description) {
    if (!parser) {
        return;
    }
    _cap_set_string_property(&(parser -> mDescription), description);
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
    _cap_set_string_property(&(parser -> mEpilogue), epilogue);
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
    _cap_set_string_property(&(parser -> mCustomHelp), help);
}

// ============================================================================
// === PARSER: ADDING FLAGS ===================================================
// ============================================================================

/**
 * Registers a flag in an argument parser.
 * 
 * Registers a new flag in `parser` under the name `flag`. The flag's name must 
 * be unique and the program exits with an error if a duplicate flag name is 
 * given. The flag name must begin with a flag prefix character, either the 
 * default '-' or one of the characters previously configured using 
 * `cap_parser_set_flag_prefix`. Flag names may not be identical to the flag 
 * separator, either the default '--', or one configured using 
 * `cap_parser_set_flag_separator`.
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
        const char * description) {
    const char * const flag_prefix = parser -> mFlagPrefixChars;
    const char * const flag_separator = parser -> mFlagSeparator;
    if (!parser) {
        fprintf(stderr, "cap: missing parser\n");
        exit(-1);
    }
    if (!flag || !strlen(flag)) {
        fprintf(stderr, "cap: missing flag name");
        exit(-1);
    }
    if (!strchr(flag_prefix, *flag)) {
        fprintf(
            stderr, "cap: invalid flag name - must begin with one of \"%s\"\n",
            flag_prefix);
        exit(-1);
    }
    if (flag_separator && strcmp(flag_separator, flag) == 0) {
        fprintf(
            stderr, "cap: invalid flag name - flag names may not be identical "
            "to the flag separato \"%s\"\n", flag_separator);
        exit(-1);
    }
    if (min_count < 0) {
        fprintf(stderr, "cap: min_count requirement must not be negative\n");
        exit(-1);
    }
    if (max_count >= 0 && max_count < min_count) {
        fprintf(
            stderr, "cap: max_count requirement must not be less than"
            " min_count\n");
        exit(-1);
    }
    if (min_count == 0 && max_count == 0) {
        fprintf(stderr, "cap: min_count and max_count cannot be both zero\n");
        exit(-1);
    }
    for (size_t i = 0; i < parser -> mFlagCount; ++i) {
        FlagInfo * fi = parser -> mFlags + i;
        if (strcmp(fi -> mName, flag) == 0) {
            fprintf(stderr, "cap: duplicate flag definition %s\n", flag);
            exit(-1);
        }
    }
    if (parser -> mFlagCount >= parser -> mFlagAlloc) {
        size_t alloc_size = parser -> mFlagAlloc;
        alloc_size = alloc_size ? alloc_size * 2 : 1;
        parser -> mFlagAlloc = alloc_size;
        parser -> mFlags = (FlagInfo *) realloc(
            parser -> mFlags, alloc_size * sizeof(FlagInfo));
    }
    FlagInfo new_flag = (FlagInfo) {
        .mName = copy_string(flag),
        .mMetaVar = metavar ? copy_string(metavar) : NULL,
        .mDescription = description ? copy_string(description) : NULL,
        .mType = type,
        .mMinCount = min_count,
        .mMaxCount = max_count
    };
    parser -> mFlags[parser -> mFlagCount++] = new_flag;
}

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
        const char * metavar, const char * description) {
    if (!parser) {
        fprintf(stderr, "cap: missing parser\n");
        exit(-1);
    }
    if (!name || strlen(name) == 0) {
        fprintf(stderr, "cap: invalid argument name\n");
        exit(-1);
    }
    if (type == DT_PRESENCE) {
        fprintf(
            stderr, "cap: data type DT_PRESENCE is invalid for positional"
            " arguments\n");
        exit(-1);
    }
    for (size_t i = 0; i < parser -> mPositionalCount; ++i) {
        if (!strcmp(parser -> mPositionals[i].mName, name)) {
            fprintf(stderr, "cap: duplicate positional argument %s\n", name);
            exit(-1);
        }
    }
    if (parser -> mPositionalCount >= parser -> mPositionalAlloc) {
        size_t alloc_size = parser -> mPositionalAlloc;
        alloc_size = alloc_size ? alloc_size * 2 : 1;
        parser -> mPositionalAlloc = alloc_size;
        parser -> mPositionals = (PositionalInfo *) realloc(
            parser -> mPositionals, alloc_size * sizeof(PositionalInfo));
    }
    PositionalInfo new_positional = (PositionalInfo) {
        .mName = copy_string(name),
        .mMetaVar = metavar ? copy_string(metavar) : NULL,
        .mDescription = description ? copy_string(description) : NULL,
        .mType = type
    };
    parser -> mPositionals[parser -> mPositionalCount++] = new_positional;
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
const char * cap_parser_get_program_name(const ArgumentParser * parser, const char * argv0) {
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
 * in `parser`.
 */
void cap_parser_print_usage(
        const ArgumentParser * parser, FILE * file,
        const char * argv0) {
    if (!parser || !file) {
        return;
    }

    fprintf(file, "usage:\n");
    fprintf(file, "\t");
    fprintf(file, "%s", cap_parser_get_program_name(parser, argv0));

    for (size_t i = 0; i < parser -> mFlagCount; ++i) {
        const FlagInfo * fi = parser -> mFlags + i;
        fputc(' ', file);
        if (fi -> mMinCount == 0) {
            fputc('[', file);
        }
        fprintf(file, "%s", fi -> mName);
        do {
            if (fi -> mType == DT_PRESENCE) break;
            if (fi -> mMetaVar) {
                fprintf(file, " %s", fi -> mMetaVar);
                break;
            }
            const char * type_metavar;
            switch (fi -> mType) {
                case DT_DOUBLE:
                    type_metavar = "DOUBLE";
                    break;
                case DT_INT:
                    type_metavar = "INT";
                    break;
                case DT_STRING:
                    type_metavar = "STRING";
                    break;
                case DT_PRESENCE:
                default:
                    type_metavar = NULL;
            }
            if (type_metavar) {
                fprintf(file, " %s", type_metavar);
            }
        } while (false);
        if (fi -> mMinCount == 0) {
            fputc(']', file);
        }
    }

    if (parser -> mPositionalCount > 0u && parser -> mFlagSeparator) {
        fprintf(file, " [%s]", parser -> mFlagSeparator);
    }
    for (size_t i = 0; i < parser -> mPositionalCount; ++i) {
        const PositionalInfo * pi = parser -> mPositionals + i;
        if (pi -> mMetaVar) {
            fprintf(file, " %s", pi -> mMetaVar);
            continue;
        }
        fprintf(file, " <%s>", pi -> mName);
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
    if (parser -> mCustomHelp) {
        fprintf(file, "%s\n", parser -> mCustomHelp);
        return;
    }
    if (parser -> mDescription) {
        fprintf(file, "%s\n", parser -> mDescription);
    }
    if (parser -> mFlagCount) {
        fprintf(file, "\nAvailable flags:\n");
    }
    for (size_t i = 0; i < parser -> mFlagCount; ++i) {
        const FlagInfo * fi = parser -> mFlags + i;
        fprintf(file, "\t%s", fi -> mName);
        if (fi -> mType != DT_PRESENCE) {
            fprintf(file, " %s", _cap_get_flag_metavar(fi));
        }
        if (fi -> mDescription) {
            fprintf(file, "\t%s", fi -> mDescription);
        }
        fputc('\n', file);
    }

    if (parser -> mPositionalCount) {
        fprintf(file, "\nPositional Arguments:\n");
    }
    for (size_t i = 0; i < parser -> mPositionalCount; ++i) {
        const PositionalInfo * pi = parser -> mPositionals + i;
        fprintf(file, "\t%s", _cap_get_posit_metavar(pi));
        if (pi -> mDescription) {
            fprintf(file, "\t%s", pi -> mDescription);
        }
        fputc('\n', file);
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
        .mArguments = NULL,
        .mFirstErrorWord = NULL,
        .mSecondErrorWord = NULL,
        .mError = PER_NO_ERROR
    };
    
    // set up flag prefix characters and flag separator
    // prefix chars should always exist even if not explicitly configured.
    // flag separator always exists if it is not disabled.
    const char * const prefix_chars = parser -> mFlagPrefixChars;
    const char * const flag_separator = parser -> mFlagSeparator;

    int index = 0;
    bool positional_only = false;
    size_t positional_index = 0;
    while (true) {
        ++index;
        if (index >= argc) {
            break;
        }
        const char * arg = argv[index];
        
        // switch to positional-only mode if flag separator is found
        if (!positional_only && flag_separator \
                && strcmp(arg, flag_separator) == 0) {
            positional_only = true;
            continue;
        }

        // TODO: what to do if an arg is empty string?!
        if (positional_only || !strchr(prefix_chars, arg[0])) {
            // positional
            if (positional_index >= parser -> mPositionalCount) {
                result.mError = PER_TOO_MANY_POSITIONALS;
                goto fail;
            }
            const PositionalInfo * posit_info = parser -> mPositionals + positional_index;
            TypedUnion tu;
            if (!_cap_parse_word_as_type(arg, posit_info -> mType, &tu)) {
                result.mError = PER_CANNOT_PARSE_POSITIONAL;
                result.mFirstErrorWord = posit_info -> mName;
                result.mSecondErrorWord = arg;
                goto fail;
            }
            cap_pa_set_positional(parsed_arguments, posit_info -> mName, tu);
            ++positional_index;
            continue;
        }
        // parsing a flag

        // 1. is this a flag that exists?
        size_t i = 0;
        FlagInfo * flag_info = NULL;
        for ( ; i < parser -> mFlagCount; ++i) {
            FlagInfo * x = parser -> mFlags + i;
            if (strcmp(x -> mName, arg) == 0) {
                flag_info = x;
                break;
            }
        }
        if (!flag_info) {  // no such flag was found
            result.mError = PER_UNKNOWN_FLAG;
            result.mFirstErrorWord = arg;
            goto fail;
        }

        // 2. check data type and try to parse it
        DataType dtype = flag_info -> mType;
        if (dtype == DT_PRESENCE) {
            cap_pa_add_flag(parsed_arguments, arg, cap_tu_make_presence());
            continue;
        }
        // parse the next argument according to dtype
        if (index + 1 >= argc) {
            result.mError = PER_MISSING_FLAG_VALUE;
            result.mFirstErrorWord = arg;
            goto fail;
        }
        const char * value_arg = argv[index + 1];
        // This is a bit messy but it should work.
        // Generally, it is bad practice to use uninitialized objects.
        // What's even funnier is, I made factory functions for typed unions
        // but this code isn't directly using them lol.
        TypedUnion tu;
        if (!_cap_parse_word_as_type(value_arg, flag_info -> mType, &tu)) {
            result.mError = PER_CANNOT_PARSE_FLAG;
            result.mFirstErrorWord = arg;
            result.mSecondErrorWord = value_arg;
            goto fail;
        }
        // very important! must skip the word that was consumed here
        index++;
        // 3. add this new value for the flag
        cap_pa_add_flag(parsed_arguments, arg, tu);
    }

    // check min and max count requirements for flags
    for (size_t i = 0; i < parser -> mFlagCount; ++i) {
        const FlagInfo * flag_info = parser -> mFlags + i;
        size_t real_count = cap_pa_flag_count(parsed_arguments, flag_info -> mName);
        if (real_count < (unsigned int) flag_info -> mMinCount) {
            result.mError = PER_NOT_ENOUGH_FLAGS;
            result.mFirstErrorWord = flag_info -> mName;
            goto fail;
        }
        if (flag_info -> mMaxCount >= 0 && real_count > (unsigned int) flag_info -> mMaxCount) {
            result.mError = PER_TOO_MANY_FLAGS;
            result.mFirstErrorWord = flag_info -> mName;
            goto fail;
        }
    }
    // positional argument presence is checked here
    // that is easy (for now), because all positionals are required
    if (positional_index < parser -> mPositionalCount) {
        result.mError = PER_NOT_ENOUGH_POSITIONALS;
        goto fail;
    }

    result.mArguments = parsed_arguments;
    return result;

fail:
    cap_pa_destroy(parsed_arguments);
    return result;
}

/**
 * Parses command line arguments.
 * 
 * Parses command line words using a given parser. If a parsing error occurs, the program exits with an error message.
 * 
 * On successful parsing returns a pointer to a `ParsedArguments` object containing all parsed flags and positional arguments. The caller is the owner of this object - it can be used even after the parser is destroyed using `cap_parser_destroy` and needs to be destroyed using `cap_pa_destroy` and a subsequent call to `free`.
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
    fprintf(stderr, "%s: ", cap_parser_get_program_name(parser, *argv));
    switch (result.mError) {
        case PER_NOT_ENOUGH_POSITIONALS:
            fprintf(stderr, "not enough arguments");
            break;
        case PER_TOO_MANY_POSITIONALS:
            fprintf(stderr, "too many arguments");
            break;
        case PER_CANNOT_PARSE_POSITIONAL:
            fprintf(stderr, "cannot parse value '%s' for argument '%s'", result.mSecondErrorWord, result.mFirstErrorWord);
            break;
        case PER_UNKNOWN_FLAG:
            fprintf(stderr, "unknown flag '%s'", result.mFirstErrorWord);
            break;
        case PER_MISSING_FLAG_VALUE:
            fprintf(stderr, "missing value for flag '%s'", result.mFirstErrorWord);
            break;
        case PER_CANNOT_PARSE_FLAG:
            fprintf(stderr, "cannot parse value '%s' for flag '%s'", result.mSecondErrorWord, result.mFirstErrorWord);
            break;
        case PER_NOT_ENOUGH_FLAGS:
            fprintf(stderr, "not enough instances of flag '%s'", result.mFirstErrorWord);
            break;
        case PER_TOO_MANY_FLAGS:
            fprintf(stderr, "too many instances of flag '%s'", result.mFirstErrorWord);
            break;
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

bool _cap_parse_double(const char * word, double * value) {
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

bool _cap_parse_int(const char * word, int * value) {
    int v, c;
    long long int n;
    c = sscanf(word, "%i%lln", &v, &n);
    if (c != 1 || (unsigned long long int) n != strlen(word)) {
        return false;
    }
    *value = v;
    return true;
}

bool _cap_parse_word_as_type(
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

void _cap_set_string_property(char ** property, const char * value) {
    if (*property) {
        free(*property);
        *property = NULL;
    }
    if (!value) {
        return;
    }
    *property = copy_string(value);
}

void _cap_delete_string_property(char ** property) {
    if (!*property) {
        return;
    }
    free(*property);
    *property = NULL;
}

const char * _cap_get_flag_metavar(const FlagInfo * fi) {
    if (!fi || fi -> mType == DT_PRESENCE) {
        return NULL;
    }
    if (fi -> mMetaVar) {
        return fi -> mMetaVar;
    }
    return _cap_type_metavar(fi -> mType);
}

const char * _cap_get_posit_metavar(const PositionalInfo * pi) {
    if (!pi) {
        return NULL;
    }
    if (pi -> mMetaVar) {
        return pi -> mMetaVar;
    }
    return _cap_type_metavar(pi -> mType);
}

const char * _cap_type_metavar(DataType type) {
    const char * type_metavar;
    switch (type) {
        case DT_DOUBLE:
            type_metavar = "DOUBLE";
            break;
        case DT_INT:
            type_metavar = "INT";
            break;
        case DT_STRING:
            type_metavar = "STRING";
            break;
        case DT_PRESENCE:
        default:
            type_metavar = NULL;
    }
    return type_metavar;
}

#endif
