#include "test.h"
#include "cap.h"

/**
 * Test with one required and one req-variadic argument. Parsing should fail
 * if only one argument is given.
 */
bool test_required_revariadic_1() {
    const char * r_name = "required",
               * rv_name = "re_variadic";
    ArgumentParser * p = cap_parser_make_empty();
    cap_parser_add_positional(p, r_name, DT_INT, true, false, NULL, NULL);
    cap_parser_add_positional(p, rv_name, DT_DOUBLE, true, true, NULL, NULL);
    const char * args[2] = {"prog", "20"};
    ParsedArguments * pa = NULL;
    bool failed = false;
    do {
        ParsingResult res = cap_parser_parse_noexit(p, 2, args);
        pa = res.mArguments;
        if (pa) FB(failed);
        if (res.mError != PER_NOT_ENOUGH_POSITIONALS) FB(failed);
    } while (false);
    cap_parser_destroy(p);
    if (pa) cap_pa_destroy(pa);
    return !failed;
}

/**
 * Test with one required and one req-variadic argument. This should be able to
 * parse with two arguments.
 */
bool test_required_revariadic_2() {
    const char * r_name = "required",
               * rv_name = "re_variadic";
    ArgumentParser * p = cap_parser_make_empty();
    cap_parser_add_positional(p, r_name, DT_INT, true, false, NULL, NULL);
    cap_parser_add_positional(p, rv_name, DT_DOUBLE, true, true, NULL, NULL);
    const char * args[3] = {"prog", "20", "11.1"};
    ParsedArguments * pa = NULL;
    bool failed = false;
    do {
        ParsingResult res = cap_parser_parse_noexit(p, 3, args);
        pa = res.mArguments;
        if (res.mError != PER_NO_ERROR) FB(failed);
        
        if (!cap_pa_has_positional(pa, r_name)) FB(failed);
        if (cap_pa_positional_count(pa, r_name) != 1u) FB(failed);
        const TypedUnion * r_value = cap_pa_get_positional(pa, r_name);
        if (!r_value) FB(failed);
        if (r_value != cap_pa_get_positional_i(pa, r_name, 0u)) FB(failed);
        if (cap_pa_get_positional_i(pa, r_name, 1u)) FB(failed);
        if (!cap_tu_is_int(r_value)) FB(failed);
        if (cap_tu_as_int(r_value) != 20) FB(failed)

        if (!cap_pa_has_positional(pa, rv_name)) FB(failed);
        if (cap_pa_positional_count(pa, rv_name) != 1u) FB(failed);
        const TypedUnion * rv_value = cap_pa_get_positional(pa, rv_name);
        if (rv_value != cap_pa_get_positional_i(pa, rv_name, 0u)) FB(failed);
        if (!cap_tu_is_double(rv_value)) FB(failed);
        if (cap_tu_as_double(rv_value) != 11.1) FB(failed);
        if (cap_pa_get_positional_i(pa, rv_name, 1u)) FB(failed);
    } while (false);
    cap_parser_destroy(p);
    if (pa) cap_pa_destroy(pa);
    return !failed;
}

/**
 * Test with one required and one req-variadic argument. This should be able to
 * parse with more than two arguments.
 */
bool test_required_revariadic_3() {
    const char * r_name = "required",
               * rv_name = "re_variadic";
    ArgumentParser * p = cap_parser_make_empty();
    cap_parser_add_positional(p, r_name, DT_INT, true, false, NULL, NULL);
    cap_parser_add_positional(p, rv_name, DT_DOUBLE, true, true, NULL, NULL);
    const char * args[5] = {"prog", "20", "11.1", "22.2", "33.3"};
    const double numbers[3] = {11.1, 22.2, 33.3};
    ParsedArguments * pa = NULL;
    bool failed = false;
    do {
        ParsingResult res = cap_parser_parse_noexit(p, 5, args);
        pa = res.mArguments;
        if (res.mError != PER_NO_ERROR) FB(failed);
        
        if (!cap_pa_has_positional(pa, r_name)) FB(failed);
        if (cap_pa_positional_count(pa, r_name) != 1u) FB(failed);
        const TypedUnion * r_value = cap_pa_get_positional(pa, r_name);
        if (!r_value) FB(failed);
        if (r_value != cap_pa_get_positional_i(pa, r_name, 0u)) FB(failed);
        if (cap_pa_get_positional_i(pa, r_name, 1u)) FB(failed);
        if (!cap_tu_is_int(r_value)) FB(failed);
        if (cap_tu_as_int(r_value) != 20) FB(failed)

        if (!cap_pa_has_positional(pa, rv_name)) FB(failed);
        size_t count = cap_pa_positional_count(pa, rv_name);
        if (count != 3u) FB(failed);
        size_t i = 0u, stop = 5u;
        for ( ; i < stop; ++i) {
            const TypedUnion * rv_value = cap_pa_get_positional_i(pa, rv_name, i);
            if (i >= count) {
                if (rv_value) FB(failed);
                continue;
            }
            if (!rv_value) FB(failed); 
            if (!cap_tu_is_double(rv_value)) break;
            if (cap_tu_as_double(rv_value) != numbers[i]) break;
        }
        if (i < stop) FB(failed);

        if (cap_pa_get_positional(pa, rv_name) != cap_pa_get_positional_i(pa, rv_name, 0u)) FB(failed);
        if (cap_pa_get_positional_i(pa, rv_name, 3u)) FB(failed);
    } while (false);
    cap_parser_destroy(p);
    if (pa) cap_pa_destroy(pa);
    return !failed;
}

