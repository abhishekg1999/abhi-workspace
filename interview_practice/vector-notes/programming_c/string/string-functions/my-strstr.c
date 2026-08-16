#include <stdio.h>
 
// returns true if `X` and `Y` are the same
int compare(const char *X, const char *Y)
{
    while (*X && *Y)
    {
        if (*X != *Y) {
            return 0;  /* return 0 if not equal */
        }
 
        X++;
        Y++;
    }
 
    return (*Y == '\0');  /* return 1 if equal at last */
}
 
// Function to implement `strstr()` function
const char* strstr(const char* X, const char* Y)
{
    while (*X != '\0')
    {
        if ((*X == *Y) && compare(X, Y)) {
            return X;
        }
        X++;
    }
 
    return NULL;
}
 
int main()
{
    char *X = "Techie Delight - Ace the Technical Interviews";
    char *Y = "Ace";
 
    printf("%s\n", strstr(X, Y));
 
    return 0;
}
