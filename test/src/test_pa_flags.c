#include "cap.h"

#include "test.h"

#include <assert.h>

static bool _compare_typed_union(const TypedUnion * a, const TypedUnion * b) {
    if (!a && !b) {
        return true;
    }
    if (!a || !b) {
        return false;
    }
    if (a -> mType != b -> mType) {
        return false;
    }
    bool result = false;
    switch (a -> mType) {
        case DT_PRESENCE:
            result = true;
            break;
        case DT_STRING:
            result = (strcmp(cap_tu_as_string(a), cap_tu_as_string(b)) == 0);
            break;
        case DT_DOUBLE:
            result =  (cap_tu_as_double(a) == cap_tu_as_double(b));
            break;
        case DT_INT:
            result =  (cap_tu_as_int(a) == cap_tu_as_int(b));
            break;
        default:
            assert(false && "unreachable");
    }
    return result;
}

static bool _compare_flag_values(
        const ParsedArguments * pa, const char * flag, size_t count, const TypedUnion * order) {
    if (!pa || !flag) {
        return true;
    }
    if (!cap_pa_has_flag(pa, flag)) {
        return count == 0u;
    }
    size_t fc = cap_pa_flag_count(pa, flag);
    if (fc != count) {
        return false;
    }
    for (size_t i = 0; i < count; ++i) {
        const TypedUnion * tu = cap_pa_get_flag_i(pa, flag, i);
        if (!tu) {
            return false;
        }
        if (!_compare_typed_union(tu, order + i)) {
            return false;
        }
    }
    return true;
}

bool test_creation_flags() {
    ParsedArguments * pa = cap_pa_make_empty();

    const char * flag_names[6] = {"a", "b", "c", "anarchy", "biscuits", "coffee"};
    for (int i = 0; i < 6; ++i) {
        const char * f = flag_names[i];
        if (cap_pa_has_flag(pa, f)) goto fail;
        if (cap_pa_get_flag(pa, f)) goto fail;
    }

    cap_pa_destroy(pa);
    return true;
fail:
    cap_pa_destroy(pa);
    return false;
}

bool test_addition_flags_int() {
    ParsedArguments * pa = cap_pa_make_empty();

    const char * sus_flag = "--susint";
    const int PI = 4;

    if (cap_pa_has_flag(pa, sus_flag)) goto fail;
    if (cap_pa_flag_count(pa, sus_flag) != 0u) goto fail;

    cap_pa_add_flag(pa, sus_flag, cap_tu_make_int(PI));
    if (!cap_pa_has_flag(pa, sus_flag)) goto fail;
    if (cap_pa_flag_count(pa, sus_flag) != 1u) goto fail;
    
    const TypedUnion * sus_value = cap_pa_get_flag(pa, sus_flag);
    if (!sus_value) goto fail;
    if (sus_value -> mType != DT_INT) goto fail;
    if (sus_value -> mValue.asInt != PI) goto fail;

    cap_pa_destroy(pa);
    return true;
fail:
    cap_pa_destroy(pa);
    return false;
}

bool test_addition_flags_double() {
    ParsedArguments * pa = cap_pa_make_empty();

    const char * sus_flag = "--sus";
    const double PI = 3.14;

    if (cap_pa_has_flag(pa, sus_flag)) goto fail;
    if (cap_pa_flag_count(pa, sus_flag) != 0u) goto fail;

    cap_pa_add_flag(pa, sus_flag, cap_tu_make_double(PI));
    if (!cap_pa_has_flag(pa, sus_flag)) goto fail;
    if (cap_pa_flag_count(pa, sus_flag) != 1u) goto fail;
    
    const TypedUnion * sus_value = cap_pa_get_flag(pa, sus_flag);
    if (!sus_value) goto fail;
    if (sus_value -> mType != DT_DOUBLE) goto fail;
    if (sus_value -> mValue.asDouble != PI) goto fail;

    cap_pa_destroy(pa);
    return true;
fail:
    cap_pa_destroy(pa);
    return false;
}

bool test_addition_flags_presence() {
    ParsedArguments * pa = cap_pa_make_empty();

    const char * sus_flag = "--sus";

    if (cap_pa_has_flag(pa, sus_flag)) goto fail;
    if (cap_pa_flag_count(pa, sus_flag) != 0u) goto fail;

    cap_pa_add_flag(pa, sus_flag, cap_tu_make_presence());
    if (!cap_pa_has_flag(pa, sus_flag)) goto fail;
    if (cap_pa_flag_count(pa, sus_flag) != 1u) goto fail;
    
    const TypedUnion * sus_value = cap_pa_get_flag(pa, sus_flag);
    if (!sus_value) goto fail;
    if (sus_value -> mType != DT_PRESENCE) goto fail;
    
    cap_pa_destroy(pa);
    return true;
fail:
    cap_pa_destroy(pa);
    return false;
}

