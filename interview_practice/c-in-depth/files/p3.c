#include <stdlib.h>
#include <stdio.h>
int main()
{
  FILE *fptr1,*fptr2;
  char fname[20];
  printf("Enter the path of first file:");
  scanf("%s",fname);
  fptr1=fopen(fname,"r");
  if(fptr1==NULL)
  {
    printf("Error \n");
	exit(1);
  }

  fptr2=fopen("file.txt","r");
  if(fptr2==NULL)
  {
   printf("Errror in file open\n");
   exit(1);
   }

   fclose(fptr1);
   fclose(fptr2);
   return 0;
  
}
