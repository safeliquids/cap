#include "header_analysis.h"
#include "slicer.h"

#include <stddef.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <stdlib.h>

static const char * const HELP_FLAG = "--help";
static const char * const SYMBOL_FILE_FLAG = "-S";
static const char * const RESULT_HEADER_FLAG = "-H";
static const char * const RESULT_SOURCE_FLAG = "-C";

void print_usage(FILE * file, const char * prog) {
    fprintf(file, "usage:\n");
    fprintf(file, "\t%s %s\n", prog, HELP_FLAG);
    fprintf(
        file, "\t%s %s HEADER %s SOURCE [%s SYMFILE] FILE [FILE ...]\n", 
        prog, RESULT_HEADER_FLAG, RESULT_SOURCE_FLAG, SYMBOL_FILE_FLAG);
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
    fprintf(file, "\t%s\tName of the resulting header file\n", RESULT_HEADER_FLAG);
    fprintf(file, "\t%s\tName of the resulting source file\n", RESULT_SOURCE_FLAG);
    fprintf(
        file, "\t%s\tSymbols listed in this file will be written in the "
        "resulting header file. If omitted, all symbols are written.", SYMBOL_FILE_FLAG);  
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
    const char ** symbol_file_name, const char ** result_header_file_name,
    const char ** result_source_file_name) 
{
    static const int S_FILE = 0,
                     S_SYMBOL_FILE = 1,
                     S_RESULT_HEADER = 2,
                     S_RESULT_SOURCE = 3;
    int state = S_FILE;
    for (int i = 1; i < argc; ++i) {
        const char * arg = argv[i];
        if (arg[0] == 0) {
            fprintf(stderr, "%s: empty file names are not allowed\n", *argv);
            exit(-1);
        }

        if (state == S_SYMBOL_FILE) {
            *symbol_file_name = arg;
            state = S_FILE;
            continue;
        }
        if (state == S_RESULT_HEADER) {
            *result_header_file_name = arg;
            state = S_FILE;
            continue;
        }
        if (state == S_RESULT_SOURCE) {
            *result_source_file_name = arg;
            state = S_FILE;
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

        if (!strcmp(arg, HELP_FLAG)) {
            print_help(stdout, *argv);
            exit(1);
        }
        if (!strcmp(arg, SYMBOL_FILE_FLAG)) {
            state = S_SYMBOL_FILE;
            continue;
        }
        if (!strcmp(arg, RESULT_HEADER_FLAG)) {
            state = S_RESULT_HEADER;
            continue;
        }
        if (!strcmp(arg, RESULT_SOURCE_FLAG)) {
            state = S_RESULT_SOURCE;
            continue;
        }

        print_usage(stderr, *argv);
        exit( -1);
    }
}

int main (int argc, const char ** argv) {
    const char ** sources = (const char **) malloc((argc - 1) * sizeof(const char *));
    const char ** headers = (const char **) malloc((argc - 1) * sizeof(const char *));
    size_t source_count = 0,
           header_count = 0;
    const char * symbol_file_name = NULL;
    const char * result_header_file_name = NULL;
    const char * result_source_file_name = NULL;

    parse_arguments(
        argc, argv, &source_count, sources, &header_count, headers, 
        &symbol_file_name, &result_header_file_name, &result_source_file_name);

    if (result_header_file_name == NULL) {
        fprintf(stderr, "slicer: no output header file\n");
        return -1;
    }
    if (result_source_file_name == NULL) {
        fprintf(stderr, "slicer: no output source file\n");
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

    bool success = slice(
        symbol_file_name, header_count, headers, source_count, sources);
    
    // I could free stuff here, but who cares, it's the main function

    return success ? 0 : -1;
}
