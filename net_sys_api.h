#ifndef NET_SYS_API_H_
#define NET_SYS_API_H_

#include<sys/ipc.h>
#include<sys/msg.h>
#include"common.h"
#include"transport_layer.h"


/***********these paths for message queue IPC***************/
#define KEY_POOL_PATH		"/dev/null"
#define KEY_INTERFACE_PATH		"/etc/profile"
#define KEY_INTERFACE_SND_ID	1
#define KEY_INTERFACE_RCV_ID	2
#define APP_MAX_NUM 		10
#define MSG_MAX_LENGTH		2000



typedef struct {
	key_t 	rcv_key;
	key_t 	snd_key;
}app_key_t;

typedef struct {
	int 	snd_id;
	int 	rcv_id;
}msgid_t;

typedef struct {
	app_key_t 	app_key;
	msgid_t		msgid;
	int 		used;
}key_entry; 

typedef struct {
	long 	mtype;
	unsigned char data_type;
	unsigned char dst_addr;
	char	data[MSG_MAX_LENGTH];
}app_msg_buf_t;

enum action {
	SUCCESS,
	FAILED,
	GET,
	RELEASE
};

typedef struct {
	long		mtype;
	enum action	instruction;
	unsigned char port;
	app_key_t 	app_key;
}if_msg_buf_t;





extern void APP_rcv(char *buff, int length, U8 port);
extern int net_sys_api_init(void);

#endif

