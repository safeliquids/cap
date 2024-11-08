#include "cap.h"
#include "test.h"

/**
 * Test with one optional, one variadic and one required argument. This should 
 * fail to create.
 */
bool test_optional_variadic_required() {
    ArgumentParser * p = cap_parser_make_empty();
    bool failed = false;
    do {
        AddPositionalError e;
        e = cap_parser_add_positional_noexit(
            p, "optional", DT_STRING, false, false, NULL, NULL);
        if (e != APE_OK) FB(failed);

        e = cap_parser_add_positional_noexit(
            p, "variadic", DT_STRING, false, true, NULL, NULL);
        if (e != APE_OK) FB(failed);

        e = cap_parser_add_positional_noexit(
            p, "required", DT_STRING, true, false, NULL, NULL);
        if (e != APE_ANYTHING_AFTER_VARIADIC 
                && e != APE_REQUIRED_AFTER_OPTIONAL) FB(failed);
    } while (false);
    cap_parser_destroy(p);
    return !failed;
}

/**
 * test with one required, one optional and one req-variadic. This should fail 
 * to create.
 */
bool test_required_optional_revariadic() {
    ArgumentParser * p = cap_parser_make_empty();
    bool failed = false;
    do {
        AddPositionalError e;
        e = cap_parser_add_positional_noexit(
            p, "required", DT_STRING, true, false, NULL, NULL);
        if (e != APE_OK) FB(failed);

        e = cap_parser_add_positional_noexit(
            p, "optional", DT_STRING, false, false, NULL, NULL);
        if (e != APE_OK) FB(failed);

        e = cap_parser_add_positional_noexit(
            p, "revariadic", DT_STRING, true, true, NULL, NULL);
        if (e != APE_REQUIRED_AFTER_OPTIONAL) FB(failed);
    } while (false);
    cap_parser_destroy(p);
    return !failed;
}

/**
 * Test with one required, one optional, and one variadic argument. This should
 * be able to parse without the optional/variadic arguments.
 */
bool test_required_optional_variadic_0() {
    ArgumentParser * p = cap_parser_make_empty();
    const char * r_name = "required",
               * o_name = "optional",
               * v_name = "variadic";
    const char * word = "word";
    const char * args[2] = {"prog", word};

    cap_parser_add_positional(p, r_name, DT_STRING, true, false, NULL, NULL);
    cap_parser_add_positional(p, o_name, DT_INT, false, false, NULL, NULL);
    cap_parser_add_positional(p, v_name, DT_DOUBLE, false, true, NULL, NULL);

    ParsedArguments * pa = NULL;
    bool failed = false;
    do {
        ParsingResult res = cap_parser_parse_noexit(p, 2, args);
        pa = res.mArguments;
        if (res.mError != PER_NO_ERROR) FB(failed);
        if (!pa) FB(failed);

        // check required arg
        if (!cap_pa_has_positional(pa, r_name)) FB(failed);
        if (cap_pa_positional_count(pa, r_name) != 1u) FB(failed);
        const TypedUnion * r_value = cap_pa_get_positional(pa, r_name);
        if (!r_value) FB(failed);
        if (r_value != cap_pa_get_positional_i(pa, r_name, 0u)) FB(failed);
        if (cap_pa_get_positional_i(pa, r_name, 1u)) FB(failed);
        if (!cap_tu_is_string(r_value)) FB(failed);
        if (strcmp(cap_tu_as_string(r_value), word)) FB(failed);

        // check absence of optional arg
        if (cap_pa_has_positional(pa, o_name)) FB(failed);
        if (cap_pa_positional_count(pa, o_name) != 0u) FB(failed);
        const TypedUnion * o_value = cap_pa_get_positional(pa, o_name);
        if (o_value) FB(failed);

        //check absence of variadic value
        if (cap_pa_has_positional(pa, v_name)) FB(failed);
        if (cap_pa_positional_count(pa, v_name) != 0u) FB(failed);
        size_t i = 0u, limit = 10;
        for ( ; i < limit; ++i) {
            const TypedUnion * v_value = cap_pa_get_positional_i(pa, v_name, i);
            if (v_value) break;
        }
        if (i < limit) FB(failed);
    } while(false);
    cap_parser_destroy(p);
    if (pa) cap_pa_destroy(pa);
    return !failed;
}

