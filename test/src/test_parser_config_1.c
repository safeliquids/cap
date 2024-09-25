#include "cap.h"
#include "test.h"

/**
 * test deafult behavior: parsing this should fail since the flag "--wrong" 
 * is not configured.
 */
bool test_prefix_default_0() {
    ArgumentParser * parser = cap_parser_make_empty();
    const char * foo = "--foo";
    const char * wrong = "--wrong";
    const char * word = "word";
    cap_parser_add_flag(parser, foo, DT_PRESENCE, 0, -1, NULL, NULL);
    cap_parser_add_positional(parser, word, DT_STRING, NULL, NULL);
    const char * args[3] = {"p", foo, wrong};

    bool failed = false;

    ParsingResult res = cap_parser_parse_noexit(parser, 4, args);
    ParsedArguments * pa = res.mArguments;
    do {
        if (res.mError == PER_NO_ERROR) FB(failed)
    } while (false);

    cap_pa_destroy(pa);
    return !failed;
}

/**
 * test default behavior: the default flag prefix char should be - and the 
 * default flag separator should be --. One flag --foo should be parsed and
 * one positional named "word" should contain a string "--foo".
 */
bool test_prefix_default_1() {
    ArgumentParser * parser = cap_parser_make_empty();
    cap_parser_set_flag_separator(parser, "--", NULL);
    const char * foo = "--foo";
    const char * word = "word";
    cap_parser_add_flag(parser, foo, DT_PRESENCE, 0, -1, NULL, NULL);
    cap_parser_add_positional(parser, word, DT_STRING, NULL, NULL);
    const char * args[4] = {"p", foo, "--", foo};

    bool failed = false;

    ParsingResult res = cap_parser_parse_noexit(parser, 4, args);
    ParsedArguments * pa = res.mArguments;
    do {
        if (res.mError != PER_NO_ERROR) FB(failed)
        if (!res.mArguments) FB(failed)
        if (!cap_pa_has_flag(pa, foo)) FB(failed)
        if (cap_pa_flag_count(pa, foo) != 1u) FB(failed)
        if (!cap_tu_is_presence(cap_pa_get_flag(pa, foo))) FB(failed)

        if (cap_pa_has_positional(pa, "--")) FB(failed)
        if (cap_pa_has_flag(pa, "--")) FB(failed)

        if (!cap_pa_has_positional(pa, word)) FB(failed)
        if (!cap_tu_is_string(cap_pa_get_positional(pa, word))) FB(failed)
        if (strcmp(cap_tu_as_string(cap_pa_get_positional(pa, word)), "--foo")) FB(failed)
    } while (false);

    cap_pa_destroy(pa);
    return !failed;
}

/**
 * test default behavior: the default flag prefix char should be - and the 
 * default flag separator should be --. The flag --foo should be parsed twice.
 */
bool test_prefix_default_2() {
    ArgumentParser * parser = cap_parser_make_empty();
    cap_parser_set_flag_separator(parser, "--", NULL);
    const char * foo = "--foo";
    const char * word = "word";
    cap_parser_add_flag(parser, foo, DT_PRESENCE, 0, -1, NULL, NULL);
    // cap_parser_add_positional(parser, word, DT_STRING, NULL, NULL);
    const char * args[4] = {"p", foo, foo, "--"};

    bool failed = false;

    ParsingResult res = cap_parser_parse_noexit(parser, 4, args);
    ParsedArguments * pa = res.mArguments;
    do {
        if (res.mError != PER_NO_ERROR) FB(failed)
        if (!res.mArguments) FB(failed)
        if (!cap_pa_has_flag(pa, foo)) FB(failed)
        if (cap_pa_flag_count(pa, foo) != 2u) FB(failed)
        if (!cap_tu_is_presence(cap_pa_get_flag(pa, foo))) FB(failed)
        if (!cap_tu_is_presence(cap_pa_get_flag_i(pa, foo, 1))) FB(failed)

        if (cap_pa_has_positional(pa, "--")) FB(failed)
        if (cap_pa_has_flag(pa, "--")) FB(failed)

        if (cap_pa_has_positional(pa, word)) FB(failed)
    } while (false);

    cap_pa_destroy(pa);
    return !failed;
}

/**
 * test default behavior: the default flag prefix char should be - and the 
 * default flag separator should be --. This should fail with too many arguments.
 */
