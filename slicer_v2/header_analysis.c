#include "header_analysis.h"
#include "slicer_utils.h"

#include <assert.h>
#include <ctype.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define LINE_BUFFER_SIZE 1024

bool read_public_symbols_inner(
    LimitedLineReader * reader, StringList * symbols, 
    const char * symbol_file_name) 
{
    while (true) {
        llr_advance(reader);
        switch (reader -> status) {
            case OK:
                break;
            case END:
                return true;
            case TOO_LONG:
                fprintf(
                    stderr, "slicer: line %zu of file %s was too long\n", 
                    reader -> line_number, symbol_file_name);
                return false;
            case FAIL:
                fprintf(
                    stderr, "slicer: error reading from file %s\n", 
                    symbol_file_name);
                return false;
            default:
                assert(false && "unreachable in read_public_symbols_inner");
        }
        char * s = reader -> line_buffer;
        s[reader -> line_length - 1] = 0;  // removes newline
        while (isspace(*s)) {
            ++s;
        }
        if (!*s || *s == '#') {
            continue;
        }
        // this ignores duplicate symbols, idk if that is good
        sl_insert_if_missing(symbols, s);  
    }
    return true;
}

bool read_public_symbols(const char * symbol_file_name, StringList * symbols) {
    LimitedLineReader reader = llr_open(symbol_file_name);
    if (reader.status == FAIL) {
        fprintf(stderr, "slicer: cannot read from file %s\n", symbol_file_name);
        return false;
    }
    bool success = read_public_symbols_inner(
        &reader, symbols, symbol_file_name);
    llr_close(&reader);
    return success;
}

void register_headers(
        size_t count, const char * const * filenames,
        Header * registered_headers) {
    for (size_t i = 0; i < count; ++i) {
        const char * raw_file_name = filenames[i];
        // extract only the file name. let's be lazy and assume that it is the
        // part after the last slash
        const char * last_slash = strrchr(raw_file_name, '/');
        // insert windows stuff here...
        const char * file_name = last_slash ? last_slash + 1 : raw_file_name;

        registered_headers[i] = (Header) {
            .name = file_name,
            .filename = raw_file_name,
            .includes = NULL,
            .inc_count = 0,
            .inc_alloc = 0,
            .times_included = 0,
            .spent = false
        };
    }
}

size_t header_index(size_t count, const Header * headers, const char * name) {
    for (size_t i = 0; i < count; ++i) {
        if (!strcmp(name, headers[i].name)) {
            return i;
        }
    }
    return (size_t) -1;
}

/**
 * returns a pointer to a filename in an include directive, or NULL if given
 * line is not an include directive. 
 * 
 * NB: the returned pointer points to one character before the actual name, 
 * to distinguish between user and system includes. User includes will have the 
 * first character '"', system would be '<'.
 * 
 */
const char * find_include(const char * line) {
    static char include[LLR_LINE_BUFFER_LENGTH];
    if (strncmp(line, "#include", 8)) {
        return NULL;
    }
    const char * opening = line + 8;
    while (*opening && isspace(*opening)) {
        ++opening;
    }
    char closing_char;
    switch (*opening) {
        case '"':
            closing_char = '"';
            break;
        case '<':
            closing_char = '>';
            break;
        default:
            return NULL;
    }
    const char * closing = strchr(opening + 1, closing_char);
    if (!closing) {
        return NULL;
    }
    size_t len = closing - opening;
    memcpy(include, opening, len);
    include[len] = 0;
    return include;
}

/**
 * goes through one header file and looks for include directives referencing 
 * other listed headers. Records this information inside this_header.
 * 
 * Returns false if any error occurs. Prints an error message before returning 
 * false.
 */
bool scan_one_header_for_includes(
    LimitedLineReader * reader, Header * this_header, size_t header_count,
    const Header * headers, StringList * sys_includes)
{
    while (true) {
        llr_advance(reader);
        switch (reader -> status) {
            case OK:
                break;
            case FAIL:
                fprintf(
                    stderr, "slicer: error reading from file %s:%zu\n", 
                    this_header -> name, reader -> line_number);
                return false;
            case TOO_LONG:
                fprintf(
                    stderr, "slicer: error reading from file %s:%zu\n", 
                    this_header -> name, reader -> line_number);
                return false;
            case END:
                return true;
            default:
                assert(false && "unreachable in scan_one_header_for_includes");
        }

        char * s = reader -> line_buffer;
        while (*s && isspace(*s)) 
            ++s;
        if (*s != '#')
            continue;

        const char * included_file = find_include(s);
        if (!included_file) {
            continue;
        }
        if (*included_file == '<') {
            // add to system files
            sl_insert_if_missing(sys_includes, included_file + 1);
            continue;
        }
        const char * user_include = included_file + 1;
        size_t include_index = header_index(header_count, headers, user_include);
        if (include_index == (size_t)-1) {
            fprintf(stderr, "slicer: found unknown user include \"%s\" in file %s:%zu\n", user_include, this_header -> name, reader -> line_number);
            return false;
        }
        // reallocate include list if necessary
        if (this_header -> inc_count >= this_header -> inc_alloc) {
            this_header -> inc_alloc = this_header -> inc_alloc 
                ? this_header -> inc_alloc * 2 : 4;
            this_header -> includes = (size_t *) realloc(
                this_header -> includes, 
                this_header -> inc_alloc * sizeof(size_t));
        }
        this_header -> includes[this_header -> inc_count++] = include_index;
    }
    return true;
}

bool scan_headers_for_includes(
    size_t header_count, Header * headers, StringList * sys_includes) 
{
    for (size_t i = 0; i < header_count; ++i) {
        LimitedLineReader reader = llr_open(headers[i].filename);
        if (reader.status == FAIL) {
            fprintf(stderr, "slicer: cannot open file %s\n", headers[i].filename);
            return false;
        }
        bool success = scan_one_header_for_includes(
            &reader, headers + i, header_count, headers, 
            sys_includes);
        llr_close(&reader);
        if (!success) {
            // error message was printed by scan_one_header_for_includes
            return false;
        }
    }
    return true;
}

