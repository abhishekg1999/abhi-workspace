#include <stdio.h>
#include <limits.h>

#define ele 5
int main()
{

	int i,large,sec_large,a[ele]={10,20,30,40,50};

	
	/* logic 1
	if(a[0]>a[1]) {
		large=a[0];
		sec_large=a[1];
	} else {
		large=a[1];
		sec_large=a[0];
	}

	for(i=2;i<5;i++) {
		if(a[i]>large){
			sec_large=large;
			large=a[i];
		}
		else if(a[i]>sec_large && a[i]!=large)
			sec_large=a[i];
	}
	*/

	/* logic 2 
	large=a[0];
        for(i=1; i<ele; i++)
                if(a[i]>large)
                large= a[i];

        sec_large=INT_MIN;
        for(i=0; i<ele; i++)
                if(a[i]>sec_large && a[i]<large)
                        sec_large=a[i];
	*/

	/* logic 3 */
	large =INT_MIN;
	sec_large = INT_MIN;

	for(i=0; i<ele; i++) {
	   if(a[i]>large) {
	      sec_large=large;
	      large = a[i];
	   }
	   else if(a[i]>sec_large && a[i]!=large)
              sec_large = a[i];
	}

	printf("l=%d\tsl=%d\n",large,sec_large);
}
