#include "net_package.h"



static inline void set_addr(U8 *toaddr, U8 fromaddr)
{
        *toaddr = fromaddr;
}

static inline U8 get_addr(U8 fromaddr)
{
        return fromaddr;
}

inline void set_snd_addr(ap_package_t *pkt, U8 addr)
{
	set_addr(&pkt->snd_addr, addr);
}

inline U8 get_snd_addr(ap_package_t *pkt)
{
	return get_addr(pkt->snd_addr);
}
inline void set_rcv_addr(ap_package_t *pkt, U8 addr)
{
	set_addr(&pkt->rcv_addr, addr);
}
inline U8 get_rcv_addr(ap_package_t *pkt)
{
	return get_addr(pkt->rcv_addr);
}
inline void set_src_addr(ap_package_t *pkt, U8 addr)
{
	set_addr(&pkt->src_addr, addr);
}
inline U8 get_src_addr(ap_package_t *pkt)
{
	return get_addr(pkt->src_addr);
}
inline void set_dst_addr(ap_package_t *pkt, U8 addr)
{
	set_addr(&pkt->dst_addr, addr);
}
inline U8 get_dst_addr(ap_package_t *pkt)
{
	return get_addr(pkt->dst_addr);
}

inline void set_ttl(ap_package_t *pkt, U8 ttl)   // 用ttl参数值为7把pkt->sign值通过一系列位运算变成0x80
{
	
	if (ttl > 7)
		ttl = 7;
        pkt->SN_TTL = pkt->SN_TTL & 0xf0;
        pkt->SN_TTL = pkt->SN_TTL | (ttl&0x0f);

}
inline U8 get_ttl(ap_package_t *pkt)//pkt->sign值为0x80，经过一系列位运算返回7
{
	U8 ttl;	
        ttl = pkt->SN_TTL & 0x0f;
	return ttl;	
}

inline void set_SN(ap_package_t *pkt, U8 SN)
{
    pkt->SN_TTL = pkt->SN_TTL & 0x0f;
    pkt->SN_TTL = pkt->SN_TTL | (SN & 0x0f)<<4;
}

inline U8 get_SN(ap_package_t *pkt)
{
    U8 SN;
    SN = (pkt->SN_TTL & 0xf0)>>4;
    return SN;
}

inline void set_H(ap_package_t *pkt, U8 H)
{
    pkt->SEQ_H = pkt->SEQ_H & 0xfe;
    pkt->SEQ_H = pkt->SEQ_H | (H&0x01);
}

inline U8 get_H(ap_package_t *pkt)
{
        return pkt->SEQ_H & 0x01;
}

inline void set_type(ap_package_t *pkt, U8 type) //type只有第三第四两位
{
     pkt->PR_TYPE_SubT = pkt->PR_TYPE_SubT & 0xcf;
     pkt->PR_TYPE_SubT = pkt->PR_TYPE_SubT | (type&0x03)<<4;
}

inline U8 get_type(ap_package_t *pkt)
{
    U8 type;
    type = (pkt->PR_TYPE_SubT & 0x30)>>4;
    return type;
}

inline void set_port(ap_package_t *pkt, U8 port)
{
        pkt->port = port;
}
inline U8 get_port(ap_package_t *pkt)
{
        return pkt->port;
}

inline void set_data_len(ap_package_t *pkt, unsigned int data_len)
{
	pkt->data_len = data_len;
}
inline unsigned int get_data_len(ap_package_t *pkt)
{
	return pkt->data_len;
}

inline void set_CoS(ap_package_t *pkt, U8 data_type)
{
        pkt->CoS_ACK_Rev = pkt->CoS_ACK_Rev & 0x1f;
        pkt->CoS_ACK_Rev = (pkt->CoS_ACK_Rev | data_type&0x07) <<5;
}

inline unsigned int get_CoS(ap_package_t *pkt)
{
        U8 CoS;
        CoS = (pkt->CoS_ACK_Rev & 0xe0) >>5;
        return CoS;
}

void init_package_head(ap_package_t *pkt, U8 data_type,U8 src_addr, U8 dst_addr, U8 port, U8 ttl, U8 type)
{
	memset(pkt, 0, sizeof(ap_package_t));	
	set_src_addr(pkt, src_addr);
	set_dst_addr(pkt, dst_addr);
	set_port(pkt, port);
    set_ttl(pkt, ttl);//赋值为7
    set_type(pkt, type);//type传入参数为0
    set_CoS(pkt,data_type);
}
