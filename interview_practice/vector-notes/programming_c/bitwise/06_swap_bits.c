#include <stdio.h>

void printbits(int num){

	for(int pos=31; pos>=0; pos--){
		
		printf("%d", num>>pos&1);
		if(pos%8==0)
			printf(" ");
	}

	printf("\n");

}

void swap_nibble(int *ptr){

	unsigned char *cp = (char*)ptr;
	*cp = (*cp)>>4|(*cp)<<4;
}

void swap_8bit(int *ptr){

	unsigned char *cp1 = (char*)ptr;
	unsigned char *cp2 = cp1+1;

        unsigned char temp;
	temp = *cp1;
	*cp1 = *cp2;
	*cp2 = temp;
}

void swap_16bit(int *ptr){

        unsigned short int *p1 = (short int*)ptr;
        unsigned short int *p2 = p1+1;

        unsigned short int temp;
        temp = *p1;
        *p1 = *p2;
        *p2 = temp;
}

int main(){

	unsigned int temp = 0x12345678;
	
	printbits(temp);
	swap_16bit(&temp);

	printbits(temp);
	return 0;
}
