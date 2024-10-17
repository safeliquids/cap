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
        if (failed) break;
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
        if (failed) break;
    } while(false);

    if (pa) {
        cap_pa_destroy(pa);
        pa = NULL;
    }
    cap_parser_destroy(p);
    return !failed;
}

/**
 * Set up custom flag prefix characters and create a flag with them.
 * Then, create an alias that begins with one of the prefix characters.
 */
bool test_custom_flag_prefix_1() {
    ArgumentParser * p = cap_parser_make_empty();
    cap_parser_set_flag_prefix(p, "/");
    const char * const x_flag = "/X";
    const char * const z_flag = "/Z";
    cap_parser_add_flag(p, x_flag, DT_PRESENCE, 0, -1, NULL, NULL);
    cap_parser_add_flag_alias(p, x_flag, z_flag);

    bool failed = false;
    ParsedArguments * pa = NULL;
    do {
        const char * args[4] = {"prog", z_flag, x_flag, z_flag};
        ParsingResult res = cap_parser_parse_noexit(p, 4, args);
        pa = res.mArguments;
        if (res.mError != PER_NO_ERROR) FB(failed);
        if (!cap_pa_has_flag(pa, x_flag)) FB(failed);
        if (cap_pa_flag_count(pa, x_flag) != 3u) FB(failed);
        for (size_t i = 0u; i < 3u; ++i) {
            const TypedUnion * tu = cap_pa_get_flag_i(pa, x_flag, i);
            if (!tu) FB(failed);
            if (!cap_tu_is_presence(tu)) FB(failed);
        }
        if (failed) break;
    } while(false);
    cap_parser_destroy(p);
    cap_pa_destroy(pa);
    return !failed;
}

/**
 * Set up custom flag prefix characters and create a flag with them.
 * Then, create an alias that begins with one of the prefix characters.
 */
bool test_custom_flag_prefix_2() {
    ArgumentParser * p = cap_parser_make_empty();
    cap_parser_set_flag_prefix(p, "-+");
    const char * const x_flag = "+X";
    const char * const z_flag = "-Z";
    cap_parser_add_flag(p, x_flag, DT_PRESENCE, 0, -1, NULL, NULL);
    cap_parser_add_flag_alias(p, x_flag, z_flag);

    bool failed = false;
    ParsedArguments * pa = NULL;
    do {
        const char * args[4] = {"prog", z_flag, x_flag, z_flag};
        ParsingResult res = cap_parser_parse_noexit(p, 4, args);
        pa = res.mArguments;
        if (res.mError != PER_NO_ERROR) FB(failed);
        if (!cap_pa_has_flag(pa, x_flag)) FB(failed);
        if (cap_pa_flag_count(pa, x_flag) != 3u) FB(failed);
        for (size_t i = 0u; i < 3u; ++i) {
            const TypedUnion * tu = cap_pa_get_flag_i(pa, x_flag, i);
            if (!tu) FB(failed);
            if (!cap_tu_is_presence(tu)) FB(failed);
        }
        if (failed) break;
    } while(false);
    cap_parser_destroy(p);
    cap_pa_destroy(pa);
    return !failed;
}

/**
 * Set up custom flag prefix characters and create a flag with them.
 * Then, try to create an alias with the wrong prefix character. This should 
 * fail with AFAE_INVALID_PREFIX.
 */
bool test_custom_flag_prefix_3() {
    ArgumentParser * p = cap_parser_make_empty();
    cap_parser_set_flag_prefix(p, "+");  // notice it is only +, unlike previous test
    const char * const x_flag = "+X";
    const char * const z_flag = "-Z";
    cap_parser_add_flag(p, x_flag, DT_PRESENCE, 0, -1, NULL, NULL);

    bool failed = false;
    do {
        AddFlagAliasError e = cap_parser_add_flag_alias_noexit(p, x_flag, z_flag);
        if (e == AFAE_OK) FB(failed);
        if (e != AFAE_INVALID_PREFIX) FB(failed);
    } while(false);
    cap_parser_destroy(p);
    return !failed;
}

