#include "cap.h"
#include "test.h"

/**
 * Test with one req-variadic argument. This should be able to parse at least 
 * one argument.
 */
bool test_revariadic_1() {
    ArgumentParser * p = cap_parser_make_empty();
    const char * rv_name = "revariadic";
    ParsedArguments * pa = NULL;
    const char * args[1] = {"P"};
    bool failed = false;
    do {
        AddPositionalError e;
        e = cap_parser_add_positional_noexit(
            p, rv_name, DT_INT, true, true, NULL, NULL);
        if (e != APE_OK) FB(failed);
        ParsingResult res = cap_parser_parse_noexit(p, 1, args);
        pa = res.mArguments;
        if (res.mError != PER_NOT_ENOUGH_POSITIONALS) FB(failed);
    } while(false);
    if (pa) cap_pa_destroy(pa);
    cap_parser_destroy(p);
    return !failed;
}

/**
 * Test with one req-variadic argument. This should be able to parse at least 
 * one argument.
 */
bool test_revariadic_2() {
    ArgumentParser * p = cap_parser_make_empty();
    const char * rv_name = "revariadic";
    ParsedArguments * pa = NULL;
    const char * args[2] = {"P", "1"};
    bool failed = false;
    do {
        AddPositionalError e;
        e = cap_parser_add_positional_noexit(
            p, rv_name, DT_INT, true, true, NULL, NULL);
        if (e != APE_OK) FB(failed);
        ParsingResult res = cap_parser_parse_noexit(p, 2, args);
        pa = res.mArguments;
        if (res.mError != PER_NO_ERROR) FB(failed);
        if (!cap_pa_has_positional(pa, rv_name)) FB(failed);
        if (cap_pa_positional_count(pa, rv_name) != 1u) FB(failed);
        const TypedUnion * v = cap_pa_get_positional(pa, rv_name);
        if (!v) FB(failed);
        if (!cap_tu_is_int(v)) FB(failed);
        if (cap_tu_as_int(v) != 1) FB(failed);

        if (cap_pa_get_positional_i(pa, rv_name, 1u)) FB(failed);
    } while(false);
    if (pa) cap_pa_destroy(pa);
    cap_parser_destroy(p);
    return !failed;
}

/**
 * Test with one req-variadic argument. This should be able to parse at least 
 * one argument.
 */
bool test_revariadic_3() {
    ArgumentParser * p = cap_parser_make_empty();
    const char * rv_name = "revariadic";
    ParsedArguments * pa = NULL;
    const char * args[4] = {"P", "1", "2", "3"};
    bool failed = false;
    do {
        AddPositionalError e;
        e = cap_parser_add_positional_noexit(
            p, rv_name, DT_INT, true, true, NULL, NULL);
        if (e != APE_OK) FB(failed);
        ParsingResult res = cap_parser_parse_noexit(p, 4, args);
        pa = res.mArguments;
        if (res.mError != PER_NO_ERROR) FB(failed);
        if (!cap_pa_has_positional(pa, rv_name)) FB(failed);
        if (cap_pa_positional_count(pa, rv_name) != 3u) FB(failed);
        for (int i = 1; i <= 3; ++i) {
            const TypedUnion * v = cap_pa_get_positional_i(
                pa, rv_name, (size_t) i-1);
            if (!v) FB(failed);
            if (!cap_tu_is_int(v)) FB(failed);
            if (cap_tu_as_int(v) != i) FB(failed);
        }
        if (cap_pa_get_positional_i(pa, rv_name, 3u)) FB(failed);
    } while(false);
    if (pa) cap_pa_destroy(pa);
    cap_parser_destroy(p);
    return !failed;
}

/**
 * Test with two req-variadic arguments. This should fail to create.
 */
bool test_revariadic_revariadic() {
    ArgumentParser * p = cap_parser_make_empty();
    const char * rv_name = "revariadic";
    const char * rv_name_2 = "revariadic_again";
    bool failed = false;
    do {
        AddPositionalError e;
        e = cap_parser_add_positional_noexit(
            p, rv_name, DT_INT, true, true, NULL, NULL);
        if (e != APE_OK) FB(failed);
        
        e = cap_parser_add_positional_noexit(
            p, rv_name_2, DT_DOUBLE, true, true, NULL, NULL);
        if (e == APE_OK) FB(failed);
    } while(false);
    cap_parser_destroy(p);
    return !failed;
}

/**
 * Test with one req-variadic and one variadic argument. This should fail to 
 * create.
 */
bool test_revariadic_variadic() {
    ArgumentParser * p = cap_parser_make_empty();
    const char * rv_name = "revariadic";
    const char * v_name = "variadic";
    bool failed = false;
    do {
        AddPositionalError e;
        e = cap_parser_add_positional_noexit(
            p, rv_name, DT_INT, true, true, NULL, NULL);
        if (e != APE_OK) FB(failed);
        
        e = cap_parser_add_positional_noexit(
            p, v_name, DT_DOUBLE, false, true, NULL, NULL);
        if (e == APE_OK) FB(failed);
    } while(false);
    cap_parser_destroy(p);
    return !failed;
}

/**
 * Test with one variadic argument. This should be able to parse any number of
 * arguments.
 */
