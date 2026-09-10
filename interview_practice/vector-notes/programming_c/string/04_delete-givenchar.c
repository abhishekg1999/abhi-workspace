#include <stdio.h>
#include <string.h>

#define size 10
int main(){
	
	char s[size] = "embedded";
	int i,j;
	char ch = 'e';
	
	for(i=0; i<size; i++){	
	   if(s[i] == ch){

	   	for(j=i; j<size; j++)
	   	    s[j]=s[j+1];
	   	
		i--;  //for consicutive character
	   }
	}
	
	printf("%s\n", s);
	return 0;
}
