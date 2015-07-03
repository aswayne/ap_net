#ifndef TRANSPORT_LAYER_H_
#define TRANSPORT_LAYER_H_

#include "net_package.h"
#include "common.h"
#include "net_sys_api.h"
#include "network_layer.h"




extern void TL_send(U8 data_type,U8 dst_addr, char *data, int length, U8 port);
extern void TL_rcv(ap_package_t *pkt);
extern int transport_layer_init(void);
extern int transport_layer_exit(void);

#endif

