#include <linux/cdev.h>
#include <linux/errname.h>
#include <linux/kdev_t.h>
#include <linux/module.h>
#include <linux/mutex.h>
#include <linux/slab.h>
#include <linux/fs.h>

#define pr_fmt(fmt) KBUILD_MODNAME ": " fmt

#define MIDI_NUM_NOTES 128

// TODO: NOTE: ALERT: MIDI FORMAT IS BIG ENDIAN

#define MIDI_HEADER_CHUNK_BEGIN 0x4D54686400000006	// "MThd" + u32 header length (always 6)

#define MIDI_FORMAT_SINGLE_TRACK 	0
#define MIDI_FORMAT_MULTI_TRACK_SYNC 	1
#define MIDI_FORMAT_MULTI_TRACK_ASYNC 	2

#define MIDI_60_BPM_IN_TICKS_PER_QUARTER_NOTE 0x60

#define MIDI_TRACK_CHUNK_BEGIN 0x4D54726B		// "MTrk"

#define KKEY_NAME "kkey"

enum midi_event {
	MIDI_EVENT_NOTE_ON 	= 0x08,
	MIDI_EVENT_NOTE_OFF 	= 0x09,
	MIDI_EVENT_META 	= 0xFF,
};

enum midi_meta_event {
	MIDI_META_EVENT_END_OF_TRACK = 0x2F,
};

union midimsg {
	struct {
		u8 delta;
		u8 cmd_and_channel;	// bits 0-3: channel, bits 4-7: midi_event_cmd
		u8 note_number;
		u8 velocity;
	} notemsg;
	struct {
		u8 delta;
		u8 cmd;
		u8 meta_cmd;
		u8 data;
	} metamsg;
};


struct midifile {
	u64 header_chunk_begin;
	u8 header_chunk_mode;
	u8 num_tracks;
	u8 ticks_per_quarter_note;
	u32 track_chunk_begin;
	u32 track_length_bytes;
	union midimsg * msgs;
	union midimsg end_of_track;
};

struct midinote {
	u8 note;
	struct device dev;
};

struct kkey {
	dev_t devnum;
	struct class * class;
	struct cdev cdev;
	struct midinote notes[MIDI_NUM_NOTES];
	struct mutex lock;
	struct midifile file;
} kkey;

static int kkey_open(struct inode * inode, struct file * filep) {
	pr_info("open\n");
	return 0;
}

static int kkey_close(struct inode * inode, struct file * filep) {
	pr_info("close\n");
	return 0;
}

static ssize_t kkey_read(struct file * filep, char * __user buf, size_t count, loff_t *fpos) {
	pr_info("read");
	return 0;
}

static ssize_t kkey_write(struct file * filep, const char * __user buf, size_t count, loff_t *fpos) {
	pr_info("write");
	return 0;
}

static long kkey_ioctl(struct file * filep, unsigned int cmd, unsigned long arg) {
	pr_info("ioctl");
	return 0;
}

static loff_t kkey_llseek(struct file * filep, loff_t off, int whence) {
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
static char * kkey_devnode(const struct device *dev, umode_t * mode) {
	if (mode) {
		*mode = 0666;
	}

	return NULL;
}

static int __init kkey_init(void) {
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

	return 0;
	//
	//cdev_del(&kkey.cdev);
err_cdev_add:
	class_destroy(kkey.class);
err_class_create:
	unregister_chrdev_region(kkey.devnum, MIDI_NUM_NOTES);
err_alloc_chrdev_region:
	return ret;
}

static void kkey_exit(void) {
	cdev_del(&kkey.cdev);
	class_destroy(kkey.class);
	unregister_chrdev_region(kkey.devnum, MIDI_NUM_NOTES);
	pr_info("exit end\n");
}

module_init(kkey_init);
module_exit(kkey_exit);

MODULE_LICENSE("GPL");
