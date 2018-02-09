/* Necessary includes for device drivers */
#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h> /* printk() */
#include <linux/slab.h> /* kmalloc() */
#include <linux/fs.h> /* everything... */
#include <linux/errno.h> /* error codes */
#include <linux/types.h> /* size_t */
#include <linux/proc_fs.h>
#include <linux/fcntl.h> /* O_ACCMODE */
#include <linux/jiffies.h> /* jiffies */
#include <asm/system.h> /* cli(), *_flags */
#include <asm/uaccess.h> /* copy_from/to_user */
#include <linux/proc_fs.h>
#include <linux/delay.h>
#include <linux/string.h>
#include <asm/arch/hardware.h> /* GPIO access */
#include <asm/arch/gpio.h>
#include <asm/arch/pxa-regs.h>

#define MAX_COOKIE_LENGTH       PAGE_SIZE
#define MYGPIO 113
#define PWMDCR

MODULE_LICENSE("Dual BSD/GPL");


static int motor_open(struct inode *inode, struct file *filp);
static int motor_release(struct inode *inode, struct file *filp);
static ssize_t motor_write(struct file *filp, const char *buf, size_t count, loff_t *f_pos);
//static ssize_t motor_read(struct file *filp,  char *buf, size_t count, loff_t *f_pos);
static int motor_init(void);
static void motor_exit(void);




/*
 * The file operations for the pipe device
 * (some are overlayed with bare scull)
 */
struct file_operations motor_fops = {
	write: motor_write,
        //read:  motor_read,
	open: motor_open,
	release: motor_release,
};


char *ptr_tmp;
   long ret_tmp;

int go[4];
int gi[2];
int count;
int BTN0 ;
int t,j;
int rate, rate_flag = 0;
int count_set;
int count_flag;
int mov;
int df1=0;
int df0=0;
int df=0;
int dfx;
int y;
int n_flag=0;
char axis[256];
int ArrayX[256];
int ArrayY[256];;
char *z;
/* Declaration of the init and exit functions */
module_init(motor_init);
module_exit(motor_exit);

static int motor_major = 61;

/* Buffer to store data */
static char *motor_buffer;
/* length of the current message */


static unsigned capacity = 512;

static int motor_init(void)
{
	int result;
	t = HZ;
	n_flag=0;

	//pxa_gpio_mode(GPIO17_PWM1_MD);
	//pxa_gpio_mode(GPIO16_PWM0_MD );


	//PWM_PWDUTY0 = 0x3ff;

	//PWM_PWDUTY1 = 0x3ff;
	gpio_set_value( 16, 0);  
	gpio_set_value( 17, 0);	
	gpio_set_value( 28, 0);  
	gpio_set_value( 29, 0);	


	/* Registering device */
	result = register_chrdev(motor_major, "motor", &motor_fops);
	if (result < 0)
	{
		printk(KERN_ALERT"motor: cannot obtain major number %d\n", motor_major);
		return result;
	}

	/* Allocation the timer */
	

    /* Allocating nibbler */
	motor_buffer = kmalloc(capacity, GFP_KERNEL);
	if (!motor_buffer)
        {
                printk(KERN_ALERT "Insufficient kernel memory\n");
                result = -ENOMEM;
                goto fail;
        }
	memset (motor_buffer, 0, capacity);

	printk(KERN_ALERT "Inserting motor module\n"); 
	return 0;

fail:
	motor_exit();
	return result;
}


static void motor_exit(void) {
	
	
	unregister_chrdev(motor_major, "motor");
	/* Freeing buffer memory */
	if (motor_buffer)
	kfree (motor_buffer);
	printk(KERN_ALERT "Removing motor module\n");

}

static int motor_open(struct inode *inode, struct file *filp) {
	return 0;
}

static int motor_release(struct inode *inode, struct file *filp) {
	return 0;
}



static ssize_t motor_write(struct file *filp, const char *buf, size_t count, loff_t *f_pos) {                    //creating a new timer

printk(KERN_ALERT "Inside motor write function\n");
int len;
//msleep(10000);
printk(KERN_ALERT "data returned from user is %s\n",buf);
strcpy (axis,buf);
len =strlen(axis);
//msleep(600);

if(strcmp(axis,"start") == 0){
		printk(KERN_ALERT "Forward from user\n");
		df1 = 1;
}

else if(strcmp(axis,"turn") == 0){
		printk(KERN_ALERT "Turn motion\n");
		df1 = 5;
}

else if(strcmp(axis,"stop") == 0){
		printk(KERN_ALERT "Stop motion\n");
		df1 = 10;
}

if(df1 == 1)
{

		//pxa_set_cken(CKEN1_PWM1, 1);
		//PWM_PWDUTY1 = 0x000;       //17
		//pxa_set_cken(CKEN0_PWM0, 1);
		//PWM_PWDUTY0 = 0x000;      //16 
		gpio_set_value( 17, 1);  
		gpio_set_value( 16, 0);	
		gpio_set_value( 28, 1);  
		gpio_set_value( 29, 0);	
		printk ("forward motion\n"); 
}

else if(df1 == 10)
{

		//pxa_set_cken(CKEN1_PWM1, 1);							
		//PWM_PWDUTY1 = 0x3ff;
		//pxa_set_cken(CKEN0_PWM0, 1);
		//PWM_PWDUTY0 = 0x3ff;  	
	        gpio_set_value( 17, 0);  
		gpio_set_value( 16, 0);	
		gpio_set_value( 28, 0);  
		gpio_set_value( 29, 0);	
                printk ("stationary\n"); 
}

else if(df1 == 5)
{
		
		//pxa_set_cken(CKEN1_PWM1, 1);
		//PWM_PWDUTY1 = 0x200;       //17
		//pxa_set_cken(CKEN0_PWM0, 1);
		//PWM_PWDUTY0 = 0x200;      //16
		/gpio_set_value( 17, 0);  
		gpio_set_value( 16, 1);	
		gpio_set_value( 29, 1);  
		//gpio_set_value( 29, 1);	
		gpio_set_value( 28, 0);  
		//gpio_set_value( 29, 1);	
                printk ("Turn\n"); 
}

return 0;
}
