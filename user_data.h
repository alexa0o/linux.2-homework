#ifndef __PHONEBOOK_USER_DATA_H__
#define __PHONEBOOK_USER_DATA_H__

#include <linux/slab.h>
//#include <linux/phonebook.h>

struct user_data {
  char* surname;
  char* name;
  char* age;
  char* email;
  char* phone;
  int   id;
};

static int id_counter = 0;

static char* copystr(const char* s) {
  int len = strlen(s);
  char* new_str = (char*) kmalloc((len + 1) * sizeof(char), GFP_KERNEL);
  strncpy(new_str, s, len);
  new_str[len] = '\0';
  return new_str;
}

static struct user_data* new_user(const char* name, const char* surname, const char* age, const char* email, const char* phone) {
  struct user_data* user = (struct user_data*) kmalloc(sizeof(struct user_data), GFP_KERNEL);
  if (user == NULL)
    return NULL;
  user->id = id_counter++;
  user->name = copystr(name);
  user->surname = copystr(surname);
  user->age = copystr(age);
  user->email = copystr(email);
  user->phone = copystr(phone);

  return user;  
}

static int delete_user(struct user_data* user) {
  if (user == NULL)
    return 1;
  kfree(user->name);
  kfree(user->surname);
  kfree(user->age);
  kfree(user->email);
  kfree(user->phone);
  kfree(user);
  return 0;
}

static char* parse_value(const char* s, int* pos) {
  int size = 0;
  const char* start = s + *pos;
  for (; s[*pos] != '\0' && s[*pos] != '\n' && s[(*pos)++] != ' '; ++size);
  char* value = (char*) kmalloc((size + 1) * sizeof(char), GFP_KERNEL);
  strncpy(value, start, size);
  value[size] = '\0';
  return value;
}

static struct user_data* parse_new_user(const char* s) {
  int pos = 0;
  for (; s[pos++] != ' ';);

  char* surname = parse_value(s, &pos);
  char* name = parse_value(s, &pos);
  char* age = parse_value(s, &pos);
  char* email = parse_value(s, &pos);
  char* phone = parse_value(s, &pos);

  struct user_data* user = new_user(name, surname, age, email, phone);

  kfree(name);
  kfree(surname);
  kfree(age);
  kfree(email);
  kfree(phone);

  return user;
}

static char user_data_to_str_buffer[256];
static int user_data_to_str_buffer_len = 0;

static void write_value(const char* value, int* pos) {
  int i;
  for (i = 0; value[i] != '\0'; ++i) {
    user_data_to_str_buffer[(*pos)++] = value[i];
    ++user_data_to_str_buffer_len;
  }
  user_data_to_str_buffer[(*pos)++] = ' ';
}

static void user_data_to_str(struct user_data* user) {
  int pos = 0;
  user_data_to_str_buffer_len = 0;

  if (user == NULL) {
    write_value("User not found", &pos);
  } else {
    write_value(user->surname, &pos);
    write_value(user->name, &pos);
    write_value(user->age, &pos);
    write_value(user->email, &pos);
    write_value(user->phone, &pos);
  }
  user_data_to_str_buffer[pos] = '\0';
  ++user_data_to_str_buffer_len;
}

#endif // __PHONEBOOK_USER_DATA_H__
