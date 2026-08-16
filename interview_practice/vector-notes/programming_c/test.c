#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(){
	
	FILE *fp;
	char ch;
	int i;
	int c=0;
	
	fp=fopen("file.txt", "r+");
	if(fp==0){
		printf("file not present\n");
		return -1;
	}
	
	/* find file size */
	while(fgetc(fp) !=-1)
		c++;
	
	rewind(fp);
	char *p = malloc(c+1);  //total character + \0

	/* store data into buffer p */
	i=0;
	while((ch=fgetc(fp)) !=-1)
		p[i++]=ch;
		
	p[i]='\0';
	rewind(fp);
		
	/* replace character in buffer */
	for(i=0; p[i]; i++)
		if(p[i] == 'e')
		p[i]='a';

	/* store data back to file */		
	for(i=0; p[i]; i++)
		fputc(p[i], fp);
	
	fclose(fp);
	return 0;
}
