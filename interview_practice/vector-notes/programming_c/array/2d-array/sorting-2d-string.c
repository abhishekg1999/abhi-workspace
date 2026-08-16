#include <stdio.h>
#include <string.h>

#define r 3
#define c 10

void printarray(char (*p)[c]){

	for(int i=0; i<r; i++)
		printf("%s ", p[i]);
	
	printf("\n");
}

void bubble_sort(char (*p)[c]){
	
	char temp[c];
	for(int i=0; i<r-1; i++){
	    for(int j=0; j<r-1-i; j++)
	    	if(strcmp(p[j], p[j+1]) > 0){
	    	   strcpy(temp, p[j]);
	    	   strcpy(p[j], p[j+1]);
	    	   strcpy(p[j+1], temp);
	    	}
	}	
}

void selection_sort(char (*p)[c]){
	
	char temp[c];
	for(int i=0; i<r-1; i++){
	    for(int j=i+1; j<r; j++)
	    	if(strcmp(p[i], p[j]) > 0){
	    	   strcpy(temp, p[i]);
	    	   strcpy(p[i], p[j]);
	    	   strcpy(p[j], temp);
	    	}
	}	
}

int main(){
	
	char s[r][c] = {"abcd", "mnop", "efgh"};
	printarray(s);
	
	selection_sort(s);
	printarray(s);
	printf("\n");
	return 0;
}
