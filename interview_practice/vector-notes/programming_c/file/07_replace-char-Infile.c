#include <stdio.h>
#include <stdlib.h>
int main(int argc,char **argv)
{
	if(argc!=4)
	{
		printf("usage wrong\n");
		return ;
	}

	int i=0,count=0;
	char *p,ch;

	FILE *fp=fopen(argv[1],"r+");
	if(fp==0)
	{
		printf("file not present \n");
		return ;
	}

	//find size of file
	while((ch=fgetc(fp))!=-1)
		count++;
		rewind(fp);

	//allocate memory equal to file size
	p=malloc(count+1);

	//copy file into allocated memory
	while((ch=fgetc(fp))!=-1)
		p[i++]=ch;
		p[i]='\0';
		rewind(fp);

	//replace char 
	for(i=0;p[i];i++)
	{
		if(p[i]==argv[2][0])
			p[i]=argv[3][0];
	}

	//restore back to file
	for(i=0;p[i];i++)
		fputc(p[i],fp);
}

/*   or   */ 

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
