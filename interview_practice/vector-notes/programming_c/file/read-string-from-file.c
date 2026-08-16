#include <stdio.h>


int main(){

        FILE *fp;
        char buf[100];

        /* read from file */
        fp =fopen("test_file", "r");
        while (fgets(buf, sizeof(buf), fp) !=NULL);

        printf("file data =%s\n", buf);

        fclose(fp);
        return 0;
}
