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
        .mFlags = NULL,
        .mFlagCount = 0u,
        .mFlagAlloc = 0u,
        .mPositionals = NULL,
        .mPositionalCount = 0u,
        .mPositionalAlloc = 0u,
        .mFlagPrefixChars = NULL,
        .mFlagSeparator = NULL,
        .mUseFlagSeparator = true
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
    for (size_t i = 0; i < parser -> mFlagCount; ++i) {
        FlagInfo * fi = parser -> mFlags + i;
        free(fi -> mName);
        fi -> mName = NULL;
    }
    for (size_t i = 0; i < parser -> mPositionalCount; ++i) {
        PositionalInfo * pi = parser -> mPositionals + i;
        free(pi -> mName);
        pi -> mName = NULL;
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
    free(parser -> mFlagSeparator);
    parser -> mFlagSeparator = NULL;
    parser -> mUseFlagSeparator = true;
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
        if (!parser -> mUseFlagSeparator) return;
        free(parser -> mFlagSeparator);
        parser -> mFlagSeparator = NULL;
        parser -> mUseFlagSeparator = false;
        return;
    }
    if (strlen(separator) == 0) {
        fprintf(stderr, "cap: missing flag separator\n");
        exit(-1);
    }
    if (parser -> mUseFlagSeparator) {
        free(parser -> mFlagSeparator);
    }
    parser -> mUseFlagSeparator = true;
    parser -> mFlagSeparator = copy_string(separator);
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
 * `cap_parser_set_flag_prefix`.
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
 * @param parser object to create the flag in
 * @param flag null-terminated name of the new flag, including flag prefix 
 *        charactes (e.g. '-')
 * @param type data type of the flag's values
 * @param min_count minimum number of times the flag must be given on the 
 *        command line. Must be at least zero.
 * @param max_count maximum number of times the flag may be given on the 
 *        command line. Must be either negative or at least `min_count`.
 */
void cap_parser_add_flag(
        ArgumentParser * parser, const char * flag, DataType type, 
        int min_count, int max_count) {
    const char * flag_prefix = parser -> mFlagPrefixChars;
    if (!flag_prefix) flag_prefix = "-";
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
            stderr, "cap: invalid flag name (must begin with one of \"%s\")\n",
            flag_prefix);
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
 * @param parser object to configure
 * @param name name of the new argument
 * @param type data type of the new argument
 */
void cap_parser_add_positional(
        ArgumentParser * parser, const char * name, DataType type) {
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
        .mType = type
    };
    parser -> mPositionals[parser -> mPositionalCount++] = new_positional;
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
        .mErrorMessageFormat = NULL,
        .mFirstErrorWord = NULL,
        .mSecondErrorWord = NULL,
        .mSuccess = false
    };
    
    // set up flag prefix characters and flag separator
    const char * prefix_chars = parser -> mFlagPrefixChars;
    if (!prefix_chars) {
        prefix_chars = "-";
    }
    char * flag_separator = parser -> mFlagSeparator;
    bool flag_separator_is_generated = false;
    if (!flag_separator && parser -> mUseFlagSeparator) {
        // generate one!
        flag_separator_is_generated = true;
        flag_separator = (char *) malloc(3 * sizeof(char));
        flag_separator[0] = flag_separator[1] = *prefix_chars;
        flag_separator[2] = 0;
    }

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
        if (!positional_only && parser -> mUseFlagSeparator \
                && strcmp(arg, flag_separator) == 0) {
            positional_only = true;
            continue;
        }

        // TODO: what to do if an arg is empty string?!
        if (positional_only || !strchr(prefix_chars, arg[0])) {
            // positional
            if (positional_index >= parser -> mPositionalCount) {
                result.mErrorMessageFormat = "too many arguments";
                goto fail;
            }
            const PositionalInfo * posit_info = parser -> mPositionals + positional_index;
            TypedUnion tu;
            if (!_cap_parse_word_as_type(arg, posit_info -> mType, &tu)) {
                result.mErrorMessageFormat = "cannot parse value '%s' for argument '%s'";
                result.mFirstErrorWord = arg;
                result.mSecondErrorWord = posit_info -> mName;
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
            result.mErrorMessageFormat = "unknown flag '%s'";
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
            result.mErrorMessageFormat = "expected a value for flag '%s'";
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
            result.mErrorMessageFormat = "cannot parse value '%s' for flag '%s'";
            result.mFirstErrorWord = value_arg;
            result.mSecondErrorWord = arg;
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
            result.mErrorMessageFormat = "too few occurences of flag %s";
            result.mFirstErrorWord = flag_info -> mName;
            goto fail;
        }
        if (flag_info -> mMaxCount >= 0 && real_count > (unsigned int) flag_info -> mMaxCount) {
            result.mErrorMessageFormat = "too many occurences of flag %s";
            result.mFirstErrorWord = flag_info -> mName;
            goto fail;
        }
    }
    // positional argument presence is checked here
    // that is easy (for now), because all positionals are required
    if (positional_index < parser -> mPositionalCount) {
        result.mErrorMessageFormat = "not enought arguments";
        goto fail;
    }

    // release memory used for generad flag separator
    if (flag_separator_is_generated) {
        free(flag_separator);
    }
    result.mArguments = parsed_arguments;
    result.mSuccess = true;
    return result;

fail:
    // release memory used for generad flag separator
    if (flag_separator_is_generated) {
        free(flag_separator);
    }
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
    if (!result.mSuccess) {
        fprintf(stderr, "%s: ", *argv);
        if (!result.mFirstErrorWord) {
            fprintf(stderr, result.mErrorMessageFormat);
        }
        else if (!result.mSecondErrorWord) {
            fprintf(stderr, result.mErrorMessageFormat, result.mFirstErrorWord);
        }
        else {
            fprintf(
                stderr, result.mErrorMessageFormat, result.mFirstErrorWord,
                result.mSecondErrorWord);
        }
        fprintf(stderr, "\n");
        exit(-1);
    }
    return result.mArguments;
}

// ============================================================================
// === PARSER: IMPLEMENTATION OF PRIVATE FUNCTIONS ============================
// ============================================================================

bool _cap_parse_double(const char * word, double * value) {
    double v;
    int c;
    long long int n;
    c = sscanf(word, "%lf%zn", &v, &n);
    if (c != 1 || (unsigned long long int) n != strlen(word)) {
        return false;
    }
    *value = v;
    return true;
}

bool _cap_parse_int(const char * word, int * value) {
    int v, c;
    long long int n;
    c = sscanf(word, "%i%zn", &v, &n);
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

#endif
