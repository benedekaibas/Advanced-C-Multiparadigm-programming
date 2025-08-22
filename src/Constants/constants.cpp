// DESIGN GOALS OF C++

/*
By understanding the goals of C++, we can understand why we use constants in the language.
The goals of C++ described by Stroustrup:
    Type safety
    Resource safety
    Performance control
    Predictability
    Readability
    Learnability
*/



// MAPPING SEMANTIC ISSUE TO SYNTAX

/* C language I/O */
#include <stdio.h>

int main()
{
    FILE *fp = fopen("input.txt", "r");
    . . .
    fprintf(fp, "%s\n", "Hello input!");
    return 0; /* not strictly necessary since C99 */
}

// C++ example
#include <iostream>
#include <fstream>

int main()
{
    std::ifstream f;
    . . .
    f << "Hello input!" << std::endl;
    return 0;
}

/*
In C we would get a runtime error since we open the file in reading mode, but we want to write on it.
In C++ it would be a compiler error since as we know the ifstream does not have insertion operator.

In C++ it's important to focus on detecting errors that happen in compilation time.
*/



// SIDE NOTE: SCWARTZ ERROR
