#include "typed_union.h"
#include "test.h"

#include <stdbool.h>
#include <string.h>

bool test_create_double() {
    const double pi = 3.14;
    TypedUnion tu = cap_tu_make_double(pi);
    if (tu.mType != DT_DOUBLE) goto fail;
    if (tu.mValue.asDouble != pi) goto fail;

    cap_tu_destroy(&tu);
    return true;
fail:
    cap_tu_destroy(&tu);
    return false;
}

bool test_create_int() {
    const int pi = 4;
    TypedUnion tu = cap_tu_make_int(pi);
    if (tu.mType != DT_INT) goto fail;
    if (tu.mValue.asInt != pi) goto fail;
    
    cap_tu_destroy(&tu);
    return true;
fail:
    cap_tu_destroy(&tu);
    return false;
}

bool test_create_presence() {
    TypedUnion tu = cap_tu_make_presence();
    if (tu.mType != DT_PRESENCE) goto fail;
    
    cap_tu_destroy(&tu);
    return true;
fail:
    cap_tu_destroy(&tu);
    return false;}

bool test_create_string() {
    const char * pi = "a very nice number!";
    TypedUnion tu = cap_tu_make_string(pi);
    if (tu.mType != DT_STRING) goto fail;
    if (strcmp(pi, tu.mValue.asString) != 0) goto fail;

    cap_tu_destroy(&tu);
    return true;
fail:
    cap_tu_destroy(&tu);
    return false;
}

bool test_type_check_double() {
    TypedUnion tu = cap_tu_make_double(100);
    if (cap_tu_is_int(&tu)) goto fail;
    if (cap_tu_is_presence(&tu)) goto fail;
    if (cap_tu_is_string(&tu)) goto fail;
    if (!cap_tu_is_double(&tu)) goto fail;

    cap_tu_destroy(&tu);
    return true;
fail:
    cap_tu_destroy(&tu);
    return false;
}

bool test_type_check_int() {
    TypedUnion tu = cap_tu_make_int(-1);
    if (cap_tu_is_double(&tu)) goto fail;
    if (cap_tu_is_presence(&tu)) goto fail;
    if (cap_tu_is_string(&tu)) goto fail;
    if (!cap_tu_is_int(&tu)) goto fail;
        
    cap_tu_destroy(&tu);
    return true;
fail:
    cap_tu_destroy(&tu);
    return false;
}

bool test_type_check_presence() {
    TypedUnion tu = cap_tu_make_presence();
    if (cap_tu_is_double(&tu)) goto fail;
    if (cap_tu_is_int(&tu)) goto fail;
    if (cap_tu_is_string(&tu)) goto fail;
    if (!cap_tu_is_presence(&tu)) goto fail;
        
    cap_tu_destroy(&tu);
    return true;
fail:
    cap_tu_destroy(&tu);
    return false;
}

bool test_type_check_string() {
    TypedUnion tu = cap_tu_make_string("hello, world");
    if (cap_tu_is_double(&tu)) goto fail;
    if (cap_tu_is_int(&tu)) goto fail;
    if (cap_tu_is_presence(&tu)) goto fail;

    if (!cap_tu_is_string(&tu)) goto fail;
    
    cap_tu_destroy(&tu);
    return true;
fail:
    cap_tu_destroy(&tu);
    return false;
}

bool test_value_conversion_double() {
    const double num = 11.11;
    TypedUnion tu = cap_tu_make_double(num);
    if (num != tu.mValue.asDouble) goto fail;

    cap_tu_destroy(&tu);
    return true;
fail:
    cap_tu_destroy(&tu);
    return false;
}

bool test_value_conversion_int() {
    const int num = 55;
    TypedUnion tu = cap_tu_make_int(num);
    if (num != tu.mValue.asInt) goto fail;
    
    cap_tu_destroy(&tu);
    return true;
fail:
    cap_tu_destroy(&tu);
    return false;
}

bool test_value_conversion_string() {
    const char * s = "abcd";
    TypedUnion tu = cap_tu_make_string(s);
    if (strcmp(s, tu.mValue.asString) != 0) goto fail;

    cap_tu_destroy(&tu);
    return true;
fail:
    cap_tu_destroy(&tu);
    return false;
}

int main() {
    bool a, b, c;
    a = TEST_GROUP(
        "tu: creation", false, false, test_create_double,
        test_create_int, test_create_presence,
        test_create_string);
    b = TEST_GROUP(
        "tu: type checking", false, false, test_type_check_double,
        test_type_check_int, test_type_check_presence,
        test_type_check_string);
    c = TEST_GROUP(
        "tu: value conversion", false, false, test_value_conversion_double,
        test_value_conversion_int, test_value_conversion_string);
    
    return a && b && c ? 0 : 1;
}
