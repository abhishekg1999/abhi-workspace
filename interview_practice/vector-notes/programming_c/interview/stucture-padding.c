#include <stdio.h>
#pragma pack(2)

struct temp
{
	char ch;
	int i;
	float f;
};

int main()
{
	struct temp x;
	printf("size =%d\n",sizeof(x));
}

/*
 
#pragma pack(1) → alignment = 1 byte
Address →   0   1   2   3   4   5   6   7   8
            ---------------------------------
            ch  i   i   i   i   f   f   f   f

Total size = 9 byte

#pragma pack(2) → alignment = 2 bytes
Address →   0   1   2   3   4   5   6   7   8   9
            ------------------------------------
            ch  P   i   i   i   i   f   f   f   f

Total size = 10 bytes

*/
