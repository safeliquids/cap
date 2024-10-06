#ifndef __HEADER_ANALYSIS_H__
#define __HEADER_ANALYSIS_H__

#include "string_list.h"

#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>

typedef struct THeader {
    const char * name;
    const char * filename;

    size_t * includes;
    size_t inc_count;
    size_t inc_alloc;

    size_t times_included;

    bool spent;
} Header;

/**
 * extracts basic information about symbols from a file
 * 
 * Reads file pointed to by symbol_file_name and reads symbol names from 
 * it. Each symbol should be on a separate line and should be unique. Read 
 * symbols are stored in an allocated array and returned using the symbols and
 * symbol_count output parameters.
 */
bool read_public_symbols(const char * symbol_file_name, StringList * symbols);

/**
 * Registers header files from a list.
 * 
 * For each filename listed in filenames, extracts the base name (after last
 * slash) and stores the base name as well as the raw filename in a given 
 * array. The array should have enough space to store as many Header objects 
 * as there are filenames given.
 */
void register_headers(
    size_t count, const char * const * filenames, Header * registered_headers);

/**
 * Computes the index of a file in the list.
 */
size_t header_index(size_t count, const Header * headers, const char * name);

/**
 * Extract information from listed headers about included other headers.
 * 
 * Read listed header files and look for include directives of other user 
 * files. This data is stored in the list of headers.
 * 
 * Also search the files for system includes and place them in system_includes, 
 * and their number into system_inc_count.
 * 
 * Returns false if a reading error occurs.
 */
bool scan_headers_for_includes(
    size_t header_count, Header * headers,  StringList * system_includes);

#endif