/**
 * Set up custom flag prefix characters and create a flag with them.
 * Then create an alias that differs from the flag with only the prefix.
 * This should succeed.
 */
bool test_custom_flag_prefix_4() {
    ArgumentParser * p = cap_parser_make_empty();
    cap_parser_set_flag_prefix(p, "+-");
    const char * const x_flag = "+X";
    const char * const alias = "-X";
    cap_parser_add_flag(p, x_flag, DT_PRESENCE, 0, -1, NULL, NULL);
    cap_parser_add_flag_alias(p, x_flag, alias);
    cap_parser_destroy(p);
    return true;
}

/**
 * Set up custom flag prefix characters. Create a flag and an alias. Then 
 * create another flag and an alias for it which only differs from the other 
 * flag's alias with the prefix. This should succeed.
 * 
 * Then, try to parse some arguments using the aliases. This should parse
 * each of them correctly, by assigning values to the correct flag.
 */
bool test_custom_flag_prefix_5() {
    ArgumentParser * p = cap_parser_make_empty();
    cap_parser_set_flag_prefix(p, "+-");
    const char * const x_flag = "+X";
    const char * const z_flag = "+Z";
    const char * const x_alias = "-XALIAS";
    const char * const z_alias = "+XALIAS";
    cap_parser_add_flag(p, x_flag, DT_PRESENCE, 0, -1, NULL, NULL);
    cap_parser_add_flag_alias(p, x_flag, x_alias);
    cap_parser_add_flag(p, z_flag, DT_DOUBLE, 0, -1, NULL, NULL);
    cap_parser_add_flag_alias(p, z_flag, z_alias);

    const char * args[6] = {"prog", z_alias, "3.14", x_alias, z_alias, "10"};
    bool failed = false;
    ParsedArguments * pa = NULL;
    do {
        ParsingResult res = cap_parser_parse_noexit(p, 6, args);
        pa = res.mArguments;
        if (res.mError != PER_NO_ERROR) FB(failed);
        if (!cap_pa_has_flag(pa, x_flag)) FB(failed);
        if (cap_pa_flag_count(pa, x_flag) != 1u) FB(failed);

        if (cap_pa_flag_count(pa, z_flag) != 2u) FB(failed);
        const double expected_numbers[2] = {3.14, 10};
        for (size_t i = 0; i < 2u; ++i) {
            const TypedUnion * tu = cap_pa_get_flag_i(pa, z_flag, i);
            if (!tu) FB(failed);
            if (!cap_tu_is_double(tu)) FB(failed);
            if (cap_tu_as_double(tu) != expected_numbers[i]) FB(failed);
        }
        if (failed) break;
    } while(false);
    cap_parser_destroy(p);
    cap_pa_destroy(pa);
    return !failed;
}

/**
 * Try to add a flag alias without giving a parser. This should fail with 
 * AFAE_MISSING_PARSER.
 */
bool test_invalid_alias_1() {
    bool failed = false;
    do {
        AddFlagAliasError e = cap_parser_add_flag_alias_noexit(NULL, "-x", "--xyz");
        if (e != AFAE_MISSING_PARSER) FB(failed);
    } while(false);
    return !failed;
}

/**
 * Try to add a flag alias without giving a flag name. This should fail with 
 * AFAE_MISSING_NAME.
 */
bool test_invalid_alias_2() {
    ArgumentParser * p = cap_parser_make_empty();
    cap_parser_add_flag(p, "-x", DT_PRESENCE, 0, -1, NULL, NULL);
    bool failed = false;
    do {
        AddFlagAliasError e = cap_parser_add_flag_alias_noexit(p, NULL, "--xyz");
        if (e != AFAE_MISSING_NAME) FB(failed);
    } while(false);
    cap_parser_destroy(p);
    return !failed;
}

/**
 * Try to add a flag alias with an empty flag name. This should fail with 
 * AFAE_MISSING_NAME.
 */
