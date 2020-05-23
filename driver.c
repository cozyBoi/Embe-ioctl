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

//Global variable
static int fpga_dot_port_usage = 0;
static unsigned char *iom_fpga_dot_addr;

static int fpga_fnd_port_usage = 0;
static unsigned char *iom_fpga_fnd_addr;

static int ledport_usage = 0;
static unsigned char *iom_fpga_led_addr;

static int fpga_text_lcd_port_usage = 0;
static unsigned char *iom_fpga_text_lcd_addr;

// define functions...
ssize_t iom_fpga_driver_write(struct file *inode, const char *gdata, size_t length, loff_t *off_what);
int iom_fpga_driver_open(struct inode *minode, struct file *mfile);
int iom_fpga_driver_release(struct inode *minode, struct file *mfile);
int iom_fpga_driver_ioctl(struct inode *, struct file *, unsigned int, unsigned long);

// define file_operations structure
struct file_operations iom_fpga_driver_fops =
{
    owner:        THIS_MODULE,
    open:        iom_fpga_driver_open,
    write:        iom_fpga_driver_write,
    release:    iom_fpga_driver_release,
    ioctl:    iom_fpga_driver_ioctl,
};

//the function which opens the drivers;
int iom_fpga_driver_open(struct inode *minode, struct file *mfile)
{
    if(fpga_dot_port_usage != 0) return -EBUSY;
    if(fpga_fnd_port_usage != 0) return -EBUSY;
    if(ledport_usage != 0) return -EBUSY;
    if(fpga_text_lcd_port_usage != 0) return -EBUSY;

    fpga_dot_port_usage = 1;
    fpga_fnd_port_usage = 1;
    ledport_usage = 1;
    fpga_text_lcd_port_usage = 1;
    return 0;
}

//the function which releases the drivers;
int iom_fpga_driver_release(struct inode *minode, struct file *mfile)
{
    fpga_dot_port_usage = 0;
    fpga_fnd_port_usage = 0;
    ledport_usage = 0;
    fpga_text_lcd_port_usage = 0;
    return 0;
}

// when write to fpga_dot device  ,call this function
ssize_t iom_fpga_driver_write(struct file *inode, const char *gdata, size_t length, loff_t *off_what)
{
    //여기에 소스 다 복붙하고 각각 조금씩 수정해야함
    int i;

    unsigned char value[10];
    unsigned short int _s_value;
    const char *tmp = gdata;

    if (copy_from_user(&value, tmp, length))
        return -EFAULT;

    for(i=0;i<length;i++)
    {
        _s_value = value[i] & 0x7F;
        outw(_s_value,(unsigned int)iom_fpga_dot_addr+i*2);
    }
    
    return length;
}


int iom_fpga_driver_ioctl(struct inode *, struct file *, unsigned int, unsigned long){
    
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

    printk("init module, %s major number : %d\n", "dev drivers", 242);

    return 0;
}

void __exit iom_fpga_driver_exit(void)
{
    iounmap(iom_fpga_dot_addr);
    iounmap(iom_fpga_text_lcd_addr);
    iounmap(iom_fpga_led_addr);
    iounmap(iom_fpga_fnd_addr);
    
    unregister_chrdev(242, "/dev/dev_driver");
}

module_init(iom_fpga_driver_init);
module_exit(iom_fpga_driver_exit);
MODULE_LICENSE("GPL");
MODULE_AUTHOR("Huins");
