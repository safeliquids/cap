#ifndef __SLICER_H__
#define __SLICER_H__

#include <stdbool.h>
#include <stddef.h>

bool slice(
    const char * const symbol_file_name, 
    const size_t header_count, const char * const * const header_file_names, 
    const size_t source_count, const char * const * const source_file_names);

#endif