bool test_invalid_alias_2x() {
    ArgumentParser * p = cap_parser_make_empty();
    cap_parser_add_flag(p, "-x", DT_PRESENCE, 0, -1, NULL, NULL);
    bool failed = false;
    do {
        AddFlagAliasError e = cap_parser_add_flag_alias_noexit(p, "", "--xyz");
        if (e != AFAE_MISSING_NAME) FB(failed);
    } while(false);
    cap_parser_destroy(p);
    return !failed;
}

/**
 * Try to create NULL flag alias. This should fail with AFAE_MISSING_ALIAS.
 */
bool test_invalid_alias_3() {
    ArgumentParser * p = cap_parser_make_empty();
    cap_parser_add_flag(p, "-x", DT_PRESENCE, 0, -1, NULL, NULL);
    bool failed = false;
    do {
        AddFlagAliasError e = cap_parser_add_flag_alias_noexit(p, "-x", NULL);
        if (e != AFAE_MISSING_ALIAS) FB(failed);
    } while(false);
    cap_parser_destroy(p);
    return !failed;
}

/**
 * Try to create an empty flag alias. This should fail with AFAE_MISSING_ALIAS.
 */
bool test_invalid_alias_3x() {
    ArgumentParser * p = cap_parser_make_empty();
    cap_parser_add_flag(p, "-x", DT_PRESENCE, 0, 1, NULL, NULL);
    bool failed = false;
    do {
        AddFlagAliasError e = cap_parser_add_flag_alias_noexit(p, "-x", "");
        if (e != AFAE_MISSING_ALIAS) FB(failed);
    } while(false);
    cap_parser_destroy(p);
    return !failed;
}

/**
 * Try to create an alias for a flag which does not exist. This should fail 
 * with AFAE_FLAG_DOES_NOT_EXIST.
 */
bool test_invalid_alias_4() {
    ArgumentParser * p = cap_parser_make_empty();
    bool failed = false;
    do {
        AddFlagAliasError e = cap_parser_add_flag_alias_noexit(p, "-x", "-A");
        if (e != AFAE_FLAG_DOES_NOT_EXIST) FB(failed);
    } while(false);
    cap_parser_destroy(p);
    return !failed;
}

/**
 * Try to create an alias for a flag which does not exist. This should fail 
 * with AFAE_FLAG_DOES_NOT_EXIST.
 */
bool test_invalid_alias_4x() {
    ArgumentParser * p = cap_parser_make_empty();
    cap_parser_add_flag(p, "-x", DT_PRESENCE, 0, 1, NULL, NULL);
    bool failed = false;
    do {
        AddFlagAliasError e = cap_parser_add_flag_alias_noexit(p, "-y", "-A");
        if (e != AFAE_FLAG_DOES_NOT_EXIST) FB(failed);
    } while(false);
    cap_parser_destroy(p);
    return !failed;
}

/**
 * Try to create an alias for a flag using the flag's name. This 
 * should fail with AFAE_DUPLICATE_ALIAS.
 */
bool test_invalid_alias_5_1() {
    ArgumentParser * p = cap_parser_make_empty();
    cap_parser_add_flag(p, "-x", DT_PRESENCE, 0, 1, NULL, NULL);
    bool failed = false;
    do {
        AddFlagAliasError e = cap_parser_add_flag_alias_noexit(p, "-x", "-x");
        if (e != AFAE_DUPLICATE_ALIAS) FB(failed);
    } while(false);
    cap_parser_destroy(p);
    return !failed;
}

/**
 * Try to create an alias for a flag with the flag's name. This 
 * should fail with AFAE_DUPLICATE_ALIAS.
 */
bool test_invalid_alias_5_2() {
    ArgumentParser * p = cap_parser_make_empty();
    cap_parser_add_flag(p, "-x", DT_PRESENCE, 0, 1, NULL, NULL);
    cap_parser_add_flag(p, "-y", DT_PRESENCE, 0, 1, NULL, NULL);
    bool failed = false;
    do {
        AddFlagAliasError e = cap_parser_add_flag_alias_noexit(p, "-y", "-y");
        if (e != AFAE_DUPLICATE_ALIAS) FB(failed);
    } while(false);
    cap_parser_destroy(p);
    return !failed;
}

