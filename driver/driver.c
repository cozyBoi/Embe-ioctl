#include <linux/module.h>
#include <linux/fs.h>
#include <linux/init.h>
#include <linux/slab.h>
#include <linux/platform_device.h>
#include <linux/delay.h>

#include <asm/io.h>
#include <asm/ioctl.h>
#include <linux/kernel.h>
#include <linux/ioport.h>
#include <linux/module.h>
#include <linux/fs.h>
#include <linux/init.h>
#include <linux/version.h>
#include <linux/timer.h>
#include <linux/uaccess.h>


unsigned char fpga_number[10][10] = {
    {0x3e,0x7f,0x63,0x73,0x73,0x6f,0x67,0x63,0x7f,0x3e}, // 0
    {0x0c,0x1c,0x1c,0x0c,0x0c,0x0c,0x0c,0x0c,0x0c,0x1e}, // 1
    {0x7e,0x7f,0x03,0x03,0x3f,0x7e,0x60,0x60,0x7f,0x7f}, // 2
    {0xfe,0x7f,0x03,0x03,0x7f,0x7f,0x03,0x03,0x7f,0x7e}, // 3
    {0x66,0x66,0x66,0x66,0x66,0x66,0x7f,0x7f,0x06,0x06}, // 4
    {0x7f,0x7f,0x60,0x60,0x7e,0x7f,0x03,0x03,0x7f,0x7e}, // 5
    {0x60,0x60,0x60,0x60,0x7e,0x7f,0x63,0x63,0x7f,0x3e}, // 6
    {0x7f,0x7f,0x63,0x63,0x03,0x03,0x03,0x03,0x03,0x03}, // 7
    {0x3e,0x7f,0x63,0x63,0x7f,0x7f,0x63,0x63,0x7f,0x3e}, // 8
    {0x3e,0x7f,0x63,0x63,0x7f,0x3f,0x03,0x03,0x03,0x03} // 9
};

unsigned char fpga_set_full[10] = {
    // memset(array,0x7e,sizeof(array));
    0x7f,0x7f,0x7f,0x7f,0x7f,0x7f,0x7f,0x7f,0x7f,0x7f
};

unsigned char fpga_set_blank[10] = {
    // memset(array,0x00,sizeof(array));
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00
};

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
unsigned int kernel_call_cnt;

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

static int _interval;
static int _cnt;
static int _init;
static int blink_cnt = 1;
// define functions...
int iom_fpga_driver_open(struct inode *minode, struct file *mfile);
int iom_fpga_driver_release(struct inode *minode, struct file *mfile);
//int iom_fpga_driver_ioctl(struct inode *, struct file *, unsigned int, unsigned long);
long iom_fpga_driver_ioctl(struct file *, unsigned int, unsigned long);

int fnd_write(unsigned int value[4]);
int dot_write(unsigned char value[10]);
int lcd_write(unsigned char value[33]);
int led_write(unsigned char  value);



// define file_operations structure
struct file_operations iom_fpga_driver_fops =
{
owner:        THIS_MODULE,
open:        iom_fpga_driver_open,
release:    iom_fpga_driver_release,
unlocked_ioctl:    iom_fpga_driver_ioctl,
};

//var
static int locNotZero;
static unsigned int loc[4];

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
    
    printk("loc : ");
    for(i = 0; i < 4; i++){
        printk("%d ", loc[i]);
    }
    printk("\n");
    //return "not 0 location"
    for(i = 0; i < 4; i++){
        if(loc[i] != 0) return i;
    }
    return -1;
}

void concat_two_arr(unsigned char a[16], unsigned char b[16], unsigned char string[33], int start_a, int start_b){
    
    int i;
    for(i = 0; i < 32; i++){
        string[i] = ' ';
    }
    string[i] = 0;
    
    for(i = 0; STNUM[i] != 0 && i < 16; i++){
        string[start_a + i] = STNUM[i];
    }
    
    for(i = 0; NAME[i] != 0 && i < 16; i++){
        string[16 + start_b + i] = NAME[i];
    }
}

void update_loc(){
    if(blink_cnt == 8){
        unsigned int tmp;
        blink_cnt = 0;
        tmp = loc[locNotZero];
        loc[locNotZero] = 0;
        locNotZero = (locNotZero - 1);
        if(locNotZero == -1) locNotZero = 3;
        loc[locNotZero] = tmp;
    }
    blink_cnt++;
    loc[locNotZero] = (loc[locNotZero] + 1) % 9;
    if(loc[locNotZero] == 0) loc[locNotZero] = 1;
}

static int start_name = 0, start_num = 0;
static int len_name = 5, len_num = 8;
static int unit_name = 1, unit_num = 1;

