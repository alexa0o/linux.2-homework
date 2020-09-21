#ifndef __PHONEBOOK_PHONEBOOK_H__
#define __PHONEBOOK_PHONEBOOK_H__

#include <linux/list.h>
#include <linux/slab.h>

#include "user_data.h"

struct phonebook_node {
  struct user_data* data;
  struct list_head  list;
};

LIST_HEAD(phonebook);

static struct user_data* phonebook_get_by_surname(const char* surname) {
  struct list_head* pos = NULL;

  list_for_each(pos, &phonebook) {
    struct phonebook_node* node = NULL;
    node = list_entry(pos, struct phonebook_node, list);
    if (node == NULL)
      return NULL;
    if (strcmp(node->data->surname, surname) == 0)
      return node->data;
  }

  return NULL;
}

static int phonebook_add_new_user(struct user_data* new_user) {
  struct phonebook_node* node = (struct phonebook_node*) kmalloc(sizeof(struct phonebook_node), GFP_KERNEL);
  if (node == NULL)
    return 1;
  node->data = new_user;
  list_add_tail(&node->list, &phonebook);
  return 0;
}

static int phonebook_delete_user_by_id(int id) {
  struct list_head* pos = NULL;
  struct list_head* tmp = NULL;

  list_for_each_safe(pos, tmp, &phonebook) {
    struct phonebook_node* node = NULL;
    node = list_entry(pos, struct phonebook_node, list);
    if (node == NULL)
      return 1;
    if (node->data->id == id) {
      list_del(pos);
      delete_user(node->data);
      kfree(node);
      return 0;
    }
  }

  return 2;
}

static int phonebook_delete_user_by_surname(const char* surname) {
  struct user_data* user = phonebook_get_by_surname(surname);
  return phonebook_delete_user_by_id(user->id);
}

static int phonebook_free_list(void) {
  struct list_head* pos = NULL;
  struct list_head* tmp = NULL;

  list_for_each_safe(pos, tmp, &phonebook) {
    struct phonebook_node* node = NULL;
    node = list_entry(pos, struct phonebook_node, list);
    list_del(pos);
    delete_user(node->data);
    kfree(node);
  }

  return 0;
}

#endif // __PHONEBOOK_PHONEBOOK_H__
