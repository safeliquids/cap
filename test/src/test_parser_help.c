#include "cap.h"
#include "test.h"

/**
 * Test return code with help configured.
 */
bool test_with_help() {
    ArgumentParser * p = cap_parser_make_empty();
    cap_parser_set_help_flag(p, "--help", NULL);

    const char * a[4] = {"prog", "--help", "-a", "abcd"};
    ParsingResult res = cap_parser_parse_noexit(p, 4, a);
    ParsedArguments * pa = res.mArguments;
    bool failed = false;
    do {
        if (res.mError != PER_HELP) FB(failed)
        if (res.mArguments) FB(failed);
    } while (false);

    cap_pa_destroy(pa);
    cap_parser_destroy(p);
    return !failed;
}

/**
 * Test default config
 * 
 * By default, the -h flag should produce help 
 */
bool test_with_default() {
    ArgumentParser * p = cap_parser_make_default();
    cap_parser_enable_help(p, false);
    cap_parser_enable_usage(p, false);

    const char * a[4] = {"prog", "-h", "-a", "abcd"};
    ParsingResult res = cap_parser_parse_noexit(p, 4, a);
    ParsedArguments * pa = res.mArguments;
    bool failed = false;
    do {
        if (res.mError != PER_HELP) FB(failed)
        if (res.mArguments) FB(failed);
    } while (false);

    cap_pa_destroy(pa);
    cap_parser_destroy(p);
    return !failed;
}

/**
 * Test without help
 * 
 * Parsing should fail with unrecoglized flag.
 */
bool test_without_help() {
    ArgumentParser * p = cap_parser_make_empty();

    const char * a[4] = {"prog", "-h", "-a", "abcd"};
    ParsingResult res = cap_parser_parse_noexit(p, 4, a);
    ParsedArguments * pa = res.mArguments;
    bool failed = false;
    do {
        if (res.mError != PER_UNKNOWN_FLAG) FB(failed)
        if (res.mArguments) FB(failed);
    } while (false);

    cap_pa_destroy(pa);
    cap_parser_destroy(p);
    return !failed;
}

/**
 * Test without help
 * 
 * The help flag is explicitly removed. Parsing should fail with unknown flag.
 */
bool test_without_help_2() {
    ArgumentParser * p = cap_parser_make_default();
    cap_parser_enable_help(p, false);
    cap_parser_enable_usage(p, false);
    cap_parser_set_help_flag(p, NULL, NULL);

    const char * a[4] = {"prog", "-h", "-a", "abcd"};
    ParsingResult res = cap_parser_parse_noexit(p, 4, a);
    ParsedArguments * pa = res.mArguments;
    bool failed = false;
    do {
        if (res.mError != PER_UNKNOWN_FLAG) FB(failed)
        if (res.mArguments) FB(failed);
    } while (false);

    cap_pa_destroy(pa);
    cap_parser_destroy(p);
    return !failed;
}

/**
 * Test replacing the default with a new flag.
 * 
 * Parsing should return with PER_HELP
 */
bool test_replace_default() {
    ArgumentParser * p = cap_parser_make_default();
    cap_parser_enable_help(p, false);
    cap_parser_enable_usage(p, false);
    cap_parser_set_help_flag(p, "--me-when-help", NULL);

    const char * a[4] = {"prog", "--me-when-help", "-a", "abcd"};
    ParsingResult res = cap_parser_parse_noexit(p, 4, a);
    ParsedArguments * pa = res.mArguments;
    bool failed = false;
    do {
        if (res.mError != PER_HELP) FB(failed)
        if (res.mArguments) FB(failed);
    } while (false);

    cap_pa_destroy(pa);
    cap_parser_destroy(p);
    return !failed;
}

/**
 * Test setting the help flag multiple times.
 * 
 * Parsing should fail with unknown flag
 */
bool test_replace_default_2() {
    ArgumentParser * p = cap_parser_make_empty();
    cap_parser_enable_help(p, false);
    cap_parser_enable_usage(p, false);
    cap_parser_set_help_flag(p, "--me-when-help", NULL);
    cap_parser_set_help_flag(p, "--bonk", NULL);

    const char * a[4] = {"prog", "--me-when-help", "-a", "abcd"};
    ParsingResult res = cap_parser_parse_noexit(p, 4, a);
    ParsedArguments * pa = res.mArguments;
    bool failed = false;
    do {
        if (res.mError != PER_UNKNOWN_FLAG) FB(failed)
        if (res.mArguments) FB(failed);
    } while (false);

    cap_pa_destroy(pa);
    cap_parser_destroy(p);
    return !failed;
}

/**
 * Test setting the help flag multiple times.
 * 
 * Parsing should succeed with per_help
 */
