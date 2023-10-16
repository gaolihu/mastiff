#include <termios.h>
#include <errno.h>
#include <signal.h>
#include <assert.h>
#include <fcntl.h>
#include <pthread.h>
#include <stdbool.h>
#include <sys/prctl.h>

#include <chrono>
#include <thread>

#include "cyber/common/log.h"

#include "modules/chassis/devices/dev_libs/dev_tty.h"

#define TTY_DEVICE	"/dev/tty"
#define THREAD_NAME	"tty-detect"

static int tty_fd = -1;
static struct termios save_tty;
static struct sigaction save_sigtstp, save_sigint;
static tty_hand* tty_handle_list_head = NULL;
static tty_hand* tty_handle_list_cursor = NULL;

static int readkey(int blocking)
{
	unsigned char key;
	ssize_t count;

	if (!blocking) {
		/* tty_fd should be a tty in noncanonical mode with VMIN = VTIME = 0 */

		count = read(tty_fd, &key, 1);
		if (count == -1 && errno != EINTR) {
			//AWARN << "tty read error!";
			return -1;
		}

		return (count == 1) ? key : 0;
	} else {
		struct termios tty, save_tty;

		if (tcgetattr(tty_fd, &tty) == -1) {
			AWARN << "tty tcgetattr error!";
			return -1;
		}

		save_tty = tty;

		/* change terminal temporarily to get a blocking read() */
		tty.c_cc[VMIN] = 1;

		if (tcsetattr(tty_fd, TCSANOW, &tty) == -1) {
			AWARN << "tty tcsetattr error";
			return -1;
		}

		do
			count = read(tty_fd, &key, 1);
		while (count == -1 && errno == EINTR);

		if (count == -1)
			AWARN << "tty read error!";

		if (tcsetattr(tty_fd, TCSANOW, &save_tty) == -1) {
			AWARN << "tty tcsetattr error!";
			return -1;
		}

		if (count == -1)
			return -1;

		return (count == 1) ? key : 0;
	}

	return blocking ? -1 : 0;
}

static int restore_tty(int interrupt)
{
	struct termios tty;
	struct sigaction action;
	int result = 0;

	if (tcgetattr(tty_fd, &tty) == 0 &&
			tcsetattr(tty_fd,
				interrupt ?
				TCSAFLUSH :
				TCSADRAIN,
				&save_tty) == -1) {
		if (!interrupt)
			AWARN << "tty tcsetattr error!";
		result = -1;
	}

	save_tty = tty;

	if (sigaction(SIGINT, 0, &action) == 0 &&
			sigaction(SIGINT, &save_sigint, 0) == -1) {
		if (!interrupt)
			AWARN << "tty sigaction(SIGINT) error!";
		result = -1;
	}

	save_sigint = action;

	if (sigaction(SIGTSTP, 0, &action) == 0 &&
			sigaction(SIGTSTP, &save_sigtstp, 0) == -1) {
		if (!interrupt)
			AWARN << "tty sigaction(SIGTSTP) error!";
		result = -1;
	}

	save_sigtstp = action;

	if (!interrupt) {
		if (close(tty_fd) == -1) {
			AWARN << "tty close error!";
			result = -1;
		}

		tty_fd = -1;
	}

	return result;
}

static void signal_handler(int signal)
{
	static struct sigaction save_sigcont;

	/* restore tty state and previous signal actions */
	restore_tty(1);

	/* handle SIGCONT after SIGTSTP */
	switch (signal) {
	case SIGTSTP: {
		struct sigaction action;

		sigaction(SIGCONT, 0, &save_sigcont);

		action = save_sigcont;
		action.sa_handler = signal_handler;
		sigemptyset(&action.sa_mask);
		sigaddset(&action.sa_mask, SIGTSTP);
		sigaddset(&action.sa_mask, SIGINT);
		action.sa_flags = 0;

		sigaction(SIGCONT, &action, 0);
	}
		break;

	case SIGCONT:
		sigaction(SIGCONT, &save_sigcont, 0);
		//on_same_line = 0;  /* redraw status line */
		break;
	}

	/* re-send signal, which is currently blocked */
	kill(getpid(), signal);

	/* return to previous thread, which should immediately receive the signal */
	return;
}

