#include "cap.h"

#include "test.h"

#include <stdio.h>

bool dummy() {
    return true;
}

int main() {
    TEST_GROUP("compilation", false, false, dummy);
    return 0;
}