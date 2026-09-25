#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
 
#define WR_VALUE _IOW('a', 'a', char[1024])
#define RD_VALUE _IOR('a', 'b', char[1024])
 
int main()
{
        int fd, option;
        char ubuf[1024];
        printf("******* app to test the driver *******\n");
 
        fd = open("/dev/my_device",O_RDWR);
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
                                scanf("  %[^\t\n]s", ubuf);
                                ioctl(fd, WR_VALUE, ubuf);
                                printf("Done!\n");
                                break;
                        case 2:
                                ioctl(fd, RD_VALUE, ubuf);
                                printf("Data = %s\n\n", ubuf);
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
}
