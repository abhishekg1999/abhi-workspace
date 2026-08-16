#include <stdlib.h>
#include <stdio.h>
struct tag
{
  struct int x;
  static int y;
 };

int main()
{
 struct tag s;
 s.x=4;
 s.y=5;
  
}
