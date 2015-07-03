#include "net_sys_api.h"

/***************declaration****************/
static int init_key(void);
static int get_app_key(app_key_t *app_key, U8 port);
static int release_app_key(U8 port);
static void *app_msg_rcv_pthread(void *arg);
static int start_new_app_rcv_pthread(U8 port);
static void *interface_msg_rcv_pthread(void *arg);
static inline int check_port(U8 port);
static void send_msg_to_app(char *buff, int length, int msgid);
static void APP_snd(U8 data_type,U8 dst_addr, char *buf, int length, U8 port);


/*****just as parameter for app-rcv-pthread*****/
static U8 port_array[APP_MAX_NUM];
/****APP get data_snd_key and data_snd_key from key_pool in this module through these common keys blow****/
static key_t		interface_snd_key, interface_rcv_key;
static key_entry 	key_pool[APP_MAX_NUM];
//static pthread_mutex_t		key_pool_lock = PTHREAD_MUTEX_INITIALIZER;

static int init_key(void)
{
	int 	id, index, msgid;
	key_t	key;
	
	memset(key_pool, 0, sizeof(key_pool));
	for (id=1; id<=APP_MAX_NUM*2; id++) {
		key = ftok(KEY_POOL_PATH, id);
		if (key < 0)
			return -1;
		index = (id-1)/2;
		if (id%2 == 1)
			key_pool[index].app_key.snd_key = key;
		else 
			key_pool[index].app_key.rcv_key = key;	
		/********delete exist queue**********/	
		msgid = msgget(key, SVMSG_MODE);
		msgctl(msgid, IPC_RMID, NULL);	
		//printf("msg queue key %x\n", key);
	}
	
	interface_snd_key = ftok(KEY_INTERFACE_PATH, KEY_INTERFACE_SND_ID);
	interface_rcv_key = ftok(KEY_INTERFACE_PATH, KEY_INTERFACE_RCV_ID);
	if (interface_snd_key<0 || interface_rcv_key<0)
		return -1;
	/********delete exist queue**********/		
	msgid = msgget(interface_snd_key, SVMSG_MODE);
	msgctl(msgid, IPC_RMID, NULL);	
	msgid = msgget(interface_rcv_key, SVMSG_MODE);
	msgctl(msgid, IPC_RMID, NULL);	
	
	return 0;	
}

/*
static int get_app_key(app_key_t *app_key)
{
	int 	i;
	pthread_t	tid;
	
	if (app_key == NULL)
		return -1;
		
	if (pthread_mutex_lock(&key_pool_lock) < 0)
		return -3;	
		
	for (i=0; i<APP_MAX_NUM; i++) {
		if (key_pool[i].used == 0) {
			if (start_new_app_rcv_pthread(i) < 0)
				continue;
			app_key->snd_key = 	key_pool[i].app_key.rcv_key;
			app_key->rcv_key = 	key_pool[i].app_key.snd_key; 
			key_pool[i].used = 1;
			
			if (pthread_mutex_unlock(&key_pool_lock) < 0) 
				sys_err("failed to unlock key_pool_lock");		
			return 0;
		}	
	}
	
	if (pthread_mutex_unlock(&key_pool_lock) < 0)
		sys_err("failed to unlock key_pool_lock");
	return -2;
}
static int release_app_key(app_key_t *app_key)
{
	int 	i;
	
	if (app_key == NULL)
		return -1;
	if (pthread_mutex_lock(&key_pool_lock) < 0)
		return -3;	
	for (i=0; i<APP_MAX_NUM; i++) {
		if (key_pool[i].app_key.snd_key == app_key->snd_key
		 && key_pool[i].app_key.rcv_key == app_key->rcv_key) { 
			if (msgctl(key_pool[i].msgid.snd_id, IPC_RMID, NULL) < -1)
				sys_err("failed remove message queue");
			if (msgctl(key_pool[i].msgid.rcv_id, IPC_RMID, NULL) < -1)
				sys_err("failed remove message queue");
			key_pool[i].used = 0;
			key_pool[i].msgid.snd_id = 0;
			key_pool[i].msgid.rcv_id = 0;	
			if (pthread_mutex_unlock(&key_pool_lock) < 0)
				sys_err("failed to unlock key_pool_lock");
			return 0;
		}	
	}
	if (pthread_mutex_unlock(&key_pool_lock) < 0)
		sys_err("failed to unlock key_pool_lock");
	return -2;
}
*/