/**
 * Try to create an alias when a flag with the same name already exists. This 
 * should fail with AFAE_DUPLICATE_ALIAS.
 */
bool test_invalid_alias_5_3() {
    ArgumentParser * p = cap_parser_make_empty();
    cap_parser_add_flag(p, "-x", DT_PRESENCE, 0, 1, NULL, NULL);
    cap_parser_add_flag(p, "-y", DT_PRESENCE, 0, 1, NULL, NULL);
    bool failed = false;
    do {
        AddFlagAliasError e = cap_parser_add_flag_alias_noexit(p, "-x", "-y");
        if (e != AFAE_DUPLICATE_ALIAS) FB(failed);
    } while(false);
    cap_parser_destroy(p);
    return !failed;
}

/**
 * Try to create an alias when a flag with the same name already exists. This 
 * should fail with AFAE_DUPLICATE_ALIAS.
 */
bool test_invalid_alias_5_4() {
    ArgumentParser * p = cap_parser_make_empty();
    cap_parser_add_flag(p, "-x", DT_PRESENCE, 0, 1, NULL, NULL);
    cap_parser_add_flag(p, "-y", DT_PRESENCE, 0, 1, NULL, NULL);
    bool failed = false;
    do {
        AddFlagAliasError e = cap_parser_add_flag_alias_noexit(p, "-y", "-x");
        if (e != AFAE_DUPLICATE_ALIAS) FB(failed);
    } while(false);
    cap_parser_destroy(p);
    return !failed;
}

/**
 * Try to create an alias when another alias for the same flag with the same 
 * name exists. This should fail with AFAE_DUPLICATE_ALIAS.
 */
bool test_invalid_alias_5_5() {
    ArgumentParser * p = cap_parser_make_empty();
    cap_parser_add_flag(p, "-x", DT_PRESENCE, 0, 1, NULL, NULL);
    bool failed = false;
    do {
        AddFlagAliasError e = cap_parser_add_flag_alias_noexit(p, "-x", "-y");
        if (e != AFAE_OK) FB(failed);
        e = cap_parser_add_flag_alias_noexit(p, "-x", "-y");
        if (e != AFAE_DUPLICATE_ALIAS) FB(failed);
    } while(false);
    cap_parser_destroy(p);
    return !failed;
}

/**
 * Try to create an alias when another alias for the same flag with the same 
 * name exists. This should fail with AFAE_DUPLICATE_ALIAS.
 */
bool test_invalid_alias_5_6() {
    ArgumentParser * p = cap_parser_make_empty();
    cap_parser_add_flag(p, "-x", DT_PRESENCE, 0, 1, NULL, NULL);
    bool failed = false;
    do {
        AddFlagAliasError e = cap_parser_add_flag_alias_noexit(p, "-x", "-y");
        if (e != AFAE_OK) FB(failed);
        e = cap_parser_add_flag_alias_noexit(p, "-x", "-z");
        if (e != AFAE_OK) FB(failed);
        e = cap_parser_add_flag_alias_noexit(p, "-x", "-y");
        if (e != AFAE_DUPLICATE_ALIAS) FB(failed);
    } while(false);
    cap_parser_destroy(p);
    return !failed;
}

/**
 * Try to create an alias when another alias for the same flag with the same 
 * name exists. This should fail with AFAE_DUPLICATE_ALIAS.
 */
bool test_invalid_alias_5_7() {
    ArgumentParser * p = cap_parser_make_empty();
    cap_parser_add_flag(p, "-x", DT_PRESENCE, 0, 1, NULL, NULL);
    bool failed = false;
    do {
        AddFlagAliasError e = cap_parser_add_flag_alias_noexit(p, "-x", "-y");
        if (e != AFAE_OK) FB(failed);
        e = cap_parser_add_flag_alias_noexit(p, "-x", "-z");
        if (e != AFAE_OK) FB(failed);
        e = cap_parser_add_flag_alias_noexit(p, "-x", "-z");
        if (e != AFAE_DUPLICATE_ALIAS) FB(failed);
    } while(false);
    cap_parser_destroy(p);
    return !failed;
}

