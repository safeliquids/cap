#include "cap.h"
#include "test.h"

/**
 * Test that creating an alias does not invalidate a flag.
 * 
 * Create a flag and an alias for it. Check that the original flag can still
 * be parsed correctly. Parsed arguments should contain the flag with the 
 * original name.
 */
bool test_flag_with_alias_1() {
    const char * const long_flag = "--flie";
    const char * const short_flag = "-f";
    const char * const file = "file.txt";
    
    ArgumentParser * p = cap_parser_make_empty();

    cap_parser_add_flag(p, long_flag, DT_STRING, 0, 1, NULL, NULL);
    cap_parser_add_flag_alias(p, long_flag, short_flag);

    const char * args[3] = {"prog", long_flag, file};
    
    ParsingResult res = cap_parser_parse_noexit(p, 3, args);
    ParsedArguments * pa = res.mArguments;
    bool failed = false;
    do {
        if (res.mError != PER_NO_ERROR) FB(failed);
        if (!pa) FB(failed);

        if (!cap_pa_has_flag(pa, long_flag)) FB(failed);
        const TypedUnion * file_name_tu = cap_pa_get_flag(pa, long_flag);
        if (!file_name_tu) FB(failed);
        
        if (!cap_tu_is_string(file_name_tu)) FB(failed);
        if (strcmp(cap_tu_as_string(file_name_tu), file)) FB(failed);
    } while(false);

    if (pa) {
        cap_pa_destroy(pa);
    }
    cap_parser_destroy(p);
    return !failed;
}

/**
 * Test that alias can be parsed.
 * 
 * Create a flag and an alias for it. Check that the alias can
 * be parsed correctly. Parsed arguments should contain the flag with the 
 * original name.
 */
bool test_flag_with_alias_2() {
    static const char * const long_flag = "--file";
    static const char * const short_flag = "-f";
    const char * const file = "file.txt";
    
    ArgumentParser * p = cap_parser_make_empty();

    cap_parser_add_flag(p, long_flag, DT_STRING, 0, 1, NULL, NULL);
    cap_parser_add_flag_alias(p, long_flag, short_flag);

    const char * args[3] = {"prog", short_flag, file};

    ParsingResult res = cap_parser_parse_noexit(p, 3, args);
    ParsedArguments * pa = res.mArguments;
    bool failed = false;
    do {
        if (res.mError != PER_NO_ERROR) FB(failed);
        if (!pa) FB(failed);

        if (!cap_pa_has_flag(pa, long_flag)) FB(failed);
        const TypedUnion * file_name_tu = cap_pa_get_flag(pa, long_flag);
        if (!file_name_tu) FB(failed);
        
        if (!cap_tu_is_string(file_name_tu)) FB(failed);
        if (strcmp(cap_tu_as_string(file_name_tu), file)) FB(failed);
    } while(false);

    if (pa) {
        cap_pa_destroy(pa);
    }
    cap_parser_destroy(p);
    return !failed;
}

/**
 * Test that alias and original can be parsed.
 * 
 * Create a flag and an alias for it. Check that the alias and the original 
 * flag be parsed correctly. Parsed arguments should contain the flag with the
 * original name and two values.
 */
bool test_flag_with_alias_3() {
    const char * const long_flag = "--flie";
    const char * const short_flag = "-f";
    const char * const file = "file.txt";
    const char * const file2 = "file2.txt";
    
    ArgumentParser * p = cap_parser_make_empty();

    cap_parser_add_flag(p, long_flag, DT_STRING, 0, 2, NULL, NULL);
    cap_parser_add_flag_alias(p, long_flag, short_flag);

    const char * args[5] = {"prog", long_flag, file, short_flag, file2};

    ParsingResult res = cap_parser_parse_noexit(p, 5, args);
    ParsedArguments * pa = res.mArguments;
    bool failed = false;
    do {
        if (res.mError != PER_NO_ERROR) FB(failed);
        if (!pa) FB(failed);

        if (!cap_pa_has_flag(pa, long_flag)) FB(failed);
        if (cap_pa_flag_count(pa, long_flag) != 2u) FB(failed);
        const TypedUnion * file_name_tu_1 = cap_pa_get_flag(pa, long_flag);
        const TypedUnion * file_name_tu_2 = cap_pa_get_flag_i(pa, long_flag, 1);
        if (!file_name_tu_1) FB(failed);
        if (!file_name_tu_2) FB(failed);
        
        if (!cap_tu_is_string(file_name_tu_1)) FB(failed);
        if (strcmp(cap_tu_as_string(file_name_tu_1), file)) FB(failed);

        if (!cap_tu_is_string(file_name_tu_2)) FB(failed);
        if (strcmp(cap_tu_as_string(file_name_tu_2), file2)) FB(failed);
    } while(false);

    if (pa) {
        cap_pa_destroy(pa);
    }
    cap_parser_destroy(p);
    return !failed;
}

