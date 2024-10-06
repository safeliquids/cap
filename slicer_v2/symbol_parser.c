#include "symbol_parser.h"
#include "slicer_utils.h"

#include <ctype.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define BUFFER_LENGTH 1024

static const char * _get_word(FILE *, int);
static char _skip_rest_of_function(FILE *);

static const char * const WORD_STATIC = "static";
static const char * const WORD_CONST = "const";
static const char * const WORD_TYPEDEF = "typedef";
static const char * const WORD_STRUCT = "struct";
static const char * const WORD_ENUM = "enum";

int fgetc_skip_space(FILE * file) {
    int c;
    do {
        c = fgetc(file);
    } while (isspace(c));
    return c;
}

Token get_next_token(FILE * file) {
    Token res;
    int c = fgetc_skip_space(file);
    if (c == EOF && feof(file)) {
        res.kind = TK_END;
        return res;
    }
    if (c == EOF) {
        res.kind = TK_FAIL;
        return res;
    }
    if (c == '*') {
        res.kind = TK_STAR;
        return res;
    }
    // this is technically not correct, preprocessor macros are not allowed 
    // after other code on a line
    if (c == '#') {
        res.kind = skip_rest_of_line(file) ? TK_PREPROCESSOR : TK_FAIL;
        return res;
    }
    if (c == '/') {
    // assuming we are on top level, so this should always be a comment
        c = fgetc(file);
        if (c != '/' && c != '*') {
            res.kind = TK_FAIL;
            return res;
        }
        bool skipped_comment = 
            c == '/' ? skip_line_comment(file) : skip_comment(file);
        res.kind = skipped_comment ? TK_COMMENT : TK_FAIL;
        return res;
    }
    if ((c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z') || c == '_') {
        const char * word = _get_word(file, c);
        if (!strcmp(word, WORD_STATIC)) {
            res.kind = TK_STATIC;
            return res;
        }
        if (!strcmp(word, WORD_CONST)) {
            res.kind = TK_CONST;
            return res;
        }
        if (!strcmp(word, WORD_TYPEDEF)) {
            res.kind = TK_FAIL;
            return res;
        }
        if (!strcmp(word, WORD_STRUCT)) {
            res.kind = TK_FAIL;
            return res;
        }
        if (!strcmp(word, WORD_ENUM)) {
            res.kind = TK_FAIL;
            return res;
        }
        res.kind = TK_IDENTIFIER;
        res.identifier = copy_string(word);
        return res;
    }
    res.kind = TK_FAIL;
    return res;
}

PartialSymbol get_next_partial_symbol(FILE * file) {
    char * first_identifier = NULL,
         * second_identifier = NULL;
    bool is_function = false;
    bool started_symbol = false;
    PartialSymbol symbol = {NULL, SK_UNKNOWN, false};
    while (true) {
        Token tok = get_next_token(file);
        if (tok.kind == TK_END && !started_symbol) {
            symbol.kind = SK_END;
            return symbol;
        }
        if (tok.kind == TK_FAIL || tok.kind == TK_END) {
            symbol.kind = SK_UNKNOWN;
            break;
        }
        if (tok.kind == TK_PREPROCESSOR || tok.kind == TK_COMMENT) {
            continue;
        }

        if (!started_symbol) started_symbol = true;

        if (tok.kind == TK_STATIC) {
            symbol.is_static = true;
            continue;
        }
        if (tok.kind != TK_IDENTIFIER) {
            // this consumes comments and preprocessor macros also. 
            // preprocessor code should not appear here, but it is siimply 
            // not checked
            continue;
        }
        if (!first_identifier) {
            first_identifier = tok.identifier;
            continue;
        }
        second_identifier = tok.identifier;
        is_function = true;
        break;
    }
    if (is_function) {
        free(first_identifier);
        symbol.identifier = second_identifier;
        char last_char = _skip_rest_of_function(file);
        switch (last_char) {
            case ';': 
                symbol.kind = SK_FUN_DEC;
                break;
            case '}':
                symbol.kind = SK_FUN_DEF;
                break;
            default:
                symbol.kind = SK_UNKNOWN;
                free(symbol.identifier);
                symbol.identifier = NULL;
        }
        return symbol;
    }
    if (first_identifier)
        free(first_identifier);
    if (second_identifier)
        free(second_identifier);
    return symbol;
}

bool skip_rest_of_line(FILE * file) {
    int c;
    do {
        c = fgetc(file);
    } while (c != EOF && c != '\n');
    if (c == '\n') {
        return true;
    }
    return feof(file);
}

bool skip_comment(FILE * file) {
    // leading / * has been consumed
    int c;
    while (true) {
        c = fgetc(file);
        if (c == EOF)
            return false;
        if (c != '*')
            continue;

        c = fgetc(file);
        if (c == EOF)
            return false;
        if (c == '/')
            return true;
    }
}

bool skip_line_comment(FILE * file) {
    // leading // has been consumed
    return skip_rest_of_line(file);
}

bool skip_parentheses(FILE * file, char open) {
    if (!is_open_paren(open)) {
        return false;
    }
    const char close = get_close_paren(open);
    int c;
    while (true) {
        c = fgetc(file);
        if (c == EOF) {
            return false;
        }
        if (c == close) {
            return true;
        }
        if (c == '/') {
            c = fgetc(file);
            if (c != '/' && c != '*') {
                continue;
            }
            bool skipped_comment = c == '/' ? skip_line_comment(file) : skip_comment(file);
            if (!skipped_comment) {
                return false;
            }
            continue;
        }
        if (is_open_paren(c)) {
            if (!skip_parentheses(file, c)) {
                return false;
            }
        }
    }
    return false;
}

bool is_open_paren(char c) {
    return c == '(' || c == '[' || c == '{';
}

bool is_close_paren(char c) {
    return c == ')' || c == ']' || c == '}';
}

bool is_close_paren_for(char open, char c) {
    switch (open) {
        case '(':
            return c == ')';
        case '[':
            return c == ']';
        case '{':
            return c == '}';
        default:
            break;
    }
    return false;
}

char get_close_paren(char open) {
    switch (open) {
        case '(':
            return ')';
        case '[':
            return ']';
        case '{':
            return '}';
        default:
            break;
    }
    return 0;
}

static const char * _get_word(FILE * file, int c) {
    static char buffer[BUFFER_LENGTH];
    size_t length = 0;
    while (isalnum(c) || c == '_') {
        buffer[length++] = c;
        c = fgetc(file);
    }
    buffer[length] = 0;
    if (c != EOF) {
        ungetc(c, file);
    }
    return buffer;
}

/**
 * begins after the function identifier, before first paren
 */
static char _skip_rest_of_function(FILE * file) {
    int c = fgetc_skip_space(file);
    if (c == EOF) {
        return 0;
    }
    if (c != '(' || !skip_parentheses(file, '(')) {
        return 0;
    }
    c = fgetc_skip_space(file);
    if (c == ';') {
        return ';';
    }
    if (c != '{' || !skip_parentheses(file, '{')) {
        return 0;
    }
    return '}';
}

