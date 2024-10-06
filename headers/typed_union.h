#ifndef __TYPED_UNION_H__
#define __TYPED_UNION_H__

#include <stdbool.h>

// ============================================================================
// === TYPED UNION: TYPE DEFINITIONS ==========================================
// ============================================================================

/**
 * Identifies data type of parsed values
 * 
 * For the purposes of the `cap` library, every value has a data type. That
 * type is, however, not known at compile-time (because parsers are created by
 * the user at run-time). Values are stored as Typed Unions,
 * and their type is represented by DataType.
 * @see TypedUnion
 */
typedef enum {
    /// integer value, corresponds to the `int` type
    DT_INT,
    /// real value, corresponds to the `double` type
    DT_DOUBLE,
    /// character string value, corresponds to a null-terminated string
    DT_STRING,
    /// used for flags that do not store any information other than their 
    /// presence or absence
    DT_PRESENCE  
} DataType;

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
TypedUnion cap_tu_make_double(double value);

/**
 * Create a new `TypedUnion` of type `int`
 */
TypedUnion cap_tu_make_int(int value);

/**
 * Create a new `TypedUnion` of type `presence`
 * 
 * The presence type is used for flags that do not store any explicit value.
 */
TypedUnion cap_tu_make_presence();

/**
 * Create a new `TypedUnion` of type `string`
 * 
 * `TypedUnion`s created by this factory contain a dynamically allocated copy
 * of the string used to create them. The owner of this object should dispose
 * of it using the function `cap_tu_destroy`. `TypedUnion` objects are usually 
 * owned by a `ParsedArguments` object so users of the `cap` library do not 
 * need to call `cap_du_destroy` directly.
 */
TypedUnion cap_tu_make_string(const char * value);

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
void cap_tu_destroy(TypedUnion * tu);

// ============================================================================
// === TYPED UNION CHECKS =====================================================
// ============================================================================

/**
 * Checks if `tu` has type `DT_DOUBLE`.
 */
bool cap_tu_is_double(const TypedUnion * tu);

/**
 * Checks if `tu` has type `DT_INT`.
 */
bool cap_tu_is_int(const TypedUnion * tu);

/**
 * Checks if `tu` has type `DT_PRESENCE`.
 */
bool cap_tu_is_presence(const TypedUnion * tu);

/**
 * Checks if `tu` has type `DT_STRING`.
 */
bool cap_tu_is_string(const TypedUnion * tu);

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
double cap_tu_as_double(const TypedUnion * tu);

/**
 * Retrieves an `int` value.
 * 
 * @param tu typed union to take the value from. It must be of type `DT_INT`.
 *        The type is checked using an `assert` statement.
 * @return `int` value stored int `tu`
 */
int cap_tu_as_int(const TypedUnion * tu);

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
const char * cap_tu_as_string(const TypedUnion * tu);

/**
 * Get an string representation of type.
 *
 * Returns a null-terminated string representing the given data type. If 
 * DT_PRESENCE is given, returns NULL instead. This should be used in help and
 * usage messages.
 *
 * @param type data type to display
 * @return string representation of the type, or NULL if type is  DT_PRESENCE 
 */
const char * cap_type_metavar(DataType type);

#endif
