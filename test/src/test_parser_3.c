#include "parser.h"
#include "parsed_arguments.h"
#include "typed_union.h"
#include "test.h"

/**
 * test correct parsing
 * 
 * positionals are mixed with flags
 */
bool test_flags_positionals() {
    ArgumentParser * p = cap_parser_make_default();
    const char * a = "-a";
    const char * b = "-b";
    const char * c = "-c";
    const char * word = "word";
    const char * another = "another";

    cap_parser_add_flag(p, a, DT_PRESENCE, 0, 1, NULL, NULL);
    cap_parser_add_positional(p, word, DT_STRING, "WORD", NULL);
    cap_parser_add_flag(p, b, DT_DOUBLE, 0, -1, "B", NULL);
    cap_parser_add_flag(p, c, DT_STRING, 1, 2, "P", NULL);
    cap_parser_add_positional(p, another, DT_INT, "ANOTHER", NULL);

    const char * string = "string";
    const char * anotherstring = "anotherstring";
    const char * abcd = "abcd";

    const char * args[13] = {"prog", b, "0", abcd, c, string, "100", c, anotherstring, b, "-1", b, "-100"};

    ParsingResult res = cap_parser_parse_noexit(p, 13, args);
    cap_parser_destroy(p);
    ParsedArguments * pa = res.mArguments;
    bool failed = false;
    do {
        if (res.mError != PER_NO_ERROR) FB(failed)
        if (!pa) FB(failed)
        
        if (cap_pa_flag_count(pa, a) != 0u) FB(failed)

        if (cap_pa_flag_count(pa, b) != 3u) FB(failed)
        if (!cap_tu_is_double(cap_pa_get_flag_i(pa, b, 0))) FB(failed)
        if (0 != cap_tu_as_double(cap_pa_get_flag_i(pa, b, 0))) FB(failed)
        if (!cap_tu_is_double(cap_pa_get_flag_i(pa, b, 1))) FB(failed)
        if (-1 != cap_tu_as_double(cap_pa_get_flag_i(pa, b, 1))) FB(failed)
        if (!cap_tu_is_double(cap_pa_get_flag_i(pa, b, 2))) FB(failed)
        if (-100 != cap_tu_as_double(cap_pa_get_flag_i(pa, b, 2))) FB(failed)
        
        if (cap_pa_flag_count(pa, c) != 2u) FB(failed)
        if (!cap_tu_is_string(cap_pa_get_flag_i(pa, c, 0))) FB(failed)
        if (strcmp(string, cap_tu_as_string(cap_pa_get_flag_i(pa, c, 0)))) FB(failed)
        if (!cap_tu_is_string(cap_pa_get_flag_i(pa, c, 1))) FB(failed)
        if (strcmp(anotherstring, cap_tu_as_string(cap_pa_get_flag_i(pa, c, 1)))) FB(failed)

        if (!cap_pa_has_positional(pa, word)) FB(failed)
        if (!cap_tu_is_string(cap_pa_get_positional(pa, word))) FB(failed)
        if (strcmp(abcd, cap_tu_as_string(cap_pa_get_positional(pa, word))))

        if (!cap_pa_has_positional(pa, another)) FB(failed)
        if (!cap_tu_is_int(cap_pa_get_positional(pa, another))) FB(failed)
        if (100 != cap_tu_as_int(cap_pa_get_positional(pa, another))) FB(failed)
    } while(false);

    cap_pa_destroy(pa);
    return !failed;
}

/**
 * test correct parsing
 * 
 * positionals are before flags
 */
