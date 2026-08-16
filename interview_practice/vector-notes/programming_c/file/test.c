#include <stdio.h>
#include <stdlib.h>

int main(int argc,char *argv[])
{
	FILE *fs;
	char ch,*p;
	int count=0;
	/*if(argc!=3){
		printf("enter the proper source and dest files\n");
		return;
	}

	fs=fopen(argv[1],"r");
	if(fs==0){
		printf("file not present create the file first\n");
		return;
	}
	
	while((ch=fgetc(fs))!=-1)
		if(ch==argv[2][0])
			count++;

	printf("char present =%d\n",count);
	*/

	int i=0;
	char s[10];
	fs=fopen(argv[1],"r+");
	
	/*
	if(fs==0){
		printf("failed to open the file \n");
		return ;
	}

	while(fscanf(fs,"%s",s)!=-1)
		if(strcmp(s,argv[2])==0)
			count++;

	printf("match strings =%d\n",count);
	*/

	while((ch=fgetc(fs))!=-1)
		count++;

	rewind(fs);
	
	p=malloc(count+1);
	while((ch=fgetc(fs))!=-1)
		p[i++]=ch;
		p[i]='\0';
		rewind(fs);

	printf("before replace =%s\n",p);
	for(i=0;p[i];i++)
		if(p[i]==argv[2][0])
			p[i]=argv[3][0];

	printf("after replace =%s\n",p);
	for(i=0;p[i];i++)
		fputc(p[i],fs);
}	
