#ifndef __COMMON_H__
#define __COMMON_H__

#include <stdbool.h>

#define TEST_GROUP(name, fail_fast, quiet, ...) (test_group((name), (fail_fast), (quiet), __VA_ARGS__, NULL))

bool test_group(const char * name, bool fail_fast, bool quiet, ...);
bool test_single(bool quiet, bool (*test_function)());

#endif
