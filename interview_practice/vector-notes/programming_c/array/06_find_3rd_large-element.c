#include <stdio.h>
#include <limits.h>

#define ele 6
int main() {

        int a[ele]= {10,20,20,30,40,50};
        int i, large, sec_large, third_large;

        /* logic 1      
        large=a[0];
        for(i=1; i<ele; i++)
                if(a[i]>large)
                large= a[i];

        sec_large=INT_MIN;
        for(i=0; i<ele; i++)
                if(a[i]>sec_large && a[i]<large)
                        sec_large=a[i];

        third_large = INT_MIN;
        for(i=0; i<ele; i++)
                if(a[i]>third_large && a[i]<sec_large)
                        third_large = a[i];
        */

        /* logic 2 */
        large=INT_MIN;
        sec_large = INT_MIN;
        third_large = INT_MIN;

        for(i=0; i<ele; i++) {
           if(a[i]>large) {
             third_large=sec_large;
             sec_large = large;
             large = a[i];
           } 
           else if(a[i]>sec_large && a[i]<large){
             third_large = sec_large;
             sec_large = a[i];
           }
           else if(a[i]>third_large && a[i]<sec_large){
             third_large = a[i];
           }
         }

        /* logic 3 =>  we can short the array and pick large, sec_large and third_large */

        printf("large =%d sec_large =%d third_large =%d\n", large, sec_large, third_large);
        printf("\n");   
        return 0;
}
