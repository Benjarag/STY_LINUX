/////////////////////////////
/////////// A //////////////
///////////////////////////

#include <stdio.h>

void update_value(int val) { // val is a COPY of value (12)
    val = 42;                // Only the COPY is changed to 42
}

int main() {
    int value = 12;          // Original variable
    update_value(value);     // Passes a COPY of value (12)
    printf("value is %d\n", value); // Prints 12 (original value)
}
//ANSWER: 12

/////////////////////////////
//////////// B /////////////
///////////////////////////

//ANSWER
void update_value(int *val) {
    *val = 42; // Dereference the pointer to modify the original variable
}

int main() {
    int value = 12;
    update_value(&value); // Pass the address of value
    printf("value is %d\n", value); // Prints 42
}

/////////////////////////////
/////////// C //////////////
///////////////////////////

#include <stdio.h>

void update_value(char *val) {
    val = "YES";
}

int main() {
    char *answer = "NO";
    update_value(answer);
    printf("My answer is %s\n", answer);
}

/////////////////////////////
//////// CHANGED TO ////////
///////////////////////////
//ANSWER

#include <stdio.h>

void update_value(char **val) { // Takes a pointer to a pointer
    *val = "YES"; // Dereference to modify the original pointer
}

int main() {
    char *answer = "NO";
    update_value(&answer); // Pass the address of the pointer
    printf("My answer is %s\n", answer); // Prints "YES"
}

