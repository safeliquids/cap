#include "helper_functions.h"
#include "typed_union.h"

#include <assert.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

// ============================================================================
// === TYPED UNION CREATION AND DESTRUCTION ===================================
// ============================================================================

TypedUnion cap_tu_make_double(double value) {
    return (TypedUnion) { .mType = DT_DOUBLE, .mValue = { .asDouble = value } };
}

TypedUnion cap_tu_make_int(int value) {
    return (TypedUnion) { .mType = DT_INT, .mValue = { .asInt = value } };
}

TypedUnion cap_tu_make_presence() {
    return (TypedUnion) { .mType = DT_PRESENCE };
}

TypedUnion cap_tu_make_string(const char * value) {
    char * const value_copy = copy_string(value);
    return (TypedUnion) { .mType = DT_STRING, .mValue = { .asString = value_copy } };
}

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

bool cap_tu_is_double(const TypedUnion * tu) {
    return tu -> mType == DT_DOUBLE;
}

bool cap_tu_is_int(const TypedUnion * tu) {
    return tu -> mType == DT_INT;
}

bool cap_tu_is_presence(const TypedUnion * tu) {
    return tu -> mType == DT_PRESENCE;
}

bool cap_tu_is_string(const TypedUnion * tu) {
    return tu -> mType == DT_STRING;
}

// ============================================================================
// === TYPED UNION CONVERSIONS ================================================
// ============================================================================

double cap_tu_as_double(const TypedUnion * tu) {
    assert(tu -> mType == DT_DOUBLE);
    return tu -> mValue.asDouble;
}

int cap_tu_as_int(const TypedUnion * tu) {
    assert(tu -> mType == DT_INT);
    return tu -> mValue.asInt;
}

const char * cap_tu_as_string(const TypedUnion * tu) {
    assert(tu -> mType == DT_STRING);
    return tu -> mValue.asString;
}

const char * cap_type_metavar(DataType type) {
    const char * type_metavar;
    switch (type) {
        case DT_DOUBLE:
            type_metavar = "DOUBLE";
            break;
        case DT_INT:
            type_metavar = "INT";
            break;
        case DT_STRING:
            type_metavar = "STRING";
            break;
        case DT_PRESENCE:
        default:
            type_metavar = NULL;
    }
    return type_metavar;
}