/**
 * Test with one required, one optional, and one variadic argument. This should
 * be able to parse with the optional argument.
 */
bool test_required_optional_variadic_1() {
    ArgumentParser * p = cap_parser_make_empty();
    const char * r_name = "required",
               * o_name = "optional",
               * v_name = "variadic";
    const char * word = "word";
    const char * args[3] = {"prog", word, "10"};

    cap_parser_add_positional(p, r_name, DT_STRING, true, false, NULL, NULL);
    cap_parser_add_positional(p, o_name, DT_INT, false, false, NULL, NULL);
    cap_parser_add_positional(p, v_name, DT_DOUBLE, false, true, NULL, NULL);

    ParsedArguments * pa = NULL;
    bool failed = false;
    do {
        ParsingResult res = cap_parser_parse_noexit(p, 3, args);
        pa = res.mArguments;
        if (res.mError != PER_NO_ERROR) FB(failed);
        if (!pa) FB(failed);

        // check required arg
        if (!cap_pa_has_positional(pa, r_name)) FB(failed);
        if (cap_pa_positional_count(pa, r_name) != 1u) FB(failed);
        const TypedUnion * r_value = cap_pa_get_positional(pa, r_name);
        if (!r_value) FB(failed);
        if (r_value != cap_pa_get_positional_i(pa, r_name, 0u)) FB(failed);
        if (cap_pa_get_positional_i(pa, r_name, 1u)) FB(failed);
        if (!cap_tu_is_string(r_value)) FB(failed);
        if (strcmp(cap_tu_as_string(r_value), word)) FB(failed);

        // check optional arg
        if (!cap_pa_has_positional(pa, o_name)) FB(failed);
        if (cap_pa_positional_count(pa, o_name) != 1u) FB(failed);
        const TypedUnion * o_value = cap_pa_get_positional(pa, o_name);
        if (!o_value) FB(failed);
        if (o_value != cap_pa_get_positional_i(pa, o_name, 0u)) FB(failed);
        if (cap_pa_get_positional_i(pa, o_name, 1u)) FB(failed);
        if (!cap_tu_is_int(o_value)) FB(failed);
        if (cap_tu_as_int(o_value) != 10) FB(failed);

        //check absence of variadic value
        if (cap_pa_has_positional(pa, v_name)) FB(failed);
        if (cap_pa_positional_count(pa, v_name) != 0u) FB(failed);
        size_t i = 0u, limit = 10;
        for ( ; i < limit; ++i) {
            const TypedUnion * v_value = cap_pa_get_positional_i(pa, v_name, i);
            if (v_value) break;
        }
        if (i < limit) FB(failed);
    } while(false);
    cap_parser_destroy(p);
    if (pa) cap_pa_destroy(pa);
    return !failed;
}

/**
 * Test with one required, one optional, and one variadic argument. This should
 * be able to parse with the optional argument and one value for the variadic.
 */
