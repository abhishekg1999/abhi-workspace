#include <stdio.h>
#include <stdint.h>

void printbits(int num){

	for(int pos=31; pos>=0; pos--){
		
		printf("%d", num>>pos&1);
		if(pos%8==0)
			printf(" ");
	}

	printf("\n");

}

/* byte/8-bit swap */
uint32_t swap_b0_and_b1(uint32_t num){
	
	uint32_t  byte1 = (num & 0x0000FF00) >>8;
	uint32_t  byte0 = (num & 0x000000FF) <<8;

	uint32_t remain = (num& 0xFFFF0000);

	return remain | byte0 | byte1;
}

uint32_t swap_b0_and_b2(uint32_t num){

        uint32_t  byte2 = (num & 0x00FF0000) >>16;
        uint32_t  byte0 = (num & 0x000000FF) <<16;

        uint32_t remain = (num & 0xFF00FF00);

        return remain | byte0 | byte2;
}

uint32_t swap_b0_and_b3(uint32_t num){

        uint32_t  byte3 = (num & 0xFF000000) >>24;
        uint32_t  byte0 = (num & 0x000000FF) <<24;

        uint32_t remain = (num & 0x00FFFF00);

        return remain | byte0 | byte3;
}

uint32_t swap_b1_and_b2(uint32_t num){

        uint32_t  byte2 = (num & 0x00FF0000) >>8;
        uint32_t  byte1 = (num & 0x0000FF00) <<8;

        uint32_t remain = (num & 0xFF0000FF);

        return remain | byte1 | byte2;
}
	
uint32_t swap_b1_and_b3(uint32_t num){

        uint32_t  byte3 = (num & 0xFF000000) >>16;
        uint32_t  byte1 = (num & 0x0000FF00) <<16;

        uint32_t remain = (num & 0x00FF00FF);

        return remain | byte1 | byte3;
}

uint32_t swap_b2_and_b3(uint32_t num){

        uint32_t  byte3 = (num & 0xFF000000) >>8;
        uint32_t  byte2 = (num & 0x00FF0000) <<8;

        uint32_t remain = (num & 0x0000FFFF);

        return remain | byte2 | byte3;
}

uint32_t swap_16bit(uint32_t num){

	uint32_t  high16 = (num & 0xFFFF0000) >>16;
        uint32_t  low16  = (num & 0x0000FFFF) <<16;

        return low16 | high16;
}

uint32_t swap_32bit(uint32_t num){

        uint32_t  byte3 = (num & 0xFF000000) >>24;
        uint32_t  byte2 = (num & 0x00FF0000) >>8;
	uint32_t  byte1 = (num & 0x0000FF00) <<8;
        uint32_t  byte0 = (num & 0x000000FF) <<24;

        return byte3|byte2|byte1|byte0;
}

int main(){

	uint32_t reg = 0x12345678;
	
	printbits(reg);
	uint32_t temp = swap_32bit(reg);
	printbits(temp);	

	return 0;
}	
