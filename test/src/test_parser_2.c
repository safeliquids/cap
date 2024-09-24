#include "cap.h"
#include "test.h"

/*
 * TODO:
 * - multiple positionals
 * - positionals that start with a dash (such as negative numbers)
 */

bool test_one_positional_int() {
    const char * name = "argument";
    ArgumentParser * parser = cap_parser_make_empty();
    cap_parser_add_positional(parser, name, DT_INT, NULL, NULL);

    const char * a[2] = {"program", "100"};

    ParsingResult res = cap_parser_parse_noexit(parser, 2, a);
    ParsedArguments * pa = res.mArguments;
    if (res.mError != PER_NO_ERROR) goto fail;
    if (!res.mArguments) goto fail;
    if (!cap_pa_has_positional(pa, name)) goto fail;
    if (!cap_tu_is_int(cap_pa_get_positional(pa, name))) goto fail;
    if (cap_tu_as_int(cap_pa_get_positional(pa, name)) != 100) goto fail;

    cap_pa_destroy(pa);
    cap_parser_destroy(parser);
    return true;
fail:
    cap_pa_destroy(pa);
    cap_parser_destroy(parser);
    return false;
}

bool test_one_positional_double() {
    const char * name = "argument";
    ArgumentParser * parser = cap_parser_make_empty();
    cap_parser_add_positional(parser, name, DT_DOUBLE, NULL, NULL);

    const char * a[2] = {"program", "3.14e10"};

    ParsingResult res = cap_parser_parse_noexit(parser, 2, a);
    ParsedArguments * pa = res.mArguments;
    if (res.mError != PER_NO_ERROR) goto fail;
    if (!res.mArguments) goto fail;
    if (!cap_pa_has_positional(pa, name)) goto fail;
    if (!cap_tu_is_double(cap_pa_get_positional(pa, name))) goto fail;
    if (cap_tu_as_double(cap_pa_get_positional(pa, name)) != 3.14e10) goto fail;

    cap_pa_destroy(pa);
    cap_parser_destroy(parser);
    return true;
fail:
    cap_pa_destroy(pa);
    cap_parser_destroy(parser);
    return false;
}

bool test_one_positional_string() {
    const char * name = "argument";
    const char * value = "word";
    ArgumentParser * parser = cap_parser_make_empty();
    cap_parser_add_positional(parser, name, DT_STRING, NULL, NULL);

    const char * a[2] = {"program", value};

    ParsingResult res = cap_parser_parse_noexit(parser, 2, a);
    ParsedArguments * pa = res.mArguments;
    if (res.mError != PER_NO_ERROR) goto fail;
    if (!res.mArguments) goto fail;
    if (!cap_pa_has_positional(pa, name)) goto fail;
    if (!cap_tu_is_string(cap_pa_get_positional(pa, name))) goto fail;
    if (strcmp(cap_tu_as_string(cap_pa_get_positional(pa, name)), value)) goto fail;

    cap_pa_destroy(pa);
    cap_parser_destroy(parser);
    return true;
fail:
    cap_pa_destroy(pa);
    cap_parser_destroy(parser);
    return false;
}

bool test_one_positional_missing() {
    const char * name = "argument";
    ArgumentParser * parser = cap_parser_make_empty();
    cap_parser_add_positional(parser, name, DT_INT, NULL, NULL);

    const char * a[2] = {"program", "10"};

    ParsingResult res = cap_parser_parse_noexit(parser, 1, a);
    ParsedArguments * pa = res.mArguments;
    if (res.mError == PER_NO_ERROR) goto fail;
    if (res.mArguments) goto fail;

    cap_pa_destroy(pa);
    cap_parser_destroy(parser);
    return true;
fail:
    cap_pa_destroy(pa);
    cap_parser_destroy(parser);
    return false;
}

bool test_one_positional_not_parseable_int() {
    const char * name = "argument";
    ArgumentParser * parser = cap_parser_make_empty();
    cap_parser_add_positional(parser, name, DT_INT, NULL, NULL);

    const char * a[2] = {"program", "ten"};

    ParsingResult res = cap_parser_parse_noexit(parser, 2, a);
    ParsedArguments * pa = res.mArguments;
    if (res.mError == PER_NO_ERROR) goto fail;
    if (res.mArguments) goto fail;

    cap_pa_destroy(pa);
    cap_parser_destroy(parser);
    return true;
fail:
    cap_pa_destroy(pa);
    cap_parser_destroy(parser);
    return false;
}

bool test_one_positional_not_parseable_double() {
    const char * name = "argument";
    ArgumentParser * parser = cap_parser_make_empty();
    cap_parser_add_positional(parser, name, DT_DOUBLE, NULL, NULL);

    const char * a[2] = {"program", "-3.14.10"};

    ParsingResult res = cap_parser_parse_noexit(parser, 2, a);
    ParsedArguments * pa = res.mArguments;
    if (res.mError == PER_NO_ERROR) goto fail;
    if (res.mArguments) goto fail;

    cap_pa_destroy(pa);
    cap_parser_destroy(parser);
    return true;
fail:
    cap_pa_destroy(pa);
    cap_parser_destroy(parser);
    return false;
}

int main() {
    bool a, b;
    a = TEST_GROUP(
        "parser-2: one positional", false, false, test_one_positional_int,
        test_one_positional_double, test_one_positional_string);
    b = TEST_GROUP(
        "parser-2: errors", false, false, test_one_positional_missing,
        test_one_positional_not_parseable_int,
        test_one_positional_not_parseable_double);
    return a && b ? 0 : 1;
}