bool test_flags_positionals_2() {
    ArgumentParser * p = cap_parser_make_default();
    const char * a = "-a";
    const char * b = "-b";
    const char * c = "-c";
    const char * word = "word";
    const char * another = "another";

    cap_parser_add_flag(p, a, DT_PRESENCE, 0, 1, NULL, NULL);
    cap_parser_add_positional(p, word, DT_STRING, "WORD", NULL);
    cap_parser_add_flag(p, b, DT_DOUBLE, 0, -1, "B", NULL);
    cap_parser_add_flag(p, c, DT_STRING, 1, 2, "P", NULL);
    cap_parser_add_positional(p, another, DT_INT, "ANOTHER", NULL);

    const char * string = "string";
    const char * anotherstring = "anotherstring";
    const char * abcd = "abcd";

    const char * args[13] = {"prog", abcd, "100", c, string, b, "0", c, anotherstring, b, "-1", b, "-100"};

    ParsingResult res = cap_parser_parse_noexit(p, 13, args);
    cap_parser_destroy(p);
    ParsedArguments * pa = res.mArguments;
    bool failed = false;
    do {
        if (res.mError != PER_NO_ERROR) FB(failed)
        if (!pa) FB(failed)
        
        if (cap_pa_flag_count(pa, a) != 0u) FB(failed)

        if (cap_pa_flag_count(pa, b) != 3u) FB(failed)
        if (!cap_tu_is_double(cap_pa_get_flag_i(pa, b, 0))) FB(failed)
        if (0 != cap_tu_as_double(cap_pa_get_flag_i(pa, b, 0))) FB(failed)
        if (!cap_tu_is_double(cap_pa_get_flag_i(pa, b, 1))) FB(failed)
        if (-1 != cap_tu_as_double(cap_pa_get_flag_i(pa, b, 1))) FB(failed)
        if (!cap_tu_is_double(cap_pa_get_flag_i(pa, b, 2))) FB(failed)
        if (-100 != cap_tu_as_double(cap_pa_get_flag_i(pa, b, 2))) FB(failed)
        
        if (cap_pa_flag_count(pa, c) != 2u) FB(failed)
        if (!cap_tu_is_string(cap_pa_get_flag_i(pa, c, 0))) FB(failed)
        if (strcmp(string, cap_tu_as_string(cap_pa_get_flag_i(pa, c, 0)))) FB(failed)
        if (!cap_tu_is_string(cap_pa_get_flag_i(pa, c, 1))) FB(failed)
        if (strcmp(anotherstring, cap_tu_as_string(cap_pa_get_flag_i(pa, c, 1)))) FB(failed)

        if (!cap_pa_has_positional(pa, word)) FB(failed)
        if (!cap_tu_is_string(cap_pa_get_positional(pa, word))) FB(failed)
        if (strcmp(abcd, cap_tu_as_string(cap_pa_get_positional(pa, word))))

        if (!cap_pa_has_positional(pa, another)) FB(failed)
        if (!cap_tu_is_int(cap_pa_get_positional(pa, another))) FB(failed)
        if (100 != cap_tu_as_int(cap_pa_get_positional(pa, another))) FB(failed)
    } while(false);

    cap_pa_destroy(pa);
    return !failed;
}

/**
 * Test correct parsing
 * 
 * positionals are mixed with flags and are next to each other.
 */