static int get_app_key(app_key_t *app_key, U8 port)
{
	pthread_t	tid;
	
	if (app_key == NULL)
		return -1;
	if (port >= APP_MAX_NUM) {
		sys_info("the port user requesting is invaild");
		return -1;
	}
		
// 	if (pthread_mutex_lock(&key_pool_lock) < 0)
// 		return -3;	

	if (key_pool[port].used == 0) {
			if (start_new_app_rcv_pthread(port) < 0)
				return -1;
			app_key->snd_key = 	key_pool[port].app_key.rcv_key;
			app_key->rcv_key = 	key_pool[port].app_key.snd_key; 
			key_pool[port].used = 1;
			//printf("get_app_key:snd_key=%x rcv_key=%x\n", app_key->snd_key, app_key->rcv_key);
// 			if (pthread_mutex_unlock(&key_pool_lock) < 0) 
// 				sys_err("failed to unlock key_pool_lock");		
			return 0;
	} else {
		sys_info("the port user requesting is busy");
		return -1;
	}	
	
// 	if (pthread_mutex_unlock(&key_pool_lock) < 0)
// 		sys_err("failed to unlock key_pool_lock");
// 	return -2;
}

static int release_app_key(U8 port)
{
	int 	i;
	
	if (port >= APP_MAX_NUM) {
		sys_info("release user port is invaild");
		return -1;
	}
// 	if (pthread_mutex_lock(&key_pool_lock) < 0)
// 		return -3;	
	if (msgctl(key_pool[port].msgid.snd_id, IPC_RMID, NULL) < -1)
		sys_err("failed remove message queue");
	if (msgctl(key_pool[port].msgid.rcv_id, IPC_RMID, NULL) < -1)
		sys_err("failed remove message queue");	
	key_pool[port].used = 0;
	key_pool[port].msgid.snd_id = -1;
	key_pool[port].msgid.rcv_id = -1;
	
// 	if (pthread_mutex_unlock(&key_pool_lock) < 0)
// 		sys_err("failed to unlock key_pool_lock");
	return 0;
}

static void *app_msg_rcv_pthread(void *arg)  //这个子线程专门用来处理和向下转发app发来的数据  ，参数为申请的端口
{
	U8 		port;
	int 	msgid, rcv_msgid;
	key_t 	key;
	app_msg_buf_t	rcv_data;
	ssize_t	n;	
	
	port = *(U8*)arg;
	key = key_pool[port].app_key.snd_key; 
	msgid = msgget(key, SVMSG_MODE | IPC_CREAT | IPC_NOWAIT);  
	if (msgid < 0) {
		sys_err("failed to create message queue");
		goto p_exit;
	}
	key_pool[port].msgid.snd_id = msgid;			//key_pool的app_key已经在init_key函数的循环中被自动生成。

	key = key_pool[port].app_key.rcv_key; 
	msgid = msgget(key, SVMSG_MODE | IPC_CREAT | IPC_NOWAIT);
	if (msgid < 0) {
		sys_err("failed to create message queue");
		goto p_exit;
	}
	key_pool[port].msgid.rcv_id = msgid;
	rcv_msgid = msgid;
printf("net_system: app_msg_rcv_pthread running port is %d, msgsndid=%d, msgrcvid=%d\n", port, key_pool[port].msgid.snd_id, key_pool[port].msgid.rcv_id);	
	while(1) {
		n = msgrcv(rcv_msgid, &rcv_data, MSG_MAX_LENGTH+2, 0, 0);
		if (n < 0) {
			if (errno == EINTR)
				continue;
			else if (errno == EIDRM)
				goto p_exit;
			else if (errno == E2BIG) {
				sys_err("the size of app package is too big, net-system will close message queue");
				goto p_exit;
			}
		}
#ifdef DE_BUG	
if (rcv_data.dst_addr != 0)		
printf("app_msg_rcv_pthread: port=%d len=%d dst=%d\n", port, n-1, rcv_data.dst_addr);	
#endif
		APP_snd(rcv_data.data_type,rcv_data.dst_addr, rcv_data.data, n-2, port);	

	}

p_exit:
	if (key_pool[port].msgid.snd_id != 0)	
		if (msgctl(key_pool[port].msgid.snd_id, IPC_RMID, NULL) < -1)
			sys_err("failed remove message queue");
	if (key_pool[port].msgid.rcv_id != 0)		
		if (msgctl(key_pool[port].msgid.rcv_id, IPC_RMID, NULL) < -1)
			sys_err("failed remove message queue");
printf("net_system: app_msg_rcv_pthread exit port is %d, msgsndid=%d, msgrcvid=%d\n", port, key_pool[port].msgid.snd_id, key_pool[port].msgid.rcv_id);	
}

