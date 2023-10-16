#ifndef _TTY_H__
#define _TTY_H__

#include <functional>

//typedef int(*tty_handle)(int);
using tty_handle = std::function<int(int)>;

typedef struct tty_hand {
	struct tty_hand* next;

	char hand_key[128];
	tty_handle tty;
} tty_hand;

//for user
void tty_register_handle(const char*, tty_handle, const int);
void tty_stop();

#endif
