#include "cap.h"

#include "test.h"

// static bool _compare_typed_union(const TypedUnion * a, const TypedUnion * b) {
//     if (!a && !b) {
//         return true;
//     }
//     if (!a || !b) {
//         return false;
//     }
//     if (a -> mType != b -> mType) {
//         return false;
//     }
//     bool result = false;
//     switch (a -> mType) {
//         case DT_PRESENCE:
//             result = true;
//             break;
//         case DT_STRING:
//             result = (strcmp(cap_tu_as_string(a), cap_tu_as_string(b)) == 0);
//             break;
//         case DT_DOUBLE:
//             result =  (cap_tu_as_double(a) == cap_tu_as_double(b));
//             break;
//         case DT_INT:
//             result =  (cap_tu_as_int(a) == cap_tu_as_int(b));
//             break;
//         default:
//             assert(false && "unreachable");
//     }
//     return result;
// }

bool test_empty() {
    ParsedArguments * pa = cap_pa_make_empty();

    const char * names[6] = {"a", "b", "c", "anarchy", "biscuits", "coffee"};
    for (int i = 0; i < 6; ++i) {
        const char * f = names[i];
        if (cap_pa_has_positional(pa, f)) goto fail;
        if (cap_pa_get_positional(pa, f)) goto fail;
    }

    cap_pa_destroy(pa);
    return true;

fail:
    cap_pa_destroy(pa);
    return false;
}

bool test_one_argument() {
    ParsedArguments * pa = cap_pa_make_empty();

    const char * name = "something";
    const int some_value = -1;

    if (cap_pa_has_positional(pa, name)) goto fail;
    if (cap_pa_get_positional(pa, name)) goto fail;

    cap_pa_set_positional(pa, name, cap_tu_make_int(some_value));
    if (!cap_pa_has_positional(pa, name)) goto fail;
    if (!cap_pa_get_positional(pa, name)) goto fail;
    
    const TypedUnion * tu = cap_pa_get_positional(pa, name);
    if (!cap_tu_is_int(tu)) goto fail;
    if (cap_tu_as_int(tu) != some_value) goto fail;

    cap_pa_destroy(pa);
    return true;

fail:
    cap_pa_destroy(pa);
    return false;
}

bool test_one_argument_set() {
    ParsedArguments * pa = cap_pa_make_empty();

    const char * name = "something";
    const int some_value = -1;
    const char * another_value = "nice!";

    // check that it is empty at first

    if (cap_pa_has_positional(pa, name)) goto fail;
    if (cap_pa_get_positional(pa, name)) goto fail;

    // check that an argument was created
    cap_pa_set_positional(pa, name, cap_tu_make_int(some_value));
    if (!cap_pa_has_positional(pa, name)) goto fail;
    if (!cap_pa_get_positional(pa, name)) goto fail;

    // check the value of that argument
    const TypedUnion * tu = cap_pa_get_positional(pa, name);
    if (!cap_tu_is_int(tu)) goto fail;
    if (cap_tu_as_int(tu) != some_value) goto fail;

    // overwrite the argument with a new value
    cap_pa_set_positional(pa, name, cap_tu_make_string(another_value));
    if (!cap_pa_has_positional(pa, name)) goto fail;
    if (!cap_pa_get_positional(pa, name)) goto fail;

    tu = cap_pa_get_positional(pa, name);
    if (!cap_tu_is_string(tu)) goto fail;
    if (strcmp(cap_tu_as_string(tu), another_value)) goto fail;

    cap_pa_destroy(pa);
    return true;

fail:
    cap_pa_destroy(pa);
    return false;
}

