#ifndef NET_PACKAGE_H_
#define NET_PACKAGE_H_

#include "common.h"

#define PACKAGE_HEAD_LENGTH	  13
#define DATA_MAX_LENGTH		  1588
#define PACKAGE_MAX_LENGTH 	  PACKAGE_HEAD_LENGTH + DATA_MAX_LENGTH +  PACKAGE_CRC_LENGTH 
#define PACKAGE_CRC_LENGTH	  4

#define MAX_TTL 	7


typedef struct {
        unsigned int data_len;
        U8 port;
        U8 PR_TYPE_SubT;
        U8 rcv_addr;
        U8 snd_addr;
        U8 dst_addr;
        U8 src_addr;
        U8 SEQ_H;
        U8 SN_TTL;
        U8 CoS_ACK_Rev;
        char data[DATA_MAX_LENGTH];
        unsigned int CRC;
}ap_package_t;

extern inline void set_snd_addr(ap_package_t *pkt, U8 addr);
extern inline U8 get_snd_addr(ap_package_t *pkt);
extern inline void set_rcv_addr(ap_package_t *pkt, U8 addr);
extern inline U8 get_rcv_addr(ap_package_t *pkt);
extern inline void set_src_addr(ap_package_t *pkt, U8 addr);
extern inline U8 get_src_addr(ap_package_t *pkt);
extern inline void set_dst_addr(ap_package_t *pkt, U8 addr);
extern inline U8 get_dst_addr(ap_package_t *pkt);
extern inline void set_ttl(ap_package_t *pkt, U8 ttl);
extern inline U8 get_ttl(ap_package_t *pkt);
extern inline void set_sign(ap_package_t *pkt, U8 sign);
extern inline U8 get_sign(ap_package_t *pkt);
extern inline void set_type(ap_package_t *pkt, U8 type);
extern inline U8 get_type(ap_package_t *pkt);
extern inline void set_port(ap_package_t *pkt, U8 port);
extern inline U8 get_port(ap_package_t *pkt);
extern inline void set_data_len(ap_package_t *pkt, unsigned int data_len);
extern inline unsigned int get_data_len(ap_package_t *pkt);
extern inline void set_SN(ap_package_t *pkt, U8 SN);
extern inline U8 get_SN(ap_package_t *pkt);
extern inline void set_H(ap_package_t *pkt, U8 H);
extern inline U8 get_H(ap_package_t *pkt);
inline void set_CoS(ap_package_t *pkt, U8 data_type);
inline unsigned int get_CoS(ap_package_t *pkt);
extern void init_package_head(ap_package_t *pkt, U8 data_type,U8 src_addr, U8 dst_addr, U8 port, U8 ttl, U8 type);


#endif