/**
 * Test that multiple aliases can be parsed.
 * 
 * Create a flag and two different aliases. Check that each of the aliases can
 * be parsed any number of times, and are stored under the original flag name.
 */
bool test_flag_with_alias_4() {
    const char * const long_flag = "--flie";
    const char * const mid_flag = "--fi";
    const char * const short_flag = "-f";
    const char * const file1 = "file1.txt";
    const char * const file2 = "file2.txt";
    const char * const file3 = "file3.txt";
    const char * const file4 = "file4.txt";
    
    ArgumentParser * p = cap_parser_make_empty();

    cap_parser_add_flag(p, long_flag, DT_STRING, 0, -1, NULL, NULL);
    cap_parser_add_flag_alias(p, long_flag, mid_flag);
    cap_parser_add_flag_alias(p, long_flag, short_flag);

    const char * args[9] = {
        "prog", short_flag, file1, mid_flag, file2, mid_flag, file3, 
        short_flag, file4};

    ParsingResult res = cap_parser_parse_noexit(p, 9, args);
    ParsedArguments * pa = res.mArguments;
    bool failed = false;
    do {
        if (res.mError != PER_NO_ERROR) FB(failed);
        if (!pa) FB(failed);

        if (!cap_pa_has_flag(pa, long_flag)) FB(failed);
        if (cap_pa_flag_count(pa, long_flag) != 4u) FB(failed);
        const char * const expected_files[4] = {file1, file2, file3, file4};
        size_t i;
        for (i = 0; i < 4u; ++i) {
            const TypedUnion * tu = cap_pa_get_flag_i(pa, long_flag, i);
            if (!tu) FB(failed);
            if (!cap_tu_is_string(tu)) FB(failed);
            if (strcmp(cap_tu_as_string(tu), expected_files[i])) FB(failed);
        }
    } while(false);

    if (pa) {
        cap_pa_destroy(pa);
    }
    cap_parser_destroy(p);
    return !failed;
}

/**
 * Test parsing several flags and their aliases.
 * 
 * Create two different flags and an alias for each. Test that these flags 
 * and their aliases can be parsed and are stored under the main flag name.
 */
bool test_flags_and_aliases() {
    const char * const long_a = "--all";
    const char * const long_b = "--big";
    const char * const alias_a = "-a";
    const char * const alias_b = "--BIG";
    const char * const alias_bb = "--quite-bad-alias-for-big";
    const int big_number = 1001;
    const char * const big_arg = "1001";
    const int small_number = 1;
    const char * const small_arg = "1";

    ArgumentParser * p = cap_parser_make_empty();
    cap_parser_add_flag(p, long_a, DT_PRESENCE, 0, -1, NULL, NULL);
    cap_parser_add_flag_alias(p, long_a, alias_a);
    cap_parser_add_flag(p, long_b, DT_INT, 0, -1, NULL, NULL);
    cap_parser_add_flag_alias(p, long_b, alias_b);
    cap_parser_add_flag_alias(p, long_b, alias_bb);

    const char * args[15] = {
        "prog", alias_b, big_arg, alias_a, long_a, alias_a, 
        alias_bb, small_arg, alias_a, long_b, big_arg, alias_a, long_a, 
        alias_bb, big_arg};

    ParsedArguments * pa = NULL;
    bool failed = false;
    do {
        ParsingResult res = cap_parser_parse_noexit(p, 15, args);
        pa = res.mArguments;
        if (res.mError != PER_NO_ERROR) FB(failed);
        if (!pa) FB(failed);

        if (cap_pa_flag_count(pa, long_a) != 6u) FB(failed);
        size_t i;
        for (i = 0; i < 5u; ++i) {
            const TypedUnion * tu = cap_pa_get_flag_i(pa, long_a, i);
            if (!tu) FB(failed);
            if (!cap_tu_is_presence(tu)) FB(failed);
        }

        const int expected_numbers[4] = {big_number, small_number, big_number, big_number};
        if (cap_pa_flag_count(pa, long_b) != 4u) FB(failed);
        for (i = 0; i < 4u; ++i) {
            const TypedUnion * tu = cap_pa_get_flag_i(pa, long_b, i);
            if (!tu) FB(failed);
            if (!cap_tu_is_int(tu)) FB(failed);
            if (cap_tu_as_int(tu) != expected_numbers[i]) FB(failed);
        }
    } while(false);

    if (pa) {
        cap_pa_destroy(pa);
        pa = NULL;
    }
    cap_parser_destroy(p);
    return !failed;
}

int main() {
    bool a;
    a = TEST_GROUP(
        "parser-flag-alias", false, false, test_flag_with_alias_1,
        test_flag_with_alias_2, test_flag_with_alias_3, test_flag_with_alias_4,
        test_flags_and_aliases);
    return a ? 0 : 1;
}
