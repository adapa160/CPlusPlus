#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/init.h>
#include <linux/slab.h>
#include <linux/printk.h>
#include <linux/kernel.h>
#include <linux/usb.h>

// (------------------------Note: Please don't include libusb library--------------------------)
// TODO 0 Include necessary files to use functions like kmalloc(), kfree() and usb_control_msg()


#undef VERIFY_OCTAL_PERMISSIONS
#define VERIFY_OCTAL_PERMISSIONS(perms) (perms)


#define DRIVER_AUTHOR "RTS Fohler AG"                           // TODO 1			
#define DRIVER_DESC "RTS Labor - USB Driver"

//idVendor=16c0, idProduct=0478

#define VENDOR_ID 0x16c0                                       // TODO 1
#define PRODUCT_ID 0x478                                     // TODO 1
#define USB_ENDPOINT_IN   0x80




/*******************************************************************************************/
/*   TODO 2 Create a usb_device_id structure with above defined Vender ID and Product ID   */
/*******************************************************************************************/
 struct usb_device_id;
static struct usb_device_id my_usb_id_table [] = {
{
    USB_DEVICE(VENDOR_ID,PRODUCT_ID)
},{ }
};
MODULE_DEVICE_TABLE ( usb , my_usb_id_table);



/*******************************************************************************************/
/*      				TODO 2 END					   */
/*******************************************************************************************/





struct usb_rts {
	struct usb_device *	udev;
	unsigned char		yellow;
	unsigned char		red;
	unsigned char		green;
	unsigned char		button;
	unsigned char		pot;
};




static void write_value(struct usb_device *rts, short val, short ind)	// Write to device
{
	int retval;
	unsigned char *buffer;
/*******************************************************************************************/
/*					TODO 3 Begin
	- Allocate 8-bytes dynamically in kernel space for 'buffer'. Print error if
	  there is any.
	- Write value 'val' and index 'ind' to control endpoint. Use your own defined
	  request and requesttype. Use 'buffer' created above as data. Store returned
	  value in 'retval'.
	- Print 'retval' value.
	- Free buffer.				   					   */
/*******************************************************************************************/


buffer = kmalloc(8,GFP_KERNEL);
if(!buffer){
  
   printk(KERN_INFO, "retval -1");
   goto error;
}



retval = usb_control_msg(rts,usb_sndctrlpipe(rts, USB_ENDPOINT_IN),0x3231,0x3232,val,ind,*buffer,8,1000);
error:
kfree(buffer);
return retval;


/*******************************************************************************************/
/*      				TODO 3 END		  		           */
/*******************************************************************************************/
}








static int read_value(struct usb_device *rts, short index, unsigned char *where)		// Read from device
{
	int retval;
	unsigned char *buffer;
/*******************************************************************************************/
/*					TODO 4 Begin
	- Allocate 8-bytes dynamically in kernel space for 'buffer'. Print error if
	  there is any.
	- Read a byte in 'buffer' with value '0' and index 'index' from control endpoint.
	  Use your request and requesttype defined for reading a value in firmware. Use
	  'buffer' created above as data. Store returned value in 'retval'.
	- Print return value.
	- Copy first byte of returned buffer in 'where'.
	- Free buffer.				   					   */
/*******************************************************************************************/
buffer = kmalloc(8,GFP_KERNEL);
if(!buffer){

   printk(KERN_INFO, "retval -1");
   goto error;
}
retval = usb_control_msg(rts,usb_rcvctrlpipe(rts, USB_ENDPOINT_IN),0x3231,0x3232,0,index,*buffer,8,1000);

where=(char)retval;
error:
kfree(buffer);



/*******************************************************************************************/
/*      				TODO 4 END		  		           */
/*******************************************************************************************/
	return 0;
}






