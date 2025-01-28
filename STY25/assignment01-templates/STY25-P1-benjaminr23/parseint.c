#include "parseint.h"
#include <string.h> // for strlen

/*
 * Returns the value of c or -1 on error
 */
int convertDecimalDigit(char c)
{
    (void)c;

    if (c >= '0' && c <= '9') {
        return c - '0'; // converting char to int by minusing '0' or 48
    }
    return -1;
}


/*
 * Calculates value of the octal string
 * Returns the integer or -1 for invalid input.
 */
int calculateOctal(const char *octalString) {
    int result = 0;

    // for char in stringstring
    for (int i = 0; octalString[i] != '\0'; i++) {
        char c = octalString[i];

        // octal digits
        if (c < '0' || c > '7') {
            return -1;
        }

        // char to int
        result = result * 8 + (c - '0');
    }

    return result;
}

/*
 * Calculates value of the decimal string
 * Returns the integer or -1 for invalid input.
 */
int calculateDecimal(const char *decimalString) {
    int result = 0;

    // for char in string
    for (int i = 0; decimalString[i] != '\0'; i++) {
        char c = decimalString[i];

        if (c < '0' || c > '9') {
            return -1;
        }

        // char to int
        result = result * 10 + (c - '0');
    }

    return result;
}

/*
 * Parses a non-negative integer, interpreted as octal when starting with 0,
 * decimal otherwise. Returns -1 on error.
 */
int parseInt(char *string)
{
    (void)string;

    int result = -1;
    if (strlen(string) > 0) {
        if (string[0] == '0') {
            result = calculateOctal(string);
        }
        else {
            result = calculateDecimal(string);;
        }

    }

    return result;
}
