#include "flag_info.h"
#include "helper_functions.h"
#include "parsed_arguments.h"
#include "parser.h"
#include "positional_info.h"
#include "typed_union.h"

#include <assert.h>
#include <stddef.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// ============================================================================
// === PARSER: DEFINITION OF PRIVATE TYPES ====================================
// ============================================================================

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
// === PARSER: CREATION AND DESTRUCTION =======================================
// ============================================================================

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

ArgumentParser * cap_parser_make_default() {
    ArgumentParser * parser = cap_parser_make_empty();
    cap_parser_set_help_flag(parser, "-h", NULL);
    cap_parser_set_flag_separator(parser, "--", NULL);
    cap_parser_enable_help(parser, true);
    cap_parser_enable_usage(parser, true);
    return parser;
}

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

void cap_parser_set_program_name(ArgumentParser * parser, const char * name) {
    if (!parser) {
        return;
    }
    set_string_property(&(parser -> mProgramName), name);
}

void cap_parser_set_description(
        ArgumentParser * parser, const char * description) {
    if (!parser) {
        return;
    }
    set_string_property(&(parser -> mDescription), description);
}

void cap_parser_set_epilogue(ArgumentParser * parser, const char * epilogue) {
    if (!parser) {
        return;
    }
    set_string_property(&(parser -> mEpilogue), epilogue);
}

void cap_parser_set_custom_help(ArgumentParser * parser, const char * help) {
    if (!parser) {
        return;
    }
    set_string_property(&(parser -> mCustomHelp), help);
}

void cap_parser_set_custom_usage(ArgumentParser * parser, const char * usage) {
    if (!parser) {
        return;
    }
    set_string_property(&(parser -> mCustomUsage), usage);
}

void cap_parser_enable_help(ArgumentParser * parser, bool enable) {
    if (!parser) return;
    parser -> mEnableHelp = enable;
}

void cap_parser_enable_usage(ArgumentParser * parser, bool enable) {
    if (!parser) {
        return;
    }
    parser -> mEnableUsage = enable;
}

// ============================================================================
// === PARSER: ADDING FLAGS ===================================================
// ============================================================================

void cap_parser_add_flag(
        ArgumentParser * parser, const char * flag, DataType type, 
        int min_count, int max_count, const char * metavar,
        const char * description) {
    const char * const flag_prefix = parser -> mFlagPrefixChars;
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
    // this also checks agains the help flag and flag separator if they exist
    if (_cap_parser_find_flag(parser, flag)) {
        fprintf(stderr, "cap: duplicate flag definition %s\n", flag);
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
}

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
        const PositionalInfo * pi = parser -> mPositionals[i];
        if (!strcmp(pi -> mName, name)) {
            fprintf(stderr, "cap: duplicate positional argument %s\n", name);
            exit(-1);
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
	name, metavar, description, type); 
    parser -> mPositionals[parser -> mPositionalCount++] = new_positional;
}

// ============================================================================
// === PARSER: HELP ===========================================================
// ============================================================================

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
        fprintf(file, " [%s]", parser -> mHelpFlagInfo -> mName);
    }

    for (size_t i = 0; i < parser -> mFlagCount; ++i) {
        const FlagInfo * fi = parser -> mFlags[i];
        fputc(' ', file);
        if (fi -> mMinCount == 0) {
            fputc('[', file);
        }
        fprintf(file, "%s", fi -> mName);
        if (fi -> mType != DT_PRESENCE) {
            fprintf(file, " %s", cap_get_flag_metavar(fi));
        }
        if (fi -> mMinCount == 0) {
            fputc(']', file);
        }
    }

    if (parser -> mPositionalCount > 0u && parser -> mFlagSeparatorInfo) {
        fprintf(file, " [%s]", parser -> mFlagSeparatorInfo -> mName);
    }
    for (size_t i = 0; i < parser -> mPositionalCount; ++i) {
        const PositionalInfo * pi = parser -> mPositionals[i];
        if (pi -> mMetaVar) {
            fprintf(file, " %s", pi -> mMetaVar);
            continue;
        }
        fprintf(file, " <%s>", pi -> mName);
    }
    fputc('\n', file);
}

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
    if (parser -> mFlagCount) {
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
    if (
            parser -> mHelpFlagInfo 
	    && !strcmp(flag, parser -> mHelpFlagInfo -> mName)) {
        return parser -> mHelpFlagInfo;
    }
    if (
            parser -> mFlagSeparatorInfo 
	    && !strcmp(flag, parser -> mFlagSeparatorInfo -> mName)) {
        return parser -> mFlagSeparatorInfo;
    }
    for (size_t i = 0; i < parser -> mFlagCount; ++i) {
        FlagInfo * fi = parser -> mFlags[i];
        if (!strcmp(flag, fi -> mName)) {
            return fi;
        }
    }
    return NULL;
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
            cap_pa_set_positional(
                result -> mArguments, posit_info -> mName, 
		one_posit_res.mValue);
            ++positional_index;
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
		result -> mFirstErrorWord = one_flag_res.mFlag -> mName;
		return;
	    case OFPE_CANNOT_PARSE_FLAG:
		result -> mError = PER_CANNOT_PARSE_FLAG;
		result -> mFirstErrorWord = one_flag_res.mFlag -> mName;
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
    // that is easy (for now), because all positionals are required
    if (result -> mArguments -> mPositionalCount < parser -> mPositionalCount) {
        result -> mError = PER_NOT_ENOUGH_POSITIONALS;
        return;
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
