#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <sys/syscall.h>

#define STRING_SIZE 32

struct user_data {
  char surname[STRING_SIZE];
  char name[STRING_SIZE];
  char age[STRING_SIZE];
  char email[STRING_SIZE];
  char phone[STRING_SIZE];
  int  id;
};

void set_user_data(struct user_data* user, char* surname, char* name, char* age, char* email, char* phone, int id) {
  strcpy(user->surname, surname);
  strcpy(user->name, name);
  strcpy(user->age, age);
  strcpy(user->email, email);
  strcpy(user->phone, phone);
  user->id = id;
}

int main(void) {
  struct user_data user;
  set_user_data(&user, "Vladimirov", "Vladimir", "27", "bigvova@ya.ru", "8-966-737-13-37", 0);
  syscall(437, &user);

  set_user_data(&user, "Username", "User", "33", "user@user.user", "8-800-555-35-35", 1);
  syscall(437, &user);

  set_user_data(&user, "Suer", "Ands", "56", "ands.suer@en.uk", "33-33-33", 2);

  syscall(436, "Username", 8, &user);
  syscall(438, "Vladimirov", 10);

  if (strcmp(user.name, "User") == 0)
    printf("Ok!\n");
}
