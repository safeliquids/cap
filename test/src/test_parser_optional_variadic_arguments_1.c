#include "test.h"
#include "cap.h"

/**
 * test with one optional and one req-variadic argument. This should fail 
 * to create.
 */
bool test_optional_revariadic() {
    ArgumentParser * p = cap_parser_make_empty();
    bool failed = false;
    do {
        AddPositionalError e;
        e = cap_parser_add_positional_noexit(
            p, "optional", DT_STRING, false, false, NULL, NULL);
        if (e != APE_OK) FB(failed);

        e = cap_parser_add_positional_noexit(
            p, "revariadic", DT_STRING, true, true, NULL, NULL);
        if (e != APE_REQUIRED_AFTER_OPTIONAL) FB(failed);
    } while(false);
    cap_parser_destroy(p);
    return !failed;
}

/**
 * test with one optional and one variadic argument. This should work with 
 * no arguments.
 */
bool test_optional_variadic_0() {
    ArgumentParser * p = cap_parser_make_empty();
    const char * o_name = "optional",
               * v_name = "variadic";
    cap_parser_add_positional(p, o_name, DT_STRING, false, false, NULL,  NULL);
    cap_parser_add_positional(p, v_name, DT_STRING, false, true, NULL, NULL);
    const char * args[11] = {
        "prog",
        "alpha", "beta", "gamma", "delta", "epsilon", "phi", "eta", "iota",
        "kappa", "lambda"};
    ParsedArguments * pa = NULL;
    bool failed = false;
    do {
        ParsingResult res = cap_parser_parse_noexit(p, 1, args);
        pa = res.mArguments;
        if (res.mError != PER_NO_ERROR) FB(failed);
        if (!pa) FB(failed);

        if (cap_pa_has_positional(pa, o_name)) FB(failed);
        if (cap_pa_positional_count(pa, o_name) != 0u) FB(failed);
        if (cap_pa_get_positional(pa, o_name)) FB(failed);
        if (cap_pa_get_positional_i(pa, o_name, 0u)) FB(failed);
        if (cap_pa_get_positional_i(pa, o_name, 10u)) FB(failed);

        if (cap_pa_has_positional(pa, v_name)) FB(failed);
        size_t count = cap_pa_positional_count(pa, v_name),
               i = 0u,
               limit = 14;
        if (count != 0u) FB(failed);
        for ( ; i < limit; ++i) {
            const TypedUnion * v_value = cap_pa_get_positional_i(pa, v_name, i);
            if (i >= count) {
                if(v_value) break;
                continue;
            }
            if (!v_value) break;
            if (!cap_tu_is_string(v_value)) break;
            if (strcmp(cap_tu_as_string(v_value), args[i + 2])) break;
        }
        if (i < limit) FB(failed);
    } while(false);
    cap_parser_destroy(p);
    cap_pa_destroy(pa);
    return !failed;
}

/**
 * test with one optional and one variadic argument. This should work with 
 * one argument.
 */
bool test_optional_variadic_1() {
    ArgumentParser * p = cap_parser_make_empty();
    const char * o_name = "optional",
               * v_name = "variadic";
    cap_parser_add_positional(p, o_name, DT_STRING, false, false, NULL,  NULL);
    cap_parser_add_positional(p, v_name, DT_STRING, false, true, NULL, NULL);
    const char * args[11] = {
        "prog",
        "alpha", "beta", "gamma", "delta", "epsilon", "phi", "eta", "iota",
        "kappa", "lambda"};
    ParsedArguments * pa = NULL;
    bool failed = false;
    do {
        ParsingResult res = cap_parser_parse_noexit(p, 2, args);
        pa = res.mArguments;
        if (res.mError != PER_NO_ERROR) FB(failed);
        if (!pa) FB(failed);

        if (!cap_pa_has_positional(pa, o_name)) FB(failed);
        if (cap_pa_positional_count(pa, o_name) != 1u) FB(failed);
        const TypedUnion * o_value = cap_pa_get_positional(pa, o_name);
        if (o_value != cap_pa_get_positional_i(pa, o_name, 0u)) FB(failed);
        if (!o_value) FB(failed);
        if (!cap_tu_is_string(o_value)) FB(failed);
        if (strcmp(cap_tu_as_string(o_value), args[1])) FB(failed);

        if (cap_pa_has_positional(pa, v_name)) FB(failed);
        size_t count = cap_pa_positional_count(pa, v_name),
               i = 0u,
               limit = 14;
        if (count != 0u) FB(failed);
        for ( ; i < limit; ++i) {
            const TypedUnion * v_value = cap_pa_get_positional_i(pa, v_name, i);
            if (i >= count) {
                if(v_value) break;
                continue;
            }
            if (!v_value) break;
            if (!cap_tu_is_string(v_value)) break;
            if (strcmp(cap_tu_as_string(v_value), args[i + 2])) break;
        }
        if (i < limit) FB(failed);
    } while(false);
    cap_parser_destroy(p);
    cap_pa_destroy(pa);
    return !failed;
}

