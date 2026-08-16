#include <stdio.h>


int main(){

	pal	
	int num,i;
	for(num=50, count=0; count<15; num++) {
	   for(i=2; i<num; i++) 
              if(num%i==0) {
	        count++;
		break;
	      }

	   if(i==num) printf("%d\t", num);
	}

	printf("\n");
	
	
			
	return 0;
}