bool test_flags_positionals_3() {
    ArgumentParser * p = cap_parser_make_default();
    const char * a = "-a";
    const char * b = "-b";
    const char * c = "-c";
    const char * word = "word";
    const char * another = "another";

    cap_parser_add_flag(p, a, DT_PRESENCE, 0, 1, NULL, NULL);
    cap_parser_add_positional(p, word, DT_STRING, "WORD", NULL);
    cap_parser_add_flag(p, b, DT_DOUBLE, 0, -1, "B", NULL);
    cap_parser_add_flag(p, c, DT_STRING, 1, 2, "P", NULL);
    cap_parser_add_positional(p, another, DT_INT, "ANOTHER", NULL);

    const char * string = "string";
    const char * anotherstring = "anotherstring";
    const char * abcd = "abcd";

    const char * args[17] = {"prog", b, "21", b, "23", abcd, "100", c, string, b, "0", c, anotherstring, b, "-1", b, "-100"};

    ParsingResult res = cap_parser_parse_noexit(p, 17, args);
    cap_parser_destroy(p);
    ParsedArguments * pa = res.mArguments;
    bool failed = false;
    do {
        if (res.mError != PER_NO_ERROR) FB(failed)
        if (!pa) FB(failed)
        
        if (cap_pa_flag_count(pa, a) != 0u) FB(failed)

        if (cap_pa_flag_count(pa, b) != 5u) FB(failed)
        if (!cap_tu_is_double(cap_pa_get_flag_i(pa, b, 0))) FB(failed)
        if (21 != cap_tu_as_double(cap_pa_get_flag_i(pa, b, 0))) FB(failed)
        if (!cap_tu_is_double(cap_pa_get_flag_i(pa, b, 1))) FB(failed)
        if (23 != cap_tu_as_double(cap_pa_get_flag_i(pa, b, 1))) FB(failed)
        if (!cap_tu_is_double(cap_pa_get_flag_i(pa, b, 2))) FB(failed)
        if (0 != cap_tu_as_double(cap_pa_get_flag_i(pa, b, 2))) FB(failed)
        if (!cap_tu_is_double(cap_pa_get_flag_i(pa, b, 3))) FB(failed)
        if (-1 != cap_tu_as_double(cap_pa_get_flag_i(pa, b, 3))) FB(failed)
        if (!cap_tu_is_double(cap_pa_get_flag_i(pa, b, 4))) FB(failed)
        if (-100 != cap_tu_as_double(cap_pa_get_flag_i(pa, b, 4))) FB(failed)
        
        if (cap_pa_flag_count(pa, c) != 2u) FB(failed)
        if (!cap_tu_is_string(cap_pa_get_flag_i(pa, c, 0))) FB(failed)
        if (strcmp(string, cap_tu_as_string(cap_pa_get_flag_i(pa, c, 0)))) FB(failed)
        if (!cap_tu_is_string(cap_pa_get_flag_i(pa, c, 1))) FB(failed)
        if (strcmp(anotherstring, cap_tu_as_string(cap_pa_get_flag_i(pa, c, 1)))) FB(failed)

        if (!cap_pa_has_positional(pa, word)) FB(failed)
        if (!cap_tu_is_string(cap_pa_get_positional(pa, word))) FB(failed)
        if (strcmp(abcd, cap_tu_as_string(cap_pa_get_positional(pa, word))))

        if (!cap_pa_has_positional(pa, another)) FB(failed)
        if (!cap_tu_is_int(cap_pa_get_positional(pa, another))) FB(failed)
        if (100 != cap_tu_as_int(cap_pa_get_positional(pa, another))) FB(failed)
    } while(false);

    cap_pa_destroy(pa);
    return !failed;
}

/**
 * Test correct parsing
 * 
 * Positionals are after all flags. One positional is a negative number 
 * (begins with -) and is after the default flag separator (--).
 */
