#include "testlib.h"
#include "print.h"

int main()
{
    test_start("print.c");

    print_line(42, "Hello World!");

    print_line(-100, "Negative Test");

    print_line(0, "Zero Test");

    print_line(1234567890, "This is a very long string to test used for testing exactly that.");

    return test_end();
}
