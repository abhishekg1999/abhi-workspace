#include <stdio.h>
struct stu
{
	int rollno;
	char name[10];
	float marks;
};

int main()
{
	struct stu student[3],temp;
	int i,j,ele;
	ele=sizeof(student)/sizeof(student[0]); //number of student
	printf("%d\n",ele);
	for(i=0;i<ele;i++)
	{
		printf("enter the rollno name marks of %d:\n",i);
		scanf("%d%s%f",&student[i].rollno,student[i].name,&student[i].marks);
	}

	printf("student data before sort \n");
	for(i=0;i<ele;i++)
		printf("rollno=%d\tname=%s\tmarks=%f\n",student[i].rollno,student[i].name,student[i].marks);
	
	for(i=0;i<ele-1;i++)
		for(j=0;j<ele-1-i;j++)
		{
			if(student[j].rollno>student[j+1].rollno)
			{
				temp=student[j];
				student[j]=student[j+1];
				student[j+1]=temp;
			}
		}

	
	printf("student data after sort \n");
	for(i=0;i<ele;i++)
		printf("rollno=%d\tname=%s\tmarks=%f\n",student[i].rollno,student[i].name,student[i].marks);
}
