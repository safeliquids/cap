#include "cap.h"
#include "test.h"

/*
 * Test two optional flags, with no flags being given as arguments.
 * This should work no problem and create and empty ParsedArguments
 */

bool test_multiple_optional_flags_1() {
    const char * flag1 = "--one",
               * flag2 = "--two";
    ArgumentParser * parser = cap_parser_make_empty();
    ParsedArguments * pa = NULL;

    cap_parser_add_flag(parser, flag1, DT_DOUBLE, 0, 1);
    cap_parser_add_flag(parser, flag2, DT_PRESENCE, 0, 1);

    bool failed = false;
    do {
        // case 1: no flags
        // this should succeed
        const char * a[1] = {"program"};
        ParsingResult res = cap_parser_parse_noexit(parser, 1, a);
        pa = res.mArguments;
        if (!res.mSuccess) {
            failed = true;
            break;
        }
        if (!res.mArguments) {
            failed = true;
            break;
        }
        if (cap_pa_has_flag(pa, flag1)) {failed = true; break;}
        if (cap_pa_has_flag(pa, flag2)) {failed = true; break;}
    } while (false);
    cap_parser_destroy(parser);
    cap_pa_destroy(pa);
    return !failed;
}

/*
 * Test two optional flags, with only one being given
 * This should create a ParsedArguments with only the one flag present.
 */
bool test_multiple_optional_flags_2() {
    const char * flag1 = "--one",
               * flag2 = "--two";
    ArgumentParser * parser = cap_parser_make_empty();
    ParsedArguments * pa = NULL;

    cap_parser_add_flag(parser, flag1, DT_DOUBLE, 0, 1);
    cap_parser_add_flag(parser, flag2, DT_PRESENCE, 0, 1);

    bool failed = false;
    do {
        const char * b[2] = {"prog", flag2};
        ParsingResult res = cap_parser_parse_noexit(parser, 2, b);
        pa = res.mArguments;
        if (!res.mSuccess) {
            failed = true;
            break;
        }
        if (!res.mArguments) {
            failed = true;
            break;
        }
        if (cap_pa_has_flag(pa, flag1)) {failed = true; break;}

        if (!cap_pa_has_flag(pa, flag2)) {failed = true; break;}
        if (cap_pa_flag_count(pa, flag2) != 1) {failed = true; break;}
        if (!cap_tu_is_presence(cap_pa_get_flag(pa, flag2))) {failed = true; break;}
    } while (false);
    cap_parser_destroy(parser);
    cap_pa_destroy(pa);
    return !failed;
}

/*
 * Test two optional flags with one present.
 * This should create a ParsedArguments with only that flag present and its value (double).
 */
bool test_multiple_optional_flags_3() {
    const char * flag1 = "--one",
               * flag2 = "--two";
    ArgumentParser * parser = cap_parser_make_empty();
    ParsedArguments * pa = NULL;

    cap_parser_add_flag(parser, flag1, DT_DOUBLE, 0, 1);
    cap_parser_add_flag(parser, flag2, DT_PRESENCE, 0, 1);

    bool failed = false;
    do {
        const char * b[3] = {"prog", flag1, "0"};
        ParsingResult res = cap_parser_parse_noexit(parser, 3, b);
        pa = res.mArguments;
        if (!res.mSuccess) {
            failed = true;
            break;
        }
        if (!res.mArguments) {
            failed = true;
            break;
        }
        if (!cap_pa_has_flag(pa, flag1)) {failed = true; break;}
        if (cap_pa_flag_count(pa, flag1) != 1) {failed = true; break;}
        if (!cap_tu_is_double(cap_pa_get_flag(pa, flag1))) {failed = true; break;}
        if (cap_tu_as_double(cap_pa_get_flag(pa, flag1)) != 0.0) {failed = true; break;}

        if (cap_pa_has_flag(pa, flag2)) {failed = true; break;}
    } while (false);
    cap_parser_destroy(parser);
    cap_pa_destroy(pa);
    return !failed;
}

/*
 * Test two optional flags with both being given.
 * This should create a ParsedArguments containing both flags and their values.
 */
