#include <stdio.h>

typedef struct {
        char name[10];
        int roll;
        float marks;
}student;

int main(){
        student s[2];
        for(int i=0; i<2; i++){
                printf("enter the name roll marks for %d:\n",i+1);
                scanf("%s%d%f", s[i].name, &s[i].roll, &s[i].marks);
        }

        for(int i=0; i<2; i++)
                printf("name =%s roll=%d marks=%f\n", s[i].name, s[i].roll, s[i].marks);
        return 0;
}