/**
 * Try to create an alias when an alias for a different flag with the same
 * name exists. This should fail with AFAE_DUPLICATE_ALIAS.
 */
bool test_invalid_alias_5_8() {
    ArgumentParser * p = cap_parser_make_empty();
    cap_parser_add_flag(p, "-x", DT_PRESENCE, 0, 1, NULL, NULL);
    cap_parser_add_flag(p, "--something", DT_PRESENCE, 0, -1, NULL, NULL);
    cap_parser_add_flag_alias(p, "-x", "--xyz");
    bool failed = false;
    do {
        AddFlagAliasError e = cap_parser_add_flag_alias_noexit(p, "--something", "-s");
        if (e != AFAE_OK) FB(failed);
        e = cap_parser_add_flag_alias_noexit(p, "--something", "--xyz");
        if (e != AFAE_DUPLICATE_ALIAS) FB(failed);
    } while(false);
    cap_parser_destroy(p);
    return !failed;
}

/**
 * Create several valid aliases for several flags. Then, try to create an 
 * alias when an alias for a different flag with the same name exists. This 
 * should fail with AFAE_DUPLICATE_ALIAS.
 */
bool test_invalid_alias_5_9() {
    ArgumentParser * p = cap_parser_make_empty();
    cap_parser_add_flag(p, "-x", DT_PRESENCE, 0, 1, NULL, NULL);
    cap_parser_add_flag(p, "--something", DT_PRESENCE, 0, -1, NULL, NULL);
    cap_parser_add_flag(p, "--all", DT_PRESENCE, 0, 1, NULL, NULL);
    cap_parser_add_flag_alias(p, "-x", "--xyz");
    cap_parser_add_flag_alias(p, "--all", "-all");
    cap_parser_add_flag_alias(p, "--all", "-a");
    bool failed = false;
    do {
        AddFlagAliasError e = cap_parser_add_flag_alias_noexit(p, "--something", "-s");
        if (e != AFAE_OK) FB(failed);
        e = cap_parser_add_flag_alias_noexit(p, "--something", "-ss");
        if (e != AFAE_OK) FB(failed);
        e = cap_parser_add_flag_alias_noexit(p, "--something", "-sss");
        if (e != AFAE_OK) FB(failed);
        e = cap_parser_add_flag_alias_noexit(p, "--something", "-ssss");
        if (e != AFAE_OK) FB(failed);
        e = cap_parser_add_flag_alias_noexit(p, "--something", "---all");
        if (e != AFAE_OK) FB(failed);
        e = cap_parser_add_flag_alias_noexit(p, "--something", "--XYZ");
        if (e != AFAE_OK) FB(failed);
        e = cap_parser_add_flag_alias_noexit(p, "-x", "-xyz");
        if (e != AFAE_OK) FB(failed);
        e = cap_parser_add_flag_alias_noexit(p, "-x", "--alllllllllll");
        if (e != AFAE_OK) FB(failed);
        e = cap_parser_add_flag_alias_noexit(p, "-x", "-a");
        if (e != AFAE_DUPLICATE_ALIAS) FB(failed);
    } while(false);
    cap_parser_destroy(p);
    return !failed;
}

/**
 * Try to add an alias that matches the flag separator. This should fail with 
 * AFAE_DUPLICATE_ALIAS.
 */
bool test_invalid_alias_5_10() {
    ArgumentParser * p = cap_parser_make_empty();
    cap_parser_set_flag_separator(p, "-++-", NULL);
    cap_parser_add_flag(p, "-x", DT_PRESENCE, 0, 1, NULL, NULL);
    bool failed = false;
    do {
        AddFlagAliasError e = cap_parser_add_flag_alias_noexit(p, "-x", "-++-");
        if (e != AFAE_DUPLICATE_ALIAS) FB(failed);
    } while(false);
    cap_parser_destroy(p);
    return !failed;
}

