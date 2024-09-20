#include "test.h"

#include <stdarg.h>
#include <stdbool.h>
#include <stdio.h>

bool test_group(const char * name, bool fail_fast, bool quiet,  ...) {
    if (!quiet) {
        printf("running test group \"%s\":\n", name);
    }
    bool success = true;
    int success_count = 0;
    int total_test_count = 0;
    va_list functions;
    va_start(functions, quiet);
    while (true) {
        bool (*f)() = va_arg(functions, bool (*)());
        if (!f) {
            break;
        }
        bool s = test_single(quiet, f);
        success = success && s;
        ++total_test_count;
        if (s) {
            ++success_count;
        }
        if (!s && fail_fast) {
            break;
        }
    }
    va_end(functions);
    if (!quiet) {
        putchar('\n');
        printf(
            "%s (success count: %d/%d)\n", success ? "SUCCESS" : "FAILED ",
            success_count, total_test_count);
        printf("==========================================================\n");
    }
    return success;
}

bool test_single(bool quiet, bool (*test_function)()) {
    bool s = test_function();
    if (!quiet) {
        putchar(s ? '.' : 'f');
    }
    return s;
}
