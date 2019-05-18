
/************************************************************************* Original definitions and variables */

#include <linux/ctype.h>
#include <linux/config.h>
#include <linux/module.h>
#include <linux/kernel.h>  	
#include <linux/slab.h>
#include <linux/fs.h>       		
#include <linux/errno.h>  
#include <linux/types.h> 
#include <linux/proc_fs.h>
#include <linux/fcntl.h>
#include <asm/system.h>
#include <asm/uaccess.h>
#include <linux/string.h>

#include "encdec.h"

#define MODULE_NAME "encdec"

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Evgeny");

int 	encdec_open(struct inode *inode, struct file *filp);
int 	encdec_release(struct inode *inode, struct file *filp);
int 	encdec_ioctl(struct inode *inode, struct file *filp, unsigned int cmd, unsigned long arg);

ssize_t encdec_read_caesar( struct file *filp, char *buf, size_t count, loff_t *f_pos );
ssize_t encdec_write_caesar(struct file *filp, const char *buf, size_t count, loff_t *f_pos);

ssize_t encdec_read_xor( struct file *filp, char *buf, size_t count, loff_t *f_pos );
ssize_t encdec_write_xor(struct file *filp, const char *buf, size_t count, loff_t *f_pos);

int memory_size = 0;

MODULE_PARM(memory_size, "i");

int major = 0;

struct file_operations fops_caesar = {
	.open 	 =	encdec_open,
	.release =	encdec_release,
	.read 	 =	encdec_read_caesar,
	.write 	 =	encdec_write_caesar,
	.llseek  =	NULL,
	.ioctl 	 =	encdec_ioctl,
	.owner 	 =	THIS_MODULE
};

struct file_operations fops_xor = {
	.open 	 =	encdec_open,
	.release =	encdec_release,
	.read 	 =	encdec_read_xor,
	.write 	 =	encdec_write_xor,
	.llseek  =	NULL,
	.ioctl 	 =	encdec_ioctl,
	.owner 	 =	THIS_MODULE
};

// Implemetation suggestion:
// -------------------------
// Use this structure as your file-object's private data structure
typedef struct {
	unsigned char key;
	int read_state;
} encdec_private_date;

/************************************************************************* Additional definitions and variables */

#define SUCCESS 0
#define ERROR -1

#define DEVICE_CAESAR 0
#define DEVICE_XOR 1

#define KEY_DEFAULT 0
#define CAESAR_CONST 128

char *buffer_caesar, *buffer_xor;

/************************************************************************* Device methods implmentaion */

int init_module(void)
{

	// Device registration

	major = register_chrdev(major, MODULE_NAME, &fops_caesar);
	if(major < 0)
	{	
	    printk("init_module.register_chrdev - FAIL !!!\n");
		return major;
	}

	// Implemetation suggestion:
	// -------------------------
	// 1. Allocate memory for the two device buffers using kmalloc (each of them should be of size 'memory_size')

	// Memory allocation for caeser buffer

    buffer_caesar = kmalloc(memory_size, GFP_KERNEL);

    if(buffer_caesar == NULL)
    {
        printk("init_module.kmalloc.buffer_caesar - FAIL\n");
        return ERROR;
    }

    memset(buffer_caesar, 0, memory_size);

	// Memory allocation for xor buffer

    buffer_xor = kmalloc(memory_size, GFP_KERNEL);

    if(buffer_caesar == NULL)
    {
        printk("init_module.kmalloc.buffer_xor - FAIL\n");
        return ERROR;
    }

    memset(buffer_xor, 0, memory_size);

    printk("init_module - SUCCESS\n");

	return SUCCESS;
}

void cleanup_module(void)
{
	// Implemetation suggestion:
	// -------------------------	
	// 1. Unregister the device-driver

	unregister_chrdev(major, MODULE_NAME);

	// 2. Free the allocated device buffers using kfree

    kfree(buffer_caesar);
    kfree(buffer_xor);

    printk("cleanup_module - SUCCESS\n");

}

/************************************************************************* Common encription methods implmentaion */

