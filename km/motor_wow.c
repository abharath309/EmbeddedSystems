
 
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

#include <asm/arch/hardware.h>
#include <asm/arch/gpio.h>

#define MAX_COOKIE_LENGTH       PAGE_SIZE

MODULE_LICENSE("Dual BSD/GPL");


static int motor_open(struct inode *inode, struct file *filp);
static int motor_release(struct inode *inode, struct file *filp);
static ssize_t motor_write(struct file *filp, const char *buf, size_t count, loff_t *f_pos);
static ssize_t motor_read(struct file *filp,  char *buf, size_t count, loff_t *f_pos);
static int motor_init(void);
static void motor_exit(void);




/*
 * The file operations for the pipe device
 * (some are overlayed with bare scull)
 */
struct file_operations motor_fops = {
	write: motor_write,
        read:  motor_read,
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
	
	pxa_gpio_mode( 101 | GPIO_OUT ); //pwm1
	pxa_gpio_mode( 17 | GPIO_OUT ); ////leftmotor +
	pxa_gpio_mode( 16 | GPIO_OUT ); ////leftmotor -
	pxa_gpio_mode( 28 | GPIO_OUT ); ////rightmotor +
	pxa_gpio_mode( 29 | GPIO_OUT ); ////rightmotor -


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

	//buff was here
int v,q,w,len,p;

  strcpy (axis,buf);
   len =strlen(axis);
char val_temp;

for (v = 0 ; v < len ;v=v+4)
{ 
  val_temp = axis[v];
	q = v/4;
    ArrayX[q] = val_temp - '0';
  // printk("..%d..",ArrayX[q]);
  // printk("..%d..\n",q);

 }


for (w = 2 ; w < len ;w=w+4)
{ 
  val_temp = axis[w];
	q = w/4 ;

    ArrayY[q] = val_temp - '0';

  //printk("**%d**",ArrayY[q]);
         
 }



printk("BEFORE FOR\n");
for(p= 0; p < (len/4) ;p++)
{


	//printk("**in for **\n");
  if (p == 0) { 
		df0=0;
	}//printk("...in if...\n");
  else
  {
	//printk("--in else--\n");
	//printk("..%d..",ArrayX[p]);
	//printk("''%d''",ArrayX[p-1]);
	if (ArrayX[p] ==ArrayX[p-1])
	{
          if (ArrayY[p] ==ArrayY[p-1]) 
		{
			df = 0 ;    //stop
			//printk("%d\n",mov);
		}
	}

	if (ArrayX[p] ==ArrayX[p-1])
	{
          if (ArrayY[p] > ArrayY[p-1]) 
		{

			df = 4 ;    //left
			//printk("%d\n",mov);
		}
	}

	if (ArrayX[p] ==ArrayX[p-1])
	{
          if (ArrayY[p] < ArrayY[p-1]) 
		{
			df = 3 ;    //right
			//printk("%d\n",mov);
		}
	}



	if (ArrayY[p] ==ArrayY[p-1])
	{
          if (ArrayX[p] > ArrayX[p-1]) 
		{
			df=1; //forward

		}
	}




	if (ArrayY[p] ==ArrayY[p-1])
	{
          if (ArrayX[p] < ArrayX[p-1]) 
		{
			df = 2 ;    //backward
		}
	}





       //printk("##above switch##\n");


	


	switch(df) {
     					 case 1 : 	df1=0;		 //Forward
							printk ("south\n"); 
							break; 



     					 case 2 :  	df1=1;				//backward	
							 printk ("north\n"); 
							break;



 					case 3     :    df1=3;			//right
							   printk ("west\n");                   			
							break;



     					 case 4   :     df1=2;		//left
							 printk ("east\n"); 
							break;      


					
     					 case 0   :     df1=df1;		//stationary
							printk ("stationary\n"); 
							break;                    				


     					 default : 	df1=df1;
							 printk ("default\n");
							break;
		   }
		 printk ("df %d\n",df);
		 printk ("df0 %d\n",df0);
		 printk ("df1 %d\n",df1);


		if(df0==0){
			if(df1==1){
			mov=2;
			}
			if(df1==2){
			mov=4;
			}
			if(df1==3){
			mov=3;
			}
			if(df1==0){
			mov=1;
			}
		}
		if(df0==1){
			if(df1==0){
			mov=2;
			}
			if(df1==3){
			mov=4;
			}
			if(df1==2){
			mov=3;
			}
			if(df1==1){
			mov=1;
			}
		}
		if(df0==2){
			if(df1==3){
			mov=2;
			}
			if(df1==1){
			mov=4;
			}
			if(df1==0){
			mov=3;
			}
			if(df1==2){
			mov=1;
			}
		}
		if(df0==3){
			if(df1==2){
			mov=2;
			}
			if(df1==0){
			mov=4;
			}
			if(df1==1){
			mov=3;
			}
			if(df1==3){
			mov=1;
			}
		}
		

	switch(mov) {
     					 case 1 : 	gpio_set_value( 17, 1);  
							gpio_set_value( 16, 0);	
							
							gpio_set_value( 28, 1);  
							gpio_set_value( 29, 0);		
							                      			 //Forward
							printk ("forward motion\n"); 
							break; 



     					 case 2 :  	gpio_set_value( 17, 0);  
							gpio_set_value( 16, 1);	
							
							gpio_set_value( 28, 0);  
							gpio_set_value( 29, 1);				//backward	
							 printk ("backward motion\n"); 
							break;



 					case 3     :    gpio_set_value( 17, 1);  
							gpio_set_value( 16, 0);	
							
							gpio_set_value( 28, 0);  
							gpio_set_value( 29, 0);			//right
							   printk ("right turn\n");                   			
							break;



     					 case 4   :    gpio_set_value( 17, 0);  
							gpio_set_value( 16, 0);	
							
							gpio_set_value( 28, 1);  
							gpio_set_value( 29, 0);		//left
							 printk ("left turn\n"); 
							break;      


					
     					 case 0   :     gpio_set_value( 17, 0);  
							gpio_set_value( 16, 0);	
							
							gpio_set_value( 28, 0);  
							gpio_set_value( 29, 0);		//stationary
							 printk ("stationary\n"); 
							break;                      				

     					/* case "fr" : count_set = 12;			//Forwardright
							break;



     					 case "fl" : count_set = 13;			//Forwardleft		
							break;



     					 case "br" : count_set = 14;			//Backwardright
							break;



     					 case "bl" : count_set = 15;			//Backwardleft
							break;*/


					



     					 default : 	gpio_set_value( 17, 0);  
							gpio_set_value( 16, 0);	
							
							gpio_set_value( 28, 0);  
							gpio_set_value( 29, 0);	
							 printk ("default\n");
							break;
		   }		


		df0=df1;
		}

		
	}

return 0;
 

}



static ssize_t motor_read(struct file *filp, char *buf, size_t count, loff_t *f_pos)                         
{ 
printk("...%s...",buf);
	


return 0;	
}
