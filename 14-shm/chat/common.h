#ifndef COMMON_H
#define COMMON_H

#define MAX_USERS 5
#define MAX_NAME 32
#define MAX_MSG_LEN 64
#define MAX_MSGS 128

#define NEW_MSG 1
#define NEW_USER 2
#define EXIT_USER 3

struct users {
	char name[MAX_NAME];
	int last_msg_read;
	int active;
};

struct history_msg {
	char name[MAX_NAME];
	char text[MAX_MSG_LEN];
};

struct chat {
	struct users users[MAX_USERS];
	struct history_msg history[MAX_MSGS];
	int count_users;
	int count_history;
	int last_type_msg;
	int last_id_user;
};


#endif