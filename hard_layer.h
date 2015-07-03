#ifndef HARD_LAYER_H_
#define HARD_LAYER_H_

#include "transport_layer.h"
#include "common.h"

#define DEVNAME 	"/dev/fpga"
/*******ioctl for changing kernel-route-table********/
#define FIOUPDATE	1
#define FIOADD		2
#define FIODELETE	3


extern int hard_layer_init(void);
extern void hard_layer_exit(void);
extern void HL_send(ap_package_t *pkt);
extern ssize_t HL_rcv(ap_package_t *pkt);
extern int HL_ioctl(void *route_entry, int cmd, int length);



#endif

