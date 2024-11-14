#ifndef __DATA_TYPE_H__
#define __DATA_TYPE_H__

/** @file */

// ============================================================================
// === DATA TYPE IDENTIFICATION ===============================================
// ============================================================================

/**
 * Identifies data type of parsed values
 * 
 * For the purposes of the `cap` library, every value has a data type. That
 * type is, however, not known at compile-time (because parsers are created by
 * the user at run-time). Values are stored as Typed Unions,
 * and their type is represented by DataType.
 * @see TypedUnion
 * @ingroup typed_union
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

#endif