bool test_multiple_optional_flags_4() {
    const char * flag1 = "--one",
               * flag2 = "--two";
    ArgumentParser * parser = cap_parser_make_empty();
    ParsedArguments * pa = NULL;

    cap_parser_add_flag(parser, flag1, DT_DOUBLE, 0, 1);
    cap_parser_add_flag(parser, flag2, DT_PRESENCE, 0, 1);

    bool failed = false;
    do {
        const char * b[4] = {"prog", flag1, "0", flag2};
        ParsingResult res = cap_parser_parse_noexit(parser, 4, b);
        pa = res.mArguments;
        if (!res.mSuccess) {
            failed = true;
            break;
        }
        if (!res.mArguments) {
            failed = true;
            break;
        }
        if (!cap_pa_has_flag(pa, flag1)) {failed = true; break;}
        if (cap_pa_flag_count(pa, flag1) != 1) {failed = true; break;}
        if (!cap_tu_is_double(cap_pa_get_flag(pa, flag1))) {failed = true; break;}
        if (cap_tu_as_double(cap_pa_get_flag(pa, flag1)) != 0.0) {failed = true; break;}

        if (!cap_pa_has_flag(pa, flag2)) {failed = true; break;}
        if (cap_pa_flag_count(pa, flag2) != 1) {failed = true; break;}
        if (!cap_tu_is_presence(cap_pa_get_flag(pa, flag2))) {failed = true; break;}
    } while (false);
    cap_parser_destroy(parser);
    cap_pa_destroy(pa);
    return !failed;
}

/* 
 * Test two optional flags, with an unknown flag given.
 * This should fail. In normal application, the function cap_parser_parse()
 * should be used instead of cap_parser_parse_noexit(). The noexit function
 * exists primarily for unit testing purposes.
 */
bool test_multiple_optional_flags_5() {
    const char * flag1 = "--one",
               * flag2 = "--two";
    ArgumentParser * parser = cap_parser_make_empty();
    ParsedArguments * pa = NULL;

    cap_parser_add_flag(parser, flag1, DT_DOUBLE, 0, 1);
    cap_parser_add_flag(parser, flag2, DT_PRESENCE, 0, 1);

    bool failed = false;
    do {
        const char * a[4] = {"prog", "--three", "0", flag2};
        ParsingResult res = cap_parser_parse_noexit(parser, 4, a);
        pa = res.mArguments;
        if (res.mSuccess) { failed = true; break; }
        if (res.mArguments) { failed = true; break; }
    } while (false);
    cap_parser_destroy(parser);
    cap_pa_destroy(pa);
    return !failed;
}

/*
 * Two optional flags, where one is given too many times
 * This should fail;
 */
bool test_multiple_optional_flags_6() {
    const char * flag1 = "--one",
               * flag2 = "--two";
    ArgumentParser * parser = cap_parser_make_empty();
    ParsedArguments * pa = NULL;

    cap_parser_add_flag(parser, flag1, DT_DOUBLE, 0, 1);
    cap_parser_add_flag(parser, flag2, DT_PRESENCE, 0, 1);

    bool failed = false;
    do {
        const char * a[6] = {"prog", flag1, "0", flag2, flag1, "1"};
        ParsingResult res = cap_parser_parse_noexit(parser, 6, a);
        pa = res.mArguments;
        if (res.mSuccess) {
            failed = true;
            break;
        }
        if (res.mArguments) {failed = true; break;}
    } while (false);
    cap_parser_destroy(parser);
    cap_pa_destroy(pa);
    return !failed;
}

/*
 * One required and one optional flag, with one given too many times
 * This should fail.
 */
bool test_multiple_optional_flags_7() {
    const char * flag1 = "--one",
               * flag2 = "--two";
    ArgumentParser * parser = cap_parser_make_empty();
    ParsedArguments * pa = NULL;

    cap_parser_add_flag(parser, flag1, DT_DOUBLE, 1, 2);
    cap_parser_add_flag(parser, flag2, DT_PRESENCE, 0, 1);

    bool failed = false;
    do {
        // case 7: flag too many times again
        const char * a[8] = {"prog", flag1, "0", flag2, flag1, "1", flag1, "2"};
        ParsingResult res = cap_parser_parse_noexit(parser, 8, a);
        pa = res.mArguments;
        if (res.mSuccess) {
            failed = true;
            break;
        }
        if (res.mArguments) {failed = true; break;}
    } while (false);
    cap_parser_destroy(parser);
    cap_pa_destroy(pa);
    return !failed;
}

/*
 * Test one optional and one required flag, with the optional flag given
 * too many times.
 * This should fail.
 */
bool test_multiple_optional_flags_8() {
    const char * flag1 = "--one",
               * flag2 = "--two";
    ArgumentParser * parser = cap_parser_make_empty();
    ParsedArguments * pa = NULL;

    cap_parser_add_flag(parser, flag1, DT_DOUBLE, 1, 2);
    cap_parser_add_flag(parser, flag2, DT_PRESENCE, 0, 1);

    bool failed = false;
    do {
        const char * a[7] = {"prog", flag1, "0", flag2, flag1, "1", flag2};
        ParsingResult res = cap_parser_parse_noexit(parser, 7, a);
        pa = res.mArguments;
        if (res.mSuccess) {
            failed = true;
            break;
        }
        if (res.mArguments) {failed = true; break;}
    } while (false);
    cap_parser_destroy(parser);
    cap_pa_destroy(pa);
    return !failed;
}

