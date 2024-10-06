#include "header_analysis.h"
#include "slicer_utils.h"
#include "symbol_parser.h"

#include <stdbool.h>
#include <stddef.h>
#include <stdlib.h>

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

bool _generate_output(
    size_t header_count, Header * headers, size_t source_count, 
    const char * const * source_file_names, const StringList * public_symbols, 
    const StringList * system_includes, FILE * result_h, FILE * result_c)
{
    // go through headers and put their function declarations into the source 
    // file. Functions listed in public_symbols shouls also be put in the 
    // header file.
    return false;

}

bool _slice_inner(
    const char * const symbol_file_name, 
    const size_t header_count, const char * const * const header_file_names, 
    const size_t source_count, const char * const * const source_file_names,
    StringList * public_symbols, Header * headers, StringList * system_includes,
    const char * result_h_file_name, const char * result_c_file_name)
{
    if (symbol_file_name) {
        bool reading_success = read_public_symbols(
            symbol_file_name, public_symbols);
        if (!reading_success) {
            return false;
            // error message is printed in read_symbol_file if something bad happened
        }
        if (sl_is_empty(public_symbols)) {
            fprintf(stderr, "slicer: no public symbols\n");
            return false; 
        }

#if SLICER_DEBUG == 1
        printf("public symbols:\n");
        for (const StringList * s = public_symbols; s -> tail; s = s -> tail) {
            printf("\t%s\n", s -> value);
        }
#endif
    }

    register_headers(header_count, header_file_names, headers);

    if (!scan_headers_for_includes(header_count, headers, system_includes)) {
        return false;
    }
    
#if SLICER_DEBUG == 1
    printf("\nsystem includes:\n");
    for (StringList * si = system_includes; si -> tail; si = si -> tail) {
        printf("\t%s\n", si -> value);
    }
    printf("\nheaders and their includes:\n");
    for (size_t i = 0; i < header_count; ++i) {
        const Header * this_header = headers + i;
        printf("(%2zu) %s:\n", i, this_header -> name);
        for (size_t j = 0; j < this_header -> inc_count; ++j) {
            size_t include_index = this_header -> includes[j];
            const Header * included = headers + include_index;
            printf("\t(%2zu)%s\n", include_index, included -> name);
        }
        printf("\n");
    }
#endif

    bool sorting_success = topsort_headers(header_count, headers);
    if (!sorting_success) {
        fprintf(stderr, "slicer: include statements are circular\n");
        return false;
    }

#if SLICER_DEBUG == 1
    printf("\ntopologically ordered headers:\n");
    for (size_t i = 0; i < header_count; ++i) {
        const Header * this_header = headers + i;
        printf("(%2zu) %s:\n", i, this_header -> name);
    }
#endif

    // // reproducion
    // FILE * result_h = fopen(result_h_file_name, "w");
    // if (!result_h) {
    //     fprintf(stderr, "slicer: cannot open file %s\n", result_h_file_name);
    //     return false;
    // }
    // FILE * result_c = fopen(result_c_file_name, "w");
    // if (!result_c) {
    //     fprintf(stderr, "slicer: cannot open file %s\n", result_c_file_name);
    //     fclose(result_h);
    //     return false;
    // }
    // bool writing_success = _generate_output(
    //     header_count, headers, source_count, source_file_names, public_symbols,
    //     system_includes, result_h, result_c);
    // fclose(result_h);
    // fclose(result_c);
    // if (!writing_success) {
    //     return false;
    // }
    // return true;


#if SLICER_DEBUG == 1
    printf("\nfirst header symbols:\n");
    FILE * first_header = fopen(headers[0].filename, "r");
    if (!first_header) {
        fprintf(
            stderr, "slicer: cannot open file %s\n", 
            headers[0].filename);
        return false;
    }
    bool good_reading = true;
    while (true) {
        // long int before_symbol = ftell(first_header);
        PartialSymbol symbol = get_next_partial_symbol(first_header);
        if (symbol.kind == SK_END) {
            break;
        }
        if (symbol.kind == SK_UNKNOWN) {
            good_reading = false;
            break;
        }
        if (symbol.kind == SK_FUN_DEC) {
            printf(
                "\tfunction declaration: %s %s\n", 
                symbol.is_static ? "static" : "      ", symbol.identifier);
        }
        if (symbol.kind == SK_FUN_DEF) {
            printf(
                "\tfunction definition: %s %s\n", 
                symbol.is_static ? "static" : "      ", symbol.identifier);
        }
        if (symbol.kind == SK_TYPE_DEC) {
            printf(
                "\ttype declaration: %s %s\n", 
                "      ", symbol.identifier ? symbol.identifier : "");
        }
        if (symbol.kind == SK_TYPE_DEF) {
            printf(
                "\ttype definition: %s %s\n", 
                "      ", symbol.identifier ? symbol.identifier : "");
        }
        if (symbol.identifier) {
            free(symbol.identifier);
        }
    }
    fclose(first_header);
    if (!good_reading) {
        fprintf(
            stderr, "slicer: could not read from file %s\n", 
            headers[0].filename);
        return false;
    }
#endif

    return false;

}

bool slice(
    const char * symbol_file_name, 
    size_t header_count, const char * const * header_file_names, 
    size_t source_count, const char * const * source_file_names,
    const char * result_header_file_name, const char * result_source_file_name) 
{
    StringList * public_symbols = sl_empty();
    StringList * system_includes = sl_empty();
    Header * headers = (Header *) malloc(header_count * sizeof(Header));

    bool success = _slice_inner(
        symbol_file_name, header_count, header_file_names, source_count,
        source_file_names, public_symbols, headers, system_includes,
        result_header_file_name, result_source_file_name);

    sl_destroy(public_symbols);
    sl_destroy(system_includes);
    for (size_t i = 0; i < header_count; ++i) {
        Header * h = headers + i;
        if (h -> includes) {
            free(h -> includes);
            h -> includes = NULL;
        }
    }
    free(headers);
    return success;
}


