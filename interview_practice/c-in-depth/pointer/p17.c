#include <stdio.h>
int main()
{
 int arr[6]={1,2,3,4,5,6};
 int *p=arr;
 printf("size of p=%u,size of arr=%u\n",sizeof(p),sizeof(arr));
 return 0;
  
}
