#include <linux/kernel.h>
#include <linux/phonebook.h>
#include <linux/fs.h>
#include <asm/segment.h>
#include <asm/uaccess.h>
#include <linux/buffer_head.h>
#include <linux/slab.h>

struct file *file_open(const char *path, int flags, int rights) {
  struct file *filp = NULL;
  mm_segment_t oldfs;
  int err = 0;

  oldfs = get_fs();
  set_fs(KERNEL_DS);
  filp = filp_open(path, flags, rights);
  set_fs(oldfs);
  if (IS_ERR(filp)) {
    err = PTR_ERR(filp);
    return NULL;
  }
  return filp;
}

void file_close(struct file *file) {
  filp_close(file, NULL);
}

int file_read(struct file *file, unsigned long long offset, unsigned char *data, unsigned int size) {
  mm_segment_t oldfs;
  int ret;

  oldfs = get_fs();
  set_fs(KERNEL_DS);

  ret = vfs_read(file, data, size, &offset);

  set_fs(oldfs);
  return ret;
}

int file_write(struct file *file, unsigned long long offset, unsigned char *data, unsigned int size) {
  mm_segment_t oldfs;
  int ret;

  oldfs = get_fs();
  set_fs(KERNEL_DS);

  ret = vfs_write(file, data, size, &offset);

  set_fs(oldfs);
  return ret;
}

asmlinkage long sys_get_user(const char* surname, unsigned int len, struct user_data* output_data) {
  struct file* file = file_open("/dev/phonebook", O_RDWR, 0);
  
  char* req = (char*) kmalloc(len + 2, GFP_KERNEL);
  req[0] = '1';
  req[1] = ' ';
  copy_from_user(req + 2, surname, len);
  file_write(file, 0, req, len + 2);
  kfree(req);

  char ans[256];
  unsigned int size = 0;
  size = file_read(file, 0, ans, 256);
  copy_to_user(output_data->surname, ans, size);

  file_close(file);
  return 0;
}

asmlinkage long sys_add_user(struct user_data* input_user) {
  struct file* file = file_open("/dev/phonebook", O_RDWR, 0);

  char req[256];
  int pos = 2;
  req[0] = '0';
  req[1] = ' ';
  int len;
  len = strlen(input_user->surname);
  copy_from_user(req + pos, input_user->surname, len);
  pos += len;
  req[pos++] = ' ';
  len = strlen(input_user->name);
  copy_from_user(req + pos, input_user->name, len);
  pos += len;
  req[pos++]  = ' ';
  file_write(file, 0, req, pos);

  file_close(file);
  return 0;
}

asmlinkage long sys_del_user(const char* surname, unsigned int len) {
  struct file* file = file_open("/dev/phonebook", O_RDWR, 0);

  char req[256];
  req[0] = '2';
  req[1] = ' ';
  copy_from_user(req + 2, surname, len);
  file_write(file, 0, req, len + 2);

  file_close(file);
  return 0;
}