bool test_flags_positionals_4() {
    ArgumentParser * p = cap_parser_make_default();
    const char * a = "-a";
    const char * b = "-b";
    const char * c = "-c";
    const char * word = "word";
    const char * another = "another";

    cap_parser_add_flag(p, a, DT_PRESENCE, 0, 1, NULL, NULL);
    cap_parser_add_positional(p, word, DT_STRING, "WORD", NULL);
    cap_parser_add_flag(p, b, DT_DOUBLE, 0, -1, "B", NULL);
    cap_parser_add_flag(p, c, DT_STRING, 1, 2, "P", NULL);
    cap_parser_add_positional(p, another, DT_INT, "ANOTHER", NULL);

    const char * string = "string";
    const char * anotherstring = "anotherstring";
    const char * abcd = "abcd";

    const char * args[14] = {"prog", b, "0", c, string, c, anotherstring, b, "-1", b, "-100", abcd, "--", "-100"};

    ParsingResult res = cap_parser_parse_noexit(p, 14, args);
    cap_parser_destroy(p);
    ParsedArguments * pa = res.mArguments;
    bool failed = false;
    do {
        if (res.mError != PER_NO_ERROR) FB(failed)
        if (!pa) FB(failed)
        
        if (cap_pa_flag_count(pa, a) != 0u) FB(failed)

        if (cap_pa_flag_count(pa, b) != 3u) FB(failed)
        if (!cap_tu_is_double(cap_pa_get_flag_i(pa, b, 0))) FB(failed)
        if (0 != cap_tu_as_double(cap_pa_get_flag_i(pa, b, 0))) FB(failed)
        if (!cap_tu_is_double(cap_pa_get_flag_i(pa, b, 1))) FB(failed)
        if (-1 != cap_tu_as_double(cap_pa_get_flag_i(pa, b, 1))) FB(failed)
        if (!cap_tu_is_double(cap_pa_get_flag_i(pa, b, 2))) FB(failed)
        if (-100 != cap_tu_as_double(cap_pa_get_flag_i(pa, b, 2))) FB(failed)
        
        if (cap_pa_flag_count(pa, c) != 2u) FB(failed)
        if (!cap_tu_is_string(cap_pa_get_flag_i(pa, c, 0))) FB(failed)
        if (strcmp(string, cap_tu_as_string(cap_pa_get_flag_i(pa, c, 0)))) FB(failed)
        if (!cap_tu_is_string(cap_pa_get_flag_i(pa, c, 1))) FB(failed)
        if (strcmp(anotherstring, cap_tu_as_string(cap_pa_get_flag_i(pa, c, 1)))) FB(failed)

        if (!cap_pa_has_positional(pa, word)) FB(failed)
        if (!cap_tu_is_string(cap_pa_get_positional(pa, word))) FB(failed)
        if (strcmp(abcd, cap_tu_as_string(cap_pa_get_positional(pa, word))))

        if (!cap_pa_has_positional(pa, another)) FB(failed)
        if (!cap_tu_is_int(cap_pa_get_positional(pa, another))) FB(failed)
        if (-100 != cap_tu_as_int(cap_pa_get_positional(pa, another))) FB(failed)
    } while(false);

    cap_pa_destroy(pa);
    return !failed;
}

/**
 * Test correct parsing
 * 
 * Positionals are after all flags. The flag separator (--) is given after
 * all arguments.
 */
bool test_flags_positionals_5() {
    ArgumentParser * p = cap_parser_make_default();
    const char * a = "-a";
    const char * b = "-b";
    const char * c = "-c";
    const char * word = "word";
    const char * another = "another";

    cap_parser_add_flag(p, a, DT_PRESENCE, 0, 1, NULL, NULL);
    cap_parser_add_positional(p, word, DT_STRING, "WORD", NULL);
    cap_parser_add_flag(p, b, DT_DOUBLE, 0, -1, "B", NULL);
    cap_parser_add_flag(p, c, DT_STRING, 1, 2, "P", NULL);
    cap_parser_add_positional(p, another, DT_INT, "ANOTHER", NULL);

    const char * string = "string";
    const char * anotherstring = "anotherstring";
    const char * abcd = "abcd";

    const char * args[14] = {"prog", b, "0", c, string, c, anotherstring, b, "-1", b, "-100", abcd, "100", "--"};

    ParsingResult res = cap_parser_parse_noexit(p, 14, args);
    cap_parser_destroy(p);
    ParsedArguments * pa = res.mArguments;
    bool failed = false;
    do {
        if (res.mError != PER_NO_ERROR) FB(failed)
        if (!pa) FB(failed)
        
        if (cap_pa_flag_count(pa, a) != 0u) FB(failed)

        if (cap_pa_flag_count(pa, b) != 3u) FB(failed)
        if (!cap_tu_is_double(cap_pa_get_flag_i(pa, b, 0))) FB(failed)
        if (0 != cap_tu_as_double(cap_pa_get_flag_i(pa, b, 0))) FB(failed)
        if (!cap_tu_is_double(cap_pa_get_flag_i(pa, b, 1))) FB(failed)
        if (-1 != cap_tu_as_double(cap_pa_get_flag_i(pa, b, 1))) FB(failed)
        if (!cap_tu_is_double(cap_pa_get_flag_i(pa, b, 2))) FB(failed)
        if (-100 != cap_tu_as_double(cap_pa_get_flag_i(pa, b, 2))) FB(failed)
        
        if (cap_pa_flag_count(pa, c) != 2u) FB(failed)
        if (!cap_tu_is_string(cap_pa_get_flag_i(pa, c, 0))) FB(failed)
        if (strcmp(string, cap_tu_as_string(cap_pa_get_flag_i(pa, c, 0)))) FB(failed)
        if (!cap_tu_is_string(cap_pa_get_flag_i(pa, c, 1))) FB(failed)
        if (strcmp(anotherstring, cap_tu_as_string(cap_pa_get_flag_i(pa, c, 1)))) FB(failed)

        if (!cap_pa_has_positional(pa, word)) FB(failed)
        if (!cap_tu_is_string(cap_pa_get_positional(pa, word))) FB(failed)
        if (strcmp(abcd, cap_tu_as_string(cap_pa_get_positional(pa, word))))

        if (!cap_pa_has_positional(pa, another)) FB(failed)
        if (!cap_tu_is_int(cap_pa_get_positional(pa, another))) FB(failed)
        if (100 != cap_tu_as_int(cap_pa_get_positional(pa, another))) FB(failed)
    } while(false);

    cap_pa_destroy(pa);
    return !failed;
}

