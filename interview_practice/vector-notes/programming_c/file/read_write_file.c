#include <stdlib.h>
#include <string.h>

int main(int argc, char **argv){
	
	FILE *fp1;
	char s[20] = "hello vvdn";
	char temp[20];
	char ch;
	int i;
	
	fp1=fopen("file.txt", "w+");
	if(fp1==0){
		printf("file not present\n");
		return -1;
	}

	/* write into file */	
	for(i=0; s[i]; i++)
		fputc(s[i], fp1);

	rewind(fp1);
	i=0;
	/* read from file */
	while((ch=fgetc(fp1)) !=-1)
		temp[i++]=ch;
	temp[i] = '\0';
	
	printf("temp =%s\n", temp);
	return 0;
}