static ssize_t set_red(struct device *dev, struct device_attribute *attr, const char *buf, size_t count)
{
	struct usb_interface *intf = to_usb_interface(dev);
	struct usb_rts *rts = usb_get_intfdata(intf);
/*******************************************************************************************/
/*					TODO 5 Begin
	- Store value in string 'buf' in one byte (i.e. char) 'red' variable in 
	  usb data structure (i.e. struct usb_rts).
	- Call write_value() with appropriate arguments.				   */
/*******************************************************************************************/
buf=(char)rts->red;


write_value(rts->udev,rts->red,0);
/*******************************************************************************************/
/*      				TODO 5 END		  		           */
/*******************************************************************************************/
	return count;
}





/*******************************************************************************************/
/*					TODO 6 Begin
	- Create an attribute called dev_attr_red (with show() defined as NULL).	   */
/*******************************************************************************************/


static DEVICE_ATTR(red, 777, NULL, set_red);	


/*******************************************************************************************/
/*      				TODO 6 END		  		           */
/*******************************************************************************************/

static int rts_probe(struct usb_interface *interface, const struct usb_device_id *id)
{
	struct usb_device *udev = interface_to_usbdev(interface);
	struct usb_rts *dev = NULL;
	int retval = -ENOMEM;

/*******************************************************************************************/
/*					TODO 7 Begin
	NOTE: See Appendix B for APIs
	- Allocate memory dynamically in the Kernel RAM for the created device data
	  structure using kmalloc().
	- Goto 'error' label if unable to allocate memory.				   
	- Initialize the created device data structure to zero using memset().
	- Increment reference count of usb_device structure using usb_get_dev().
	- Store pointer to usb_device strucutre in created usb data structure.
	- Set usb interface data to created usb data structure using
	  usb_set_intfdata().
	- Get the usb_device structure from the interface structure (i.e. usb_interface.dev)
	  and create file for red LED by using the following API,
	  int device_create_file (struct device * dev, const struct device_attribute * attr);
											   */
/*******************************************************************************************/


	dev = kmalloc(8,GFP_KERNEL);
	if (!dev)
	{
        	retval = -ENOMEM;
		goto error;
	}


memset(dev,0,8);
	

usb_get_dev(udev);


dev->udev = udev;
	

usb_set_intfdata(interface, dev);	

	
	device_create_file(&interface->dev, &dev_attr_red);
/*******************************************************************************************/
/*      				TODO 7 END		  		           */
/*******************************************************************************************/
error:	
printk(KERN_ERR "Teensy device attached.\n");
	return 0;


}

static void rts_disconnect(struct usb_interface *interface)
{
	struct usb_rts *dev;

/*******************************************************************************************/
/*					TODO 8 Begin
	- Remove red LED file/attribute by using following API,
	  void device_remove_file (struct device * dev, const struct device_attribute * attr);
	- Get usb data structure from usb_interface structure.
	- Set usb_interface data to NULL.
	- Release the use of usb device structure using usb_put_dev().
	- Deallocate memory from Kernel RAM fro the device structure using kfree().
											   */
/*******************************************************************************************/

device_remove_file(&interface->dev, &dev_attr_red);


    dev = usb_get_intfdata(interface);


    usb_set_intfdata(interface, NULL);


    usb_put_dev(dev->udev);


    kfree(dev);

/*******************************************************************************************/
/*      				TODO 8 END		  		           */
/*******************************************************************************************/

	printk(KERN_ERR "Teensy device dettached.\n");
}

static struct usb_driver rts_driver = {
.name = "teensy",
.probe = rts_probe, // TODO 9//giveproduct id and name
.disconnect = rts_disconnect, // TODO 9
.id_table = my_usb_id_table, // TODO 9
};

static int __init usb_rts_init(void)
{
	int retval = 0;

	retval = usb_register(&rts_driver);
	if (retval)
		pr_err("usb_register failed. Error number %d", retval);	// Use err() instead of pr_err() for old kernels
	return retval;
}

static void __exit usb_rts_exit(void)
{
	usb_deregister(&rts_driver);
}

module_init (usb_rts_init);
module_exit (usb_rts_exit);

MODULE_AUTHOR(DRIVER_AUTHOR);
MODULE_DESCRIPTION(DRIVER_DESC);
MODULE_LICENSE("GPL");