/**
 * Test parsing error
 * 
 * Positionals are mixed with flags. One of them begins with '-' and no flag
 * separator was present before it. Therefore, the parser considers it an 
 * unknown flag.
 */
bool test_flags_positionals_fail() {
    ArgumentParser * p = cap_parser_make_default();
    const char * a = "-a";
    const char * b = "-b";
    const char * c = "-c";
    const char * word = "word";
    const char * another = "another";

    cap_parser_add_flag(p, a, DT_PRESENCE, 0, 1, NULL, NULL);
    cap_parser_add_positional(p, word, DT_STRING, "WORD", NULL);
    cap_parser_add_flag(p, b, DT_DOUBLE, 0, -1, "B", NULL);
    cap_parser_add_flag(p, c, DT_STRING, 1, 2, "P", NULL);
    cap_parser_add_positional(p, another, DT_INT, "ANOTHER", NULL);

    const char * string = "string";
    const char * anotherstring = "anotherstring";
    const char * abcd = "abcd";

    const char * args[13] = {"prog", b, "0", c, string, c, anotherstring, b, "-1", b, "-100", abcd, "-100"};

    ParsingResult res = cap_parser_parse_noexit(p, 13, args);
    cap_parser_destroy(p);
    ParsedArguments * pa = res.mArguments;
    bool failed = false;
    do {
        if (res.mError != PER_UNKNOWN_FLAG) FB(failed)
        if (pa) FB(failed)
    } while(false);

    cap_pa_destroy(pa);
    return !failed;
}

/**
 * Test parsing error
 * 
 * A flag was given too many times.
 */
bool test_flags_positionals_fail_2() {
    ArgumentParser * p = cap_parser_make_default();
    const char * a = "-a";
    const char * b = "-b";
    const char * c = "-c";
    const char * word = "word";
    const char * another = "another";

    cap_parser_add_flag(p, a, DT_PRESENCE, 0, 1, NULL, NULL);
    cap_parser_add_positional(p, word, DT_STRING, "WORD", NULL);
    cap_parser_add_flag(p, b, DT_DOUBLE, 0, -1, "B", NULL);
    cap_parser_add_flag(p, c, DT_STRING, 1, 2, "P", NULL);
    cap_parser_add_positional(p, another, DT_INT, "ANOTHER", NULL);

    const char * string = "string";
    const char * anotherstring = "anotherstring";
    const char * abcd = "abcd";

    const char * args[16] = {"prog", b, "0", c, string, c, anotherstring, c, anotherstring, b, "-1", b, "-100", "--", abcd, "-100"};

    ParsingResult res = cap_parser_parse_noexit(p, 16, args);
    cap_parser_destroy(p);
    ParsedArguments * pa = res.mArguments;
    bool failed = false;
    do {
        if (res.mError != PER_TOO_MANY_FLAGS) FB(failed)
        if (pa) FB(failed)
    } while(false);

    cap_pa_destroy(pa);
    return !failed;
}