/**
 * Test with one required and two req-variadic arguments. This should fail to 
 * create.
 */
bool test_required_revariadic_revariadic() {
    ArgumentParser * p = cap_parser_make_empty();
    bool failed = false;
    do {
        AddPositionalError e;
        e = cap_parser_add_positional_noexit(p, "required", DT_STRING, true, false, NULL, NULL);
        if (e != APE_OK) FB(failed);

        e = cap_parser_add_positional_noexit(p, "revariadic", DT_STRING, true, true, NULL, NULL);
        if (e != APE_OK) FB(failed);
        
        e = cap_parser_add_positional_noexit(p, "revariadic_2", DT_STRING, true, true, NULL, NULL);
        if (e != APE_ANYTHING_AFTER_VARIADIC) FB(failed);
    } while(false);
    cap_parser_destroy(p);
    return !failed;
}

/**
 * Test with one required, one req-variadic, and one variadic argument. This 
 * should fail to create.
 */
bool test_required_revariadic_variadic() {
    ArgumentParser * p = cap_parser_make_empty();
    bool failed = false;
    do {
        AddPositionalError e;
        e = cap_parser_add_positional_noexit(p, "required", DT_STRING, true, false, NULL, NULL);
        if (e != APE_OK) FB(failed);

        e = cap_parser_add_positional_noexit(p, "revariadic", DT_STRING, true, true, NULL, NULL);
        if (e != APE_OK) FB(failed);
        
        e = cap_parser_add_positional_noexit(p, "variadic", DT_STRING, false, true, NULL, NULL);
        if (e != APE_ANYTHING_AFTER_VARIADIC) FB(failed);
    } while(false);
    cap_parser_destroy(p);
    return !failed;
}

/**
 * Test with one required, one req-variadic, and one required argument. This 
 * should fail to create.
 */
bool test_required_revariadic_required() {
    ArgumentParser * p = cap_parser_make_empty();
    bool failed = false;
    do {
        AddPositionalError e;
        e = cap_parser_add_positional_noexit(p, "required", DT_STRING, true, false, NULL, NULL);
        if (e != APE_OK) FB(failed);

        e = cap_parser_add_positional_noexit(p, "revariadic", DT_STRING, true, true, NULL, NULL);
        if (e != APE_OK) FB(failed);
        
        e = cap_parser_add_positional_noexit(p, "required_2", DT_STRING, true, false, NULL, NULL);
        if (e != APE_ANYTHING_AFTER_VARIADIC) FB(failed);
    } while(false);
    cap_parser_destroy(p);
    return !failed;
}

/**
 * Test with one required and one variadic argument. This should be able to 
 * parse one argument.
 */
bool test_required_variadic_1() {
    const char * r_name = "required",
               * v_name = "variadic";
    ArgumentParser * p = cap_parser_make_empty();
    cap_parser_add_positional(p, r_name, DT_INT, true, false, NULL, NULL);
    cap_parser_add_positional(p, v_name, DT_DOUBLE, false, true, NULL, NULL);
    const char * args[2] = {"prog", "20"};
    ParsedArguments * pa = NULL;
    bool failed = false;
    do {
        ParsingResult res = cap_parser_parse_noexit(p, 2, args);
        pa = res.mArguments;
        if (res.mError != PER_NO_ERROR) FB(failed);
        
        if (!cap_pa_has_positional(pa, r_name)) FB(failed);
        if (cap_pa_positional_count(pa, r_name) != 1u) FB(failed);
        const TypedUnion * r_value = cap_pa_get_positional(pa, r_name);
        if (!r_value) FB(failed);
        if (r_value != cap_pa_get_positional_i(pa, r_name, 0u)) FB(failed);
        if (cap_pa_get_positional_i(pa, r_name, 1u)) FB(failed);
        if (!cap_tu_is_int(r_value)) FB(failed);
        if (cap_tu_as_int(r_value) != 20) FB(failed)

        if (cap_pa_has_positional(pa, v_name)) FB(failed);
        if (cap_pa_positional_count(pa, v_name) != 0u) FB(failed);
        size_t i = 0u, limit = 3u;
        for ( ; i < limit; ++i) {
            if (cap_pa_get_positional_i(pa, v_name, i)) break;
        }
        if (i < limit) FB(failed);
    } while (false);
    cap_parser_destroy(p);
    if (pa) cap_pa_destroy(pa);
    return !failed;
}

