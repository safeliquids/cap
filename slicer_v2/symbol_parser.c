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

void token_destroy(Token * tok) {
    if (tok -> kind == TK_IDENTIFIER) {
        free(tok -> identifier);
        tok -> identifier = NULL;
    }
}

void symbol_destroy(PartialSymbol * symbol) {
    if (symbol -> identifier) {
        free(symbol -> identifier);
        symbol -> identifier = NULL;
    }
}

int fgetc_skip_space(FILE * file) {
    int c;
    do {
        c = fgetc(file);
    } while (isspace(c));
    return c;
}

Token get_next_token(FILE * file) {
    Token res = (Token) {
        .kind = TK_FAIL,
        .identifier = NULL
    };
    int c = fgetc_skip_space(file);
    if (c == EOF && feof(file)) {
        res.kind = TK_END;
        return res;
    }
    if (c == EOF) {
        res.kind = TK_FAIL;
        return res;
    }
    if (c == '{') {
        res.kind = TK_LEFT_CURLY;
        return res;
    }
    if (c == '(') {
        res.kind = TK_LEFT_PAREN;
        return res;
    }
    if (c == ';') {
        res.kind = TK_SEMICOLON;
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
            res.kind = TK_TYPEDEF;
            return res;
        }
        if (!strcmp(word, WORD_STRUCT)) {
            res.kind = TK_STRUCT;
            return res;
        }
        if (!strcmp(word, WORD_ENUM)) {
            res.kind = TK_ENUM;
            return res;
        }
        res.kind = TK_IDENTIFIER;
        res.identifier = copy_string(word);
        return res;
    }
    res.kind = TK_FAIL;
    return res;
}

Token get_next_real_token(FILE * file) {
    Token t;
    do {
        t = get_next_token(file);
    } while (t.kind == TK_PREPROCESSOR || t.kind == TK_COMMENT);
    return t;
}

PartialSymbol _symbol_struct_enum(FILE * file) {
    // struct or enum was consumed
    char * identifier = NULL;
    bool has_body = false;
    Token tok = get_next_token(file);
    if (tok.kind == TK_IDENTIFIER) {
        identifier = tok.identifier;
        tok = get_next_token(file);
    }
    if (tok.kind == TK_LEFT_CURLY) {
        // don't need to destroy it, but technically should
        // token_destroy(&tok);
        has_body = true;
        if (!skip_parentheses(file, '{')) {
            free(identifier);
            return (PartialSymbol) {
                .identifier = NULL,
                .kind = SK_UNKNOWN,
            };
        }
        tok = get_next_token(file);
    }
    if (tok.kind == TK_SEMICOLON) {
        return (PartialSymbol) {
            .identifier = identifier,
            .kind = has_body ? SK_TYPE_DEF : SK_TYPE_DEC,
            .is_static = false
        }; 
    }
    token_destroy(&tok);
    free(identifier);
    return (PartialSymbol) {
        .identifier = NULL,
        .kind = SK_UNKNOWN,
    };
}

PartialSymbol _symbol_typedef(FILE * file) {
    // typedef was consumed
    PartialSymbol symbol = (PartialSymbol) {
        .identifier = NULL,
        .kind = SK_UNKNOWN
    };
    Token tok = get_next_real_token(file);
    if (tok.kind != TK_STRUCT && tok.kind != TK_ENUM) {
        token_destroy(&tok);
        return symbol;
    }
    char * identifier = NULL;
    tok = get_next_token(file);
    if (tok.kind == TK_IDENTIFIER) {
        token_destroy(&tok);
        tok = get_next_token(file);
    }
    if (tok.kind == TK_LEFT_CURLY) {
        if (!skip_parentheses(file, '{')) {
            return symbol;
        }
        tok = get_next_token(file);
    }
    if (tok.kind == TK_IDENTIFIER) {
        identifier = tok.identifier;
        // don't destroy tok
        tok = get_next_token(file);
    }
    if (tok.kind == TK_SEMICOLON) {
        return (PartialSymbol) {
            .identifier = identifier,
            .kind = SK_TYPE_DEF,
            .is_static = false
        };
    }
    token_destroy(&tok);
    return symbol;
}

PartialSymbol _symbol_function(FILE * file, Token first_token) {
    char * second_identifier = NULL;
    bool first_identifier = false;
    bool is_static = false;
    bool failed = false;
    bool has_body = false;
    bool first_iteration = true;
    while (true) {
        Token tok = first_iteration ? first_token : get_next_token(file);
        first_iteration = false;
        if (tok.kind == TK_STATIC) {
            is_static = true;
            continue;
        }
        if (tok.kind == TK_STAR || tok.kind == TK_CONST) {
            continue;
        }
        if (tok.kind != TK_IDENTIFIER) {
            failed = true;
            break;
        }
        if (!first_identifier) {
            first_identifier = true;
            token_destroy(&tok);
            continue;
        }
        second_identifier = tok.identifier;
        tok = get_next_token(file);
        if (tok.kind != TK_LEFT_PAREN || !skip_parentheses(file, '(')) {
            token_destroy(&tok);
            failed = true;
            break;
        }
        tok = get_next_token(file);
        if (tok.kind == TK_SEMICOLON) {
            break;
        }
        if (tok.kind != TK_LEFT_CURLY || !skip_parentheses(file, '{')) {
            token_destroy(&tok);
            failed = true;
            break;
        }
        has_body = true;
        break;
    };
    if (failed) {
        free(second_identifier);
        return (PartialSymbol) {
            .identifier = NULL,
            .kind = SK_UNKNOWN,
        };
    }
    return (PartialSymbol) {
        .identifier = second_identifier,
        .kind = has_body ? SK_FUN_DEF : SK_FUN_DEC,
        .is_static = is_static
    };
}

PartialSymbol get_next_partial_symbol(FILE * file) {
    Token tok = get_next_real_token(file);
    if (tok.kind == TK_END) {
        return (PartialSymbol) {
            .identifier = NULL,
            .kind = SK_END,
            .is_static = false
        };
    }
    if (tok.kind == TK_FAIL) {
        return (PartialSymbol) {
            .identifier = NULL,
            .kind = SK_UNKNOWN,
            .is_static = false
        };
    }
    if (tok.kind == TK_TYPEDEF) {
        return _symbol_typedef(file);
    }
    if (tok.kind == TK_STRUCT || tok.kind == TK_ENUM) {
        return _symbol_struct_enum(file);
    }
    return _symbol_function(file, tok);
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