/**
 * Test parsing error.
 * 
 * A required flag was not given enough times.
 */
bool test_flags_positionals_fail_3() {
    ArgumentParser * p = cap_parser_make_default();
    const char * a = "-a";
    const char * b = "-b";
    const char * c = "-c";
    const char * word = "word";
    const char * another = "another";

    cap_parser_add_flag(p, a, DT_PRESENCE, 0, 1, NULL, NULL);
    cap_parser_add_positional(p, word, DT_STRING, "WORD", NULL);
    cap_parser_add_flag(p, b, DT_DOUBLE, 0, -1, "B", NULL);
    cap_parser_add_flag(p, c, DT_STRING, 1, 2, "P", NULL);
    cap_parser_add_positional(p, another, DT_INT, "ANOTHER", NULL);

    // const char * string = "string";
    // const char * anotherstring = "anotherstring";
    const char * abcd = "abcd";

    const char * args[9] = {"prog", b, "0", b, "-1", b, "-100", abcd, "100"};

    ParsingResult res = cap_parser_parse_noexit(p, 9, args);
    cap_parser_destroy(p);
    ParsedArguments * pa = res.mArguments;
    bool failed = false;
    do {
        if (res.mError != PER_NOT_ENOUGH_FLAGS) FB(failed)
        if (pa) FB(failed)
    } while(false);

    cap_pa_destroy(pa);
    return !failed;
}

/**
 * Test parsing error.
 * 
 * A required flag was given, but, it was after the flag separator. Therefore, 
 * the parser does not consider it a flag and fails with not enough flags.
 */
bool test_flags_positionals_fail_4() {
    ArgumentParser * p = cap_parser_make_default();
    const char * a = "-a";
    const char * b = "-b";
    const char * c = "-c";
    const char * word = "word";
    const char * another = "another";

    cap_parser_add_flag(p, a, DT_PRESENCE, 0, 1, NULL, NULL);
    cap_parser_add_positional(p, word, DT_STRING, "WORD", NULL);
    cap_parser_add_flag(p, b, DT_DOUBLE, 0, -1, "B", NULL);
    cap_parser_add_flag(p, c, DT_STRING, 1, 2, "P", NULL);
    cap_parser_add_positional(p, another, DT_INT, "ANOTHER", NULL);

    // const char * string = "string";
    // const char * anotherstring = "anotherstring";
    // const char * abcd = "abcd";

    const char * args[10] = {"prog", b, "0", b, "-1", b, "-100", "--", c, "20"};

    ParsingResult res = cap_parser_parse_noexit(p, 10, args);
    cap_parser_destroy(p);
    ParsedArguments * pa = res.mArguments;
    bool failed = false;
    do {
        if (res.mError != PER_NOT_ENOUGH_FLAGS) FB(failed)
        if (pa) FB(failed)
    } while(false);

    cap_pa_destroy(pa);
    return !failed;
}

/**
 * Test parsing error
 * 
 * Too many positionals
 */
bool test_flags_positionals_fail_5() {
    ArgumentParser * p = cap_parser_make_default();
    const char * a = "-a";
    const char * b = "-b";
    const char * c = "-c";
    const char * word = "word";
    const char * another = "another";

    cap_parser_add_flag(p, a, DT_PRESENCE, 0, 1, NULL, NULL);
    cap_parser_add_positional(p, word, DT_STRING, "WORD", NULL);
    cap_parser_add_flag(p, b, DT_DOUBLE, 0, -1, "B", NULL);
    cap_parser_add_flag(p, c, DT_STRING, 1, 2, "P", NULL);
    cap_parser_add_positional(p, another, DT_INT, "ANOTHER", NULL);

    const char * string = "string";
    const char * anotherstring = "anotherstring";
    const char * abcd = "abcd";

    const char * args[12] = {"prog", b, "0", b, "-1", b, "-100", c, string, abcd, "0", anotherstring};

    ParsingResult res = cap_parser_parse_noexit(p, 12, args);
    cap_parser_destroy(p);
    ParsedArguments * pa = res.mArguments;
    bool failed = false;
    do {
        if (res.mError != PER_TOO_MANY_POSITIONALS) FB(failed)
        if (pa) FB(failed)
    } while(false);

    cap_pa_destroy(pa);
    return !failed;
}

