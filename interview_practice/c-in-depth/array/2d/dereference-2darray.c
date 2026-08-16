#include <stdio.h>
void main()
{
  int b[2][3]={10,20,30,40,50,60};
  printf("sizeof(b)=%d sizeof(b[0])=%d\n",sizeof(b),sizeof(b[0]));
  printf("b=%lu\n",b); //adds of 1st 1d array
  printf("b+1=%lu\n",b+1); //adds of 2nd 2d array
  printf("&b=%lu\n",&b);   //adds of 2d array
  printf("&b+1=%lu\n",&b+1);  //adds of 2d array + 1 or (calculate => adds of b/&b + sizeof 2d array == &b+1)
  printf("b[0]=%lu\n",b[0]);  //adds of 1st ele in 1d array
  printf("b[0]+1=%lu\n",b[0]+1); //adds of 2nd ele in 1d array
  printf("&b[0]=%lu\n",&b[0]);  //&b[0] (adds of 1d array)
  printf("&b[0]+1=%lu\n",&b[0]+1); // &b[0]+1 (adds of 1d array + 1) or (calculate=> adds of b[0]/&b[0]+ sizeof 1d arrray == &b[0]+1)
  printf("&b[0][0]=%lu\n",&b[0][0]); //adds of 1st element 
  printf("&b[0][0]+1=%lu\n",&b[0][0]+1); //adds of 1st ele + 1 == adds of 2nd ele
  b[0][0]++;
  //b++; //error: lvalue required as increment operand
  //b[0]++; //error: lvalue required as increment operand
}