static int tty_setup(void)
{
	struct termios tty;
	struct sigaction action;

	tty_fd = open(TTY_DEVICE, O_RDONLY);
	if (tty_fd == -1) {
		AWARN << "tty: " << TTY_DEVICE << "open failed!";
		return -1;
	}

	if (tcgetattr(tty_fd, &save_tty) == -1) {
		AWARN << "tty tcgetattr error!";
		return -1;
	}

	if (sigaction(SIGTSTP, 0, &save_sigtstp) == -1) {
		AWARN << "tty sigaction(SIGTSTP) error!";
		return -1;
	}

	if (sigaction(SIGINT, 0, &save_sigint) == -1) {
		AWARN << "tty sigaction(SIGINT) error!";
		return -1;
	}

	/* catch SIGTSTP and SIGINT so the tty state can be restored */
	action = save_sigtstp;
	action.sa_handler = signal_handler;
	sigemptyset(&action.sa_mask);
	sigaddset(&action.sa_mask, SIGINT);
	action.sa_flags = 0;
	if (sigaction(SIGTSTP, &action, 0) == -1) {
		AWARN << "tty sigaction(SIGTSTP) error!";
		goto fail;
	}

	action = save_sigint;
	action.sa_handler = signal_handler;
	sigemptyset(&action.sa_mask);
	sigaddset(&action.sa_mask, SIGTSTP);
	sigaddset(&action.sa_mask, SIGCONT);
	action.sa_flags = 0;
	if (sigaction(SIGINT, &action, 0) == -1) {
		AWARN << "tty sigaction(SIGINT) error!";
		goto fail;
	}

	/* turn off echo and canonical mode */
	tty = save_tty;
	tty.c_lflag &= ~(ECHO | ICANON);

	/* set VMIN = VTIME = 0 so read() always returns immediately */
	tty.c_cc[VMIN]  = 0;
	tty.c_cc[VTIME] = 0;

	if (tcsetattr(tty_fd, TCSAFLUSH, &tty) == -1) {
		AWARN << "tty tcsetattr error!";
		goto fail;
	}

	//AINFO << "set up tty over OK";

	return 0;

fail:
	sigaction(SIGINT,  &save_sigint,  0);
	sigaction(SIGTSTP, &save_sigtstp, 0);
	return -1;
}

static int cmd = 0;
static int lcmd = 0;
static int val = 0;
static bool check_val = false;
static int tty_filter(tty_hand* tty_hand)
{
	int k = -1;
	int ret = 0;

	int command = readkey(0);

	if (command == -1 || command == 0)
		return 0;

	//command store
	if (command == 'c') {
		AINFO << "command: " << cmd;
		check_val = true;
	}

	if (command == 0xa) {
		ret = (cmd & 0xffff) | ((val & 0xffff) << 16);
        AINFO << tty_hand->hand_key <<
            ", command: " << cmd <<
            ", value: " << val;

		cmd = val = 0;
		check_val = false;
		return ret;
	}

	switch (command) {
	case 'w':
		cmd = val = 0;
		check_val = false;
		return 0x1b4b41; //forward
	case 's':
		cmd = val = 0;
		check_val = false;
		return 0x1b4b42; //backward
	case 'd':
		cmd = val = 0;
		check_val = false;
		return 0x1b4b43; //right
	case 'a':
		cmd = val = 0;
		check_val = false;
		return 0x1b4b44; //left
	case 'e':
		cmd = val = 0;
		check_val = false;
		return 0x1b4b45; //stop
	case 't':
		cmd = val = 0;
		check_val = false;
		return 0x1b4b46; //
	case 'y':
		cmd = val = 0;
		check_val = false;
		return 0x1b4b47; //
	case 'u':
		cmd = val = 0;
		check_val = false;
		return 0x1b4b48; //
	case 'i':
		cmd = val = 0;
		check_val = false;
		return 0x1b4b49; //
	case 'o':
		cmd = val = 0;
		check_val = false;
		return 0x1b4b50; //
	case 'p':
		cmd = val = 0;
		check_val = false;
		return 0x1b4b51; //
	case 'l':
		cmd = val = 0;
		check_val = false;
		return 0x1b4b52; //
	case 'n':
		cmd = val = 0;
		check_val = false;
		return 0x1b4b53; //
	case 'm':
		cmd = val = 0;
		check_val = false;
		return 0x1b4b54; //
	case 'v':
		cmd = val = 0;
		check_val = false;
		return 0x1b4b55; //
	case 'x':
		cmd = val = 0;
		check_val = false;
		return 0x1b4b56; //
	case 'z':
		cmd = val = 0;
		check_val = false;
		return 0x1b4b57; //
	case 'g':
		cmd = val = 0;
		check_val = false;
		return 0x1b4b58; //
	case 'b':
		cmd = val = 0;
		check_val = false;
		return 0x1b4b59; //
	case 'f':
		cmd = val = 0;
		check_val = false;
		return 0x1b4b60; //
	case 'h':
		cmd = val = 0;
		check_val = false;
		return 0x1b4b61; //
	case 'j':
		cmd = val = 0;
		check_val = false;
		return 0x1b4b62; //
	case 'k':
		cmd = val = 0;
		check_val = false;
		return 0x1b4b63; //
	case 'q':
		cmd = val = 0;
		check_val = false;
		return 'q'; //quit program

		//switch tty handler
	case 'r':
		printf("shift tty handler: {%p ~ %s} -> {%p ~ %s}",
				tty_hand, tty_hand->hand_key,
                tty_hand->next,
                tty_hand->next->hand_key);
		printf("shift tty handler: {%s} -> {%s}\n",
				tty_hand->hand_key, tty_hand->next->hand_key);
		cmd = val = 0;
		check_val = false;

		tty_handle_list_cursor = tty_handle_list_cursor->next;
		return 0;

	case 0x1b: cmd = 0x1b; break;
	case 0x5b: lcmd = 0x5b; break;
	case 0x41: //up
	case 0x42: //down
	case 0x43: //right
	case 0x44: //left
		   if (cmd == 0x1b && lcmd == 0x5b) {
			   k = cmd << 16 | lcmd << 8 | command;
			   cmd = lcmd = 0;
			   return k;
		   } else
			   return 0;

	case '0': k = 0; break;
	case '1': k = 1; break;
	case '2': k = 2; break;
	case '3': k = 3; break;
	case '4': k = 4; break;
	case '5': k = 5; break;
	case '6': k = 6; break;
	case '7': k = 7; break;
	case '8': k = 8; break;
	case '9': k = 9; break;
	case '+': return ++lcmd;
	case '-': return --lcmd;
	default: break;
	}

	if (k != -1) {
		if (check_val)
			val = val * 10 + k;
		else
			cmd = cmd * 10 + k;
	}

	return ret;
}

