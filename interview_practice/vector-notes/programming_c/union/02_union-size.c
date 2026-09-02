#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#pragma pack(1)   //Force 1-byte alignment
typedef union {
	char name[10];
	int roll;
}student;
#pragma pack()   //Restore default alignment

/* or 
typedef union {
	char name[10];
	int roll;
} __attribute__((packed)) student;  //use compiler attribute to avaid padding

*/
	
int main(){
	
	student s;
	printf("%ld\n", sizeof(student));
	
	return 0;
}
