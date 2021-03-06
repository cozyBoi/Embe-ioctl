/* FPGA FND Test Application
File : fpga_test_fnd.c
Auth : largest@huins.com */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <asm/ioctl.h>
#include <fcntl.h>

#include <string.h>

#define DEVICE "/dev/dev_driver"

#define SET_OPTION 0x9999
#define COMMAND 0x10000

typedef struct ioctl_arguments {
    int interval;
    int cnt;
    int init;
 }_argus;

_argus msg;

int pow10(int a){
    int i, ret = 1;
    for(i = 0; i < a; i++){
        ret *= 10;
    }
    
    return ret;
}

int main(int argc, char **argv)
{
	int dev;
	unsigned char retval;
	int i;
	int str_size;

    int data1 = 0, data2 = 0, data3 = 0;

	if(argc!=4) {
		printf("please input the parameter! \n");
		printf("ex)./app a1 a2 a3\n");
		return -1;
    }
    
    data1 = atoi(argv[1]);
    data2 = atoi(argv[2]);
    data3 = atoi(argv[3]);
    printf("data %d %d %d\n", data1, data2, data3);
    
    msg.interval = data1;
    msg.cnt = data2;
    msg.init = data3;
    unsigned int CMD = SET_OPTION;
    dev = open(DEVICE, O_RDWR);
    printf("ioctl : SET_OPTION\n");
    ioctl(dev, CMD, &msg);
    printf("ioctl : COMMAND\n");
    CMD = COMMAND;
    ioctl(dev, CMD, &msg);
    
	close(dev);

	return(0);
}