/**
 * Test with one required and one variadic argument. This should be able to 
 * parse two arguments.
 */
bool test_required_variadic_2() {
    const char * r_name = "required",
               * v_name = "variadic";
    ArgumentParser * p = cap_parser_make_empty();
    cap_parser_add_positional(p, r_name, DT_INT, true, false, NULL, NULL);
    cap_parser_add_positional(p, v_name, DT_DOUBLE, false, true, NULL, NULL);
    const char * args[3] = {"prog", "20", "6.5"};
    const double numbers[1] = {6.5};
    ParsedArguments * pa = NULL;
    bool failed = false;
    do {
        ParsingResult res = cap_parser_parse_noexit(p, 3, args);
        pa = res.mArguments;
        if (res.mError != PER_NO_ERROR) FB(failed);
        
        if (!cap_pa_has_positional(pa, r_name)) FB(failed);
        if (cap_pa_positional_count(pa, r_name) != 1u) FB(failed);
        const TypedUnion * r_value = cap_pa_get_positional(pa, r_name);
        if (!r_value) FB(failed);
        if (r_value != cap_pa_get_positional_i(pa, r_name, 0u)) FB(failed);
        if (cap_pa_get_positional_i(pa, r_name, 1u)) FB(failed);
        if (!cap_tu_is_int(r_value)) FB(failed);
        if (cap_tu_as_int(r_value) != 20) FB(failed)

        if (!cap_pa_has_positional(pa, v_name)) FB(failed);
        size_t count = cap_pa_positional_count(pa, v_name);
        if (count != 1u) FB(failed);
        size_t i = 0u, limit = 5u;
        for ( ; i < limit; ++i) {
            const TypedUnion * v_value = cap_pa_get_positional_i(pa, v_name, i);
            if (i >= count) {
                if (v_value) FB(failed);
                continue;
            }
            if (!v_value) FB(failed);
            if (!cap_tu_is_double(v_value)) FB(failed);
            if (cap_tu_as_double(v_value) != numbers[i]) FB(failed);
        }
        if (i < limit) FB(failed);
    } while (false);
    cap_parser_destroy(p);
    if (pa) cap_pa_destroy(pa);
    return !failed;
}

/**
 * Test with one required and one variadic argument. This should be able to 
 * parse more than two arguments.
 */
bool test_required_variadic_3() {
    const char * r_name = "required",
               * v_name = "variadic";
    ArgumentParser * p = cap_parser_make_empty();
    cap_parser_add_positional(p, r_name, DT_INT, true, false, NULL, NULL);
    cap_parser_add_positional(p, v_name, DT_DOUBLE, false, true, NULL, NULL);
    const char * args[5] = {"prog", "20", "6.5", "21.1", "1.0"};
    const double numbers[3] = {6.5, 21.1, 1.0};
    ParsedArguments * pa = NULL;
    bool failed = false;
    do {
        ParsingResult res = cap_parser_parse_noexit(p, 5, args);
        pa = res.mArguments;
        if (res.mError != PER_NO_ERROR) FB(failed);
        
        if (!cap_pa_has_positional(pa, r_name)) FB(failed);
        if (cap_pa_positional_count(pa, r_name) != 1u) FB(failed);
        const TypedUnion * r_value = cap_pa_get_positional(pa, r_name);
        if (!r_value) FB(failed);
        if (r_value != cap_pa_get_positional_i(pa, r_name, 0u)) FB(failed);
        if (cap_pa_get_positional_i(pa, r_name, 1u)) FB(failed);
        if (!cap_tu_is_int(r_value)) FB(failed);
        if (cap_tu_as_int(r_value) != 20) FB(failed)

        if (!cap_pa_has_positional(pa, v_name)) FB(failed);
        size_t count = cap_pa_positional_count(pa, v_name);
        if (count != 3u) FB(failed);
        size_t i = 0u, limit = 5u;
        for ( ; i < limit; ++i) {
            const TypedUnion * v_value = cap_pa_get_positional_i(pa, v_name, i);
            if (i >= count) {
                if (v_value) FB(failed);
                continue;
            }
            if (!v_value) FB(failed);
            if (!cap_tu_is_double(v_value)) FB(failed);
            if (cap_tu_as_double(v_value) != numbers[i]) FB(failed);
        }
        if (i < limit) FB(failed);
    } while (false);
    cap_parser_destroy(p);
    if (pa) cap_pa_destroy(pa);
    return !failed;
}

