#include "hard_layer.h"

static int fpga_fd=-1;

static void* net_hard_rcv_pthread(void *arg)
{
	ssize_t 	n;
	ap_package_t	 newpkt;
	
	while (1) {
		n = HL_rcv(&newpkt);
		if (n != PACKAGE_MAX_LENGTH || get_src_addr(&newpkt) == 0) {
			sys_info("received invaild data from hard layer");
			printf("n=%d src_addr=%d\n", n, get_src_addr(&newpkt));
			continue;
		}
		NL_rcv(&newpkt);
	}
}

void HL_send(ap_package_t *pkt)
{	
	if (fpga_fd < 0)
		return;
		
	ssize_t n, left;
	left = PACKAGE_MAX_LENGTH;
	while (left > 0) {
		n = write(fpga_fd, pkt, left);
		if (n < 0) {
			if (errno == EINTR)	
				continue;
			else 
				sys_err("HL_send: failed write");		
		}
		left -= n;	
	} 
#ifdef DE_BUG
	if (get_dst_addr(pkt) != 0)
 	printf("HL_send: src=%d dst=%d port=%d H=%d SN=%d type=%d ttl=%d data_len=%d writen=%d\n", get_src_addr(pkt), get_dst_addr(pkt), get_port(pkt), get_H(pkt), get_SN(pkt), get_type(pkt), get_ttl(pkt), get_data_len(pkt), n);
#endif		
}

ssize_t HL_rcv(ap_package_t *pkt)
{
	if (fpga_fd < 0)
		return;

	ssize_t  n;
reread:	
	n = read(fpga_fd, pkt, PACKAGE_MAX_LENGTH);
	if (n < 0) {
		if (errno == EINTR)
			goto reread;
		else 
			sys_err("HL_rcv: failed read");
		return -1;
	}
#ifdef DE_BUG
	if (get_dst_addr(pkt) != 0)
	printf("HL_rcv: src=%d dst=%d port=%d H=%d SN=%d type=%d ttl=%d data_len=%d writen=%d\n", get_src_addr(pkt), get_dst_addr(pkt), get_port(pkt), get_H(pkt), get_SN(pkt), get_type(pkt), get_ttl(pkt), get_data_len(pkt), n);
#endif	
	return n;
}

int HL_ioctl(void *route_entry, int cmd, int length)
{
	int 	n;
	n = ioctl(fpga_fd, cmd, route_entry);
	if (n < 0) {
		sys_err("HL_ioctl:failed iotcl");
		return -1;
	}
	return n;
}

int hard_layer_init(void)
{
	pthread_t tid;
	
	fpga_fd = open(DEVNAME, O_RDWR);
	if (fpga_fd < 0) {
		sys_err("can't open fpga device");
		return -1;
	}
	if (pthread_create(&tid, NULL, net_hard_rcv_pthread, NULL) < 0) {
		sys_err("failed to create app_msg_rcv_pthread");
		return -1;
	}
	return 0;
}

void hard_layer_exit(void)
{
	if (fpga_fd > 0)
		close(fpga_fd);
}