/**
 * test with one optional and one variadic argument. This should work with 
 * three arguments.
 */
bool test_optional_variadic_2() {
    ArgumentParser * p = cap_parser_make_empty();
    const char * o_name = "optional",
               * v_name = "variadic";
    cap_parser_add_positional(p, o_name, DT_STRING, false, false, NULL,  NULL);
    cap_parser_add_positional(p, v_name, DT_STRING, false, true, NULL, NULL);
    const char * args[11] = {
        "prog",
        "alpha", "beta", "gamma", "delta", "epsilon", "phi", "eta", "iota",
        "kappa", "lambda"};
    ParsedArguments * pa = NULL;
    bool failed = false;
    do {
        ParsingResult res = cap_parser_parse_noexit(p, 4, args);
        pa = res.mArguments;
        if (res.mError != PER_NO_ERROR) FB(failed);
        if (!pa) FB(failed);

        if (!cap_pa_has_positional(pa, o_name)) FB(failed);
        if (cap_pa_positional_count(pa, o_name) != 1u) FB(failed);
        const TypedUnion * o_value = cap_pa_get_positional(pa, o_name);
        if (!o_value) FB(failed);
        if (o_value != cap_pa_get_positional_i(pa, o_name, 0u)) FB(failed);
        if (!cap_tu_is_string(o_value)) FB(failed);
        if (strcmp(cap_tu_as_string(o_value), args[1])) FB(failed);

        if (!cap_pa_has_positional(pa, v_name)) FB(failed);
        size_t count = cap_pa_positional_count(pa, v_name),
               i = 0u,
               limit = 14;
        if (count != 2u) FB(failed);
        for ( ; i < limit; ++i) {
            const TypedUnion * v_value = cap_pa_get_positional_i(pa, v_name, i);
            if (i >= count) {
                if(v_value) break;
                continue;
            }
            if (!v_value) break;
            if (!cap_tu_is_string(v_value)) break;
            if (strcmp(cap_tu_as_string(v_value), args[i + 2])) break;
        }
        if (i < limit) FB(failed);
    } while(false);
    cap_parser_destroy(p);
    cap_pa_destroy(pa);
    return !failed;
}

/**
 * test with one optional and one variadic argument. This should work with 
 * ten arguments.
 */
bool test_optional_variadic_3() {
    ArgumentParser * p = cap_parser_make_empty();
    const char * o_name = "optional",
               * v_name = "variadic";
    cap_parser_add_positional(p, o_name, DT_STRING, false, false, NULL,  NULL);
    cap_parser_add_positional(p, v_name, DT_STRING, false, true, NULL, NULL);
    const char * args[11] = {
        "prog",
        "alpha", "beta", "gamma", "delta", "epsilon", "phi", "eta", "iota",
        "kappa", "lambda"};
    ParsedArguments * pa = NULL;
    bool failed = false;
    do {
        ParsingResult res = cap_parser_parse_noexit(p, 11, args);
        pa = res.mArguments;
        if (res.mError != PER_NO_ERROR) FB(failed);
        if (!pa) FB(failed);

        if (!cap_pa_has_positional(pa, o_name)) FB(failed);
        if (cap_pa_positional_count(pa, o_name) != 1u) FB(failed);
        const TypedUnion * o_value = cap_pa_get_positional(pa, o_name);
        if (o_value != cap_pa_get_positional_i(pa, o_name, 0u)) FB(failed);
        if (!o_value) FB(failed);
        if (!cap_tu_is_string(o_value)) FB(failed);
        if (strcmp(cap_tu_as_string(o_value), args[1])) FB(failed);

        if (!cap_pa_has_positional(pa, v_name)) FB(failed);
        size_t count = cap_pa_positional_count(pa, v_name),
               i = 0u,
               limit = 14;
        if (count != 9u) FB(failed);
        for ( ; i < limit; ++i) {
            const TypedUnion * v_value = cap_pa_get_positional_i(pa, v_name, i);
            if (i >= count) {
                if(v_value) break;
                continue;
            }
            if (!v_value) break;
            if (!cap_tu_is_string(v_value)) break;
            if (strcmp(cap_tu_as_string(v_value), args[i + 2])) break;
        }
        if (i < limit) FB(failed);
    } while(false);
    cap_parser_destroy(p);
    cap_pa_destroy(pa);
    return !failed;
}

