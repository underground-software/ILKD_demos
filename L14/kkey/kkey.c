#include <linux/cdev.h>
#include <linux/errname.h>
#include <linux/kdev_t.h>
#include <linux/module.h>
#include <linux/mutex.h>
#include <linux/slab.h>
#include <linux/fs.h>

#define pr_fmt(fmt) KBUILD_MODNAME ": " fmt

#define MIDI_NUM_NOTES 128

#define KKEY_NAME "kkey"

static struct kkey {
	dev_t devnum;
	struct class * class;
	struct cdev cdev;
	struct device * devices[MIDI_NUM_NOTES];
} kkey;

static int kkey_open(struct inode * inode, struct file * filep)
{
	pr_info("open\n");

	return 0;
}

static int kkey_close(struct inode * inode, struct file * filep)
{
	pr_info("close\n");

	return 0;
}



static ssize_t kkey_read(struct file * filep, char * __user buf, size_t count, loff_t *fpos)
{
	pr_info("read");

	return -EIO;
}

static ssize_t kkey_write(struct file * filep, const char * __user buf, size_t count, loff_t *fpos)
{
	pr_info("write");

	return -EIO;
}

static long kkey_ioctl(struct file * filep, unsigned int cmd, unsigned long arg)
{
	pr_info("ioctl");

	return 0;
}

static loff_t kkey_llseek(struct file * filep, loff_t off, int whence)
{
	pr_info("llseek");

	return 0;
}

struct file_operations kkey_fops = {
	.owner = THIS_MODULE,
	.open = kkey_open,
	.release = kkey_close,
	.read = kkey_read,
	.write = kkey_write,
	.unlocked_ioctl = kkey_ioctl,
	.llseek = kkey_llseek,
};

// entries in /dev belonging to class kkey should be RW to all users
static char * kkey_devnode(const struct device *dev, umode_t * mode)
{
	if (mode) {
		*mode = 0666;
	}

	return NULL;
}

static int __init kkey_init(void)
{
	int ret;
	pr_info("init start");

	if ((ret = alloc_chrdev_region(&kkey.devnum, 0, MIDI_NUM_NOTES, KKEY_NAME))) {
		pr_err("failed to allocate chrdev major/minor region: %s", errname(ret));
		goto err_alloc_chrdev_region;
	}

	char buf[64];
	pr_info("allocated major:minor = %s through minor %d\n", format_dev_t(buf, kkey.devnum), MIDI_NUM_NOTES);

	kkey.class = class_create(KKEY_NAME);
	if (IS_ERR(kkey.class)) {
		ret = PTR_ERR(kkey.class);
		pr_err("failed to create device class: %s\n", errname(ret));
		goto err_class_create;
	}

	pr_info("class '%s' created\n", KKEY_NAME);

	kkey.class->devnode = kkey_devnode;

	cdev_init(&kkey.cdev, &kkey_fops);

	if ((ret = cdev_add(&kkey.cdev, kkey.devnum, MIDI_NUM_NOTES))) {
		pr_err("failed to add kkey cdev: %s\n", errname(ret));
		goto err_cdev_add;
	}

	pr_info("added cdev\n");

	int i;
	for (i = 0; i < MIDI_NUM_NOTES; i++) {
		struct device * dev_i = device_create(kkey.class, NULL, MKDEV(MAJOR(kkey.devnum), i), NULL, KKEY_NAME "%03d", i);
		if (IS_ERR(dev_i)) {
			ret = PTR_ERR(dev_i);
			pr_err("failed to create kkey device %d: %s\n", i, errname(ret));
			goto err_device_create;
		}
		kkey.devices[i] = dev_i;
	}

	pr_info("created %d kkey devices\n", MIDI_NUM_NOTES);

	return 0;

err_device_create:
	for (; i > 0; i--)
		device_destroy(kkey.class, MKDEV(MAJOR(kkey.devnum), i-1));
err_cdev_add:
	class_destroy(kkey.class);
err_class_create:
	unregister_chrdev_region(kkey.devnum, MIDI_NUM_NOTES);
err_alloc_chrdev_region:
	return ret;
}

static void kkey_exit(void)
{
	for (int i = 0; i < MIDI_NUM_NOTES; i++)
		device_destroy(kkey.class, MKDEV(MAJOR(kkey.devnum), i));
	cdev_del(&kkey.cdev);
	class_destroy(kkey.class);
	unregister_chrdev_region(kkey.devnum, MIDI_NUM_NOTES);
	pr_info("exit end\n");
}

module_init(kkey_init);
module_exit(kkey_exit);

MODULE_LICENSE("GPL");