bool test_replace_default_3() {
    ArgumentParser * p = cap_parser_make_empty();
    cap_parser_enable_help(p, false);
    cap_parser_enable_usage(p, false);
    cap_parser_set_help_flag(p, "--me-when-help", NULL);
    cap_parser_set_help_flag(p, "--bonk", NULL);

    const char * a[4] = {"prog", "--bonk", "-a", "abcd"};
    ParsingResult res = cap_parser_parse_noexit(p, 4, a);
    ParsedArguments * pa = res.mArguments;
    bool failed = false;
    do {
        if (res.mError != PER_HELP) FB(failed)
        if (res.mArguments) FB(failed);
    } while (false);

    cap_pa_destroy(pa);
    cap_parser_destroy(p);
    return !failed;
}

/**
 * Test finding the help flag in the middle of the command line.
 * 
 * Parsing should succeed with per_help
 */
bool test_find_h() {
    ArgumentParser * p = cap_parser_make_default();
    cap_parser_enable_help(p, false);
    cap_parser_enable_usage(p, false);
    cap_parser_add_flag(p, "--bonk", DT_PRESENCE, 0, 1, NULL, NULL);
    cap_parser_add_flag(p, "-a", DT_INT, 0, 1, NULL, NULL);
    cap_parser_add_flag(p, "-z", DT_PRESENCE, 0, 1, NULL, NULL);
    cap_parser_add_positional(p, "word", DT_STRING, true, false, NULL, NULL);
    cap_parser_add_positional(p, "lies", DT_INT, true, false, NULL, NULL);


    const char * a[7] = {"prog", "--bonk", "-a", "100", "wordle", "-h", "bamboo"};
    ParsingResult res = cap_parser_parse_noexit(p, 7, a);
    ParsedArguments * pa = res.mArguments;
    bool failed = false;
    do {
        if (res.mError != PER_HELP) FB(failed)
        if (res.mArguments) FB(failed);
    } while (false);

    cap_pa_destroy(pa);
    cap_parser_destroy(p);
    return !failed;
}

/**
 * Test finding the help flag in the middle of the command line.
 * 
 * Parsing should succeed with per_help
 */
bool test_find_h_2() {
    ArgumentParser * p = cap_parser_make_default();
    cap_parser_enable_help(p, false);
    cap_parser_enable_usage(p, false);
    cap_parser_add_flag(p, "--bonk", DT_PRESENCE, 0, 1, NULL, NULL);
    cap_parser_add_flag(p, "-a", DT_INT, 0, 1, NULL, NULL);
    cap_parser_add_flag(p, "-z", DT_PRESENCE, 0, 1, NULL, NULL);
    cap_parser_add_positional(p, "word", DT_STRING, true, false, NULL, NULL);
    cap_parser_add_positional(p, "lies", DT_STRING, true, false, NULL, NULL);

    cap_parser_set_help_flag(p, "--gelp", NULL);

    const char * a[7] = {"prog", "--bonk", "-a", "100", "wordle", "bamboo", "--gelp"};
    ParsingResult res = cap_parser_parse_noexit(p, 7, a);
    ParsedArguments * pa = res.mArguments;
    bool failed = false;
    do {
        if (res.mError != PER_HELP) FB(failed)
        if (res.mArguments) FB(failed);
    } while (false);

    cap_pa_destroy(pa);
    cap_parser_destroy(p);
    return !failed;
}

/**
 * Test finding the help flag in the middle of the command line.
 * 
 * Parsing should fail with unknown flag
 */
bool test_find_h_3() {
    ArgumentParser * p = cap_parser_make_default();
    cap_parser_enable_help(p, false);
    cap_parser_enable_usage(p, false);
    cap_parser_add_flag(p, "--bonk", DT_PRESENCE, 0, 1, NULL, NULL);
    cap_parser_add_flag(p, "-a", DT_INT, 0, 1, NULL, NULL);
    cap_parser_add_flag(p, "-z", DT_PRESENCE, 0, 1, NULL, NULL);
    cap_parser_add_positional(p, "word", DT_STRING, true, false, NULL, NULL);
    cap_parser_add_positional(p, "lies", DT_STRING, true, false, NULL, NULL);

    cap_parser_set_help_flag(p, "--gelp", NULL);

    const char * a[8] = {"prog", "--bonk", "-a", "100", "wordle", "-h", "bamboo", "--gelp"};
    ParsingResult res = cap_parser_parse_noexit(p, 8, a);
    ParsedArguments * pa = res.mArguments;
    bool failed = false;
    do {
        if (res.mError != PER_UNKNOWN_FLAG) FB(failed)
        if (res.mArguments) FB(failed);
    } while (false);

    cap_pa_destroy(pa);
    cap_parser_destroy(p);
    return !failed;
}

int main() {
    bool a, b, c;
    a = TEST_GROUP(
        "parser-help: default/empty config.", false, false,
        test_with_help, test_with_default, test_without_help, 
        test_without_help_2);
    b = TEST_GROUP(
        "parser-help: replacing default", false, false, test_replace_default, 
        test_replace_default_2, test_replace_default_3);
    c = TEST_GROUP(
        "parser-help: finding help flag", false, false, test_find_h, 
        test_find_h_2, test_find_h_3);
    return a && b && c ? 0 : 1;
}
