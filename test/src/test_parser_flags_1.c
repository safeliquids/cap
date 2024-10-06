#include "parser.h"
#include "parsed_arguments.h"
#include "typed_union.h"

#include "test.h"

/*
 * One optional flag that can be given up to once.
 */
bool test_parse_flag() {
    ArgumentParser * parser = cap_parser_make_empty();
    const char * sus_flag = "--sus";
    const char * a[2] = {"program", sus_flag};
    ParsedArguments * pa_1 = NULL,
                    * pa_2 = NULL;

    cap_parser_add_flag(parser, sus_flag, DT_PRESENCE, 0, 1, NULL, NULL);
    pa_1 = cap_parser_parse(parser, 1, a);
    if (cap_pa_has_flag(pa_1, sus_flag)) goto fail;

    pa_2 = cap_parser_parse(parser, 2, a);
    if (!cap_pa_has_flag(pa_2, sus_flag)) goto fail;
    if (cap_pa_flag_count(pa_2, sus_flag) != 1) goto fail;
    if (!cap_pa_get_flag(pa_2, sus_flag)) goto fail;
    if (!cap_tu_is_presence(cap_pa_get_flag(pa_2, sus_flag))) goto fail;


    cap_parser_destroy(parser);
    cap_pa_destroy(pa_1);
    cap_pa_destroy(pa_2);
    return true;
fail:
    cap_parser_destroy(parser);
    cap_pa_destroy(pa_1);
    cap_pa_destroy(pa_2);
    return false;
}

/*
 * One optional flag of type double that can be given up to once.
 */
bool test_parse_double_flag() {
    ArgumentParser * parser = cap_parser_make_empty();
    const char * sus_flag = "--sus";
    const char * a[3] = {"program", sus_flag, "-100"};
    ParsedArguments * pa_1 = NULL,
                    * pa_2 = NULL,
                    * pa_3 = NULL;

    cap_parser_add_flag(parser, sus_flag, DT_DOUBLE, 0, 1, NULL, NULL);
    
    ParsingResult res = cap_parser_parse_noexit(parser, 1, a);
    pa_1 = res.mArguments;
    if (res.mError != PER_NO_ERROR) goto fail;
    if (!res.mArguments) goto fail;
    if (cap_pa_has_flag(pa_1, sus_flag)) goto fail;

    res = cap_parser_parse_noexit(parser, 2, a);
    pa_2 = res.mArguments;
    if (res.mError == PER_NO_ERROR) goto fail;
    if (res.mArguments) goto fail;

    res = cap_parser_parse_noexit(parser, 3, a);
    pa_3 = res.mArguments;
    if (res.mError != PER_NO_ERROR) goto fail;
    if (!res.mArguments) goto fail;
    if (!cap_pa_has_flag(pa_3, sus_flag)) goto fail;
    if (cap_pa_flag_count(pa_3, sus_flag) != 1) goto fail;
    if (!cap_pa_get_flag(pa_3, sus_flag)) goto fail;
    if (!cap_tu_is_double(cap_pa_get_flag(pa_3, sus_flag))) goto fail;
    if (cap_tu_as_double(cap_pa_get_flag(pa_3, sus_flag)) != -100) goto fail;

    cap_parser_destroy(parser);
    cap_pa_destroy(pa_1);
    cap_pa_destroy(pa_2);
    cap_pa_destroy(pa_3);
    return true;
fail:
    cap_parser_destroy(parser);
    cap_pa_destroy(pa_1);
    cap_pa_destroy(pa_2);
    cap_pa_destroy(pa_3);
    return false;
}

/*
 * One optional flag of type int that can be given up to once.
 */