int encdec_open(struct inode *inode, struct file *filp)
{

	// Input arguments check

	if (inode == NULL){
        printk("encdec_open.inode - FAIL\n");
		return ERROR;
	}

	if (filp == NULL){
        printk("encdec_open.filp - FAIL\n");
		return ERROR;
	}

	// Implemetation suggestion:
	// -------------------------
	// 1. Set 'filp->f_op' to the correct file-operations structure (use the minor value to determine which)

	int minor = MINOR(inode->i_rdev);

    if(minor == DEVICE_CAESAR)
    {
        filp->f_op = &fops_caesar;
    }
    else if(minor == DEVICE_XOR)
    {
        filp->f_op = &fops_xor;
    }
    else
    {
        printk("encdec_open.minor - FAIL\n");
        return ERROR;
    }

	// 2. Allocate memory for 'filp->private_data' as needed (using kmalloc)

    encdec_private_date *private_data = kmalloc(sizeof(encdec_private_date), GFP_KERNEL);

    if(private_data == NULL)
    {
        printk("encdec_open.kmalloc.private_data - FAIL\n");
        return ERROR;
    }

    private_data->key = KEY_DEFAULT;
    private_data->read_state = ENCDEC_READ_STATE_DECRYPT;

    // Update file pointer

    filp->private_data = private_data;

    filp->f_pos = 0;

    printk("encdec_open - SUCCESS\n");

	return SUCCESS;
}

int encdec_release(struct inode *inode, struct file *filp)
{

	// Input arguments check

	if (filp == NULL){
        printk("encdec_open.filp - FAIL\n");
		return ERROR;
	}

	// Implemetation suggestion:
	// -------------------------
	// 1. Free the allocated memory for 'filp->private_data' (using kfree)

    kfree(filp->private_data);

    printk("encdec_release - SUCCESS\n");

	return SUCCESS;
}

int encdec_ioctl(struct inode *inode, struct file *filp, unsigned int cmd, unsigned long arg)
{

	// Input arguments check

	if (inode == NULL){
        printk("encdec_open.inode - FAIL\n");
		return ERROR;
	}

	if (filp == NULL){
        printk("encdec_open.filp - FAIL\n");
		return ERROR;
	}

	// Implemetation suggestion:
	// -------------------------
	// 1. Update the relevant fields in 'filp->private_data' according to the values of 'cmd' and 'arg'

	switch(cmd) {

		case ENCDEC_CMD_CHANGE_KEY:

			((encdec_private_date *)(filp->private_data))->key = (unsigned char)arg;

			break;

		case ENCDEC_CMD_SET_READ_STATE:

			((encdec_private_date *)(filp->private_data))->read_state = (int)arg;

			break;

        case ENCDEC_CMD_ZERO:

        {

			int minor = MINOR(inode->i_rdev);

            if(minor == DEVICE_CAESAR)
            {
                memset(buffer_caesar, 0, memory_size);
            }
            else if(minor == DEVICE_XOR)
            {
                memset(buffer_xor, 0, memory_size);
            }
		    else
		    {
		        printk("encdec_ioctl.ENCDEC_CMD_ZERO.minor - FAIL\n");
		        return ERROR;
		    }

            filp->f_pos = 0;

            break;

        }

		default:

    		printk("encdec_ioctl - FAIL\n");

			return ERROR;

	}

    printk("encdec_ioctl - SUCCESS\n");

	return SUCCESS;
}

/************************************************************************* Specific encription methods implmentaion - CAESAR */

ssize_t encdec_write_caesar(struct file *filp, const char *buf, size_t count, loff_t *f_pos)
{

	// Input arguments check

	if (filp == NULL){
        printk("encdec_write_caesar.filp - FAIL\n");
		return ERROR;
	}

	if (f_pos == NULL){
        printk("encdec_open.f_pos - FAIL\n");
		return ERROR;
	}

	// Parameters initiation

    encdec_private_date *private_date = filp->private_data;

    if(private_date == NULL)
    {
        printk("encdec_write_caesar.private_date - FAIL\n");
        return ERROR;
    }

    int buffer_caesar_index = (*f_pos);

    if(buffer_caesar_index + count > memory_size)
        return -ENOSPC;

    char *buffer_temp = kmalloc(count*sizeof(char), GFP_KERNEL);

    int buffer_temp_index;

    copy_from_user(buffer_temp, buf, count);

	// Write all bytes from kernel buffer

    for(buffer_temp_index = 0; buffer_temp_index < count; buffer_temp_index++, buffer_caesar_index++)
    {
        buffer_caesar[buffer_caesar_index] = (buffer_temp[buffer_temp_index] + private_date->key) % CAESAR_CONST;
    }

    (*f_pos) = buffer_caesar_index;

    kfree(buffer_temp);

    printk("encdec_write_caesar - SUCCESS\n");

    return SUCCESS;

}