bool test_variadic_1() {
    ArgumentParser * p = cap_parser_make_empty();
    const char * v_name = "variadic";
    ParsedArguments * pa = NULL;
    const char * args[1] = {"P"};
    bool failed = false;
    do {
        AddPositionalError e;
        e = cap_parser_add_positional_noexit(
            p, v_name, DT_INT, false, true, NULL, NULL);
        if (e != APE_OK) FB(failed);
        ParsingResult res = cap_parser_parse_noexit(p, 1, args);
        pa = res.mArguments;
        if (res.mError != PER_NO_ERROR) FB(failed);
        if (cap_pa_has_positional(pa, v_name)) FB(failed);
        if (cap_pa_get_positional(pa, v_name)) FB(failed);
        if (cap_pa_get_positional_i(pa, v_name, 0u)) FB(failed);
    } while(false);
    if (pa) cap_pa_destroy(pa);
    cap_parser_destroy(p);
    return !failed;
}

/**
 * Test with one variadic argument. This should be able to parse any number of
 * arguments.
 */
bool test_variadic_2() {
    ArgumentParser * p = cap_parser_make_empty();
    const char * v_name = "variadic";
    ParsedArguments * pa = NULL;
    const char * args[2] = {"P", "100"};
    bool failed = false;
    do {
        AddPositionalError e;
        e = cap_parser_add_positional_noexit(
            p, v_name, DT_INT, false, true, NULL, NULL);
        if (e != APE_OK) FB(failed);
        ParsingResult res = cap_parser_parse_noexit(p, 2, args);
        pa = res.mArguments;
        if (res.mError != PER_NO_ERROR) FB(failed);
        if (!cap_pa_has_positional(pa, v_name)) FB(failed);
        const TypedUnion * v = cap_pa_get_positional(pa, v_name);
        if (!v) FB(failed);
        if (!cap_tu_is_int(v)) FB(failed);
        if (cap_tu_as_int(v) != 100) FB(failed);

        if (cap_pa_get_positional_i(pa, v_name, 1u)) FB(failed);
    } while(false);
    if (pa) cap_pa_destroy(pa);
    cap_parser_destroy(p);
    return !failed;
}

/**
 * Test with one variadic argument. This should be able to parse any number of
 * arguments.
 */
bool test_variadic_3() {
    ArgumentParser * p = cap_parser_make_empty();
    const char * v_name = "variadic";
    ParsedArguments * pa = NULL;
    const char * args[4] = {"P", "100", "23", "0"};
    bool failed = false;
    do {
        AddPositionalError e;
        e = cap_parser_add_positional_noexit(
            p, v_name, DT_INT, false, true, NULL, NULL);
        if (e != APE_OK) FB(failed);
        ParsingResult res = cap_parser_parse_noexit(p, 4, args);
        pa = res.mArguments;
        if (res.mError != PER_NO_ERROR) FB(failed);
        if (!cap_pa_has_positional(pa, v_name)) FB(failed);

        int numbers[3] = {100, 23, 0};
        for (size_t i = 0; i < 3u; ++i) {
            const TypedUnion * v = cap_pa_get_positional_i(pa, v_name, i);
            if (!v) FB(failed);
            if (!cap_tu_is_int(v)) FB(failed);
            if (cap_tu_as_int(v) != numbers[i]) FB(failed);
        }
        if (cap_pa_get_positional_i(pa, v_name, 3u)) FB(failed);
    } while(false);
    if (pa) cap_pa_destroy(pa);
    cap_parser_destroy(p);
    return !failed;
}

/**
 * Test with one variadic, and one req-variadic argument. This should fail to 
 * create.
 */
bool test_variadic_revariadic() {
    ArgumentParser * p = cap_parser_make_empty();
    const char * v_name = "variadic";
    const char * rv_name = "revariadic";
    bool failed = false;
    do {
        AddPositionalError e;
        e = cap_parser_add_positional_noexit(
            p, v_name, DT_INT, false, true, NULL, NULL);
        if (e != APE_OK) FB(failed);
        
        e = cap_parser_add_positional_noexit(
            p, rv_name, DT_DOUBLE, true, true, NULL, NULL);
        if (e == APE_OK) FB(failed);
    } while(false);
    cap_parser_destroy(p);
    return !failed;
}

/**
 * Test with two variadic arguments. This should fail to create.
 */
bool test_variadic_variadic() {
    ArgumentParser * p = cap_parser_make_empty();
    const char * v_name = "variadic";
    const char * v_name_2 = "variadic_again";
    bool failed = false;
    do {
        AddPositionalError e;
        e = cap_parser_add_positional_noexit(
            p, v_name, DT_INT, false, true, NULL, NULL);
        if (e != APE_OK) FB(failed);
        
        e = cap_parser_add_positional_noexit(
            p, v_name_2, DT_DOUBLE, false, true, NULL, NULL);
        if (e == APE_OK) FB(failed);
    } while(false);
    cap_parser_destroy(p);
    return !failed;
}

int main() {
    bool a, b;
    a = TEST_GROUP("p-revariadic", false, false, test_revariadic_1,
    test_revariadic_2, test_revariadic_3, test_revariadic_revariadic,
    test_revariadic_variadic);

    b = TEST_GROUP("p-variadic", false, false, test_variadic_1,
    test_variadic_2, test_variadic_3, test_variadic_revariadic,
    test_variadic_variadic);

    return a && b ? 0 : 1;
}
