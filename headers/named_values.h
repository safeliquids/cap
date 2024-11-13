#ifndef __NAMED_VALUES_H__
#define __NAMED_VALUES_H__

#include "helper_functions.h"
#include "typed_union.h"

#include <stddef.h>
#include <stdlib.h>
#include <string.h>

// ============================================================================
// === NAMED VALUES ===========================================================
// ============================================================================

/**
 * Stores values for a flag or positional argument in a `ParsedArguments`
 * object.
 * 
 * Stores the values associated with a single flag or positional in
 * a `ParsedArguments` object. Objects of this type should never be directly
 * created or accessed by the user.
 * 
 * @see ParsedArguments
 * @see TypedUnion
 */
typedef struct {
    /// Name of the flag or posittional. The object should be considered the
    /// 'owner' of this string.
    char * mName;
    /// Array of stored values. This object should be considered the owner 
    /// of these values.
    TypedUnion * mValues;
    /// Number of values stored for this flag/positional
    size_t mValueCount;
    /// Number of values that can currently be stored in `mValues`
    size_t mValueAlloc;
} NamedValues;

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
// === NAMED VALUES: DECLARATION OF PRIVATE FUNCTIONS =========================
// ============================================================================

NamedValues * _cap_nv_make_empty_inner(const char * name);
void _cap_nva_append_item(
    NamedValuesArray * nva, const char * name, const TypedUnion value);

// ============================================================================
// === NAMED VALUES ===========================================================
// ============================================================================

/**
 * Creates a new empty NamedValues object
 * 
 * Creates a new empty NamedValues object with the name `name` and, of course,
 * no values. Objects created this way should be disposed of using
 * `cap_nv_destroy`. 
 * 
 * @param name null-terminated name of the object. A copy of this string is
 *        stored in the object; if it is `NULL`, nothing happens.
 * @return new object
 */
NamedValues * cap_nv_make_empty(const char * name) {
    if (!name) {
        return NULL;
    }
    return _cap_nv_make_empty_inner(name);
}

/**
 * Creates a new NamedValues object initialized with a value
 * 
 * Creates a new `NamedValues` object with the name `name` and containing
 * `value` as the only value. Objects created this way should be disposed of
 * using `cap_nv_destroy`. 
 * 
 * @param name null-terminated name of the object. A copy of this string is
 *        stored in the object; if it is `NULL`, nothing happens.
 * @param value value to initialize the new object with
 * @return new object
 */
NamedValues * cap_nv_make(const char * name, const TypedUnion value) {
    if (!name) {
        return NULL;
    }
    NamedValues * nv = _cap_nv_make_empty_inner(name);
    nv -> mValues = (TypedUnion *) malloc(sizeof(TypedUnion));
    nv -> mValues[0] = value;
    nv -> mValueAlloc = nv -> mValueCount = 1u;
    return nv;
}

/**
 * Removes all values stored in `nv`
 * 
 * Destrys all values tored in `nv` and de-allocates the storage array.
 * 
 * @param nv object to remove values from
 */
void cap_nv_clear_values(NamedValues * nv) {
    if (!nv) {
        return;
    }
    for (size_t i = 0u; i < nv -> mValueCount; ++i) {
        cap_tu_destroy(nv -> mValues + i);
    }
    if (nv -> mValues) {
        free(nv -> mValues);
        nv -> mValues = NULL;
    }
    nv -> mValueCount = nv -> mValueAlloc = 0u;
}

/**
 * Destroys a namedValues instance
 * 
 * Destroys a given `NamedValues` object. Its name and all values stored in it
 * are also destroyed, and references to values obtained from this are
 * no longer valid.
 * 
 * @param nv object to destroy; if if is `NULL`, nothing happens.
 */
void cap_nv_destroy(NamedValues * nv) {
    if (!nv) {
        return;
    }
    cap_nv_clear_values(nv);
    delete_string_property(&(nv -> mName));
}

/**
 * Adds a new value to a NamedValues object
 * 
 * Adds a new value `value` to the `NamedValues` object `nv`. `nv` becomes the
 * owner of `value`. Only newly created `TypedUnion` values should be given
 * to this function.
 * 
 * 
 * 
 * @param nv object to append to; if if is `NULL`, nothing happens.
 * @param value value to add
 */
void cap_nv_append_value(NamedValues * nv, const TypedUnion value) {
    static const size_t INIT_ALLOC = 1u;
    if (!nv) {
        return;
    }
    size_t alloc = nv -> mValueAlloc;
    if (nv -> mValueCount >= alloc) {
        alloc = alloc ? alloc * 2 : INIT_ALLOC;
        nv -> mValues = (TypedUnion *) realloc(
            nv -> mValues, alloc * sizeof(TypedUnion));
        nv -> mValueAlloc = alloc;
    }
    nv -> mValues[nv -> mValueCount++] = value;
}

/**
 * Number of values stored
 * 
 * Returns the number of values tored in `nv`. If `nv` is NULL, returns zero.
 * 
 * @param nv object to search; if it is NULL, zero is returned.
 */
size_t cap_nv_value_count(const NamedValues * nv) {
    if (!nv) {
        return 0u;
    }
    return nv -> mValueCount;
}

/**
 * Gets a stored value at a given index.
 * 
 * Returns a pointer to a value stored in `nv` at the positiona `index`.
 * NULL is returned if `nv` is NULL or if `index` is out of range.
 * 
 * @param nv object to search; if it is NULL, NULL is returned
 * @param index index of the value to return, starting at zero; if it is out of
 *        range (i.e. `index >= # of values in nv`), NULL is returned
 * @return pointer to the referenced value, or NULL
 */
const TypedUnion * cap_nv_get_value_i(const NamedValues * nv, size_t index) {
    if (!nv || nv -> mValueCount <= index) {
        return NULL;
    }
    return nv -> mValues + index;
}

/**
 * Gets the first stored value
 * 
 * Returns the first value stored in `nv`, or NULL if `nv` is NULL. If no 
 * values are stored in `nv`, also returns NULL. This behaviour is identical 
 * to `cap_nv_get_value_i(nv, 0u)`.
 * 
 * @param nv object to search; if it is NULL, NULL is returned
 * @return pointer to the first value stored, or NULL if there are no values
 */
const TypedUnion * cap_nv_get_value(const NamedValues * nv) {
    return cap_nv_get_value_i(nv, 0u);
}

// ============================================================================
// === NAMED VALUES ARRAY =====================================================
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
// === NAMED VALUES: IMPLEMENTATION OF PRIVATE FUNCTIONS ======================
// ============================================================================

NamedValues * _cap_nv_make_empty_inner(const char * name) {
    NamedValues * nv = (NamedValues *) malloc(sizeof(NamedValues));
    nv -> mName = copy_string(name);
    nv -> mValues = NULL;
    nv -> mValueCount = nv -> mValueAlloc = 0u;
    return nv;
}

void _cap_nva_append_item(
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

#endif

