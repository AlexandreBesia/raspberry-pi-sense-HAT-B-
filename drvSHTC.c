/***************************************************************************//**
*  \file       drvSHTC.c
*
*  \details    I2C driver using the SHTC sensor. 
*
*  \author     Alexandre Besia, Filippos Gleglakos
*
* *******************************************************************************/
#include <linux/module.h>
#include <linux/init.h>
#include <linux/slab.h>
#include <linux/i2c.h>
#include <linux/delay.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <linux/uaccess.h>
#include <linux/moduleparam.h>
#include <linux/device.h>
 
#define I2C_BUS_AVAILABLE (       1 ) // I2C Bus available in our Raspberry Pi
#define SLAVE_DEVICE_NAME ( "SHTC3" ) // Device and Driver Name
#define SHTC_SLAVE_ADDR   (    0x70 ) // SHTC3 Slave Address

#define  DEVICE_NAME "drvSHTC" // The device will appear at /dev/drvTest using this value
#define  CLASS_NAME  "hearc"   // The device class -- this is a character device driver
 
//Commands
#define SHTC3_WakeUp        0x3517
#define SHTC3_Sleep         0xB098
#define SHTC3_NM_CE_ReadTH  0x7CA2
#define SHTC3_NM_CE_ReadRH  0x5C24
#define SHTC3_NM_CD_ReadTH  0x7866
#define SHTC3_NM_CD_ReadRH  0x58E0
#define SHTC3_LM_CE_ReadTH  0x6458
#define SHTC3_LM_CE_ReadRH  0x44DE
#define SHTC3_LM_CD_ReadTH  0x609C
#define SHTC3_LM_CD_ReadRH  0x401A
#define SHTC3_Software_RES  0x401A
#define SHTC3_ID            0xEFC8

static struct i2c_adapter *shtc_i2c_adapter = NULL; // I2C Adapter Structure
static struct i2c_client  *shtc_i2c_client  = NULL; // I2C Cient Structure

static int majorNumber;  // Device number -- determined automatically
static int nbrOpens = 0; // Counts the number of times the device is opened
static struct class* devTestClass = NULL;   // The device-driver class struct pointer
static struct device* devTestDevice = NULL; // The device-driver device struct pointer
 
static unsigned short TH_DATA;

/*
** This function writes the data into the I2C client
**
**  Arguments:
**      buff -> buffer to be sent
**      len  -> Length of the data
**   
*/
static int I2C_Write(unsigned char *buf, unsigned int len)
{
    /*
    ** Sending Start condition, Slave address with R/W bit, 
    ** ACK/NACK and Stop conditions will be handled internally.
    */ 
    int ret = i2c_master_send(shtc_i2c_client, buf, len);    
    return ret;
}
 
/*
** This function reads one byte of the data from the I2C client
**
**  Arguments:
**      out_buff -> buffer wherer the data to be copied
**      len      -> Length of the data to be read
** 
*/
static int I2C_Read(unsigned char *out_buf, unsigned int len)
{
    /*
    ** Sending Start condition, Slave address with R/W bit, 
    ** ACK/NACK and Stop conditions will be handled internally.
    */ 
    int ret = i2c_master_recv(shtc_i2c_client, out_buf, len);    
    return ret;
}
 
/*
** This function sends the command/data to the SHTC.
**
**  Arguments:
**      data   -> data to be written
** 
*/
static void SHTC_Write(unsigned char * data)
{
    int ret = I2C_Write(data, 2);
}

static void SHTC_WriteCommand(unsigned short cmd)
{
    char buf[] = { (cmd >> 8), cmd };
    int state = I2C_Write(buf, 2);
}

static void SHTC_Read(void)
{
    int state;
    char buf[3];

    SHTC_WriteCommand(SHTC3_NM_CD_ReadTH);
    msleep(20);

    state = I2C_Read(buf, 3);
    TH_DATA = (buf[0] << 8 | buf[1]);
}

// Prototype functions for the driver
static int     dev_open(struct inode *, struct file *);
static ssize_t dev_read(struct file *, char *, size_t, loff_t *);
static int     dev_release(struct inode *, struct file *);

static struct file_operations fops =
{
   .open = dev_open,
   .read = dev_read,
   .release = dev_release
};

/** @brief The device open function that is called each time the device is opened
 *  This will only increment the nbrOpens counter in this case.
 *  @param inodep A pointer to an inode object (defined in linux/fs.h)
 *  @param filep A pointer to a file object (defined in linux/fs.h)
 */
static int dev_open(struct inode *inodep, struct file *filep)
{
   nbrOpens++;
   pr_info("devTest: Device has been opened %d time(s)\n", nbrOpens);
   return 0;
}

/** @brief This function is called whenever device is being read from user space i.e. data is
 *  being sent from the device to the user. In this case is uses the copy_to_user() function to
 *  send the buffer string to the user and captures any errors.
 *  @param filep A pointer to a file object (defined in linux/fs.h)
 *  @param buffer The pointer to the buffer to which this function writes the data
 *  @param len The length of the b
 *  @param offset The offset if required
 */