bool test_parse_int_flag() {
    ArgumentParser * parser = cap_parser_make_empty();
    const char * sus_flag = "--sus";
    const char * a[3] = {"program", sus_flag, "-100"};
    ParsedArguments * pa_1 = NULL,
                    * pa_2 = NULL,
                    * pa_3 = NULL;

    cap_parser_add_flag(parser, sus_flag, DT_INT, 0, 1, NULL, NULL);
    
    ParsingResult res = cap_parser_parse_noexit(parser, 1, a);
    pa_1 = res.mArguments;
    if (res.mError != PER_NO_ERROR) goto fail;
    if (!res.mArguments) goto fail;
    if (cap_pa_has_flag(pa_1, sus_flag)) goto fail;

    res = cap_parser_parse_noexit(parser, 2, a);
    pa_2 = res.mArguments;
    if (res.mError == PER_NO_ERROR) goto fail;
    if (res.mArguments) goto fail;

    res = cap_parser_parse_noexit(parser, 3, a);
    pa_3 = res.mArguments;
    if (res.mError != PER_NO_ERROR) goto fail;
    if (!res.mArguments) goto fail;
    if (!cap_pa_has_flag(pa_3, sus_flag)) goto fail;
    if (cap_pa_flag_count(pa_3, sus_flag) != 1) goto fail;
    if (!cap_pa_get_flag(pa_3, sus_flag)) goto fail;
    if (!cap_tu_is_int(cap_pa_get_flag(pa_3, sus_flag))) goto fail;
    if (cap_tu_as_int(cap_pa_get_flag(pa_3, sus_flag)) != -100) goto fail;

    cap_parser_destroy(parser);
    cap_pa_destroy(pa_1);
    cap_pa_destroy(pa_2);
    cap_pa_destroy(pa_3);
    return true;
fail:
    cap_parser_destroy(parser);
    cap_pa_destroy(pa_1);
    cap_pa_destroy(pa_2);
    cap_pa_destroy(pa_3);
    return false;
}

/*
 * One optional flag of type string that can be given up to once.
 */
bool test_parse_string_flag() {
    ArgumentParser * parser = cap_parser_make_empty();
    const char * sus_flag = "--sus";
    const char * a[3] = {"program", sus_flag, "-100"};
    ParsedArguments * pa_1 = NULL,
                    * pa_2 = NULL,
                    * pa_3 = NULL;

    cap_parser_add_flag(parser, sus_flag, DT_STRING, 0, 1, NULL, NULL);
    
    ParsingResult res = cap_parser_parse_noexit(parser, 1, a);
    pa_1 = res.mArguments;
    if (res.mError != PER_NO_ERROR) goto fail;
    if (!res.mArguments) goto fail;
    if (cap_pa_has_flag(pa_1, sus_flag)) goto fail;

    res = cap_parser_parse_noexit(parser, 2, a);
    pa_2 = res.mArguments;
    if (res.mError == PER_NO_ERROR) goto fail;
    if (res.mArguments) goto fail;

    res = cap_parser_parse_noexit(parser, 3, a);
    pa_3 = res.mArguments;
    if (res.mError != PER_NO_ERROR) goto fail;
    if (!res.mArguments) goto fail;
    if (!cap_pa_has_flag(pa_3, sus_flag)) goto fail;
    if (cap_pa_flag_count(pa_3, sus_flag) != 1) goto fail;
    if (!cap_pa_get_flag(pa_3, sus_flag)) goto fail;
    if (!cap_tu_is_string(cap_pa_get_flag(pa_3, sus_flag))) goto fail;
    if (strcmp(cap_tu_as_string(cap_pa_get_flag(pa_3, sus_flag)), "-100")) goto fail;

    cap_parser_destroy(parser);
    cap_pa_destroy(pa_1);
    cap_pa_destroy(pa_2);
    cap_pa_destroy(pa_3);
    return true;
fail:
    cap_parser_destroy(parser);
    cap_pa_destroy(pa_1);
    cap_pa_destroy(pa_2);
    cap_pa_destroy(pa_3);
    return false;
}

/*
 * One required flag that can be given up to one time.
 * Tests that 
 * - parsing fails without the flag
 * - if given, it is stored correctly
 */
bool test_parse_required_flag() {
    ArgumentParser * parser = cap_parser_make_empty();
    const char * sus_flag = "--sus";
    const char * a[2] = {"program", sus_flag};
    ParsedArguments * pa_1 = NULL,
                    * pa_2 = NULL;

    cap_parser_add_flag(parser, sus_flag, DT_PRESENCE, 1, 1, NULL, NULL);
    ParsingResult res = cap_parser_parse_noexit(parser, 1, a);
    pa_1 = res.mArguments;
    if (res.mError == PER_NO_ERROR) goto fail;
    if (res.mArguments) goto fail;

    res = cap_parser_parse_noexit(parser, 2, a);
    pa_2 = res.mArguments;
    if (res.mError != PER_NO_ERROR) goto fail;
    if (!res.mArguments) goto fail;
    
    if (!cap_pa_has_flag(pa_2, sus_flag)) goto fail;
    if (cap_pa_flag_count(pa_2, sus_flag) != 1) goto fail;
    if (!cap_pa_get_flag(pa_2, sus_flag)) goto fail;
    if (!cap_tu_is_presence(cap_pa_get_flag(pa_2, sus_flag))) goto fail;

    cap_parser_destroy(parser);
    cap_pa_destroy(pa_1);
    cap_pa_destroy(pa_2);
    return true;
fail:
    cap_parser_destroy(parser);
    cap_pa_destroy(pa_1);
    cap_pa_destroy(pa_2);
    return false;
}

