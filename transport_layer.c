#include "transport_layer.h"

#define APP_DATA_MAX_LENGTH		MSG_MAX_LENGTH 

struct tl_buff {
    U8 	number;
    U8 	count;
    char package[APP_DATA_MAX_LENGTH/DATA_MAX_LENGTH+1][PACKAGE_MAX_LENGTH];
};

struct tl_buff ** rcv_buf;

void TL_send(U8 data_type,U8 dst_addr, char *data, int length, U8 port)
{
    int 	left;
    int		n, count;
    char	*ptr;
    ap_package_t 	*newpkt;

#ifdef DE_BUG
    if (dst_addr != 0)
    printf("TL_send: dst=%d port=%d length=%d\n", dst_addr, port, length);
#endif
	newpkt = (ap_package_t*)malloc(sizeof(ap_package_t));
	if (newpkt == NULL) {
		sys_info("TL_send: failed malloc and lost package");
		return;
	}	
	memset(newpkt, 0, sizeof(ap_package_t));
	init_package_head(newpkt,data_type,SRC_ADDR, dst_addr, port, MAX_TTL, 0);		//MAX_TTL=7
	//init_package_head(newpkt, SRC_ADDR, dst_addr, port, MAX_TTL, AP_PACKAGE);
	left = length;
	ptr = data;
	count = 2;

	while(left > 0)
	{
		if (left > DATA_MAX_LENGTH)
                    {
                        n = DATA_MAX_LENGTH;
                       
                    }

		else
                    {
                         n = left;
                      
                    }

                set_data_len(newpkt, n);

		if (left == length) {

                        set_H(newpkt, 1);
                        set_SN(newpkt,(length-1)/DATA_MAX_LENGTH+1);

		} else {

                          printf("SN in TL send = %d\n",count);
                        set_H(newpkt, 0);
                        set_SN(newpkt,count++);

		}
		memcpy(newpkt->data, ptr, n);
		ptr += n;
		left -= n;

		NL_send(newpkt);
	}
	free(newpkt);
}

static  int combine_package(struct tl_buff *pkt, char *package)
{

	printf("combine_package\n");
	int  i, data_len;
	int len;
	char *ptr, *tmp_ptr;

	len = 0;
	ptr = package;
	for (i=0; i<pkt->number; i++) {
			tmp_ptr = pkt->package[i];
			data_len = get_data_len((ap_package_t*)tmp_ptr);//这里类型强制转换后得到子报文长度
			memcpy(ptr, tmp_ptr+PACKAGE_HEAD_LENGTH, data_len);
			ptr += data_len;
			len += data_len;
	}
		
		
#ifdef DE_BUG
	if (get_dst_addr(pkt->package[0]) != 0) {
		printf("combine_package :pkt->package[0]:");
                for (i=8; i<PACKAGE_MAX_LENGTH-8; i++)
			printf("%d ",pkt->package[0][i]);
		printf("\n");
	}
#endif		
	
	return len;
}

void TL_rcv(ap_package_t *ap_pkt)
{

	printf("TL_rcv\n");
	char *package;
	int len;
	U8 port, src_addr, H, SN;
        struct tl_buff 	*pkt_buf;

	port = get_port(ap_pkt);
	src_addr = get_src_addr(ap_pkt);
	H = get_H(ap_pkt);
        SN = get_SN(ap_pkt);

        pkt_buf = &rcv_buf[port][src_addr];

	if (H == 1) {	
		pkt_buf->number = SN;
		pkt_buf->count = 1;
		memcpy(pkt_buf->package[0], ap_pkt, PACKAGE_MAX_LENGTH);
	
	} else {
                pkt_buf->count++;
		memcpy(pkt_buf->package[SN-1], ap_pkt, PACKAGE_MAX_LENGTH);
	}

	if (pkt_buf->number == SN && pkt_buf->count == SN) {
		package = (char*)malloc(APP_DATA_MAX_LENGTH);
		if (package == NULL)
			return;
		len = combine_package(pkt_buf, package);	//重组包，先按照一定顺序收纳到中转数组中，再通过中转数组重组包
		APP_rcv(package, len, port);
		free(package);

		package == NULL;
	}
	
#ifdef DE_BUG
	if (get_dst_addr(ap_pkt) != 0) {
		printf("TL_rcv: src=%d port=%d buf->number=%d count=%d\n", src_addr, port, pkt_buf->number, pkt_buf->count);
	}
#endif
}

int transport_layer_init(void)
{

    //申请动态数组代替先前的静态数组，因为数组太大，如果用静态的连续内存会很卡顿或者编译失败

    rcv_buf = (struct tl_buff **)malloc(APP_MAX_NUM*sizeof(struct tl_buff *));//第一维
    int i;
    for(i=0;i<APP_MAX_NUM; i++)
    {
        rcv_buf[i]=(struct tl_buff*)malloc(MAX_NODES* sizeof(struct tl_buff));//第二维
    }
    memset(rcv_buf, 0, sizeof(rcv_buf));
    return 0;
}

int transport_layer_exit(void)
{

    int i;
    for(i=0;i<APP_MAX_NUM;i++)
    {
    free(rcv_buf[i]);//释放第二维指针
    }
    free(rcv_buf);//释放第一维指针

	return 0;
}







