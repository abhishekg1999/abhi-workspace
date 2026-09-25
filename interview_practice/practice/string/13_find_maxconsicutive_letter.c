/* find the character which coming more times consicutively 
 
input str[] = "aabbbbccddddd"

output :
d=> 5 times

*/

#include <stdio.h>

void fun(char s[]){

	int max = 0;
	int count =0;
	char temp = s[0];
	char ch;

	for(int i=0; s[i]; i++){
		
		if(s[i] == temp) {
			count++;
		}
		else
			count=1;

		if(count>max){

			max=count;
			ch=temp;
		}

		temp=s[i];
	}

	printf("%c => %d\n", ch, max);
}

int main(){

	char str[] = "aabbbbbccddd";
	fun(str);
	return 0;
}