bool test_required_optional_variadic_2() {
    ArgumentParser * p = cap_parser_make_empty();
    const char * r_name = "required",
               * o_name = "optional",
               * v_name = "variadic";
    const char * word = "word";
    const char * args[4] = {"prog", word, "10", "11"};

    cap_parser_add_positional(p, r_name, DT_STRING, true, false, NULL, NULL);
    cap_parser_add_positional(p, o_name, DT_INT, false, false, NULL, NULL);
    cap_parser_add_positional(p, v_name, DT_DOUBLE, false, true, NULL, NULL);

    ParsedArguments * pa = NULL;
    bool failed = false;
    do {
        ParsingResult res = cap_parser_parse_noexit(p, 4, args);
        pa = res.mArguments;
        if (res.mError != PER_NO_ERROR) FB(failed);
        if (!pa) FB(failed);

        // check required arg
        if (!cap_pa_has_positional(pa, r_name)) FB(failed);
        if (cap_pa_positional_count(pa, r_name) != 1u) FB(failed);
        const TypedUnion * r_value = cap_pa_get_positional(pa, r_name);
        if (!r_value) FB(failed);
        if (r_value != cap_pa_get_positional_i(pa, r_name, 0u)) FB(failed);
        if (cap_pa_get_positional_i(pa, r_name, 1u)) FB(failed);
        if (!cap_tu_is_string(r_value)) FB(failed);
        if (strcmp(cap_tu_as_string(r_value), word)) FB(failed);

        // check optional arg
        if (!cap_pa_has_positional(pa, o_name)) FB(failed);
        if (cap_pa_positional_count(pa, o_name) != 1u) FB(failed);
        const TypedUnion * o_value = cap_pa_get_positional(pa, o_name);
        if (!o_value) FB(failed);
        if (o_value != cap_pa_get_positional_i(pa, o_name, 0u)) FB(failed);
        if (cap_pa_get_positional_i(pa, o_name, 1u)) FB(failed);
        if (!cap_tu_is_int(o_value)) FB(failed);
        if (cap_tu_as_int(o_value) != 10) FB(failed);

        //check variadic value
        if (!cap_pa_has_positional(pa, v_name)) FB(failed);
        if (cap_pa_positional_count(pa, v_name) != 1u) FB(failed);
        const TypedUnion * v_value = cap_pa_get_positional(pa, v_name);
        if (!v_value) FB(failed);
        if (v_value != cap_pa_get_positional_i(pa, v_name, 0u)) FB(failed);
        if (!cap_tu_is_double(v_value)) FB(failed);
        if (cap_tu_as_double(v_value) != 11.0) FB(failed);
        size_t i = 1, limit = 10;
        for ( ; i < limit; ++i) {
            const TypedUnion * v_value = cap_pa_get_positional_i(pa, v_name, i);
            if (v_value) break;
        }
        if (i < limit) FB(failed);
    } while(false);
    cap_parser_destroy(p);
    if (pa) cap_pa_destroy(pa);
    return !failed;
}

/**
 * Test with one required, one optional, and one variadic argument. This should
 * be able to parse with the optional argument and several vales for the
 * variadic argument.
 */
bool test_required_optional_variadic_3() {
    ArgumentParser * p = cap_parser_make_empty();
    const char * r_name = "required",
               * o_name = "optional",
               * v_name = "variadic";
    const char * word = "word";
    const double numbers[4] = {11, 255, 31, 0};
    const char * args[7] = {"prog", word, "10", "11", "255", "31", "0"};

    cap_parser_add_positional(p, r_name, DT_STRING, true, false, NULL, NULL);
    cap_parser_add_positional(p, o_name, DT_INT, false, false, NULL, NULL);
    cap_parser_add_positional(p, v_name, DT_DOUBLE, false, true, NULL, NULL);

    ParsedArguments * pa = NULL;
    bool failed = false;
    do {
        ParsingResult res = cap_parser_parse_noexit(p, 7, args);
        pa = res.mArguments;
        if (res.mError != PER_NO_ERROR) FB(failed);
        if (!pa) FB(failed);

        // check required arg
        if (!cap_pa_has_positional(pa, r_name)) FB(failed);
        if (cap_pa_positional_count(pa, r_name) != 1u) FB(failed);
        const TypedUnion * r_value = cap_pa_get_positional(pa, r_name);
        if (!r_value) FB(failed);
        if (r_value != cap_pa_get_positional_i(pa, r_name, 0u)) FB(failed);
        if (cap_pa_get_positional_i(pa, r_name, 1u)) FB(failed);
        if (!cap_tu_is_string(r_value)) FB(failed);
        if (strcmp(cap_tu_as_string(r_value), word)) FB(failed);

        // check optional arg
        if (!cap_pa_has_positional(pa, o_name)) FB(failed);
        if (cap_pa_positional_count(pa, o_name) != 1u) FB(failed);
        const TypedUnion * o_value = cap_pa_get_positional(pa, o_name);
        if (!o_value) FB(failed);
        if (o_value != cap_pa_get_positional_i(pa, o_name, 0u)) FB(failed);
        if (cap_pa_get_positional_i(pa, o_name, 1u)) FB(failed);
        if (!cap_tu_is_int(o_value)) FB(failed);
        if (cap_tu_as_int(o_value) != 10) FB(failed);

        //check variadic values
        if (!cap_pa_has_positional(pa, v_name)) FB(failed);
        size_t count = cap_pa_positional_count(pa, v_name);
        if (count != 4u) FB(failed);
        size_t i = 0, limit = 10;
        for ( ; i < limit; ++i) {
            const TypedUnion * v_value = cap_pa_get_positional_i(pa, v_name, i);
            if (i >= count) {
                if (v_value) break;
                continue;
            }
            if (!v_value) break;
            if (!cap_tu_is_double(v_value)) break;
            if (cap_tu_as_double(v_value) != numbers[i]) break;
        }
        if (i < limit) FB(failed);
    } while(false);
    cap_parser_destroy(p);
    if (pa) cap_pa_destroy(pa);
    return !failed;
}

