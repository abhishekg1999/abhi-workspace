#include <stdio.h>
#include <string.h>

void strrev_rec(char *start, char *end){

        if(start>=end)
                return ;

        char temp;
        temp = *start;
        *start = *end;
        *end = temp;

        strrev_rec(start+1, end-1);
}

void strcpy_rec(char *dst, char *src){

	*dst = *src;
	
	if(*src == '\0')
		return;
	
	strcpy_rec(dst+1, src+1);
}

int strlen_rec(char *s){

	if(*s == '\0')
		return 0;

	return 1 + strlen_rec(s+1);
}

void strupr_rec(char *s){

	if(*s == '\0')
		return ;

	if(*s>= 'a' && *s<= 'z')
		*s=*s-32;

	strupr_rec(s+1);
}

int strcmp_rec(char *p, char *q){


	if( *p == '\0' && *q == '\0')
		return 0;
	else if(*p > *q)
		return 1;
	else if(*p < *q)
		return -1;
	
	return strcmp_rec(p+1, q+1);
}

void strcat_rec(char *dst, char *src){

	*dst=*src;

	if(*src == '\0')
		return;

	strcat_rec(dst+1, src+1);
}

char* strchr_rec(char *s, char ch){

	if(*s == ch)
		return s;
	else if (*s == '\0')
		return 0;

	strchr_rec(s+1, ch);
}

int main(){

        char str[20] = "hello";
	char temp[10] = "hellohi";

	//strrev_rec(str, str+(strlen(str)-1));
        //strcpy_rec(temp, str);
	//printf("%d\n", strlen_rec(str));
	//strupr_rec(str);
	//strcat_rec(str+strlen(str), temp);
	//printf("%lu\n", strchr_rec(str, 'l'));
	//printf("%s\n", str);
        return 0;
}

