#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/*
 * the slicer tool
 *
 * input:
 *      - list of header and source files
 *      - a file listing the names of public symbols
 * output:
 *     - a header file declaring public symbols
 *     - a source file defining all symbols
 *
 * rules:
 *     - all symbols must be either functions or types
 *     - a list of symbols to be made public must be present
 *     - all symbols not listed will become static in the source file (i.e. inaccessible to the linker)
 *     - functions with bodies are not allowed in header files
 */

static const char * const SYMBOL_FILE_FLAG = "-S";
static const char * const HELP_FLAG = "--help";
static const size_t LINE_BUFFER_SIZE = 1024;

typedef enum {
    SK_UNKNOWN,
    SK_FUNCTION,
    SK_TYPE
} SymbolKind;

typedef struct {
    char * name;
    SymbolKind kind;
    const char * file;
    size_t line;
} Symbol;

void print_usage(FILE * file, const char * prog) {
    fprintf(file, "usage:\n");
    fprintf(file, "\t%s %s SYMFILE FILE [FILE ...]\n", prog, SYMBOL_FILE_FLAG);
    fprintf(file, "\t%s %s\n", prog, HELP_FLAG);
}

void print_help(FILE * file, const char * prog) {
    fprintf(
        file, "%s is a tool for aggregating C projects into one compilation "
        "unit.\n", prog);
    fprintf(file, "\n");
    print_usage(file, prog);
    fprintf(
        file, "\noptions:\n");
    fprintf(file, "\t%s\tDisplay this help message and exit.\n", HELP_FLAG);
    fprintf(
        file, "\t%s\tSymbols written in this file will be listed in the "
        "resulting header file.", SYMBOL_FILE_FLAG);  
    fprintf(file, "\narguments:\n");
    fprintf(file, "\tFILE\t list of header and source files to use\n");
}

bool has_suffix(const char * filename, const char * suffix) {
    size_t flen = strlen(filename),
           slen = strlen(suffix),
           offset;
    if (slen > flen) return false;
    offset = flen - slen;
    return !(bool)strcmp(filename + offset, suffix);
}

bool is_header(const char * filename) {
    return has_suffix(filename, ".h");
}

bool is_source(const char * filename) {
    return has_suffix(filename, ".c");
}

void parse_arguments(
        int argc, const char ** argv, size_t * source_count,
        const char ** sources, size_t * header_count, const char ** headers, 
        const char ** symbol_file_name) {
    bool next_is_symbol_file = false;
    for (int i = 1; i < argc; ++i) {
        const char * arg = argv[i];
        if (arg[0] == 0) {
            fprintf(stderr, "%s: empty file names are not allowed\n", *argv);
            exit(-1);
        }
        
        if (next_is_symbol_file) {
            next_is_symbol_file = false;
            *symbol_file_name = arg;
            continue;
        }

        if (arg[0] != '-') {
            // this is a file name
            if (is_header(arg)) {
                headers[(*header_count)++] = arg;
                continue;
            }
            if (!is_source(arg)) {
                fprintf(
                    stderr, "%s: file name '%s' does not look like a header or a "
                    "source file. Assuming source.\n", *argv, arg);
            }
            sources[(*source_count)++] = arg;
            continue;
        }

        if (!strcmp(arg, SYMBOL_FILE_FLAG)) {
            next_is_symbol_file = true;
            continue;
        }

        if (!strcmp(arg, HELP_FLAG)) {
            print_help(stdout, *argv);
            exit(1);
        }

        print_usage(stderr, *argv);
        exit( -1);
    }
}

bool read_public_symbols(const char * symbol_file_name, Symbol ** symbols, size_t * symbol_count) {
    FILE * symbol_file = fopen(symbol_file_name, "r");
    if (!symbol_file) {
        fprintf(stderr, "slicer: cannot read from file %s\n", symbol_file_name);
        return false;
    }
    bool success = true;
    char line_buffer[LINE_BUFFER_SIZE];
    size_t line_count = 0;
    size_t symbol_alloc = 4;
    *symbols = (Symbol *) malloc(symbol_alloc * sizeof(Symbol));
    
    while (true) {
        char * s = fgets(line_buffer, LINE_BUFFER_SIZE, symbol_file);
        int eof = feof(symbol_file);
        if (!s && eof) { 
            break;
        }
        if (!s) {
            success = false;
            fprintf(stderr, "slicer: error reading from file %s\n", symbol_file_name);
            break;
        }
        ++line_count;
        size_t len = strlen(s);
        if (s[len - 1] != '\n') {
            success = false;
            fprintf(stderr, "slicer: line %zu of file %s was too long\n", line_count, symbol_file_name);
            break;
        }
        if (len == 1u || s[0] == '#') {
            continue;
        }
        char * symbol_name = (char *) malloc(len * sizeof(char));
        memcpy(symbol_name, s, len - 1);
        symbol_name[len - 1] = 0;
        
        if (*symbol_count >= symbol_alloc) {
            symbol_alloc *= 2;
            *symbols = realloc(*symbols, symbol_alloc * sizeof(Symbol));
        }
        Symbol * new_sym = (*symbols) + (*symbol_count)++;
        new_sym -> name = symbol_name;
        new_sym -> kind = SK_UNKNOWN;
        new_sym -> file = NULL;
        new_sym -> line = 0u;
    }

    fclose(symbol_file);
    return success;
}

int main (int argc, const char ** argv) {
    const char ** sources = (const char **) malloc((argc - 1) * sizeof(const char *));
    const char ** headers = (const char **) malloc((argc - 1) * sizeof(const char *));
    size_t source_count = 0,
           header_count = 0;
    const char * symbol_file_name = NULL;

    parse_arguments(argc, argv, &source_count, sources, &header_count, headers, &symbol_file_name);
    if (symbol_file_name == NULL) {
        fprintf(stderr, "slicer: no symbol file\n");
        return -1;
    }
    if (!header_count) {
        fprintf(stderr, "slicer: no header files\n");
        return -1;
    }
    if (!source_count) {
        fprintf(stderr, "slicer: no source files\n");
        return -1;
    }

    Symbol * public_symbols = NULL;
    size_t public_symbol_count = 0;
    bool reading_success = read_public_symbols(
        symbol_file_name, &public_symbols, &public_symbol_count);
    if (!reading_success) {
        return -1;
        // error message is printed in read_symbol_file if something bad happened
    }
    if (!public_symbol_count) {
        fprintf(stderr, "slicer: no public symbols\n");
        return -1; 
    }

    for (size_t i = 0; i < public_symbol_count; ++i) {
        printf("%s\n", public_symbols[i].name);
    }

    return 0;
}

