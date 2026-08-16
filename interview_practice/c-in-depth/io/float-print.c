#include <stdio.h>
int main()
{

  float f= 23.5;
  double d=23.5;

  printf("%f %e %g\n",f,f,f);
  printf("%lf %e %g\n",d,d,d);

  printf("%d\n",f==d);
}