static bool tty_running = false;
static int g_scan_us = 10 * 1000; //10ms default
static void* tty_thread(void* arg)
{
	int cmd = 0;
	//AINFO << "Detecting tty input thread: " << THREAD_NAME;

	prctl(PR_SET_NAME, THREAD_NAME);

	if (arg) {
		/*
		 * sentinel
		 */
		assert(tty_handle_list_cursor == (tty_hand*)arg);
	}

	while (tty_running) {
		pthread_testcancel();
		if (tty_handle_list_cursor->tty) {
			cmd = tty_filter((tty_hand*)tty_handle_list_cursor);
			tty_handle_list_cursor->tty(cmd);
		}
        //looping for key check
        std::this_thread::sleep_for(std::chrono::
                microseconds(g_scan_us));
	}

	AWARN << "exit thread: " << THREAD_NAME;

	return NULL;
}

//for user
static pthread_t thread_id;
static int thread_ret = -1;
void tty_register_handle(const char* handle_name, tty_handle h, int scan_ms)
{
	AINFO << "new tty handler: {" << handle_name <<
        "}, scan interval: " << scan_ms << "ms";

    g_scan_us = scan_ms * 1000;

	tty_hand* tmp_th = NULL;
	tty_hand* iter_th = NULL;

	if (h == NULL) {
		AWARN << "Null tty handle: " << handle_name;
		return;
	}

	if (tty_handle_list_head == NULL) {
		tty_handle_list_head = (tty_hand*)
			calloc(1, sizeof(tty_hand));
		tty_handle_list_head->next = tty_handle_list_head;
		tty_handle_list_head->tty = h;
		memcpy(tty_handle_list_head->hand_key, handle_name, strlen(handle_name));

		//init the first tty handler, take it as the default handler
		tty_running = true;
		tty_setup();
		tty_handle_list_cursor = tty_handle_list_head;
		thread_ret = pthread_create(&thread_id, NULL,
				tty_thread, (void*)tty_handle_list_cursor);
		//AINFO << "handler head addr: " << tty_handle_list_head;
	} else {
		tmp_th = (tty_hand*)calloc(1, sizeof(tty_hand));
		tmp_th->tty = h;
		memcpy(tmp_th->hand_key, handle_name, strlen(handle_name));

		for (iter_th = tty_handle_list_head; ; iter_th = tty_handle_list_head->next) {
			if (iter_th->next == tty_handle_list_head) {
				iter_th->next = tmp_th;
				tmp_th->next = tty_handle_list_head;
				break;
			}
		}
		AINFO << "new: handler addr: " << tmp_th <<
            ", next: " << tmp_th->next <<
            ", nnext: " << tmp_th->next->next;
	}
}

void tty_stop()
{
	bool brk = false;
	tty_hand* iter_th = NULL;

	if (thread_ret == 0) {
		tty_running = false;
        g_scan_us = 0;
        usleep(20000);  //20ms

		pthread_join(thread_id, NULL);
		AINFO << "tty thread exit";
	} else
		return;

	for (iter_th = tty_handle_list_head->next;
			iter_th != tty_handle_list_head;
			iter_th = iter_th->next) {
		if (iter_th && iter_th->tty) {
#if 0
			AINFO << "free %p, {%s}\n", iter_th, iter_th->hand_key;
			AINFO << "iter_th: %p, next: %p, head: %p, equal: %s\n",
					iter_th, iter_th->next, tty_handle_list_head,
					iter_th->next == tty_handle_list_head ?
					"yes" : "no";
#endif
			brk = iter_th->next == tty_handle_list_head ? true : false;
			free(iter_th);

			if (brk)
				break;
		} else
			break;
	}

	if (tty_handle_list_head) {
		AINFO << "free head: " << tty_handle_list_head;
		free(tty_handle_list_head);
		tty_handle_list_head = NULL;
		tty_handle_list_cursor = NULL;
	}

	thread_ret = -1;
	AWARN << "tty thread: " << THREAD_NAME <<
        ", stop OK!\n";
}
