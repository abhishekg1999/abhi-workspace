#include <string.h>

void my_strcpy(char *d, char *s){

	int i;
	for(i=0; s[i]; i++)
		d[i]=s[i];
	d[i]='\0';

	/* or
	while(*s){
		*d=*s;
		d++;
		s++;
	}

	*d='\0';
	*/
}

int my_strlen(char *p){

	int i;
	for(i=0; p[i]; i++);
	return i;
}

void my_strrev(char *p){

	int i, j;
	char ch;
	for(j=0; p[j]; j++);
	for(i=0, j=j-1; i<j; i++, j--){
		ch=p[i];
		p[i]=p[j];
		p[j]=ch;
	}
}

void my_strrev_1(char *start, char *end) {

	char ch;
	while(start<end){
		ch=*start;
		*start=*end;
		*end=ch;

		start++;
		end--;
	}
}

void my_strupr(char *p){

	for(int i=0; p[i]; i++){
		if(p[i]>= 'a' && p[i]<= 'z')
			p[i]=p[i]-32;
	}
}

int my_strcmp(const char*p, const char *q){

	int i;
	for(i=0; p[i]; i++){

		if(p[i]!=q[i])
		break;
	}

	if(p[i]==q[i])
		return 0;
	else if(p[i]>q[i])
		return 1;
	else
		return -1;
}

void my_strcat(char *dst, char *src){

	int i,j;
	for(i=0; dst[i]; i++);
	for(j=0; src[j]; j++)
		dst[i++]=src[j];
	dst[i]='\0';

	/* or
	while(*dst)
	dst++;

	while(*src){
		*dst=*src;
		dst++;
		src++;
	}

	*dst='\0';
	*/
}

char* my_strchr(char *p, char ch){

	while(*p){
		if(*p ==ch)
		   return p;
		p++;
	}

	return 0;
}

char* my_strstr(char *p, char *q){

	int substrlen = strlen(q);
	int i, j ,k;

	for(i=0; p[i]; i++){

		k=i;
		for(j=0; q[j]; j++){
			if(q[j]==p[k]){  	/* matched char of substring with main string */
				if(j == substrlen-1)  /* if substring reached at last before \0 */
					return p+i;   /* return address of subtring find in main */
				k++;
			}
			else
				break;
		}
	}

	return 0;
}
