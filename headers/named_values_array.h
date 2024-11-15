#ifndef __NAMED_VALUES_ARRAY_H__
#define __NAMED_VALUES_ARRAY_H__

/**
 * @file
 * @addtogroup named_values
 * 
 * `NamedValuesArray` stores several `NamedValues` objects, implementing a
 * primitive multi-map of `string --> TypedUnion`. `NamedValuesArray` objects
 * are used internally by `ParsedArguments` and users of the library never needs
 * to interact with them directly. Regardless, the API offers several functions
 * for creating, deleting, and modifying these objects. These functions are
 * prefixed with `cap_nva_`.
 * 
 * `NamedValuesArray` objects should be created using `cap_nva_make_empty()` and
 * deleted using `cap_nva_destroy()`. After destroying the object, all
 * information obtained from it is invalid. e.g. pointers to stored
 * `TypedUnion`s.
 */

#include "named_values.h"
#include "typed_union.h"

#include <stddef.h>
#include <stdlib.h>
#include <string.h>

/**
 * @addtogroup named_values
 * @{
 */

// ============================================================================
// === NAMED VALUES ARRAY =====================================================
// ============================================================================

/**
 * List of NamedValues instances.
 * 
 * Stores a list of NamedValues instances.
 */
typedef struct {
    NamedValues ** mItems;
    size_t mCount;
    size_t mAlloc;
} NamedValuesArray;

// ============================================================================
// === NAMED VALUES ARRAY: DECLARATION OF PRIVATE FUNCTIONS ===================
// ============================================================================

static void _cap_nva_append_item(
    NamedValuesArray * nva, const char * name, const TypedUnion value);

// ============================================================================
// === NAMED VALUES ARRAY FUNCTIONS ===========================================
// ============================================================================

/**
 * Creates a new empty NamedValuesArray object
 * 
 * Creates and returns a new empty NamdeValuesArray object. Instances created
 * this way should be disposed of using `cap_nva_destroy`.
 */
NamedValuesArray * cap_nva_make_empty() {
    NamedValuesArray * nva = (NamedValuesArray *) malloc(
        sizeof(NamedValuesArray));
    nva -> mItems = NULL;
    nva -> mCount = nva -> mAlloc = 0u;
    return nva;
}

/**
 * Destroys a `NamedValuesArray` object
 * 
 * Destroys a `NamedValuesArray` object and all data stored in it. Any pointers
 * obtained from `nva` before this call become invalid.
 * 
 * @param nva object to destroy; if it is NULL, nothing happens
 */
void cap_nva_destroy(NamedValuesArray * nva) {
    if (!nva) {
        return;
    }
    for (size_t i = 0u; i < nva -> mCount; ++i) {
        cap_nv_destroy(nva -> mItems[i]);
        nva -> mItems[i] = NULL;
    }
    free(nva -> mItems);
    nva -> mItems = NULL;
    nva -> mAlloc = nva -> mCount = 0u;
}

/**
 * Gets the number of items in a NamedValuesArray object
 * 
 * Returns the number of items (`NamedValues` objects) stored in `nva`. 
 * If `nva` is NULL, always returns zero.
 * 
 * @param nva object to measure
 * @return number of items, or zero if `nva` is NULL
 */
size_t cap_nva_length(const NamedValuesArray * nva) {
    if (!nva) {
        return 0u;
    }
    return nva -> mCount;
}

/**
 * Gets a `NamedValues` with a given name
 * 
 * Returns a `NamedValues` object stored in `nva` with the name `name`. 
 * If `nva` or `name` are NULL, returns NULL. If no object named `name` is
 * present also returns NULL.
 * 
 * @param nva object to search
 * @param name of the item to look for
 * @return pointer to the NamedValues with the name `name`, or NULL if no such
 *         object is stored
 */
NamedValues * cap_nva_get(const NamedValuesArray * nva, const char * name) {
    if (!nva || !name) {
        return NULL;
    }
    for (size_t i = 0; i < nva -> mCount; ++i) {
        NamedValues * item = nva -> mItems[i];
        if (!strcmp(name, item -> mName)) {
            return item;
        }
    }
    return NULL;
}

/**
 * Appends a value to a `NamedValues` with a given name
 * 
 * Attempts to find a `NamedValues` object stored in `nva`. If it is found, 
 * appends `value` to it. Else a new `NamedValues` is created with the given 
 * `name` and `value` and it is stored in `nva`.
 * 
 * If `nva` or `name` are NULL, nothing happens.
 * 
 * @param nva object to search
 * @param name name of the `NamedValues` to append to
 * @param value value to append to the object found
 */
void cap_nva_append_value(
    NamedValuesArray * nva, const char * name, const TypedUnion value)
{
    if (!nva || !name) {
        return;
    }
    NamedValues * item = cap_nva_get(nva, name);
    if (!item) {
        // insert a new item at the end
        _cap_nva_append_item(nva, name, value);
        return;
    }
    cap_nv_append_value(item, value);
}

/**
 * Sets a value for a `NamedValues` with a given name
 * 
 * Attempts to find a `NamedValues` object stored in `nva`. If it is found, 
 * replaces its value(s) with a new given `value`. If it is not found, a new
 * `NamedValues` is created with `name` and `value`.
 * 
 * If `nva` or `name` are NULL, nothing happens.
 * 
 * @param nva object to search
 * @param name name of the `NamedValues` to set a value for
 * @param value value to set
 */
void cap_nva_set_value(
    NamedValuesArray * nva, const char * name, const TypedUnion value) 
{
    if (!nva || !name) {
        return;
    }
    NamedValues * item = cap_nva_get(nva, name);
    if (!item) {
        _cap_nva_append_item(nva, name, value);
        return;
    }
    cap_nv_clear_values(item);
    cap_nv_append_value(item, value);
}

// ============================================================================
// === NAMED VALUES ARRAY: IMPLEMENTATION OF PRIVATE FUNCTIONS ================
// ============================================================================

static void _cap_nva_append_item(
    NamedValuesArray * nva, const char * name, const TypedUnion value)
{
    static const size_t INIT_ALLOC = 4u;
    size_t alloc = nva -> mAlloc;
    if (nva -> mCount >= alloc) {
        alloc = alloc ? alloc * 2 : INIT_ALLOC;
        nva -> mItems = (NamedValues **) realloc(
            nva -> mItems, alloc * sizeof(NamedValues *));
        nva -> mAlloc = alloc;
    }
    // since we know that name is not NULL, this will not create NULL
    nva -> mItems[nva -> mCount++] = cap_nv_make(name, value);
}

/**
 * @}
 */

#endif