bool test_addition_flags_string() {
    ParsedArguments * pa = cap_pa_make_empty();

    const char * sus_flag = "--sus";
    const char * PI = "3.1415927";

    if (cap_pa_has_flag(pa, sus_flag)) goto fail;
    if (cap_pa_flag_count(pa, sus_flag) != 0u) goto fail;

    cap_pa_add_flag(pa, sus_flag, cap_tu_make_string(PI));
    if (!cap_pa_has_flag(pa, sus_flag)) goto fail;
    if (cap_pa_flag_count(pa, sus_flag) != 1u) goto fail;
    
    const TypedUnion * sus_value = cap_pa_get_flag(pa, sus_flag);
    if (!sus_value) goto fail;
    if (sus_value -> mType != DT_STRING) goto fail;
    if (strcmp(sus_value -> mValue.asString, PI) != 0) goto fail;

    cap_pa_destroy(pa);
    return true;
fail:
    cap_pa_destroy(pa);
    return false;
}

bool test_addition_flags_multiple_flags() {
    ParsedArguments * pa = cap_pa_make_empty();

    const char * sus_flag = "--sus";
    const char * string = "3.1415927";
    const char * sus_flag_int = "--susint";
    const int integer = 4;
    const char * sus_flag_presence = "--susp";

    const TypedUnion * sus_value;
    const TypedUnion * sus_value_int;
    const TypedUnion * sus_value_presence;

    cap_pa_add_flag(pa, sus_flag, cap_tu_make_string(string));

    if (!cap_pa_has_flag(pa, sus_flag)) goto fail;
    sus_value = cap_pa_get_flag(pa, sus_flag);
    if (!sus_value) goto fail;
    if (!cap_tu_is_string(sus_value)) goto fail;
    if (strcmp(cap_tu_as_string(sus_value), string) != 0) goto fail;

    if (cap_pa_has_flag(pa, sus_flag_int)) goto fail;
    if (cap_pa_get_flag(pa, sus_flag_int)) goto fail;

    if (cap_pa_has_flag(pa, sus_flag_presence)) goto fail;
    if (cap_pa_get_flag(pa, sus_flag_presence)) goto fail;

    cap_pa_add_flag(pa, sus_flag_int, cap_tu_make_int(integer));
    
    if (!cap_pa_has_flag(pa, sus_flag)) goto fail;
    sus_value = cap_pa_get_flag(pa, sus_flag);
    if (!sus_value) goto fail;
    if (!cap_tu_is_string(sus_value)) goto fail;
    if (strcmp(cap_tu_as_string(sus_value), string) != 0) goto fail;

    if (!cap_pa_has_flag(pa, sus_flag_int)) goto fail;
    sus_value_int = cap_pa_get_flag(pa, sus_flag_int);
    if (!sus_value_int) goto fail;
    if (!cap_tu_is_int(sus_value_int)) goto fail;
    if (cap_tu_as_int(sus_value_int) != integer) goto fail;

    if (cap_pa_has_flag(pa, sus_flag_presence)) goto fail;
    if (cap_pa_get_flag(pa, sus_flag_presence)) goto fail;

    cap_pa_add_flag(pa, sus_flag_presence, cap_tu_make_presence());

    if (!cap_pa_has_flag(pa, sus_flag)) goto fail;
    sus_value = cap_pa_get_flag(pa, sus_flag);
    if (!sus_value) goto fail;
    if (!cap_tu_is_string(sus_value)) goto fail;
    if (strcmp(cap_tu_as_string(sus_value), string) != 0) goto fail;

    if (!cap_pa_has_flag(pa, sus_flag_int)) goto fail;
    sus_value_int = cap_pa_get_flag(pa, sus_flag_int);
    if (!sus_value_int) goto fail;
    if (!cap_tu_is_int(sus_value_int)) goto fail;
    if (cap_tu_as_int(sus_value_int) != integer) goto fail;

    if (!cap_pa_has_flag(pa, sus_flag_presence)) goto fail;
    sus_value_presence = cap_pa_get_flag(pa, sus_flag_presence);
    if (!sus_value_presence) goto fail;
    if (!cap_tu_is_presence(sus_value_presence)) goto fail;

    cap_pa_destroy(pa);
    return true;
fail:
    cap_pa_destroy(pa);
    return false;
}

