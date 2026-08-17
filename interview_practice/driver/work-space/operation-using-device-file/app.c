#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

#define buf_size 1024
int8_t write_buf[buf_size];
int8_t read_buf[buf_size];

int main()
{
        int fd, option;
        printf("*******driver test app*******\n");
        fd = open("/dev/my_device", O_RDWR);
        if(fd < 0) {
                printf("Cannot open device file...\n");
                return 0;
        }
        while(1) {
                printf("****Please Enter the Option******\n");
                printf("        1. Write               \n");
                printf("        2. Read                 \n");
                printf("        3. Exit                 \n");
                scanf("%d", &option);
                
                switch(option) {
                        case 1:
                                printf("Enter the string to write into driver :");
                                scanf("  %[^\t\n]s", write_buf);
                                write(fd, write_buf, strlen(write_buf)+1);
                                printf("Done!\n");
                                break;
                        case 2:
                                read(fd, read_buf, buf_size);
                                printf("Data = %s\n\n", read_buf);
                                break;
                        case 3:
                                close(fd);
                                exit(1);
                                break;
                        default:
                                printf("InValid option\n");
                                break;
                }
        }
        close(fd);
}
