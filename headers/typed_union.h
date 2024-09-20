#ifndef __TYPED_UNION_H__
#define __TYPED_UNION_H__

#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "types.h"
#include "helper_functions.h"

// ============================================================================
// === TYPED UNION CREATION AND DESTRUCTION ===================================
// ============================================================================

/**
 * Create a new `TypedUnion` of type `double`
 */
TypedUnion cap_tu_make_double(double value) {
    return (TypedUnion) { .mType = DT_DOUBLE, .mValue = { .asDouble = value } };
}

/**
 * Create a new `TypedUnion` of type `int`
 */
TypedUnion cap_tu_make_int(int value) {
    return (TypedUnion) { .mType = DT_INT, .mValue = { .asInt = value } };
}

/**
 * Create a new `TypedUnion` of type `presence`
 * 
 * The presence type is used for flags that do not store any explicit value.
 */
TypedUnion cap_tu_make_presence() {
    return (TypedUnion) { .mType = DT_PRESENCE };
}

/**
 * Create a new `TypedUnion` of type `string`
 * 
 * `TypedUnion`s created by this factory contain a dynamically allocated copy
 * of the string used to create them. The owner of this object should dispose
 * of it using the function `cap_tu_destroy`. `TypedUnion` objects are usually 
 * owned by a `ParsedArguments` object so users of the `cap` library do not 
 * need to call `cap_du_destroy` directly.
 */
TypedUnion cap_tu_make_string(const char * value) {
    char * const value_copy = copy_string(value);
    return (TypedUnion) { .mType = DT_STRING, .mValue = { .asString = value_copy } };
}

/**
 * Destroys a `TypedUnion` object
 * 
 * Destroys a `TypedUnion` object when it is no longer needed. Destruction only 
 * has effect if the type is `DT_STRING`, as only that type contains 
 * dynamically allocated memory that must be freed.
 * 
 * @param tu `TypedUnion` to destroy. This function does nothing if `tu` is 
 *        NULL`.
 */
void cap_tu_destroy(TypedUnion * tu) {
    if (!tu) return;
    if (tu -> mType != DT_STRING) {
        return;
    }
    free(tu -> mValue.asString);
    tu -> mValue.asString = NULL;
}

// ============================================================================
// === TYPED UNION CHECKS =====================================================
// ============================================================================

/**
 * Checks if `tu` has type `DT_DOUBLE`.
 */
bool cap_tu_is_double(const TypedUnion * tu) {
    return tu -> mType == DT_DOUBLE;
}

/**
 * Checks if `tu` has type `DT_INT`.
 */
bool cap_tu_is_int(const TypedUnion * tu) {
    return tu -> mType == DT_INT;
}

/**
 * Checks if `tu` has type `DT_PRESENCE`.
 */
bool cap_tu_is_presence(const TypedUnion * tu) {
    return tu -> mType == DT_PRESENCE;
}

/**
 * Checks if `tu` has type `DT_STRING`.
 */
bool cap_tu_is_string(const TypedUnion * tu) {
    return tu -> mType == DT_STRING;
}

// ============================================================================
// === TYPED UNION CONVERSIONS ================================================
// ============================================================================

/**
 * Retrieves a `double` value.
 * 
 * @param tu typed union to take the value from. It must be of type DT_DOUBLE.
 *        The type is checked using an `assert` statement.
 * @return `double` value stored int `tu`
 */
double cap_tu_as_double(const TypedUnion * tu) {
    assert(tu -> mType == DT_DOUBLE);
    return tu -> mValue.asDouble;
}

/**
 * Retrieves an `int` value.
 * 
 * @param tu typed union to take the value from. It must be of type `DT_INT`.
 *        The type is checked using an `assert` statement.
 * @return `int` value stored int `tu`
 */
int cap_tu_as_int(const TypedUnion * tu) {
    assert(tu -> mType == DT_INT);
    return tu -> mValue.asInt;
}

/**
 * Retrieves a string value.
 * 
 * Retrieves a pointer to the null-terminated string stored in `tu`. Note that, 
 * `tu` remains the owner of the string returned. Most importantly, the 
 * returned pointer becomes invalid after `tu` is destroyed using
 * `cap_tu_destroy`.
 * 
 * @param tu typed union to take the value from. It must be of type 
 *        `DT_STRING`. The type is checked using an `assert` statement.
 * @return Pointer to the string stored in `tu`.
 */
const char * cap_tu_as_string(const TypedUnion * tu) {
    assert(tu -> mType == DT_STRING);
    return tu -> mValue.asString;
}

#endif