bool test_multiple_arguments() {
    ParsedArguments * pa = cap_pa_make_empty();

    const char * name_a = "a";
    const char * name_b = "b";
    const char * name_c = "c";
    const char * name_d = "d";
    const char * name_e = "e";

    // initialize positionals and check they are there

    cap_pa_set_positional(pa, name_a, cap_tu_make_int(1));
    cap_pa_set_positional(pa, name_b, cap_tu_make_int(2));
    cap_pa_set_positional(pa, name_c, cap_tu_make_int(3));
    cap_pa_set_positional(pa, name_d, cap_tu_make_double(4));
    cap_pa_set_positional(pa, name_e, cap_tu_make_double(5));
    
    if (!cap_pa_has_positional(pa, name_a)) goto fail;
    if (!cap_pa_get_positional(pa, name_a)) goto fail;
    if (!cap_tu_is_int(cap_pa_get_positional(pa, name_a))) goto fail;
    if (cap_tu_as_int(cap_pa_get_positional(pa, name_a)) != 1) goto fail;
    if (!cap_pa_has_positional(pa, name_b)) goto fail;
    if (!cap_pa_get_positional(pa, name_b)) goto fail;
    if (!cap_tu_is_int(cap_pa_get_positional(pa, name_b))) goto fail;
    if (cap_tu_as_int(cap_pa_get_positional(pa, name_b)) != 2) goto fail;
    if (!cap_pa_has_positional(pa, name_c)) goto fail;
    if (!cap_pa_get_positional(pa, name_c)) goto fail;
    if (!cap_tu_is_int(cap_pa_get_positional(pa, name_c))) goto fail;
    if (cap_tu_as_int(cap_pa_get_positional(pa, name_c)) != 3) goto fail;
    if (!cap_pa_has_positional(pa, name_d)) goto fail;
    if (!cap_pa_get_positional(pa, name_d)) goto fail;
    if (!cap_tu_is_double(cap_pa_get_positional(pa, name_d))) goto fail;
    if (cap_tu_as_double(cap_pa_get_positional(pa, name_d)) != 4.) goto fail;
    if (!cap_pa_has_positional(pa, name_e)) goto fail;
    if (!cap_pa_get_positional(pa, name_e)) goto fail;
    if (!cap_tu_is_double(cap_pa_get_positional(pa, name_e))) goto fail;
    if (cap_tu_as_double(cap_pa_get_positional(pa, name_e)) != 5.) goto fail;

    cap_pa_destroy(pa);
    return true;

fail:
    cap_pa_destroy(pa);
    return false;
}