void update_string(){
    if(start_name + len_name == 16 || (start_name == 0 && unit_name == -1)){
        unit_name *= -1;
    }
    
    if(start_num + len_num == 16 || (start_num == 0 && unit_num == -1)){
        unit_num *= -1;
    }
    
    start_name += unit_name;
    start_num += unit_num;
}

static void kernel_timer_blink(unsigned long timeout) {
    printk("start blink\n");
    struct struct_mydata *p_data = (struct struct_mydata*)timeout;
    unsigned char string[33];
    printk("kernel_timer_blink %d\n", p_data->count);
    kernel_call_cnt++; //count calling
    p_data->count--;
    if( p_data->count < 1 ) {
        int i;
        loc[3] = loc[2] = loc[1] = loc[0] = 0;
        fnd_write(loc);
        dot_write(fpga_set_blank);
        for(i = 0; i < 32; i++) string[i] = ' ';
        lcd_write(string);
        led_write(0);
        start_num = start_name = 0;
        blink_cnt = 1;
        return;
    }
    
    update_loc();
    update_string();
    
    //concat_two_arr(NAME, STNUM, string, start_name, start_num);
    concat_two_arr(STNUM, NAME, string, start_num, start_name);
    fnd_write(loc);
    dot_write(fpga_number[loc[locNotZero]]);
    lcd_write(string);
    led_write(1 << (8 - loc[locNotZero]));
    
    mydata.timer.expires = get_jiffies_64() + _interval * 10; //call itself after 3 second
    mydata.timer.data = (unsigned long)&mydata;
    mydata.timer.function = kernel_timer_blink;

    add_timer(&mydata.timer);
}

//int iom_fpga_driver_ioctl(struct inode *inode, struct file *flip, unsigned int cmd, unsigned long arg){
long iom_fpga_driver_ioctl(struct file *flip, unsigned int cmd, unsigned long arg){
    printk("driver ioctl : ");
    
    copy_from_user(&msg, (_argus*)arg, sizeof(_argus));
    printk("%d %d %d %d\n", cmd, msg.interval, msg.cnt, msg.init);
    _interval = msg.interval;
    _cnt = msg.cnt;
    _init = msg.init;
    unsigned char string[33];
    
    switch (cmd) {
        case SET_OPTION:
            printk("start set\n");
            locNotZero = parse_init(loc, _init);
            //concat_two_arr(NAME, STNUM, string, 0, 0);
            concat_two_arr(STNUM, NAME, string, 0, 0);
            printk("end para init\n");
            
            fnd_write(loc);
            printk("end fnd init\n");
            dot_write(fpga_number[loc[locNotZero]]);
            printk("end dot init\n");
            lcd_write(string);
            led_write(1 << (8 - loc[locNotZero]));
            printk("end set\n");
            break;
        case COMMAND:
            printk("start COMMAND\n");
            mydata.timer.expires = jiffies + _interval * 10; //after 3 second, call "blink"
            mydata.timer.data = (unsigned long)&mydata;
            mydata.timer.function = kernel_timer_blink;
            add_timer(&mydata.timer);
            mydata.count = msg.cnt;
            break;
        default:
            printk("invalid command\n");
            break;
    }
    
    return 0;
}

int fnd_write(unsigned int _value[4]){
    unsigned int value[4];
    int i = 0;
    for(i = 0; i < 4; i++){
        value[i] = _value[3-i];
    }
    unsigned short int value_short = 0;

    value_short = value[0] << 12 | value[1] << 8 |value[2] << 4 |value[3];
    outw(value_short,(unsigned int)iom_fpga_fnd_addr);
    
    return 0;
}

int dot_write(unsigned char value[10]){
    int i;

     unsigned short int _s_value = 0;
    
    for(i=0;i<10;i++)
    {
        _s_value = value[i] & 0x7F;
        outw(_s_value,(unsigned int)iom_fpga_dot_addr+i*2);
    }
    
    return 0;
}

int lcd_write(unsigned char value[33]){
    int i;

    unsigned short int _s_value = 0;

    for(i=0;i<33;i++)
    {
        _s_value = (value[i] & 0xFF) << 8 | value[i + 1] & 0xFF;
        outw(_s_value,(unsigned int)iom_fpga_text_lcd_addr+i);
        i++;
    }
    
    return 0;
}

int led_write(unsigned char  value){
    unsigned short _s_value;

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
    kernel_call_cnt = 0;
    
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
    printk("%d\n", kernel_call_cnt); //print
    del_timer_sync(&mydata.timer);
    
    unregister_chrdev(242, "/dev/dev_driver");
}

module_init(iom_fpga_driver_init);
module_exit(iom_fpga_driver_exit);
MODULE_LICENSE("GPL");
MODULE_AUTHOR("Huins");
