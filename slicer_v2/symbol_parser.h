/**
 * Partial parser for C function and type declarations/definitions.
 * 
 * This module provides two objects: a (partial) symbol parser and a symbol 
 * reproducer. They can handle C functions and type declarations. 
 * 
 * When parsing a header or source file, it is only really important to achieve
 * two things:
 * 1. find the symbol name, and
 * 2. find what kind of symbol it is (function or type)
 * 
 * For functions, it is also important to know if it is declared static or not.
 * That is because when reproducing functions into a sliced source file, 
 * non-public ones should be made static if they are not static already.
 * 
 * When reproducing symbols, it is necessary to know
 * 1. what kind of symbol it is and if it is a declaration or definition,
 * 2. if it should be reproduced into the result header and the result
 *    source, and
 * 3. if it needs to be made static in the source (for functions).
 * 
 * Depending on the symbol kind reproduction is easy.
 */

#ifndef __SYMBOL_PARSER_H__
#define __SYMBOL_PARSER_H__

#include <stdbool.h>
#include <stdio.h>

/**
 * the kind of token from partial lexical analysis
 * 
 * The kind of a token obtained from a lexer. TK_IDENTIFIER contains a string
 * representation of the token and also accounts for primitive types.
 */
typedef enum {
    TK_STATIC,
    TK_CONST,
    TK_STAR,
    TK_IDENTIFIER,
    TK_FAIL,
    TK_END,
    TK_COMMENT,
    TK_PREPROCESSOR
} TokenKind;

/**
 * lexical token
 * 
 * Token obtained from a lexer. If the kind is TK_IDENTIFIER, a copy of the 
 * identifier is placed in `identifier`. Else nothing is stored.
 */
typedef struct {
    TokenKind kind;
    char * identifier;
} Token;

typedef enum {
    SK_UNKNOWN,
    SK_END,
    SK_TYPE_DEC,
    SK_TYPE_DEF,
    SK_FUN_DEC,
    SK_FUN_DEF
} SymbolKind;

typedef struct {
    char * identifier;
    SymbolKind kind;
    bool is_static;
} PartialSymbol;

int fgetc_skip_space(FILE * file);

/**
 * obtains a token from file
 */
Token get_next_token(FILE * file);

PartialSymbol get_next_partial_symbol(FILE * file);

bool skip_rest_of_line(FILE * file);
bool skip_comment(FILE * file);
bool skip_line_comment(FILE * file);

bool skip_parentheses(FILE * file, char open);

bool is_open_paren(char c);
bool is_close_paren(char c);
bool is_close_paren_for(char open, char c);
char get_close_paren(char open);

#endif
