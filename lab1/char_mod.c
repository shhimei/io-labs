#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/version.h>
#include <linux/types.h>
#include <linux/kdev_t.h>
#include <linux/fs.h>
#include <linux/device.h>
#include <linux/cdev.h>
#include <linux/errno.h>
#include <linux/uaccess.h>
#include <linux/proc_fs.h>
#include <linux/string.h>
#include <linux/mutex.h>

#define MAJOR_N 0
#define MINOR_N 1
#define DEV_REG_NAME "my-chardev"
#define DEV_NAME "var1"
#define CLASS_NAME "my-chd-class"

static struct class *cl;
static struct cdev c_dev;
static dev_t first;
static struct proc_dir_entry* proc_entry;
char* buffer;
size_t buf_size;
struct mutex my_mutex;


MODULE_LICENSE("GPL");
MODULE_AUTHOR("Polina");
MODULE_DESCRIPTION("io-lab1 char-dev-module");


static int my_open(struct inode *i, struct file *f)
{
    pr_info("Driver: open()\n");
    return 0;
}

static int my_close(struct inode *i, struct file *f)
{
    pr_info("Driver: close()\n");
    return 0;
}

static ssize_t my_read(struct file *f, char __user *buf, size_t len, loff_t *off)
{
    pr_info("Driver: read()\n");
    pr_info("amount of ever written info into char device: %zu\n", buf_size);
    return 0;
}

static ssize_t my_write(struct file *f, const char __user *buf,  size_t len, loff_t *off)
{
    buffer = (char*) kmalloc(len * sizeof(char), GFP_KERNEL);
    if ( copy_from_user(buffer, buf, len) ) {
        return -EFAULT;
    }
    buf_size += (strlen(buffer) - 1);
    pr_info("amount of ever written info: %zu\n", buf_size);
    pr_info("written info: %s", buffer);
    return len;
}

static ssize_t proc_write(struct file *file, const char __user * ubuf, size_t count, loff_t* ppos)
{
    pr_info("attempt to write proc file\n");
    return count;
}

static ssize_t proc_read(struct file *file, char __user * ubuf, size_t count, loff_t* ppos)
{
    if (*ppos > 0)
    {
        return 0;
    } else {
        pr_info("read file, offset = %d\n", *ppos);
    }

    pr_info("proc read\n");
    if (copy_to_user(ubuf, &buf_size, sizeof(buf_size)) != 0)
    {
        pr_info("fail\n");
        return -EFAULT;
    } else {
        *ppos = buf_size;
        pr_info("amount of ever written info into char device: %zu\n", buf_size);
        return 1;
    }
    return 1;
}


static int my_dev_uevent(struct device *dev, struct kobj_uevent_env *env)
{
    add_uevent_var(env, "DEVMODE=%#o", 0660);
    return 0;
}

static struct file_operations fops =
        {
                .owner      = THIS_MODULE,
                .open       = my_open,
                .release    = my_close,
                .read       = my_read,
                .write      = my_write
        };

static struct proc_ops proc_fops = {
        .proc_read = proc_read,
        .proc_write = proc_write
};

static int __init char_mod_init(void)
{
    int err;
    pr_alert("init module registration\n");
    if ( (err = alloc_chrdev_region(&first, MAJOR_N, MINOR_N, DEV_REG_NAME)) < 0 )
    {
        return err;
    }
    pr_info("<major, minor>: <%d, %d>\n", MAJOR(first), MINOR(first));

    if ( IS_ERR(cl = class_create(THIS_MODULE, CLASS_NAME)) )
    {
        unregister_chrdev_region(first, MINOR_N);
        return PTR_ERR(cl);
    }

    cl->dev_uevent = my_dev_uevent;

    struct device *dev_err;
    if ( IS_ERR(dev_err = device_create(cl, NULL, first, NULL, DEV_NAME)) )
    {
        class_destroy(cl);
        unregister_chrdev_region(first, MINOR_N);
        return PTR_ERR(dev_err);
    }

    cdev_init(&c_dev, &fops);
    if ( (err = cdev_add(&c_dev, first, MINOR_N)) < 0)
    {
        device_destroy(cl, first);
        class_destroy(cl);
        unregister_chrdev_region(first, MINOR_N);
        return err;
    }

    proc_entry = proc_create(DEV_NAME, 0444, NULL, &proc_fops);
    pr_alert("%s: proc file is created\n", THIS_MODULE->name);
    mutex_init(&my_mutex);
    mutex_lock(&my_mutex);
    return 0;
}

static void __exit char_mod_exit(void)
{
    cdev_del(&c_dev);
    device_destroy(cl, first);
    class_destroy(cl);
    unregister_chrdev_region(first, MINOR_N);
    pr_alert("char driver unregistered\n");

    proc_remove(proc_entry);
    pr_alert("%s: proc file is deleted\n", THIS_MODULE->name);
    kfree(buffer);
    mutex_unlock(&my_mutex);
}

module_init(char_mod_init);
module_exit(char_mod_exit);

