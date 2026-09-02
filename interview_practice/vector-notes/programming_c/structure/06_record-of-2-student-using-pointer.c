#include <stdio.h>
#include <stdlib.h>

typedef struct {
        char name[10];
        int roll;
        float marks;
}student;

int main(){
        student *p[2];
        int i;

        for(i=0; i<2; i++)
                p[i]=malloc(sizeof(student));

        for(int i=0; i<2; i++){
                printf("enter the name roll marks for %d:\n",i+1);
                scanf("%s%d%f", p[i]->name, &p[i]->roll, &p[i]->marks);
        }


        for(int i=0; i<2; i++)
                printf("name =%s roll=%d marks=%f\n", p[i]->name, p[i]->roll, p[i]->marks);

        return 0;
}