bool test_multiple_arguments_set() {
    ParsedArguments * pa = cap_pa_make_empty();

    const char * name_a = "a";
    const char * name_b = "b";
    const char * name_c = "c";
    const char * name_d = "d";
    const char * name_e = "e";

    // initialize positionals and check they are there

    cap_pa_set_positional(pa, name_a, cap_tu_make_int(1));
    cap_pa_set_positional(pa, name_b, cap_tu_make_int(2));
    cap_pa_set_positional(pa, name_c, cap_tu_make_int(3));
    
    if (!cap_pa_has_positional(pa, name_a)) goto fail;
    if (!cap_pa_get_positional(pa, name_a)) goto fail;
    if (!cap_tu_is_int(cap_pa_get_positional(pa, name_a))) goto fail;
    if (cap_tu_as_int(cap_pa_get_positional(pa, name_a)) != 1) goto fail;
    if (!cap_pa_has_positional(pa, name_b)) goto fail;
    if (!cap_pa_get_positional(pa, name_b)) goto fail;
    if (!cap_tu_is_int(cap_pa_get_positional(pa, name_b))) goto fail;
    if (cap_tu_as_int(cap_pa_get_positional(pa, name_b)) != 2) goto fail;
    if (!cap_pa_has_positional(pa, name_c)) goto fail;
    if (!cap_pa_get_positional(pa, name_c)) goto fail;
    if (!cap_tu_is_int(cap_pa_get_positional(pa, name_c))) goto fail;
    if (cap_tu_as_int(cap_pa_get_positional(pa, name_c)) != 3) goto fail;

    // change some values

    cap_pa_set_positional(pa, name_c, cap_tu_make_double(500));
    
    if (!cap_pa_has_positional(pa, name_a)) goto fail;
    if (!cap_pa_get_positional(pa, name_a)) goto fail;
    if (!cap_tu_is_int(cap_pa_get_positional(pa, name_a))) goto fail;
    if (cap_tu_as_int(cap_pa_get_positional(pa, name_a)) != 1) goto fail;
    if (!cap_pa_has_positional(pa, name_b)) goto fail;
    if (!cap_pa_get_positional(pa, name_b)) goto fail;
    if (!cap_tu_is_int(cap_pa_get_positional(pa, name_b))) goto fail;
    if (cap_tu_as_int(cap_pa_get_positional(pa, name_b)) != 2) goto fail;
    if (!cap_pa_has_positional(pa, name_c)) goto fail;
    if (!cap_pa_get_positional(pa, name_c)) goto fail;
    if (!cap_tu_is_double(cap_pa_get_positional(pa, name_c))) goto fail;
    if (cap_tu_as_double(cap_pa_get_positional(pa, name_c)) != 500.) goto fail;

    cap_pa_set_positional(pa, name_c, cap_tu_make_double(501));

    if (!cap_pa_has_positional(pa, name_a)) goto fail;
    if (!cap_pa_get_positional(pa, name_a)) goto fail;
    if (!cap_tu_is_int(cap_pa_get_positional(pa, name_a))) goto fail;
    if (cap_tu_as_int(cap_pa_get_positional(pa, name_a)) != 1) goto fail;
    if (!cap_pa_has_positional(pa, name_b)) goto fail;
    if (!cap_pa_get_positional(pa, name_b)) goto fail;
    if (!cap_tu_is_int(cap_pa_get_positional(pa, name_b))) goto fail;
    if (cap_tu_as_int(cap_pa_get_positional(pa, name_b)) != 2) goto fail;
    if (!cap_pa_has_positional(pa, name_c)) goto fail;
    if (!cap_pa_get_positional(pa, name_c)) goto fail;
    if (!cap_tu_is_double(cap_pa_get_positional(pa, name_c))) goto fail;
    if (cap_tu_as_double(cap_pa_get_positional(pa, name_c)) != 501.) goto fail;

    cap_pa_set_positional(pa, name_b, cap_tu_make_double(501));

    if (!cap_pa_has_positional(pa, name_a)) goto fail;
    if (!cap_pa_get_positional(pa, name_a)) goto fail;
    if (!cap_tu_is_int(cap_pa_get_positional(pa, name_a))) goto fail;
    if (cap_tu_as_int(cap_pa_get_positional(pa, name_a)) != 1) goto fail;
    if (!cap_pa_has_positional(pa, name_b)) goto fail;
    if (!cap_pa_get_positional(pa, name_b)) goto fail;
    if (!cap_tu_is_double(cap_pa_get_positional(pa, name_b))) goto fail;
    if (cap_tu_as_double(cap_pa_get_positional(pa, name_b)) != 501.) goto fail;
    if (!cap_pa_has_positional(pa, name_c)) goto fail;
    if (!cap_pa_get_positional(pa, name_c)) goto fail;
    if (!cap_tu_is_double(cap_pa_get_positional(pa, name_c))) goto fail;
    if (cap_tu_as_double(cap_pa_get_positional(pa, name_c)) != 501.) goto fail;

    // set new positionals

    cap_pa_set_positional(pa, name_d, cap_tu_make_int(10));
    cap_pa_set_positional(pa, name_e, cap_tu_make_int(-10));

    if (!cap_pa_has_positional(pa, name_a)) goto fail;
    if (!cap_pa_get_positional(pa, name_a)) goto fail;
    if (!cap_tu_is_int(cap_pa_get_positional(pa, name_a))) goto fail;
    if (cap_tu_as_int(cap_pa_get_positional(pa, name_a)) != 1) goto fail;
    if (!cap_pa_has_positional(pa, name_b)) goto fail;
    if (!cap_pa_get_positional(pa, name_b)) goto fail;
    if (!cap_tu_is_double(cap_pa_get_positional(pa, name_b))) goto fail;
    if (cap_tu_as_double(cap_pa_get_positional(pa, name_b)) != 501.) goto fail;
    if (!cap_pa_has_positional(pa, name_c)) goto fail;
    if (!cap_pa_get_positional(pa, name_c)) goto fail;
    if (!cap_tu_is_double(cap_pa_get_positional(pa, name_c))) goto fail;
    if (cap_tu_as_double(cap_pa_get_positional(pa, name_c)) != 501.) goto fail;
    if (!cap_pa_has_positional(pa, name_d)) goto fail;
    if (!cap_pa_get_positional(pa, name_d)) goto fail;
    if (!cap_tu_is_int(cap_pa_get_positional(pa, name_d))) goto fail;
    if (cap_tu_as_int(cap_pa_get_positional(pa, name_d)) != 10) goto fail;
    if (!cap_pa_has_positional(pa, name_e)) goto fail;
    if (!cap_pa_get_positional(pa, name_e)) goto fail;
    if (!cap_tu_is_int(cap_pa_get_positional(pa, name_e))) goto fail;
    if (cap_tu_as_int(cap_pa_get_positional(pa, name_e)) != -10) goto fail;

    // change some values again

    const char * hw = "Hello, World!";

    cap_pa_set_positional(pa, name_b, cap_tu_make_string(hw));

    if (!cap_pa_has_positional(pa, name_a)) goto fail;
    if (!cap_pa_get_positional(pa, name_a)) goto fail;
    if (!cap_tu_is_int(cap_pa_get_positional(pa, name_a))) goto fail;
    if (cap_tu_as_int(cap_pa_get_positional(pa, name_a)) != 1) goto fail;
    if (!cap_pa_has_positional(pa, name_b)) goto fail;
    if (!cap_pa_get_positional(pa, name_b)) goto fail;
    if (!cap_tu_is_string(cap_pa_get_positional(pa, name_b))) goto fail;
    if (strcmp(cap_tu_as_string(cap_pa_get_positional(pa, name_b)), hw)) goto fail;
    if (!cap_pa_has_positional(pa, name_c)) goto fail;
    if (!cap_pa_get_positional(pa, name_c)) goto fail;
    if (!cap_tu_is_double(cap_pa_get_positional(pa, name_c))) goto fail;
    if (cap_tu_as_double(cap_pa_get_positional(pa, name_c)) != 501.) goto fail;
    if (!cap_pa_has_positional(pa, name_d)) goto fail;
    if (!cap_pa_get_positional(pa, name_d)) goto fail;
    if (!cap_tu_is_int(cap_pa_get_positional(pa, name_d))) goto fail;
    if (cap_tu_as_int(cap_pa_get_positional(pa, name_d)) != 10) goto fail;
    if (!cap_pa_has_positional(pa, name_e)) goto fail;
    if (!cap_pa_get_positional(pa, name_e)) goto fail;
    if (!cap_tu_is_int(cap_pa_get_positional(pa, name_e))) goto fail;
    if (cap_tu_as_int(cap_pa_get_positional(pa, name_e)) != -10) goto fail;

    cap_pa_set_positional(pa, name_e, cap_tu_make_string(hw));

    if (!cap_pa_has_positional(pa, name_a)) goto fail;
    if (!cap_pa_get_positional(pa, name_a)) goto fail;
    if (!cap_tu_is_int(cap_pa_get_positional(pa, name_a))) goto fail;
    if (cap_tu_as_int(cap_pa_get_positional(pa, name_a)) != 1) goto fail;
    if (!cap_pa_has_positional(pa, name_b)) goto fail;
    if (!cap_pa_get_positional(pa, name_b)) goto fail;
    if (!cap_tu_is_string(cap_pa_get_positional(pa, name_b))) goto fail;
    if (strcmp(cap_tu_as_string(cap_pa_get_positional(pa, name_b)), hw)) goto fail;
    if (!cap_pa_has_positional(pa, name_c)) goto fail;
    if (!cap_pa_get_positional(pa, name_c)) goto fail;
    if (!cap_tu_is_double(cap_pa_get_positional(pa, name_c))) goto fail;
    if (cap_tu_as_double(cap_pa_get_positional(pa, name_c)) != 501) goto fail;
    if (!cap_pa_has_positional(pa, name_d)) goto fail;
    if (!cap_pa_get_positional(pa, name_d)) goto fail;
    if (!cap_tu_is_int(cap_pa_get_positional(pa, name_d))) goto fail;
    if (cap_tu_as_int(cap_pa_get_positional(pa, name_d)) != 10) goto fail;
    if (!cap_pa_has_positional(pa, name_e)) goto fail;
    if (!cap_pa_get_positional(pa, name_e)) goto fail;
    if (!cap_tu_is_string(cap_pa_get_positional(pa, name_e))) goto fail;
    if (strcmp(cap_tu_as_string(cap_pa_get_positional(pa, name_e)), hw)) goto fail;

    cap_pa_destroy(pa);
    return true;

fail:
    cap_pa_destroy(pa);
    return false;
}

int main() {
    bool a, b, c;
    a = TEST_GROUP(
        "pa-posit.: creation", false, false, test_empty);
    b = TEST_GROUP(
        "pa-posit.: setting", false, false, test_one_argument,
        test_one_argument_set);
    c = TEST_GROUP(
        "pa-posit.: multiple arguments", false, false, test_multiple_arguments,
        test_multiple_arguments_set);
    return a && b && c ? 0 : 1;    
}