/**
 * Test parsing error
 * 
 * Not enough positionals were given.
 */
bool test_flags_positionals_fail_6() {
    ArgumentParser * p = cap_parser_make_default();
    const char * a = "-a";
    const char * b = "-b";
    const char * c = "-c";
    const char * word = "word";
    const char * another = "another";

    cap_parser_add_flag(p, a, DT_PRESENCE, 0, 1, NULL, NULL);
    cap_parser_add_positional(p, word, DT_STRING, "WORD", NULL);
    cap_parser_add_flag(p, b, DT_DOUBLE, 0, -1, "B", NULL);
    cap_parser_add_flag(p, c, DT_STRING, 1, 2, "P", NULL);
    cap_parser_add_positional(p, another, DT_INT, "ANOTHER", NULL);

    const char * string = "string";
    // const char * anotherstring = "anotherstring";
    const char * abcd = "abcd";

    const char * args[10] = {"prog", b, "0", abcd, b, "-1", b, "-100", c, string};

    ParsingResult res = cap_parser_parse_noexit(p, 10, args);
    cap_parser_destroy(p);
    ParsedArguments * pa = res.mArguments;
    bool failed = false;
    do {
        if (res.mError != PER_NOT_ENOUGH_POSITIONALS) FB(failed)
        if (pa) FB(failed)
    } while(false);

    cap_pa_destroy(pa);
    return !failed;
}

/**
 * Test parsing error.
 * 
 * Not enough positionals were given.
 */
bool test_flags_positionals_fail_7() {
    ArgumentParser * p = cap_parser_make_default();
    const char * a = "-a";
    const char * b = "-b";
    const char * c = "-c";
    const char * word = "word";
    const char * another = "another";

    cap_parser_add_flag(p, a, DT_PRESENCE, 0, 1, NULL, NULL);
    cap_parser_add_positional(p, word, DT_STRING, "WORD", NULL);
    cap_parser_add_flag(p, b, DT_DOUBLE, 0, -1, "B", NULL);
    cap_parser_add_flag(p, c, DT_STRING, 1, 2, "P", NULL);
    cap_parser_add_positional(p, another, DT_INT, "ANOTHER", NULL);

    const char * string = "string";
    // const char * anotherstring = "anotherstring";
    // const char * abcd = "abcd";

    const char * args[9] = {"prog", b, "0", b, "-1", b, "-100", c, string};

    ParsingResult res = cap_parser_parse_noexit(p, 9, args);
    cap_parser_destroy(p);
    ParsedArguments * pa = res.mArguments;
    bool failed = false;
    do {
        if (res.mError != PER_NOT_ENOUGH_POSITIONALS) FB(failed)
        if (pa) FB(failed)
    } while(false);

    cap_pa_destroy(pa);
    return !failed;
}

/**
 * Test parsing error.
 * 
 * An invalid value was given for an INT positional. Parsing should fail with 
 * cannot parse Positional.
 */
bool test_flags_positionals_fail_8() {
    ArgumentParser * p = cap_parser_make_default();
    const char * a = "-a";
    const char * b = "-b";
    const char * c = "-c";
    const char * word = "word";
    const char * another = "another";

    cap_parser_add_flag(p, a, DT_PRESENCE, 0, 1, NULL, NULL);
    cap_parser_add_positional(p, word, DT_STRING, "WORD", NULL);
    cap_parser_add_flag(p, b, DT_DOUBLE, 0, -1, "B", NULL);
    cap_parser_add_flag(p, c, DT_STRING, 1, 2, "P", NULL);
    cap_parser_add_positional(p, another, DT_INT, "ANOTHER", NULL);

    const char * string = "string";
    // const char * anotherstring = "anotherstring";
    const char * abcd = "abcd";

    const char * args[11] = {"prog", b, "0", abcd, abcd, b, "-1", b, "-100", c, string};

    ParsingResult res = cap_parser_parse_noexit(p, 11, args);
    cap_parser_destroy(p);
    ParsedArguments * pa = res.mArguments;
    bool failed = false;
    do {
        if (res.mError != PER_CANNOT_PARSE_POSITIONAL) FB(failed)
        if (pa) FB(failed)
    } while(false);

    cap_pa_destroy(pa);
    return !failed;
}