/**
 * Test with one required, one optional, and two variadic arguments. This 
 * should fail to create.
 */
bool test_required_optional_variadic_variadic() {
    ArgumentParser * p = cap_parser_make_empty();
    bool failed = false;
    do {
        AddPositionalError e;
        e = cap_parser_add_positional_noexit(
            p, "required", DT_STRING, true, false, NULL, NULL);
        if (e != APE_OK) FB(failed);

        e = cap_parser_add_positional_noexit(
            p, "optional", DT_STRING, false, false, NULL, NULL);
        if (e != APE_OK) FB(failed);

        e = cap_parser_add_positional_noexit(
            p, "variadic", DT_STRING, false, true, NULL, NULL);
        if (e != APE_OK) FB(failed);

        e = cap_parser_add_positional_noexit(
            p, "variadic_2", DT_STRING, false, true, NULL, NULL);
        if (e != APE_ANYTHING_AFTER_VARIADIC) FB(failed);
    } while (false);
    cap_parser_destroy(p);
    return !failed;
}

/**
 * Test with one required, one optional, one variadic and one optional 
 * argument. This should fail to create.
 */
bool test_required_optional_variadic_optional() {
    ArgumentParser * p = cap_parser_make_empty();
    bool failed = false;
    do {
        AddPositionalError e;
        e = cap_parser_add_positional_noexit(
            p, "required", DT_STRING, true, false, NULL, NULL);
        if (e != APE_OK) FB(failed);

        e = cap_parser_add_positional_noexit(
            p, "optional", DT_STRING, false, false, NULL, NULL);
        if (e != APE_OK) FB(failed);

        e = cap_parser_add_positional_noexit(
            p, "variadic", DT_STRING, false, true, NULL, NULL);
        if (e != APE_OK) FB(failed);

        e = cap_parser_add_positional_noexit(
            p, "optional_2", DT_STRING, false, false, NULL, NULL);
        if (e != APE_ANYTHING_AFTER_VARIADIC) FB(failed);
    } while (false);
    cap_parser_destroy(p);
    return !failed;
}

/**
 * Test with one required, one optional, one variadic and another required 
 * argument. This should fail to create.
 */