/**
 * Try to add an alias that matches the help flag. This should fail with 
 * AFAE_DUPLICATE_ALIAS.
 */
bool test_invalid_alias_5_11() {
    ArgumentParser * p = cap_parser_make_empty();
    cap_parser_set_help_flag(p, "-h", NULL);
    cap_parser_add_flag(p, "-x", DT_PRESENCE, 0, 1, NULL, NULL);
    bool failed = false;
    do {
        AddFlagAliasError e = cap_parser_add_flag_alias_noexit(p, "-x", "-h");
        if (e != AFAE_DUPLICATE_ALIAS) FB(failed);
    } while(false);
    cap_parser_destroy(p);
    return !failed;
}

/**
 * Create a falg and an alias for it. Then, create another flag. This should 
 * all succeed.
 */
bool test_add_flags_after_alias_1() {
    ArgumentParser * p = cap_parser_make_empty();
    cap_parser_add_flag(p, "-a", DT_PRESENCE, 0, -1, NULL, NULL);
    cap_parser_add_flag_alias(p, "-a", "-A");

    cap_parser_add_flag(p, "-b", DT_DOUBLE, 0, 1, NULL, NULL);
    return true;
}

/**
 * Create a falg and an alias for it and, create another flag. After that
 * parse some arguments using the alias and the second flag. This should
 * succeed.
 */
bool test_add_flags_after_alias_2() {
    ArgumentParser * p = cap_parser_make_empty();
    cap_parser_add_flag(p, "-a", DT_PRESENCE, 0, -1, NULL, NULL);
    cap_parser_add_flag_alias(p, "-a", "-A");
    cap_parser_add_flag(p, "-b", DT_DOUBLE, 0, 1, NULL, NULL);
    bool failed = false;
    ParsedArguments * pa = NULL;
    const char * args[4] = {"prog", "-b", "3.14", "-A"};
    do {
        ParsingResult res = cap_parser_parse_noexit(p, 4, args);
        pa = res.mArguments;
        if (res.mError != PER_NO_ERROR) FB(failed);
        if (!cap_pa_has_flag(pa, "-a")) FB(failed);
        if (!cap_pa_has_flag(pa, "-b")) FB(failed);
        if (cap_tu_is_double(cap_pa_get_flag(pa, "-b"))) FB(failed);
        if (cap_tu_as_double(cap_pa_get_flag(pa, "-b")) != 3.14) FB(failed);
    } while(false);
    cap_parser_destroy(p);
    cap_pa_destroy(pa);
    return !failed;
}

/**
 * Create a falg and an alias for it and, create another flag and an alias for
 * it as well. After that parse some arguments using the aliases. This should
 * succeed.
 */
bool test_add_flags_after_alias_3() {
    ArgumentParser * p = cap_parser_make_empty();
    cap_parser_add_flag(p, "-a", DT_PRESENCE, 0, -1, NULL, NULL);
    cap_parser_add_flag_alias(p, "-a", "-A");
    cap_parser_add_flag(p, "-b", DT_DOUBLE, 0, 1, NULL, NULL);
    cap_parser_add_flag_alias(p, "-b", "-B");
    bool failed = false;
    ParsedArguments * pa = NULL;
    const char * args[4] = {"prog", "-B", "3.14", "-A"};
    do {
        ParsingResult res = cap_parser_parse_noexit(p, 4, args);
        pa = res.mArguments;
        if (res.mError != PER_NO_ERROR) FB(failed);
        if (!cap_pa_has_flag(pa, "-a")) FB(failed);
        if (!cap_pa_has_flag(pa, "-b")) FB(failed);
        if (cap_tu_is_double(cap_pa_get_flag(pa, "-b"))) FB(failed);
        if (cap_tu_as_double(cap_pa_get_flag(pa, "-b")) != 3.14) FB(failed);
    } while(false);
    cap_parser_destroy(p);
    cap_pa_destroy(pa);
    return !failed;
}