/*
 * One flag that must be given exactly once.
 * Tests that
 * - parsing fails if flag is not present
 * - parsing succeeds if flag is given once
 * - parsing fails if it is given twice
 */
bool test_parse_flag_with_count_checking() {
    ArgumentParser * parser = cap_parser_make_empty();
    const char * sus_flag = "--sus";
    const char * a[3] = {"program", sus_flag, sus_flag};
    ParsedArguments * pa_1 = NULL,
                    * pa_2 = NULL,
                    * pa_3 = NULL;

    cap_parser_add_flag(parser, sus_flag, DT_PRESENCE, 1, 1, NULL, NULL);
    ParsingResult res = cap_parser_parse_noexit(parser, 1, a);
    pa_1 = res.mArguments;
    if (res.mError == PER_NO_ERROR) goto fail;
    if (res.mArguments) goto fail;

    res = cap_parser_parse_noexit(parser, 2, a);
    pa_2 = res.mArguments;
    if (res.mError != PER_NO_ERROR) goto fail;
    if (!res.mArguments) goto fail;
    
    if (!cap_pa_has_flag(pa_2, sus_flag)) goto fail;
    if (cap_pa_flag_count(pa_2, sus_flag) != 1) goto fail;
    if (!cap_pa_get_flag(pa_2, sus_flag)) goto fail;
    if (!cap_tu_is_presence(cap_pa_get_flag(pa_2, sus_flag))) goto fail;

    res = cap_parser_parse_noexit(parser, 3, a);
    pa_3 = res.mArguments;
    if (res.mError == PER_NO_ERROR) goto fail;
    if (res.mArguments) goto fail;

    cap_parser_destroy(parser);
    cap_pa_destroy(pa_1);
    cap_pa_destroy(pa_2);
    cap_pa_destroy(pa_3);
    return true;
fail:
    cap_parser_destroy(parser);
    cap_pa_destroy(pa_1);
    cap_pa_destroy(pa_2);
    cap_pa_destroy(pa_3);
    return false;
}



/*
 * One required flag of type int that can be given up to one time.
 * Tests that 
 * - parsing fails without the flag
 * - if given, it is stored correctly
 */
bool test_parse_required_int_flag() {
    ArgumentParser * parser = cap_parser_make_empty();
    const char * sus_flag = "--sus";
    const char * a[3] = {"program", sus_flag, "333"};
    ParsedArguments * pa_1 = NULL,
                    * pa_2 = NULL,
                    * pa_3 = NULL;

    cap_parser_add_flag(parser, sus_flag, DT_INT, 1, 1, NULL, NULL);
    ParsingResult res = cap_parser_parse_noexit(parser, 1, a);
    pa_1 = res.mArguments;
    if (res.mError == PER_NO_ERROR) goto fail;
    if (res.mArguments) goto fail;

    res = cap_parser_parse_noexit(parser, 2, a);
    pa_2 = res.mArguments;
    if (res.mError == PER_NO_ERROR) goto fail;
    if (res.mArguments) goto fail;
    
    res = cap_parser_parse_noexit(parser, 3, a);
    pa_3 = res.mArguments;
    if (res.mError != PER_NO_ERROR) goto fail;
    if (!res.mArguments) goto fail;
    if (!cap_pa_has_flag(pa_3, sus_flag)) goto fail;
    if (cap_pa_flag_count(pa_3, sus_flag) != 1) goto fail;
    if (!cap_pa_get_flag(pa_3, sus_flag)) goto fail;
    if (!cap_tu_is_int(cap_pa_get_flag(pa_3, sus_flag))) goto fail;
    if (cap_tu_as_int(cap_pa_get_flag(pa_3, sus_flag)) != 333) goto fail;

    cap_parser_destroy(parser);
    cap_pa_destroy(pa_1);
    cap_pa_destroy(pa_2);
    cap_pa_destroy(pa_3);
    return true;
fail:
    cap_parser_destroy(parser);
    cap_pa_destroy(pa_1);
    cap_pa_destroy(pa_2);
    cap_pa_destroy(pa_3);
    return false;
}

/*
 * One flag of type string that must be given exactly once.
 * Tests that
 * - parsing fails if flag is not present
 * - parsing succeeds if flag is given once
 * - parsing fails if it is given twice
 */
