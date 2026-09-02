#include <stdio.h>

void printarray(int a[], int ele){

        for(int i=0; i<ele; i++)
                printf("%d ", a[i]);

        printf("\n");
}

void bubble_sort(int a[], int ele){

        int temp;
        for(int i=0; i<ele-1; i++){
            for(int j=0; j<ele-1-i; j++)
                if( a[j]>a[j+1]){
                   temp=a[j];
                   a[j]=a[j+1];
                   a[j+1]=temp;
                }
        }
}

void selection_sort(int a[], int ele){

        int temp;
        for(int i=0; i<ele-1; i++){
            for(int j=i+1; j<ele; j++)
                if( a[i]>a[j]){
                   temp=a[i];
                   a[i]=a[j];
                   a[j]=temp;
                }
        }
}

int main()
{
	int a[6]={1,4,3,9,6,10};
    	int ele;
	ele=sizeof(a)/sizeof(a[0]);
    	
	printf("*****before sort******\n");
	printarray(a, ele);
	
	bubble_sort(a, ele);
	
	printf("\n*****after sort******\n");
	printarray(a, ele);
	
	return 0;
}
		
	