/**
 * Test with one required and two variadic arguments. This should fail to 
 * create.
 */
bool test_required_variadic_variadic() {
    ArgumentParser * p = cap_parser_make_empty();
    bool failed = false;
    do {
        AddPositionalError e;
        e = cap_parser_add_positional_noexit(p, "required", DT_STRING, true, false, NULL, NULL);
        if (e != APE_OK) FB(failed);

        e = cap_parser_add_positional_noexit(p, "variadic", DT_STRING, false, true, NULL, NULL);
        if (e != APE_OK) FB(failed);
        
        e = cap_parser_add_positional_noexit(p, "variadic_2", DT_STRING, false, true, NULL, NULL);
        if (e != APE_ANYTHING_AFTER_VARIADIC) FB(failed);
    } while(false);
    cap_parser_destroy(p);
    return !failed;
}

/**
 * Test with one required, one variadic, and one required argument. This 
 * should fail to create.
 */
bool test_required_variadic_required() {
    ArgumentParser * p = cap_parser_make_empty();
    bool failed = false;
    do {
        AddPositionalError e;
        e = cap_parser_add_positional_noexit(p, "required", DT_STRING, true, false, NULL, NULL);
        if (e != APE_OK) FB(failed);

        e = cap_parser_add_positional_noexit(p, "variadic", DT_STRING, false, true, NULL, NULL);
        if (e != APE_OK) FB(failed);
        
        e = cap_parser_add_positional_noexit(p, "required_2", DT_STRING, true, false, NULL, NULL);
        if (e != APE_ANYTHING_AFTER_VARIADIC && e != APE_REQUIRED_AFTER_OPTIONAL) FB(failed);
    } while(false);
    cap_parser_destroy(p);
    return !failed;
}

/**
 * Test with one required, one variadic, and one req-variadic argument. This 
 * should fail to create.
 */
bool test_required_variadic_revariadic() {
    ArgumentParser * p = cap_parser_make_empty();
    bool failed = false;
    do {
        AddPositionalError e;
        e = cap_parser_add_positional_noexit(p, "required", DT_STRING, true, false, NULL, NULL);
        if (e != APE_OK) FB(failed);

        e = cap_parser_add_positional_noexit(p, "variadic", DT_STRING, false, true, NULL, NULL);
        if (e != APE_OK) FB(failed);
        
        e = cap_parser_add_positional_noexit(p, "revariadic", DT_STRING, true, true, NULL, NULL);
        if (e != APE_ANYTHING_AFTER_VARIADIC && e != APE_REQUIRED_AFTER_OPTIONAL) FB(failed);
    } while(false);
    cap_parser_destroy(p);
    return !failed;
}


/**
 * Test with one req-variadic, and one required argument. This should fail to 
 * create.
 */
bool test_revariadic_required() {
    ArgumentParser * p = cap_parser_make_empty();
    bool failed = false;
    do {
        AddPositionalError e;
        e = cap_parser_add_positional_noexit(p, "revariadic", DT_STRING, true, true, NULL, NULL);
        if (e != APE_OK) FB(failed);

        e = cap_parser_add_positional_noexit(p, "required", DT_STRING, true, false, NULL, NULL);
        if (e != APE_ANYTHING_AFTER_VARIADIC) FB(failed);
    } while(false);
    cap_parser_destroy(p);
    return !failed;
}

/**
 * Test with one variadic, and one required argument. This should fail to 
 * create.
 */
bool test_variadic_required() {
    ArgumentParser * p = cap_parser_make_empty();
    bool failed = false;
    do {
        AddPositionalError e;
        e = cap_parser_add_positional_noexit(p, "variadic", DT_STRING, false, true, NULL, NULL);
        if (e != APE_OK) FB(failed);

        e = cap_parser_add_positional_noexit(p, "required", DT_STRING, true, false, NULL, NULL);
        if (e != APE_ANYTHING_AFTER_VARIADIC && e != APE_REQUIRED_AFTER_OPTIONAL) FB(failed);
    } while(false);
    cap_parser_destroy(p);
    return !failed;
}

int main() {
    bool a, b, c;
    a = TEST_GROUP(
        "p-req+revariadic", false, false, test_required_revariadic_1,
        test_required_revariadic_2, test_required_revariadic_3,
        test_required_revariadic_required, test_required_revariadic_revariadic,
        test_required_revariadic_variadic);
    b = TEST_GROUP(
        "p-req+variadic", false, false, test_required_variadic_1,
        test_required_variadic_2, test_required_variadic_3,
        test_required_variadic_required, test_required_variadic_revariadic, 
        test_required_revariadic_variadic);
    c = TEST_GROUP(
        "p-[re]variadic+required", false, false, test_revariadic_required,
        test_variadic_required);
    return a && b && c ? 0 : 1;
}
