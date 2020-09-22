#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <asm/uaccess.h>
#include <linux/uaccess.h>

#include "phonebook.h"

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Alexandr Kharitonov");
MODULE_DESCRIPTION("1st homework for linux.2 course at MIPT. Just simulate work of phonebook");
MODULE_VERSION("0.01");

#define DEVICE_NAME "phonebook"

static int device_open(struct inode*, struct file*);
static int device_release(struct inode*, struct file*);

static ssize_t device_read(struct file*, char*, size_t, loff_t*);
static ssize_t device_write(struct file*, const char*, size_t, loff_t*);

static int major_num;
static int device_open_count = 0;

static struct user_data* ret_user = NULL;

static struct file_operations file_ops = {
  .open = device_open,
  .release = device_release,
  .write = device_write,
  .read = device_read
};

static ssize_t device_read(struct file* flip, char* buffer, size_t len, loff_t* offset) {
  if (*offset != 0) {
    *offset = 0;
    return 0;
  }
  printk(KERN_INFO "read\n");
  int bytes_read;
  bytes_read = sizeof(struct user_data);

  copy_to_user(buffer, ret_user, bytes_read);

  *offset = bytes_read;
  return bytes_read;
}

static ssize_t device_write(struct file* flip, const char* buffer, size_t len, loff_t* offset) {
  // printk(KERN_ALERT "This operation is not supported...\nBut you try to write: %s\n", buffer);
  if (buffer == NULL)
    return EINVAL;

  printk(KERN_INFO "write\n");
  if (len == sizeof(struct user_data)) {
    struct user_data* new_user = (struct user_data*) kmalloc(sizeof(struct user_data), GFP_KERNEL);
    copy_from_user(new_user, buffer, len);
    phonebook_add_new_user(new_user);
  } else {
    if (buffer[0] == '+') {
      ret_user = phonebook_get_by_surname(buffer + 1);
    } else if (buffer[0] == '-') {
      phonebook_delete_user_by_surname(buffer + 1);
    }
  }

  return len;
}

static int device_open(struct inode* inode, struct file* file) {
  if (device_open_count)
    return -EBUSY;
  ++device_open_count;
  try_module_get(THIS_MODULE);
  return 0;
}

static int device_release(struct inode* inode, struct file* file) {
  --device_open_count;
  module_put(THIS_MODULE);
  return 0;
}

static int __init phonebook_init(void) {
  major_num = register_chrdev(0, DEVICE_NAME, &file_ops);
  if (major_num < 0) {
    printk(KERN_ALERT "Could not register device: %d\n", major_num);
    return major_num; 
  }
  printk(KERN_INFO "%s module loaded with device major number: %d\n", DEVICE_NAME, major_num);
  return 0;
}

static void __exit phonebook_exit(void) {
  phonebook_free_list();
  unregister_chrdev(major_num, DEVICE_NAME);
  printk(KERN_INFO "Goodbye, world!\n");
}

module_init(phonebook_init);
module_exit(phonebook_exit);
