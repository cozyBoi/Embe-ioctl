#include <linux/module.h>
#include <linux/fs.h>
#include <linux/init.h>
#include <linux/slab.h>
#include <linux/platform_device.h>
#include <linux/delay.h>

#include <asm/io.h>
#include <asm/uaccess.h>
#include <linux/kernel.h>
#include <linux/ioport.h>
#include <linux/module.h>
#include <linux/fs.h>
#include <linux/init.h>
#include <linux/version.h>
#include <include/asm/param.h>

#include "./fpga_dot_font.h"

#define IOM_FPGA_DOT_MAJOR 262        // ioboard led device major number
#define IOM_FPGA_DOT_NAME "fpga_dot"        // ioboard led device name

#define IOM_FND_MAJOR 261        // ioboard fpga device major number
#define IOM_FND_NAME "fpga_fnd"        // ioboard fpga device name

#define IOM_LED_MAJOR 260        // ioboard led device major number
#define IOM_LED_NAME "fpga_led"        // ioboard led device name

#define IOM_FPGA_TEXT_LCD_MAJOR 263        // ioboard led device major number
#define IOM_FPGA_TEXT_LCD_NAME "fpga_text_lcd"        // ioboard led device name

#define IOM_FPGA_DOT_ADDRESS 0x08000210 // pysical address
#define IOM_FND_ADDRESS 0x08000004 // pysical address
#define IOM_LED_ADDRESS 0x08000016 // pysical address
#define IOM_FPGA_TEXT_LCD_ADDRESS 0x08000090 // pysical address - 32 Byte (16 * 2)

#define SET_OPTION 0x9999
#define COMMAND 0x10000

#define MAX_BUFF 32
#define LINE_BUFF 16

unsigned char NAME[16] = {'j', 'h', 'L', 'e', 'e', 0};
unsigned char STNUM[16] = {'2', '0', '1', '7', '1', '6','7','7', 0};

//Global variable
static int fpga_dot_port_usage = 0;
static unsigned char *iom_fpga_dot_addr;

static int fpga_fnd_port_usage = 0;
static unsigned char *iom_fpga_fnd_addr;

static int ledport_usage = 0;
static unsigned char *iom_fpga_led_addr;

static int fpga_text_lcd_port_usage = 0;
static unsigned char *iom_fpga_text_lcd_addr;

static int kernel_timer_usage = 0;

static struct struct_mydata {
    struct timer_list timer;
    int count;
};

struct struct_mydata mydata;

typedef struct ioctl_arguments {
    int interval;
    int cnt;
    int init;
 }_argus;

_argus msg;

// define functions...
int iom_fpga_driver_open(struct inode *minode, struct file *mfile);
int iom_fpga_driver_release(struct inode *minode, struct file *mfile);
int iom_fpga_driver_ioctl(struct inode *, struct file *, unsigned int, unsigned long);

int fnd_write(int value);
int dot_write(int value);
int lcd_write(int value);
int led_write(int value);



// define file_operations structure
struct file_operations iom_fpga_driver_fops =
{
owner:        THIS_MODULE,
open:        iom_fpga_driver_open,
release:    iom_fpga_driver_release,
ioctl:    iom_fpga_driver_ioctl,
};

//var
int locNotZero;
unsigned int loc[4];

//the function which opens the drivers;
int iom_fpga_driver_open(struct inode *minode, struct file *mfile)
{
    if(fpga_dot_port_usage != 0) return -EBUSY;
    if(fpga_fnd_port_usage != 0) return -EBUSY;
    if(ledport_usage != 0) return -EBUSY;
    if(fpga_text_lcd_port_usage != 0) return -EBUSY;
    if(kernel_timer_usage != 0) return -EBUSY;
    
    fpga_dot_port_usage = 1;
    fpga_fnd_port_usage = 1;
    ledport_usage = 1;
    fpga_text_lcd_port_usage = 1;
    kernel_timer_usage = 1;
    return 0;
}

//the function which releases the drivers;
int iom_fpga_driver_release(struct inode *minode, struct file *mfile)
{
    fpga_dot_port_usage = 0;
    fpga_fnd_port_usage = 0;
    ledport_usage = 0;
    fpga_text_lcd_port_usage = 0;
    kernel_timer_usage = 0;
    return 0;
}


int parse_init(unsigned int loc[4], int value){
    int i = 0;
    
    //insert value in loc array
    loc[0] = value % 10;
    loc[1] = (value / 10) % 10;
    loc[2] = (value / 100) % 10;
    loc[3] = (value / 1000) % 10;
    
    //return "not 0 location"
    for(i = 0; i < 4; i++){
        if(loc[i] != 0) return ret;
    }
    return -1;
}