static ssize_t dev_read(struct file *filep, char *buffer, size_t len, loff_t *offset)
{
    SHTC_Read();   
    return (ssize_t)TH_DATA;
}

/** @brief The device release function that is called whenever the device is closed/released by
 *  the userspace program
 *  @param inodep A pointer to an inode object (defined in linux/fs.h)
 *  @param filep A pointer to a file object (defined in linux/fs.h)
 */
static int dev_release(struct inode *inodep, struct file *filep)
{
   pr_info("devTest: Device closed\n");
   return 0;
}
 
/*
** This function getting called when the slave has been found
** Note : This will be called only once when we load the driver.
*/
static int shtc_probe(struct i2c_client *client, const struct i2c_device_id *id)
{
    pr_info("SHTC Probed!!!\n");  
    return 0;
}
 
/*
** This function getting called when the slave has been removed
** Note : This will be called only once when we unload the driver.
*/
static int shtc_remove(struct i2c_client *client)
{    
    pr_info("SHTC Removed!!!\n");
    return 0;
}
 
/*
** Structure that has slave device id
*/
static const struct i2c_device_id shtc_id[] = {
        { SLAVE_DEVICE_NAME, 0 },
        { }
};
MODULE_DEVICE_TABLE(i2c, shtc_id);
 
/*
** I2C driver Structure that has to be added to linux
*/
static struct i2c_driver shtc_driver = {
        .driver = {
            .name   = SLAVE_DEVICE_NAME,
            .owner  = THIS_MODULE
        },
        .probe          = shtc_probe,
        .remove         = shtc_remove,
        .id_table       = shtc_id
};
 
/*
** I2C Board Info strucutre
*/
static struct i2c_board_info shtc_i2c_board_info = {
        I2C_BOARD_INFO(SLAVE_DEVICE_NAME, SHTC_SLAVE_ADDR)
    };
 
/*
** Initialize the SHTC driver.
*/
static int shtc_driver_init(void)
{
    int ret = -1;
    shtc_i2c_adapter = i2c_get_adapter(I2C_BUS_AVAILABLE);
    
    if( shtc_i2c_adapter != NULL )
    {
        shtc_i2c_client = i2c_new_client_device(shtc_i2c_adapter, &shtc_i2c_board_info);      
        if( shtc_i2c_client != NULL )
        {
            i2c_add_driver(&shtc_driver);
            ret = 0;
        }
        
        i2c_put_adapter(shtc_i2c_adapter);
    }
    pr_info("Driver Added!!!\n");
    return ret;
}

/**
** Initialize the device driver.
*/
static int device_init(void)
{
    pr_info("drvTest: Initializing the drvTest driver\n");

   // Try to dynamically allocate a major number for the device
   majorNumber = register_chrdev(0, DEVICE_NAME, &fops);
   if (majorNumber<0)
   {
      pr_alert("drvTest: failed to register a major number\n");
      return majorNumber;
   }
   pr_info("drvTest: registered (major number %d)\n", majorNumber);

   // Register the device class
   devTestClass = class_create(THIS_MODULE, CLASS_NAME);
   if (IS_ERR(devTestClass))
   {  // Check for error and clean up if there is
      unregister_chrdev(majorNumber, DEVICE_NAME);
      pr_alert("Failed to register device class\n");
      return PTR_ERR(devTestClass);  // Return an error on a pointer
   }
   pr_info("drvTest: device class registered\n");

   // Register the device driver
   devTestDevice = device_create(devTestClass, NULL, MKDEV(majorNumber, 0), NULL, DEVICE_NAME);
   if (IS_ERR(devTestDevice))
   {  // Clean up if there is an error
      class_destroy(devTestClass);          
      unregister_chrdev(majorNumber, DEVICE_NAME);
      pr_alert("Failed to create the device\n");
      return PTR_ERR(devTestDevice);
   }
   pr_info("drvTest: device class created\n");
   return 0;
}

/*
** Module Init function
*/
static int __init init_drivers(void)
{
    shtc_driver_init();
    return device_init();
}
 
/*
** Module Exit function
*/
static void __exit driver_exit(void)
{
    // SHTC Driver exit procedure
    i2c_unregister_device(shtc_i2c_client);
    i2c_del_driver(&shtc_driver);
    pr_info("SHTC Driver Removed!\n");
    
    // Device Driver exit procedure
    device_destroy(devTestClass, MKDEV(majorNumber, 0)); // remove the device
    class_unregister(devTestClass);                      // unregister the device class
    class_destroy(devTestClass);                         // remove the device class
    unregister_chrdev(majorNumber, DEVICE_NAME);         // unregister the major number
    pr_info("drvTest: Goodbye!\n");
}
 
module_init(init_drivers);
module_exit(driver_exit);
 
MODULE_LICENSE("GPL");
MODULE_AUTHOR("Besia, Gleglakos");
MODULE_DESCRIPTION("I2C driver using the SHTC sensor");
MODULE_VERSION("1.0");