/*
 * One required and one optional flag with the required flag absent.
 * This should fail.
 */
bool test_multiple_optional_flags_9() {
    const char * flag1 = "--one",
               * flag2 = "--two";
    ArgumentParser * parser = cap_parser_make_empty();
    ParsedArguments * pa = NULL;

    cap_parser_add_flag(parser, flag1, DT_DOUBLE, 1, 2);
    cap_parser_add_flag(parser, flag2, DT_PRESENCE, 0, 1);

    bool failed = false;
    do {
        const char * a[8] = {"prog", flag2};
        ParsingResult res = cap_parser_parse_noexit(parser, 2, a);
        pa = res.mArguments;
        if (res.mSuccess) {
            failed = true;
            break;
        }
        if (res.mArguments) {failed = true; break;}
    } while (false);
    cap_parser_destroy(parser);
    cap_pa_destroy(pa);
    return !failed;
}

/*
 * Two flags with minimum and maximum count requirements. One is given too few times.
 * This should fail.
 */
bool test_multiple_optional_flags_10() {
    const char * flag1 = "--one",
               * flag2 = "--two";
    ArgumentParser * parser = cap_parser_make_empty();
    ParsedArguments * pa = NULL;

    cap_parser_add_flag(parser, flag1, DT_DOUBLE, 1, 2);
    cap_parser_add_flag(parser, flag2, DT_INT, 2, 3);

    bool failed = false;
    do {
        const char * a[7] = {"prog", flag1, "0", flag1, "1", flag2, "10"};
        ParsingResult res = cap_parser_parse_noexit(parser, 7, a);
        pa = res.mArguments;
        if (res.mSuccess) {
            failed = true;
            break;
        }
        if (res.mArguments) {failed = true; break;}
    } while (false);
    cap_parser_destroy(parser);
    cap_pa_destroy(pa);
    return !failed;
}

/*
 * Two flags with minimum and maximum count requirements. The number of
 * occurences for each does not violate the constraints, therefore, this should
 * succeed. A ParsedArguments should be created with each flag and its values.
 */
bool test_multiple_optional_flags_11() {
    const char * flag1 = "--one",
               * flag2 = "--two";
    ArgumentParser * parser = cap_parser_make_empty();
    ParsedArguments * pa = NULL;

    cap_parser_add_flag(parser, flag1, DT_DOUBLE, 1, 2);
    cap_parser_add_flag(parser, flag2, DT_INT, 2, 3);

    bool failed = false;
    do {
        // case 11: good numbers, this should succeed
        const char * a[9] = {"prog", flag1, "0", flag2, "1", flag2, "10", flag2, "100"};
        ParsingResult res = cap_parser_parse_noexit(parser, 9, a);
        pa = res.mArguments;
        if (!res.mSuccess) {
            failed = true;
            break;
        }
        if (!res.mArguments) {failed = true; break;}

        if (cap_pa_flag_count(pa, flag1) != 1) {failed = true; break;}
        if (!cap_tu_is_double(cap_pa_get_flag(pa, flag1))) {failed = true; break;}
        if (cap_tu_as_double(cap_pa_get_flag(pa, flag1)) != 0.0) {failed = true; break;}

        if (cap_pa_flag_count(pa, flag2) != 3) {failed = true; break;}
        if (!cap_tu_is_int(cap_pa_get_flag_i(pa, flag2, 0))) {failed = true; break;}
        if (cap_tu_as_int(cap_pa_get_flag_i(pa, flag2, 0)) != 1) {failed = true; break;}
        if (!cap_tu_is_int(cap_pa_get_flag_i(pa, flag2, 1))) {failed = true; break;}
        if (cap_tu_as_int(cap_pa_get_flag_i(pa, flag2, 1)) != 10) {failed = true; break;}
        if (!cap_tu_is_int(cap_pa_get_flag_i(pa, flag2, 2))) {failed = true; break;}
        if (cap_tu_as_int(cap_pa_get_flag_i(pa, flag2, 2)) != 100) {failed = true; break;}

    } while (false);
    cap_parser_destroy(parser);
    cap_pa_destroy(pa);
    return !failed;
}

int main() {
    bool a;
    a = TEST_GROUP(
        "parser-f2: multiple flags with count requirements", false, false,
        test_multiple_optional_flags_1, test_multiple_optional_flags_2,
        test_multiple_optional_flags_3, test_multiple_optional_flags_4, 
        test_multiple_optional_flags_5, test_multiple_optional_flags_6, 
        test_multiple_optional_flags_7, test_multiple_optional_flags_8, 
        test_multiple_optional_flags_9, test_multiple_optional_flags_10, 
        test_multiple_optional_flags_11);
    return a ? 0 : 1;
}