bool test_parse_string_flag_with_count_checking() {
    ArgumentParser * parser = cap_parser_make_empty();
    const char * sus_flag = "--sus";
    const char * sus_value = "abc";
    const char * another_value = "def";
    const char * a[5] = {"program", sus_flag, sus_value, sus_flag, another_value};
    ParsedArguments * pa_1 = NULL,
                    * pa_2 = NULL,
                    * pa_3 = NULL;

    cap_parser_add_flag(parser, sus_flag, DT_STRING, 1, 1, NULL, NULL);
    ParsingResult res = cap_parser_parse_noexit(parser, 1, a);
    pa_1 = res.mArguments;
    if (res.mError == PER_NO_ERROR) goto fail;
    if (res.mArguments) goto fail;

    res = cap_parser_parse_noexit(parser, 3, a);
    pa_2 = res.mArguments;
    if (res.mError != PER_NO_ERROR) goto fail;
    if (!res.mArguments) goto fail;
    
    if (!cap_pa_has_flag(pa_2, sus_flag)) goto fail;
    if (cap_pa_flag_count(pa_2, sus_flag) != 1) goto fail;
    if (!cap_pa_get_flag(pa_2, sus_flag)) goto fail;
    if (!cap_tu_is_string(cap_pa_get_flag(pa_2, sus_flag))) goto fail;
    if (strcmp(cap_tu_as_string(cap_pa_get_flag(pa_2, sus_flag)), sus_value)) goto fail;

    res = cap_parser_parse_noexit(parser, 5, a);
    pa_3 = res.mArguments;
    if (res.mError == PER_NO_ERROR) goto fail;
    if (res.mArguments) goto fail;

    cap_parser_destroy(parser);
    cap_pa_destroy(pa_1);
    cap_pa_destroy(pa_2);
    cap_pa_destroy(pa_3);
    return true;
fail:
    cap_parser_destroy(parser);
    cap_pa_destroy(pa_1);
    cap_pa_destroy(pa_2);
    cap_pa_destroy(pa_3);
    return false;
}

bool test_more_count_checking() {
    const char * flag = "--num";
    const int numbers[10] = {10, 5, -1, 0, -1, -1, 8, 100, 101, -100};
    const char * a[21] = {
        "program", "--num", "10", "--num", "5", "--num", "-1", "--num", "0", 
        "--num", "-1", "--num", "-1", "--num", "8", "--num", "100",
        "--num", "101", "--num", "-100"};
    
    static const int lower_bound = 3;
    static const int upper_bound = 9;
    
    ArgumentParser * parser = cap_parser_make_empty();
    cap_parser_add_flag(parser, flag, DT_INT, lower_bound, upper_bound, NULL, NULL);

    ParsedArguments * pa = NULL;

    bool failed = false;

    for (int nums = 0; nums < 11; ++nums) {
        ParsingResult res = cap_parser_parse_noexit(parser, nums * 2 + 1, a);
        pa = res.mArguments;
        if ((nums < lower_bound || nums > upper_bound) && res.mError == PER_NO_ERROR) {
            failed = true;
            break;
        }
        if (nums >= lower_bound && nums <= upper_bound && res.mError != PER_NO_ERROR) {
            failed = true;
            break;
        }
        if (res.mError != PER_NO_ERROR && res.mArguments) {
            failed = true;
            break;
        }
        if (res.mError == PER_NO_ERROR && !res.mArguments) {
            failed = true;
            break;
        }
        if (res.mError != PER_NO_ERROR) {
            continue;
        }
        // check stored values
        if (nums > 0 && !cap_pa_has_flag(pa, flag)) {
            failed = true;
            break;
        }
        int fc = cap_pa_flag_count(pa, flag);
        if (fc != nums) {
            failed = true;
            break;
        }
        for (int j = 0; j < fc; ++j) {
            if (!cap_tu_is_int(cap_pa_get_flag_i(pa, flag, j))) {
                failed = true;
                break;
            }
            if (cap_tu_as_int(cap_pa_get_flag_i(pa, flag, j)) != numbers[j]) {
                failed = true;
                break;
            }
        }
        if (failed) break;
        cap_pa_destroy(pa);
        pa = NULL;
    }
    cap_pa_destroy(pa);
    cap_parser_destroy(parser);

    return !failed;
}

int main() {
    bool a, b;
    a = TEST_GROUP(
        "parser-f1: single flag", false, false, test_parse_flag,
        test_parse_double_flag, test_parse_int_flag, test_parse_string_flag);
    b = TEST_GROUP(
        "parser-f1: flag count checking", false, false,
        test_parse_required_flag, test_parse_flag_with_count_checking,
        test_parse_required_int_flag,
        test_parse_string_flag_with_count_checking,
        test_more_count_checking);
    return a && b ? 0 : 1;
}