bool test_prefix_default_3() {
    ArgumentParser * parser = cap_parser_make_empty();
    cap_parser_set_flag_separator(parser, "--", NULL);
    const char * foo = "--foo";
    const char * word = "word";
    cap_parser_add_flag(parser, foo, DT_PRESENCE, 0, -1, NULL, NULL);
    cap_parser_add_positional(parser, word, DT_STRING, NULL, NULL);
    const char * args[4] = {"p", "--", foo, foo};

    bool failed = false;

    ParsingResult res = cap_parser_parse_noexit(parser, 4, args);
    ParsedArguments * pa = res.mArguments;
    do {
        if (res.mError == PER_NO_ERROR) FB(failed)
    } while (false);

    cap_pa_destroy(pa);
    return !failed;
}

/**
 * test default behavior: the default flag prefix char should be - and the 
 * default flag separator should be --. This should parse the --foo flag twice 
 * and the --bar flag once. Both positional arguments should contain the 
 * string "--bar".
 */
bool test_prefix_default_4() {
    ArgumentParser * parser = cap_parser_make_empty();
    cap_parser_set_flag_separator(parser, "--", NULL);
    const char * foo = "--foo";
    const char * bar = "--bar";

    const char * word = "word";
    const char * wordle = "wordle";

    cap_parser_add_flag(parser, foo, DT_PRESENCE, 0, -1, NULL, NULL);
    cap_parser_add_flag(parser, bar, DT_PRESENCE, 0, -1, NULL, NULL);
    cap_parser_add_positional(parser, word, DT_STRING, NULL, NULL);
    cap_parser_add_positional(parser, wordle, DT_STRING, NULL, NULL);

    const char * args[7] = {"p", foo, bar, foo, "--", bar, bar};

    bool failed = false;

    ParsingResult res = cap_parser_parse_noexit(parser, 7, args);
    ParsedArguments * pa = res.mArguments;
    do {
        if (res.mError != PER_NO_ERROR) FB(failed)
        if (!cap_pa_has_flag(pa, foo)) FB(failed)
        if (cap_pa_flag_count(pa, foo) != 2u) FB(failed)
        if (!cap_tu_is_presence(cap_pa_get_flag_i(pa, foo, 0)))FB(failed)
        if (!cap_tu_is_presence(cap_pa_get_flag_i(pa, foo, 1)))FB(failed)

        if (!cap_pa_has_flag(pa, bar)) FB(failed)
        if (cap_pa_flag_count(pa, bar) != 1u) FB(failed)
        if (!cap_tu_is_presence(cap_pa_get_flag_i(pa, bar, 0)))FB(failed)
        
        if (!cap_pa_has_positional(pa, word)) FB(failed)
        if (!cap_tu_is_string(cap_pa_get_positional(pa, word))) FB(failed)
        if (strcmp(cap_tu_as_string(cap_pa_get_positional(pa, word)), bar)) FB(failed)

        if (!cap_pa_has_positional(pa, wordle)) FB(failed)
        if (!cap_tu_is_string(cap_pa_get_positional(pa, wordle))) FB(failed)
        if (strcmp(cap_tu_as_string(cap_pa_get_positional(pa, wordle)), bar)) FB(failed)
    } while (false);

    cap_pa_destroy(pa);
    return !failed;
}

/**
 * this should fail because flag prefix characters are not configured.
 */
bool test_prefix_custom_0() {
    ArgumentParser * parser = cap_parser_make_empty();
    const char * foo = "/foo";
    const char * bar = "/bar";

    const char * word = "word";
    const char * wordle = "wordle";

    cap_parser_add_flag(parser, "-foo", DT_PRESENCE, 0, -1, NULL, NULL);
    cap_parser_add_flag(parser, "-bar", DT_PRESENCE, 0, -1, NULL, NULL);
    cap_parser_add_positional(parser, word, DT_STRING, NULL, NULL);
    cap_parser_add_positional(parser, wordle, DT_STRING, NULL, NULL);

    const char * args[7] = {"p", foo, bar, foo, "//", bar, bar};

    bool failed = false;

    ParsingResult res = cap_parser_parse_noexit(parser, 7, args);
    ParsedArguments * pa = res.mArguments;
    do {
        if (res.mError == PER_NO_ERROR) FB(failed)
    } while (false);

    cap_pa_destroy(pa);
    return !failed;
}

/**
 * custom flag prefix was set
 * 
 * this should parse everything correctly in the same way as in 
 * test_prefix_deafult_4.
 */
