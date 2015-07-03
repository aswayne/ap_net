#ifndef NETWORK_LAYER_H_
#define NETWORK_LAYER_H_

#include<sys/ipc.h>
#include<sys/msg.h>
#include<sys/shm.h>
#include<signal.h>
#include "net_package.h"
#include "common.h"
#include "transport_layer.h"
#include "hard_layer.h"

#define KEY_RT_PATH			"/etc/passwd"
#define MAX_HOPS			MAX_NODES


enum route_protocol {
	DSR=1,
	PDQR=2,
	FHR=3
};

typedef struct {
	U8 dst_addr;
	U8 hop;
	U8 path[MAX_HOPS]; 
}rt_entry;

typedef struct {
	enum route_protocol		rp;            /*when rp == 0, indicate that route-process is not run*********/
	U8	saddr;
	U8 	update_array[MAX_NODES];
	rt_entry 	hash_cache[MAX_NODES];
}route_table_cache;


void NL_send(ap_package_t *pkt);
void NL_rcv(ap_package_t *pkt);
void NL_froword(ap_package_t *pkt);

/******open route-msg-q, route-table share-memory, init rt_cache and register signal-process-function*****/
extern int network_layer_init(void);
extern void network_layer_exit(void);

#endif
