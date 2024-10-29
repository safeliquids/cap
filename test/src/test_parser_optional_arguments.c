#include "cap.h"
#include "test.h"

/**
 * test with one optional argument. This should succeed if the argument is 
 * present or absent.
 */
bool test_optional_1() {
    ArgumentParser * p = cap_parser_make_empty();
    ParsedArguments * pa = NULL;

    const char * name = "word";
    const char * value = "spoon";
    const char * args[2] = {"p", "spoon"};
    bool failed = false;
    do {
        AddPositionalError e;
        e = cap_parser_add_positional_noexit(
            p, name, DT_STRING, false, NULL, NULL);
        if (e != APE_OK) FB(failed);

        ParsingResult res = cap_parser_parse_noexit(p, 2, args);
        pa = res.mArguments;
        if (res.mError != PER_NO_ERROR) FB(failed);
        if (!cap_pa_has_positional(pa, name)) FB(failed);
        const TypedUnion * v = cap_pa_get_positional(pa, name);
        if (!v) FB(failed);
        if (!cap_tu_is_string(v)) FB(failed);
        if (strcmp(cap_tu_as_string(v), value)) FB(failed);
    } while (false);
    cap_parser_destroy(p);
    if (pa) cap_pa_destroy(pa);
    return !failed;
}

/**
 * test with one optional argument. This should succeed if the argument is 
 * present or absent.
 */
bool test_optional_2() {
    ArgumentParser * p = cap_parser_make_empty();
    ParsedArguments * pa = NULL;

    const char * name = "word";
    const char * args[1] = {"p"};
    bool failed = false;
    do {
        AddPositionalError e;
        e = cap_parser_add_positional_noexit(
            p, name, DT_STRING, false, NULL, NULL);
        if (e != APE_OK) FB(failed);

        ParsingResult res = cap_parser_parse_noexit(p, 1, args);
        pa = res.mArguments;
        if (res.mError != PER_NO_ERROR) FB(failed);
        if (cap_pa_has_positional(pa, name)) FB(failed);
    } while (false);
    cap_parser_destroy(p);
    if (pa) cap_pa_destroy(pa);
    return !failed;
}

/**
 * test with one optional and one required argument. This should fail to 
 * create the parser.
 */
bool test_optional_required() {
    ArgumentParser * p = cap_parser_make_empty();
    bool failed = false;
    do {
        AddPositionalError e;
        e = cap_parser_add_positional_noexit(
            p, "optional", DT_STRING, false, NULL, NULL);
        if (e != APE_OK) FB(failed);
        e = cap_parser_add_positional_noexit(
            p, "required", DT_STRING, true, NULL, NULL);
        if (e == APE_OK) FB(failed);
    } while (false);
    cap_parser_destroy(p);
    return !failed;
}

/**
 * test one required and one optional argument. This should succeed and be able
 * to parse with or without the optional argument.
 */
bool test_required_optional_1() {
    ArgumentParser * p = cap_parser_make_empty();
    ParsedArguments * pa = NULL;

    const char * name_o = "opt";
    const char * name_r = "req";
    const char * args[3] = {"p", "10", "11"};
    bool failed = false;
    do {
        AddPositionalError e;
        e = cap_parser_add_positional_noexit(
            p, name_o, DT_INT, true, NULL, NULL);
        if (e != APE_OK) FB(failed);
        e = cap_parser_add_positional_noexit(
            p, name_r, DT_INT, false, NULL, NULL);
        if (e != APE_OK) FB(failed);

        ParsingResult res = cap_parser_parse_noexit(p, 3, args);
        pa = res.mArguments;
        if (res.mError != PER_NO_ERROR) FB(failed);
        
        if (!cap_pa_has_positional(pa, name_o)) FB(failed);
        const TypedUnion * v = cap_pa_get_positional(pa, name_o);
        if (!v) FB(failed);
        if (!cap_tu_is_int(v)) FB(failed);
        if (cap_tu_as_int(v) != 10) FB(failed);

        if (!cap_pa_has_positional(pa, name_r)) FB(failed);
        const TypedUnion * vv = cap_pa_get_positional(pa, name_r);
        if (!vv) FB(failed);
        if (!cap_tu_is_int(vv)) FB(failed);
        if (cap_tu_as_int(vv) != 11) FB(failed);
    } while (false);
    cap_parser_destroy(p);
    if (pa) cap_pa_destroy(pa);
    return !failed;
}

/**
 * test one required and one optional argument. This should succeed and be able
 * to parse with or without the optional argument.
 */
bool test_required_optional_2() {
    ArgumentParser * p = cap_parser_make_empty();
    ParsedArguments * pa = NULL;

    const char * name_o = "opt";
    const char * name_r = "req";
    const char * args[2] = {"p", "10"};
    bool failed = false;
    do {
        AddPositionalError e;
        e = cap_parser_add_positional_noexit(
            p, name_o, DT_INT, true, NULL, NULL);
        if (e != APE_OK) FB(failed);
        e = cap_parser_add_positional_noexit(
            p, name_r, DT_INT, false, NULL, NULL);
        if (e != APE_OK) FB(failed);

        ParsingResult res = cap_parser_parse_noexit(p, 2, args);
        pa = res.mArguments;
        if (res.mError != PER_NO_ERROR) FB(failed);
        
        if (!cap_pa_has_positional(pa, name_o)) FB(failed);
        const TypedUnion * v = cap_pa_get_positional(pa, name_o);
        if (!v) FB(failed);
        if (!cap_tu_is_int(v)) FB(failed);
        if (cap_tu_as_int(v) != 10) FB(failed);

        if (cap_pa_has_positional(pa, name_r)) FB(failed);
    } while (false);
    cap_parser_destroy(p);
    if (pa) cap_pa_destroy(pa);
    return !failed;
}

/**
 * test with one required, one optional and another required argument. This 
 * should fail to create.
 */
bool test_required_optional_required() {
    ArgumentParser * p = cap_parser_make_empty();

    const char * name_o = "opt";
    const char * name_r = "req";
    const char * name_oo = "wrong";
    bool failed = false;
    do {
        AddPositionalError e;
        e = cap_parser_add_positional_noexit(
            p, name_o, DT_INT, true, NULL, NULL);
        if (e != APE_OK) FB(failed);
        e = cap_parser_add_positional_noexit(
            p, name_r, DT_INT, false, NULL, NULL);
        if (e != APE_OK) FB(failed);
        e = cap_parser_add_positional_noexit(
            p, name_oo, DT_INT, true, NULL, NULL);
        if (e == APE_OK) FB(failed);
    } while (false);
    cap_parser_destroy(p);
    return !failed;
}

int main () {
    bool a;
    a = TEST_GROUP(
        "p-optionals", false, false, test_optional_1, test_optional_2,
        test_optional_required, test_required_optional_1, 
        test_required_optional_2, test_required_optional_required);
    return a ? 0 : 1;
}