ssize_t encdec_read_caesar(struct file *filp, char *buf, size_t count, loff_t *f_pos)
{

	// Input arguments check

	if (filp == NULL){
        printk("encdec_open.filp - FAIL\n");
		return ERROR;
	}

	if (f_pos == NULL){
        printk("encdec_open.f_pos - FAIL\n");
		return ERROR;
	}

	// Parameters initiation

    encdec_private_date *private_date = filp->private_data;

    if(private_date == NULL)
    {
        printk("encdec_open.private_date - FAIL\n");
        return ERROR;
    }

    int buffer_caesar_index = (*f_pos);

    if(buffer_caesar_index >= memory_size){
        return -EINVAL;
    }

    char *buffer_temp = kmalloc(count*sizeof(char), GFP_KERNEL);

	// Read all bytes from kernel buffer

    int buffer_temp_index;

    for (buffer_temp_index = 0; buffer_temp_index < count && buffer_caesar_index < memory_size; buffer_temp_index++, buffer_caesar_index++)
    {

        switch(private_date->read_state)
        {

            case(ENCDEC_READ_STATE_RAW):

                buffer_temp[buffer_temp_index] = buffer_caesar[buffer_caesar_index];

                break;

            case(ENCDEC_READ_STATE_DECRYPT):

                buffer_temp[buffer_temp_index] = ((buffer_caesar[buffer_caesar_index] - private_date->key) + CAESAR_CONST) % CAESAR_CONST;

                break;
        }

    }

    (*f_pos) = buffer_caesar_index;

    copy_to_user(buf, buffer_temp, count);

    kfree(buffer_temp);

    printk("encdec_read_caesar - SUCCESS\n");

    return SUCCESS;

}

/************************************************************************* Specific encription methods implmentaion - CAESAR */

ssize_t encdec_write_xor(struct file *filp, const char *buf, size_t count, loff_t *f_pos)
{

	// Input arguments check

	if (filp == NULL){
        printk("encdec_write_caesar.filp - FAIL\n");
		return ERROR;
	}

	if (f_pos == NULL){
        printk("encdec_open.f_pos - FAIL\n");
		return ERROR;
	}

	// Parameters initiation

    encdec_private_date *private_date = filp->private_data;

    if(private_date == NULL)
    {
        printk("encdec_write_caesar.private_date - FAIL\n");
        return ERROR;
    }

    int buffer_xor_index = (*f_pos);

    if(buffer_xor_index + count > memory_size)
        return -ENOSPC;

    char *buffer_temp = kmalloc(count*sizeof(char), GFP_KERNEL);

    int buffer_temp_index;

    copy_from_user(buffer_temp, buf, count);

	// Write all bytes from kernel buffer

    for(buffer_temp_index = 0; buffer_temp_index < count; buffer_temp_index++, buffer_xor_index++)
    {
        buffer_xor[buffer_xor_index] = buffer_temp[buffer_temp_index] ^ private_date->key;
    }

    (*f_pos) = buffer_xor_index;

    kfree(buffer_temp);

    printk("encdec_write_caesar - SUCCESS\n");

    return SUCCESS;

}

ssize_t encdec_read_xor(struct file *filp, char *buf, size_t count, loff_t *f_pos)
{

	// Input arguments check

	if (filp == NULL){
        printk("encdec_open.filp - FAIL\n");
		return ERROR;
	}

	if (f_pos == NULL){
        printk("encdec_open.f_pos - FAIL\n");
		return ERROR;
	}

	// Parameters initiation

    encdec_private_date *private_date = filp->private_data;

    if(private_date == NULL)
    {
        printk("encdec_open.private_date - FAIL\n");
        return ERROR;
    }

    int buffer_xor_index = (*f_pos);

    if(buffer_xor_index >= memory_size){
        return -EINVAL;
    }

    char *buffer_temp = kmalloc(count*sizeof(char), GFP_KERNEL);

	// Read all bytes from kernel buffer

    int buffer_temp_index;

    for (buffer_temp_index = 0; buffer_temp_index < count && buffer_xor_index < memory_size; buffer_temp_index++, buffer_xor_index++)
    {

        switch(private_date->read_state)
        {

            case(ENCDEC_READ_STATE_RAW):

                buffer_temp[buffer_temp_index] = buffer_xor[buffer_xor_index];

                break;

            case(ENCDEC_READ_STATE_DECRYPT):

                buffer_temp[buffer_temp_index] = buffer_xor[buffer_xor_index] ^ private_date->key;

                break;
        }

    }

    (*f_pos) = buffer_xor_index;

    copy_to_user(buf, buffer_temp, count);

    kfree(buffer_temp);

    printk("encdec_read_caesar - SUCCESS\n");

    return SUCCESS;

}
