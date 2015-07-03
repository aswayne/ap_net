#include "network_layer.h"

#define MAX_PORT 	APP_MAX_NUM
/****this key for sending route-request to route-protocol module and sharing memory****/
int		route_msgid = -1, route_shmid = -1;
route_table_cache	*rt_cache = NULL;

static void sig_update_kernel_route_table(int signo)
{
	rt_entry rt;
	int 	i, cmd;
	for (i=0; i<MAX_NODES; i++) {
		if (rt_cache->update_array[i] == 1) {
			rt = rt_cache->hash_cache[i];
			if (rt.dst_addr == 0) {
				cmd = FIODELETE;
				rt.dst_addr = i;
			} else {
				cmd = FIOUPDATE;
			}
		reioctl:
			if (HL_ioctl((void*)&rt, cmd, sizeof(rt_entry)) < 0) {
				if (errno == EINTR)
					goto reioctl;
				sys_err("failed to update kernel router");
			}
			rt_cache->update_array[i] = 0;
printf("rt update dst=%d hop=%d next=%d\n", rt.dst_addr, rt.hop, rt.path[0]);
		}
	}
}

static rt_entry* look_up_route(U8 dst_addr)
{
/*
printf("look_up_route: dst=%d\n", dst_addr);
rt_entry *rt;
rt = &rt_cache->hash_cache[dst_addr];
printf("look_up_route: rt_table dst=%d hop=%d next=%d\n", rt->dst_addr, rt->hop, rt->path[0]);
*/
	if (rt_cache == NULL || rt_cache->rp == 0)
		return NULL;
	if (rt_cache->hash_cache[dst_addr].dst_addr == dst_addr)
		return &rt_cache->hash_cache[dst_addr];
	else
		return NULL;
}

void NL_send(ap_package_t *pkt)
{
	rt_entry *rt;

#ifdef DE_BUG
	if (get_dst_addr(pkt) != 0)
 	printf("NL_send: dst=%d port=%d\n", get_dst_addr(pkt), get_port(pkt));
#endif
	if (get_dst_addr(pkt) == 0) {
		set_rcv_addr(pkt, 0);
	} else {
		rt = look_up_route(get_dst_addr(pkt));
		if (rt == NULL) {
			sys_info("have no route");
			return;
		}
		set_rcv_addr(pkt, rt->path[0]);
	}
	set_snd_addr(pkt, SRC_ADDR);
	HL_send(pkt);

#ifdef DE_BUG
printf("NL_send: rcv-addr=%d snd-addr=%d\n", get_rcv_addr(pkt), get_snd_addr(pkt));
#endif
}

void NL_rcv(ap_package_t *pkt)
{
	U8 ttl;
	U8 dst_addr, src_addr, rcv_addr ,port;

#ifdef DE_BUG
	if (get_dst_addr(pkt) != 0)
	printf("NL_rcv: dst=%d src=%d port=%d\n", get_dst_addr(pkt), get_src_addr(pkt), get_port(pkt));
#endif
	port = get_port(pkt);
	if (port >= MAX_PORT) {
		sys_info("NL_rcv: received package port is illegal,drop it");

		int i;
		char *tmp;
		tmp = (char *)pkt;
		printf("NL_RCV:");
		for (i=0; i<32; i++)
			printf(" %d", tmp[i]);
		printf("\n");

		return;
	}
	dst_addr = get_dst_addr(pkt);
	src_addr = get_src_addr(pkt);
	rcv_addr = get_rcv_addr(pkt);
	if (src_addr == SRC_ADDR)
		return;
	if (dst_addr == SRC_ADDR || dst_addr == 0)//广播报文或者本节点即目的节点，传到上层
		TL_rcv(pkt);
	else if(rcv_addr == SRC_ADDR)
	{
		ttl = get_ttl(pkt);
		if (ttl-1 <= 0)
			return;
		NL_froword(pkt);
	}
}

void NL_froword(ap_package_t *pkt)
{
	U8 ttl;
	U8 dst_addr;
	rt_entry *rt;

	ttl = get_ttl(pkt);
	dst_addr = get_dst_addr(pkt);
	rt = look_up_route(dst_addr);
	if (rt == NULL)
		return;
	set_rcv_addr(pkt, rt->path[0]);
	set_ttl(pkt, ttl-1);
	HL_send(pkt);
}

/******open route-msg-q, route-table share-memory, init rt_cache and register signal-process-function*****/
int network_layer_init(void)
{
	int 	msgid;
	key_t 	msg_key, shm_key;
	void	*shmaddr;

	msg_key = ftok(KEY_RT_PATH, 1);
	shm_key = ftok(KEY_RT_PATH, 2);
	if (msg_key<0 || shm_key<0) {
		sys_err("failed route ftok:");
		return -1;
	}
	msgid = msgget(msg_key, SVMSG_MODE);
	msgctl(msgid, IPC_RMID, NULL);
	route_msgid = msgget(msg_key, SVMSG_MODE | IPC_CREAT);
	route_shmid = shmget(shm_key, sizeof(route_table_cache)+1,SVMSG_MODE | IPC_CREAT);
	if (route_msgid<0 || route_shmid<0) {
		sys_err("failed msgget or shmget:");
		return -1;
	}

	shmaddr = shmat(route_shmid, NULL, 0);
	if ((int)shmaddr == -1) {
		sys_err("failed shmat:");
		return -1;
	}
	rt_cache = (route_table_cache*)shmaddr;
	memset(rt_cache, 0, sizeof(rt_cache));

	if (signal(SIGUSR1, sig_update_kernel_route_table) == SIG_ERR) {
		sys_err("can't catch SIGUSR1");
		return -1;
	}
	return 0;
}

void network_layer_exit(void)
{
	if (route_msgid >= 0)
		if (msgctl(route_msgid, IPC_RMID, NULL) < -1)
			sys_err("failed remove route-message-queue");
	if (route_shmid >= 0)
		if (msgctl(route_shmid, IPC_RMID, NULL) < -1)
			sys_err("failed remove route-share-memory");
}