bool test_required_optional_variadic_required() {
    ArgumentParser * p = cap_parser_make_empty();
    bool failed = false;
    do {
        AddPositionalError e;
        e = cap_parser_add_positional_noexit(
            p, "required", DT_STRING, true, false, NULL, NULL);
        if (e != APE_OK) FB(failed);

        e = cap_parser_add_positional_noexit(
            p, "optional", DT_STRING, false, false, NULL, NULL);
        if (e != APE_OK) FB(failed);

        e = cap_parser_add_positional_noexit(
            p, "variadic", DT_STRING, false, true, NULL, NULL);
        if (e != APE_OK) FB(failed);

        e = cap_parser_add_positional_noexit(
            p, "required_2", DT_STRING, true, false, NULL, NULL);
        if (e != APE_ANYTHING_AFTER_VARIADIC) FB(failed);
    } while (false);
    cap_parser_destroy(p);
    return !failed;
}

/**
 * Test with one required, one optional, one variadic and one req-variadic 
 * argument. This should fail to create.
 */
bool test_required_optional_variadic_revariadic() {
    ArgumentParser * p = cap_parser_make_empty();
    bool failed = false;
    do {
        AddPositionalError e;
        e = cap_parser_add_positional_noexit(
            p, "required", DT_STRING, true, false, NULL, NULL);
        if (e != APE_OK) FB(failed);

        e = cap_parser_add_positional_noexit(
            p, "optional", DT_STRING, false, false, NULL, NULL);
        if (e != APE_OK) FB(failed);

        e = cap_parser_add_positional_noexit(
            p, "variadic", DT_STRING, false, true, NULL, NULL);
        if (e != APE_OK) FB(failed);

        e = cap_parser_add_positional_noexit(
            p, "revariadic", DT_STRING, true, false, NULL, NULL);
        if (e != APE_ANYTHING_AFTER_VARIADIC
                && e != APE_REQUIRED_AFTER_OPTIONAL) FB(failed);
    } while (false);
    cap_parser_destroy(p);
    return !failed;
}

/**
 * Test with one required, one req-variadic, and one optional argument. This 
 * should fail to create.
 */
bool test_required_revariadic_optional() {
    ArgumentParser * p = cap_parser_make_empty();
    bool failed = false;
    do {
        AddPositionalError e;
        e = cap_parser_add_positional_noexit(
            p, "required", DT_STRING, true, false, NULL, NULL);
        if (e != APE_OK) FB(failed);

        e = cap_parser_add_positional_noexit(
            p, "revariadic", DT_STRING, true, true, NULL, NULL);
        if (e != APE_OK) FB(failed);

        e = cap_parser_add_positional_noexit(
            p, "optional", DT_STRING, false, false, NULL, NULL);
        if (e != APE_ANYTHING_AFTER_VARIADIC) FB(failed);
    } while (false);
    cap_parser_destroy(p);
    return !failed;
}

/**
 * Test with one required, one variadic, and one optional argument. This 
 * should fail to create.
 */
bool test_required_variadic_optional() {
    ArgumentParser * p = cap_parser_make_empty();
    bool failed = false;
    do {
        AddPositionalError e;
        e = cap_parser_add_positional_noexit(
            p, "required", DT_STRING, true, false, NULL, NULL);
        if (e != APE_OK) FB(failed);

        e = cap_parser_add_positional_noexit(
            p, "variadic", DT_STRING, false, true, NULL, NULL);
        if (e != APE_OK) FB(failed);

        e = cap_parser_add_positional_noexit(
            p, "optional", DT_STRING, false, false, NULL, NULL);
        if (e != APE_ANYTHING_AFTER_VARIADIC) FB(failed);
    } while (false);
    cap_parser_destroy(p);
    return !failed;
}

int main() {
    bool a, b, c;
    a = TEST_GROUP(
        "p-opt+var+req", false, false, test_optional_variadic_required);
    b = TEST_GROUP(
        "p-req+opt+[re]var", false, false, test_required_optional_revariadic,
        test_required_optional_variadic_0, test_required_optional_variadic_1,
        test_required_optional_variadic_2, test_required_optional_variadic_3);
    c = TEST_GROUP(
        "p-req+opt+var+any", false, false, 
        test_required_optional_variadic_optional, 
        test_required_optional_variadic_required, 
        test_required_optional_variadic_revariadic,
        test_required_optional_variadic_variadic);
    return a && b && c ? 0 : 1;
}