bool test_addition_flags_multiple_values() {
    ParsedArguments * pa = cap_pa_make_empty();

    const char * sus_flag = "--suspicious";
    const double D = -0.25;
    const int I = 15;
    const char * S = "oh boy...";

    const char * S2 = "another string";

    const TypedUnion expected_order[6] = {
        cap_tu_make_double(D),
        cap_tu_make_int(I),
        cap_tu_make_string(S),
        cap_tu_make_presence(),
        cap_tu_make_string(S2),
        cap_tu_make_double(D)
    };

    if (cap_pa_has_flag(pa, sus_flag)) goto fail;
    if (cap_pa_flag_count(pa, sus_flag) != 0u) goto fail;

    for (size_t i = 0; i < 6; ++i) {
        cap_pa_add_flag(pa, sus_flag, expected_order[i]);
        if (!cap_pa_has_flag(pa, sus_flag)) goto fail;
        if (cap_pa_flag_count(pa, sus_flag) != i + 1) goto fail;
        for (size_t j = 0; j < i + 1; ++j) {
            const TypedUnion * f = cap_pa_get_flag_i(pa, sus_flag, j);
            if (!f) goto fail;
            if (!_compare_typed_union(f, expected_order + j)) goto fail;
        }
    }

    cap_pa_destroy(pa);
    return true;
fail:
    cap_pa_destroy(pa);
    return false;
}

bool test_multiple_flags_and_values() {
    ParsedArguments * pa = cap_pa_make_empty();
    const char * big = "--big";
    const char * small = "--small";

    const TypedUnion big_order[4] = {
        cap_tu_make_int(1),
        cap_tu_make_presence(),
        cap_tu_make_double(-1),
        cap_tu_make_double(1e10)
    };

    const TypedUnion small_order[7] = {
        cap_tu_make_int(100),
        cap_tu_make_double(1e-10),
        cap_tu_make_string("small"),
        cap_tu_make_string("small"),
        cap_tu_make_double(0),
        cap_tu_make_double(0.125),
        cap_tu_make_int(6)        
    };

    if (cap_pa_has_flag(pa, big)) goto fail;
    if (cap_pa_get_flag(pa, big)) goto fail;
    if (cap_pa_flag_count(pa, big)) goto fail;
    if (cap_pa_has_flag(pa, small)) goto fail;
    if (cap_pa_get_flag(pa, small)) goto fail;
    if (cap_pa_flag_count(pa, small)) goto fail;

    for (size_t i = 0; i < 4; ++i) {
        cap_pa_add_flag(pa, big, big_order[i]);
        if (!_compare_flag_values(pa, big, i + 1, big_order)) goto fail;
        if (!_compare_flag_values(pa, small, i, small_order)) goto fail;
        
        cap_pa_add_flag(pa, small, small_order[i]);
        if (!_compare_flag_values(pa, big, i + 1, big_order)) goto fail;
        if (!_compare_flag_values(pa, small, i + 1, small_order)) goto fail;
    }

    for (size_t i = 4; i < 7; ++i) {
        cap_pa_add_flag(pa, small, small_order[i]);
        if (!_compare_flag_values(pa, big, 4, big_order)) goto fail;
        if (!_compare_flag_values(pa, small, i + 1, small_order)) goto fail;
    }

    const char * a = "-a";
    cap_pa_add_flag(pa, a, cap_tu_make_presence());
    if (!_compare_flag_values(pa, big, 4, big_order)) goto fail;
    if (!_compare_flag_values(pa, small, 7, small_order)) goto fail;
    if (!cap_pa_has_flag(pa, a)) goto fail;
    if (cap_pa_flag_count(pa, a) != 1u) goto fail;
    if (!cap_tu_is_presence(cap_pa_get_flag(pa, a))) goto fail;

    cap_pa_destroy(pa);
    return true;
fail:
    cap_pa_destroy(pa);
    return false;
}

int main() {
    bool a = TEST_GROUP(
        "pa-flags: creation", false, false, test_creation_flags);
    bool b = TEST_GROUP(
        "pa-flags: storing flags", false, false, test_addition_flags_int,
        test_addition_flags_double, test_addition_flags_presence,
        test_addition_flags_string);
    bool c = TEST_GROUP(
        "pa-flags: storing multiple flags", false, false, test_addition_flags_multiple_flags,
        test_addition_flags_multiple_values, test_multiple_flags_and_values);

    return a && b && c ? 0 : 1;
}
