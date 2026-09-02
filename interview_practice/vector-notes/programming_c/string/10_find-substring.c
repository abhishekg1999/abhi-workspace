#include <stdio.h>
#include <string.h>

int main(){
	
	char a[] = "embedhelloworldhelloabc";
	char b[]= "hello", *p, *q;
	int size_str = strlen(b);
	
	/*
	int count=0;
	p=a;
	while((q=strstr(p, b)) !=0){
		count++;
		p=q+size_str;
	}
	
	printf("substring =%s count=%d\n", b, count);
	*/
	
	p=strstr(a, b);
	if(p==0)
		printf("substring not present\n");
	else
		printf("sustring present\n");
		
	return 0;
}