/**
 * Create a falg and an alias for it. Then try to create another flag with the
 * same name as the alias. This should fail with AFE_DUPLICATE_FLAG.
 */
bool test_add_flags_after_alias_4() {
    ArgumentParser * p = cap_parser_make_empty();
    cap_parser_add_flag(p, "-a", DT_PRESENCE, 0, -1, NULL, NULL);
    cap_parser_add_flag_alias(p, "-a", "-A");

    bool failed = false;
    do {
        AddFlagError e = cap_parser_add_flag_noexit(p, "-A", DT_DOUBLE, 0, 1, NULL, NULL);
        if (e != AFE_DUPLICATE) FB(failed);
    } while(false);
    cap_parser_destroy(p);
    return !failed;
}

/**
 * Create two flags and aliases for them. Then try to create another flag with 
 * the same name as one of the aliases. This should fail with 
 * AFE_DUPLICATE_FLAG.
 */
bool test_add_flags_after_alias_5() {
    ArgumentParser * p = cap_parser_make_empty();
    cap_parser_add_flag(p, "-a", DT_PRESENCE, 0, -1, NULL, NULL);
    cap_parser_add_flag_alias(p, "-a", "-A");
    cap_parser_add_flag(p, "-b", DT_DOUBLE, 0, 1, NULL, NULL);
    cap_parser_add_flag_alias(p, "-b", "-B");

    bool failed = false;
    do {
        AddFlagError e = cap_parser_add_flag_noexit(p, "-A", DT_DOUBLE, 0, 1, NULL, NULL);
        if (e != AFE_DUPLICATE) FB(failed);
    } while(false);
    cap_parser_destroy(p);
    return !failed;
}

/**
 * Create two flags and aliases for them. Then try to create another flag with 
 * the same name as one of the aliases. This should fail with 
 * AFE_DUPLICATE_FLAG.
 */
bool test_add_flags_after_alias_6() {
    ArgumentParser * p = cap_parser_make_empty();
    cap_parser_add_flag(p, "-a", DT_PRESENCE, 0, -1, NULL, NULL);
    cap_parser_add_flag_alias(p, "-a", "-A");
    cap_parser_add_flag(p, "-b", DT_DOUBLE, 0, 1, NULL, NULL);
    cap_parser_add_flag_alias(p, "-b", "-B");
    cap_parser_add_flag_alias(p, "-b", "--eeeee");

    bool failed = false;
    do {
        AddFlagError e = cap_parser_add_flag_noexit(p, "--eeeee", DT_DOUBLE, 0, 1, NULL, NULL);
        if (e != AFE_DUPLICATE) FB(failed);
    } while(false);
    cap_parser_destroy(p);
    return !failed;
}

int main() {
    bool a, b, c;
    a = TEST_GROUP(
        "p-flag-alias", false, false, test_flag_with_alias_1,
        test_flag_with_alias_2, test_flag_with_alias_3, test_flag_with_alias_4,
        test_flags_and_aliases);
    b = TEST_GROUP(
        "p-flag-alias-prefix", false, false, test_custom_flag_prefix_1,
        test_custom_flag_prefix_2, test_custom_flag_prefix_3,
        test_custom_flag_prefix_4, test_custom_flag_prefix_5);
    c = TEST_GROUP(
        "p-flag-alias-errors", false, false, test_invalid_alias_1, 
        test_invalid_alias_2, test_invalid_alias_2x, test_invalid_alias_3,
        test_invalid_alias_3x, test_invalid_alias_4, test_invalid_alias_4x,
        test_invalid_alias_5_1, test_invalid_alias_5_2, test_invalid_alias_5_3,
        test_invalid_alias_5_4, test_invalid_alias_5_5, test_invalid_alias_5_6,
        test_invalid_alias_5_7, test_invalid_alias_5_8, test_invalid_alias_5_9, 
        test_invalid_alias_5_10, test_invalid_alias_5_11);
    return a && b && c ? 0 : 1;
}
