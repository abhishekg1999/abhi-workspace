#include <stdio.h>

typedef union {
        char name[10];
        int rollno;
}student;

int main(){

        student s;
//      s.name = "abhi"; //error , array is constant pointer
        strcpy(s.name, "abhi");
        printf("name =%s\n", s.name);
        s.rollno = 10;
        printf("rollno =%d\n", s.rollno);

//      printf("name =%s rollno=%d\n", s.name, s.rollno);
        return 0;
}
