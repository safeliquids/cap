#ifndef __TYPED_UNION_H__
#define __TYPED_UNION_H__

/**
 * @file
 * @defgroup typed_union Data Types and Typed Unions
 * 
 * The `TypedUnion` structure is a cell of data marked with its type. This way,
 * it is possible to store arbitrary data without using the exact type in code,
 * (which would otherwise be necessary, since C is a statically typed language.)
 * The actual type of data can be an integer (`int`), real number (`double`), or
 * null-terminated string (`char *`). A special type is what this library calls
 * "presence". It is used to identify the existence of something (e.g. a command
 * line flag) which does not store an explicit value. The presence or absence of
 * it itself *is* the information.
 *
 * The type of the value stored in a `TypedUnion` object corresponds to the
 * value of a  `DataType` enum. Those values are `DT_INT`, `DT_DOUBLE`,
 * `DT_STRING` and `DT_PRESENCE`. These identifiers are useful in other parts of
 * the API as well.
 * 
 * `TypedUnion` instances should not be created directly. Insted, factory 
 * functions such as `cap_tu_make_presence()` should be used. Similarly, when no
 * longer needed, `TypedUnion`s should be deleted using `cap_tu_destroy()`.
 * Descriptions of these functions is at the end of this section. Keep in mind
 * that, `TypedUnion`s should only be destroyed by the part of the code which
 * "owns" them. The user almost never needs to explicitly create (and own) those
 * objects, and therefore does not directly call the `cap_tu_destroy()`
 * function.
 *
 * Information can be extracted out of a `TypedUnion` using functions such as
 * `cap_tu_is_double()` (to check if the given object has type `DT_DOUBLE`) or
 * `cap_tu_as_double()` (to return the stored `double` value.)
 */

#include "data_type.h"
#include "helper_functions.h"

#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

/**
 * @addtogroup typed_union
 * @{
 */

// ============================================================================
// === TYPED UNION ============================================================
// ============================================================================

/**
 * Elementary storage of values whose type is not known at compile-time.
 * 
 * For the purposes of the `cap` library, every value has a data type. That
 * type is, however, not known at compile-time (because parsers are created by
 * the user at run-time). Values are stored in a union type and marked with
 * their type using the DataType enum.
 * @see DataType.
 * 
 * The type of a TypedUnion should always be inspected using functions such as
 * `cap_tu_is_double()`, and values from them should only be extracted using
 * functions such as `cap_tu_as_double()`, and after checking the type.
 * @see cap_tu_is_double
 * @see cap_tu_is_int
 * @see cap_tu_is_presence
 * @see cap_tu_is_string
 * @see cap_tu_as_double
 * @see cap_tu_as_int
 * @see cap_tu_as_string
 * 
 * `TypedUnion` objects should always be created using factory functions such
 * as `cap_tu_make_double()`.
 * @see cap_tu_make_double
 * @see cap_tu_make_int
 * @see cap_tu_make_presence
 * @see cap_tu_make_string
 */
typedef struct {
    /// type of the stored value
    DataType mType;
    union {
        /// stores the value for DT_INT type
        int asInt;
        /// stores the value for DT_DOUBLE type
        double asDouble;
        /// stores the value for DT_STRING type
        char * asString;
    } mValue;
} TypedUnion;

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

/**
 * @}
 */

#endif