/**
 * Test parsing error
 * 
 * An invalid value is given for a DOUBLE flag. Parsing should fail with cannot
 * parse flag.
 */
bool test_flags_positionals_fail_9() {
    ArgumentParser * p = cap_parser_make_default();
    const char * a = "-a";
    const char * b = "-b";
    const char * c = "-c";
    const char * word = "word";
    const char * another = "another";

    cap_parser_add_flag(p, a, DT_PRESENCE, 0, 1, NULL, NULL);
    cap_parser_add_positional(p, word, DT_STRING, "WORD", NULL);
    cap_parser_add_flag(p, b, DT_DOUBLE, 0, -1, "B", NULL);
    cap_parser_add_flag(p, c, DT_STRING, 1, 2, "P", NULL);
    cap_parser_add_positional(p, another, DT_INT, "ANOTHER", NULL);

    const char * string = "string";
    // const char * anotherstring = "anotherstring";
    const char * abcd = "abcd";

    const char * args[11] = {"prog", b, abcd, abcd, abcd, b, "-1", b, "-100", c, string};

    ParsingResult res = cap_parser_parse_noexit(p, 11, args);
    cap_parser_destroy(p);
    ParsedArguments * pa = res.mArguments;
    bool failed = false;
    do {
        if (res.mError != PER_CANNOT_PARSE_FLAG) FB(failed)
        if (pa) FB(failed)
    } while(false);

    cap_pa_destroy(pa);
    return !failed;
}

/**
 * Test parsing error
 * 
 * The optional flag -a was given, but, it was after the flag separator. 
 * Therefore, the parser considers it a positional and fails with too many 
 * positionals.
 */
bool test_flags_positionals_fail_10() {
    ArgumentParser * p = cap_parser_make_default();
    const char * a = "-a";
    const char * b = "-b";
    const char * c = "-c";
    const char * word = "word";
    const char * another = "another";

    cap_parser_add_flag(p, a, DT_PRESENCE, 0, 1, NULL, NULL);
    cap_parser_add_positional(p, word, DT_STRING, "WORD", NULL);
    cap_parser_add_flag(p, b, DT_DOUBLE, 0, -1, "B", NULL);
    cap_parser_add_flag(p, c, DT_STRING, 1, 2, "P", NULL);
    cap_parser_add_positional(p, another, DT_INT, "ANOTHER", NULL);

    const char * string = "string";
    // const char * anotherstring = "anotherstring";
    const char * abcd = "abcd";

    const char * args[13] = {"prog", b, "0", b, "-1", b, "-100", c, string, abcd, "0", "--", a};

    ParsingResult res = cap_parser_parse_noexit(p, 13, args);
    cap_parser_destroy(p);
    ParsedArguments * pa = res.mArguments;
    bool failed = false;
    do {
        if (res.mError != PER_TOO_MANY_POSITIONALS) FB(failed)
        if (pa) FB(failed)
    } while(false);

    cap_pa_destroy(pa);
    return !failed;
}

int main() {
    bool a, b;
    a = TEST_GROUP(
        "parser: f+p", false, false, test_flags_positionals, 
        test_flags_positionals_2, test_flags_positionals_3, 
        test_flags_positionals_4, test_flags_positionals_5);
    b = TEST_GROUP(
        "parser: f+p fails", false, false, test_flags_positionals_fail, 
        test_flags_positionals_fail_2, test_flags_positionals_fail_3, 
        test_flags_positionals_fail_4, test_flags_positionals_fail_5,
        test_flags_positionals_fail_6, test_flags_positionals_fail_7,
        test_flags_positionals_fail_8, test_flags_positionals_fail_9,
        test_flags_positionals_fail_10);
    return a && b ? 0 : 1;
}