bool test_prefix_custom_1() {
    ArgumentParser * parser = cap_parser_make_empty();
    cap_parser_set_flag_separator(parser, "//", NULL);
    const char * foo = "/foo";
    const char * bar = "/bar";

    const char * word = "word";
    const char * wordle = "wordle";

    cap_parser_add_positional(parser, word, DT_STRING, NULL, NULL);
    cap_parser_add_positional(parser, wordle, DT_STRING, NULL, NULL);

    cap_parser_set_flag_prefix(parser, "/");

    cap_parser_add_flag(parser, foo, DT_PRESENCE, 0, -1, NULL, NULL);
    cap_parser_add_flag(parser, bar, DT_PRESENCE, 0, -1, NULL, NULL);


    const char * args[7] = {"p", foo, bar, foo, "//", bar, bar};

    bool failed = false;

    ParsingResult res = cap_parser_parse_noexit(parser, 7, args);
    ParsedArguments * pa = res.mArguments;
    do {
        if (res.mError != PER_NO_ERROR) FB(failed)
        if (!cap_pa_has_flag(pa, foo)) FB(failed)
        if (cap_pa_flag_count(pa, foo) != 2u) FB(failed)
        if (!cap_tu_is_presence(cap_pa_get_flag_i(pa, foo, 0)))FB(failed)
        if (!cap_tu_is_presence(cap_pa_get_flag_i(pa, foo, 1)))FB(failed)

        if (!cap_pa_has_flag(pa, bar)) FB(failed)
        if (cap_pa_flag_count(pa, bar) != 1u) FB(failed)
        if (!cap_tu_is_presence(cap_pa_get_flag_i(pa, bar, 0)))FB(failed)
        
        if (!cap_pa_has_positional(pa, word)) FB(failed)
        if (!cap_tu_is_string(cap_pa_get_positional(pa, word))) FB(failed)
        if (strcmp(cap_tu_as_string(cap_pa_get_positional(pa, word)), bar)) FB(failed)

        if (!cap_pa_has_positional(pa, wordle)) FB(failed)
        if (!cap_tu_is_string(cap_pa_get_positional(pa, wordle))) FB(failed)
        if (strcmp(cap_tu_as_string(cap_pa_get_positional(pa, wordle)), bar)) FB(failed)
    } while (false);

    cap_pa_destroy(pa);
    return !failed;
}

/**
 * custom flag prefix was set
 * 
 * this should parse everything correctly in the same way as in 
 * test_prefix_deafult_4.
 */
bool test_prefix_custom_2() {
    ArgumentParser * parser = cap_parser_make_empty();
    cap_parser_set_flag_separator(parser, "--", NULL);
    const char * foo = "-foo";
    const char * bar = "+bar";

    const char * word = "word";
    const char * wordle = "wordle";

    cap_parser_set_flag_prefix(parser, "-+");

    cap_parser_add_flag(parser, foo, DT_PRESENCE, 0, -1, NULL, NULL);
    cap_parser_add_flag(parser, bar, DT_PRESENCE, 0, -1, NULL, NULL);
    cap_parser_add_positional(parser, word, DT_STRING, NULL, NULL);
    cap_parser_add_positional(parser, wordle, DT_STRING, NULL, NULL);


    const char * args[7] = {"p", foo, bar, foo, "--", bar, bar};

    bool failed = false;

    ParsingResult res = cap_parser_parse_noexit(parser, 7, args);
    ParsedArguments * pa = res.mArguments;
    do {
        if (res.mError != PER_NO_ERROR) FB(failed)
        if (!cap_pa_has_flag(pa, foo)) FB(failed)
        if (cap_pa_flag_count(pa, foo) != 2u) FB(failed)
        if (!cap_tu_is_presence(cap_pa_get_flag_i(pa, foo, 0)))FB(failed)
        if (!cap_tu_is_presence(cap_pa_get_flag_i(pa, foo, 1)))FB(failed)

        if (!cap_pa_has_flag(pa, bar)) FB(failed)
        if (cap_pa_flag_count(pa, bar) != 1u) FB(failed)
        if (!cap_tu_is_presence(cap_pa_get_flag_i(pa, bar, 0)))FB(failed)
        
        if (!cap_pa_has_positional(pa, word)) FB(failed)
        if (!cap_tu_is_string(cap_pa_get_positional(pa, word))) FB(failed)
        if (strcmp(cap_tu_as_string(cap_pa_get_positional(pa, word)), bar)) FB(failed)

        if (!cap_pa_has_positional(pa, wordle)) FB(failed)
        if (!cap_tu_is_string(cap_pa_get_positional(pa, wordle))) FB(failed)
        if (strcmp(cap_tu_as_string(cap_pa_get_positional(pa, wordle)), bar)) FB(failed)
    } while (false);

    cap_pa_destroy(pa);
    return !failed;
}

int main() {
    bool a, b;
    a = TEST_GROUP(
        "parser-config1: default prefix and separator",
        false, false, test_prefix_default_0, test_prefix_default_1, 
        test_prefix_default_2,  test_prefix_default_3, 
        test_prefix_default_4);
    b = TEST_GROUP("parser-config1: custom prefix", false, false, 
    test_prefix_custom_0, test_prefix_custom_1, test_prefix_custom_2);
    return a && b ? 0 : 1;
}

