#include <stdio.h>
#include <stdlib.h>
typedef struct {
        char name[10];
        int roll;
        float marks;
}student;

int main()
{
	/*
	int n;
        scanf("%d", &n);

        student *p[n];
        int i;

        for(i=0; i<n; i++)
                p[i]=malloc(sizeof(student));

	for(int i=0; i<n; i++){
                printf("enter the name roll marks for %d:\n",i+1);
                scanf("%s%d%f", p[i]->name, &p[i]->roll, &p[i]->marks);
        }


        for(int i=0; i<n; i++)
                printf("name =%s roll=%d marks=%f\n", p[i]->name, p[i]->roll, p[i]->marks);

	*/

	/*        or      */

	student **p;
        int n, i;

        printf("no of students\n");
        scanf("%d", &n);

        p=malloc(sizeof(student*)*n);
        for(i=0; i<n; i++)
                p[i]=malloc(sizeof(student));

        for(int i=0; i<n; i++){
                printf("enter the name roll marks for %d:\n",i+1);
                scanf("%s%d%f", p[i]->name, &p[i]->roll, &p[i]->marks);
        }


        for(int i=0; i<n; i++)
                printf("name =%s roll=%d marks=%f\n", p[i]->name, p[i]->roll, p[i]->marks);

}
