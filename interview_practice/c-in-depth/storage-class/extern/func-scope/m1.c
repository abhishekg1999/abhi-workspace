#include <stdio.h>
void abc(void);
void def(void);
void mno(void);
void main()
{
  abc();
  def();
  mno();  //error: ld returned 1 exit status undefined reference to `mno' 
 }

void abc(void)
{
  printf("i am in abc\n");
  }