void concat_two_arr(unsigned char a[16], unsigned char b[16], unsigned char string[33]){
    str_size=strlen(a);
    if(str_size>0) {
        strncat(string,a,str_size);
        memset(string+str_size,' ',LINE_BUFF-str_size);
    }

    str_size=strlen(b);
    if(str_size>0) {
        strncat(string,b,str_size);
        memset(string+LINE_BUFF+str_size,' ',LINE_BUFF-str_size);
    }
}

int iom_fpga_driver_ioctl(struct inode *inode, struct file *flip, unsigned int cmd, unsigned long arg){
    copy_from_user(&msg, (char *)arg, sizeof(msg));
    locNotZero = parse_init(loc, msg.init);
    unsigned char string[33];
    concat_two_arr(NAME, STNUM, string);
    
    switch (cmd) {
        case SET_OPTION:
            fnd_write(loc);
            dot_write(fpga_number[i]);
            lcd_write(string);
            led_write(1 << locNotZero);
            break;
        case COMMAND:
            fnd_write(msg.init);
            dot_write(0);
            lcd_write(0);
            led_write(0);
            break;
        default:
            printk("invalid command\n");
            break;
    }
    
    
    return 0;
}

int fnd_write(unsigned int value[4]){
    unsigned short int value_short = 0;

    value_short = value[0] << 12 | value[1] << 8 |value[2] << 4 |value[3];
    outw(value_short,(unsigned int)iom_fpga_fnd_addr);
    
    return 0;
}

int dot_write(unsigned char value[10]){
    int i;

    for(i=0;i<length;i++)
    {
        _s_value = value[i] & 0x7F;
        outw(_s_value,(unsigned int)iom_fpga_dot_addr+i*2);
    }
    
    return 0;
}

int lcd_write(unsigned char value[33]){
    int i;

    unsigned short int _s_value = 0;
    
    value[length]=0;
    printk("Get Size : %d / String : %s\n",length,value);

    for(i=0;i<length;i++)
    {
        _s_value = (value[i] & 0xFF) << 8 | value[i + 1] & 0xFF;
        outw(_s_value,(unsigned int)iom_fpga_text_lcd_addr+i);
        i++;
    }
    
    return 0;
}

int led_write(int value){
    unsigned char value;
    unsigned short _s_value;
    const char *tmp = gdata;

    if (copy_from_user(&value, tmp, 1))
        return -EFAULT;

    _s_value = (unsigned short)value;
    outw(_s_value, (unsigned int)iom_fpga_led_addr);
    
    return 0;
}


int __init iom_fpga_driver_init(void)
{
    int result;
    //물리주소를 커널로 맵핑
    iom_fpga_dot_addr = ioremap(IOM_FPGA_DOT_ADDRESS, 0x10);
    iom_fpga_text_lcd_addr = ioremap(IOM_FPGA_TEXT_LCD_ADDRESS, 0x32);
    iom_fpga_led_addr = ioremap(IOM_LED_ADDRESS, 0x1);
    iom_fpga_fnd_addr = ioremap(IOM_FND_ADDRESS, 0x4);
    
    result = register_chrdev(242, "/dev/dev_driver", &iom_fpga_driver_fops);
    if(result < 0) {
        printk(KERN_WARNING"Can't get any major of FND\n");
        return result;
    }
    
    printk("kernel_timer_init\n");

    //timer init
    init_timer(&(mydata.timer));

    printk("init module\n");
    kernel_call_cnt = (unsigned char*)vmalloc(4);
    kernel_call_cnt[0] = 0;
    
    printk("init module, %s major number : %d\n", "dev drivers", 242);
    
    return 0;
}

void __exit iom_fpga_driver_exit(void)
{
    iounmap(iom_fpga_dot_addr);
    iounmap(iom_fpga_text_lcd_addr);
    iounmap(iom_fpga_led_addr);
    iounmap(iom_fpga_fnd_addr);
    
    printk("kernel_timer_exit\n");
    printk("%d\n", kernel_call_cnt[0]); //print
    vfree(kernel_call_cnt); //free
    del_timer_sync(&mydata.timer);
    
    unregister_chrdev(242, "/dev/dev_driver");
}

module_init(iom_fpga_driver_init);
module_exit(iom_fpga_driver_exit);
MODULE_LICENSE("GPL");
MODULE_AUTHOR("Huins");