/**
 * test with one optional and two variadic arguments. This should fail to 
 * create.
 */
bool test_optional_variadic_variadic() {
    ArgumentParser * p = cap_parser_make_empty();
    bool failed = false;
    do {
        AddPositionalError e;
        e = cap_parser_add_positional_noexit(
            p, "optional", DT_STRING, true, false, NULL, NULL);
        if (e != APE_OK) FB(failed);

        e = cap_parser_add_positional_noexit(
            p, "variadic", DT_STRING, false, true, NULL, NULL);
        if (e != APE_OK) FB(failed);
        
        e = cap_parser_add_positional_noexit(
            p, "variadic_2", DT_STRING, false, true, NULL, NULL);
        if (e != APE_ANYTHING_AFTER_VARIADIC) FB(failed);
    } while(false);
    cap_parser_destroy(p);
    return !failed;
}

/**
 * test with one optional, one variadic and another optional argument. This 
 * should fail to create.
 */
bool test_optional_variadic_optional() {
    ArgumentParser * p = cap_parser_make_empty();
    bool failed = false;
    do {
        AddPositionalError e;
        e = cap_parser_add_positional_noexit(
            p, "optional", DT_STRING, true, false, NULL, NULL);
        if (e != APE_OK) FB(failed);

        e = cap_parser_add_positional_noexit(
            p, "variadic", DT_STRING, false, true, NULL, NULL);
        if (e != APE_OK) FB(failed);
        
        e = cap_parser_add_positional_noexit(
            p, "optional_2", DT_STRING, false, false, NULL, NULL);
        if (e != APE_ANYTHING_AFTER_VARIADIC) FB(failed);
    } while(false);
    cap_parser_destroy(p);
    return !failed;
}

/**
 * test with one optional, one variadic and one req-variadic argument. This 
 * should fail to create.
 */
bool test_optional_variadic_revariadic() {
    ArgumentParser * p = cap_parser_make_empty();
    bool failed = false;
    do {
        AddPositionalError e;
        e = cap_parser_add_positional_noexit(
            p, "optional", DT_STRING, true, false, NULL, NULL);
        if (e != APE_OK) FB(failed);

        e = cap_parser_add_positional_noexit(
            p, "variadic", DT_STRING, false, true, NULL, NULL);
        if (e != APE_OK) FB(failed);
        
        e = cap_parser_add_positional_noexit(
            p, "revariadic", DT_STRING, true, true, NULL, NULL);
        if (e != APE_ANYTHING_AFTER_VARIADIC 
                && e != APE_REQUIRED_AFTER_OPTIONAL) {
            FB(failed);
        }
    } while(false); 
    cap_parser_destroy(p);
    return !failed;
}

/**
 * Test with one req-variadic, and one optional argument. This should fail to 
 * create.
 */
bool test_revariadic_optional() {
    ArgumentParser * p = cap_parser_make_empty();
    bool failed = false;
    do {
        AddPositionalError e;
        e = cap_parser_add_positional_noexit(
            p, "revariadic", DT_STRING, true, true, NULL, NULL);
        if (e != APE_OK) FB(failed);
        
        e = cap_parser_add_positional_noexit(
            p, "opt", DT_STRING, false, false, NULL, NULL);
        if (e != APE_ANYTHING_AFTER_VARIADIC) FB(failed);
    } while(false);
    cap_parser_destroy(p);
    return !failed;
}

/**
 * Test with one variadic, and one optional argument. This should fail to 
 * create.
 */
bool test_variadic_optional() {
    ArgumentParser * p = cap_parser_make_empty();
    bool failed = false;
    do {
        AddPositionalError e;
        e = cap_parser_add_positional_noexit(
            p, "variadic", DT_STRING, false, true, NULL, NULL);
        if (e != APE_OK) FB(failed);
        
        e = cap_parser_add_positional_noexit(
            p, "opt", DT_STRING, false, false, NULL, NULL);
        if (e != APE_ANYTHING_AFTER_VARIADIC) FB(failed);
    } while(false);
    cap_parser_destroy(p);
    return !failed;
}

int main() {
    bool a, b, c;
    a = TEST_GROUP(
        "p-opt+variadic", false, false, test_optional_revariadic, 
        test_optional_variadic_0, test_optional_variadic_1, 
        test_optional_variadic_2, test_optional_variadic_3);
    b = TEST_GROUP(
        "p-opt+variadic+any", false, false, test_optional_variadic_optional, 
        test_optional_variadic_revariadic, test_optional_variadic_variadic);
    c = TEST_GROUP(
        "p-[re]variadic+opt", false, false, test_revariadic_optional, 
        test_variadic_optional);
    return a && b && c ? 0 : 1;
}