static int start_new_app_rcv_pthread(U8 port)  //创建用于接收数据的子线程，并传递端口参数
{
	pthread_t tid;
	
	if (pthread_create(&tid, NULL, app_msg_rcv_pthread, (void*)&port_array[port]) < 0) {
		sys_err("failed to create app_msg_rcv_pthread");
		return -1;
	}
	return 0;
}

static void *interface_msg_rcv_pthread(void *arg)  //这个子线程专门用来处理app发来的建立端口通道的请求
{
	int 	if_snd_msgid, if_rcv_msgid;
	ssize_t n;
	if_msg_buf_t  rcv_data, snd_data;
	
	if_snd_msgid = msgget(interface_snd_key, SVMSG_MODE | IPC_CREAT);//这两个消息队列专门用于端口通道的创建，即ap_socket(port)
	if_rcv_msgid = msgget(interface_rcv_key, SVMSG_MODE | IPC_CREAT);
	if (if_snd_msgid < 0 || if_rcv_msgid < 0)
		sys_exit("failed create interface-message-queue");
	while (1) {
		n = msgrcv(if_rcv_msgid, &rcv_data, MSG_MAX_LENGTH, 0, 0);
		if (n < 0) {
			if (errno == EINTR)
				continue;
			else if (errno == EIDRM)
				sys_exit("somebody illegally delete interface-message-queue, net-system will exit");
		}
		if (rcv_data.instruction == RELEASE) {
			if (release_app_key(rcv_data.port) < 0)
				sys_err("failed release app key");
			continue;	
		} else if (rcv_data.instruction != GET) {
			sys_err("receive invaild message from interface-message-queue");
			continue;
		}	
		snd_data.mtype = rcv_data.mtype;
		if (get_app_key(&snd_data.app_key, rcv_data.port) == 0) //若查询到rcv_data.port端口未被使用，则创建新的端口通道和新的接收子线程（专门处理该通道）
			snd_data.instruction = SUCCESS;
		else
			snd_data.instruction = FAILED;	
		while((n = msgsnd(if_snd_msgid, &snd_data, sizeof(snd_data)-sizeof(long), 0)) < 0)//把get_app_key获取的app_key发回给app
		 {
			if (errno == EINTR)
				continue;
			else if (errno == EIDRM)
				sys_exit("somebody illegally delete interface-message-queue, net-system will exit");	
		}	
	}	
}

static inline int check_port(U8 port)
{
	if (port >= APP_MAX_NUM)
		return -1;
	if (key_pool[port].used == 0)
		return -2;	
	return 0;	
}

static void send_msg_to_app(char *buff, int length, int msgid)
{
	app_msg_buf_t msg;
	
	if (buff == NULL | length > MSG_MAX_LENGTH | length < 0) {
		sys_info("send_msg_to_app:invalid parameter");
		return;
	}	
	memcpy(msg.data, buff, length);
	msg.mtype = 1;
	while(msgsnd(msgid, &msg, length+2,0) < 0) {
		if (errno == EINTR){
			continue;
		} else if (errno == EIDRM) {
			sys_info("send_msg_to_app:app-message-queue is down");
			return;
		} else if (errno == EAGAIN) {
			sys_info("app msg queue is full, received package will be lost and failed to send to app");
			return;
		}
	}
}

static void APP_snd(U8 data_type,U8 dst_addr, char *buf, int length, U8 port)
{
	if (dst_addr >= MAX_NODES) {
		sys_info("APP_snd:dst_addr is invalid");
		return;
	}	
	TL_send(data_type,dst_addr, buf, length, port);
	
/*	
	int i;	
	printf("APP_rcv: rcv package----len=%d\n", length);
	for (i=0; i<length; i++)
		printf("%d ",buf[i]);
	printf("\n");	
*/	
}

void APP_rcv(char *buff, int length, U8 port)
{
	int msgid;
	
	switch(check_port(port)) {
	  case -1:
	  	sys_info("the port of package is invalid and will be lost");
	  	return;
	  case -2:
	  	sys_info("destination port is unreachable, that is, there is not application binding this port");
	  	return;	
	}
	msgid = key_pool[port].msgid.snd_id;
	send_msg_to_app(buff, length, msgid);
	
	
/*	
	int i;	
	printf("APP_rcv: rcv package----len=%d\n", length);
	for (i=0; i<length; i++)
		printf("%d ",buff[i]);
	printf("\n");*/
}

int net_sys_api_init(void)
{
	U8 i;
	pthread_t tid;
	
	for (i=0; i<APP_MAX_NUM; i++) {
		port_array[i] = i;
	}
	if (init_key() < 0)
		return -1;
	if (pthread_create(&tid, NULL, interface_msg_rcv_pthread, NULL) < 0) {
		sys_err("failed to create app_msg_rcv_pthread");
		return -1;
	}	
	return 0;
}


