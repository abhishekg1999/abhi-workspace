#include <stdio.h>
#include <limits.h>
#include <string.h>
#include <mcheck.h>
#include <stdlib.h>

typedef struct {
        char name[10];
        int rollno;
}student;

/* Error:
 * We cannot use normal executable statements outside a function.
 */

student s;
strcpy(s.name, "hello");
s.rollno = 10;

int main(){
	printf("name =%s rollno =%d\n", s.name, s.rollno);
	return 0;
}	
