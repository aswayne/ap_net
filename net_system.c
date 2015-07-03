#include "net_system.h"

U8 	SRC_ADDR;

int net_system_init(void)
{
	if (net_sys_api_init() < 0) {
		sys_info("failed init net_sys_api layer");
		return -1;
	}
	if (transport_layer_init() < 0) {
		sys_info("failed init transport layer");
		return -1;
	}
	if (network_layer_init() < 0) {
		sys_info("failed init network layer");
		return -1;
	}
	if (hard_layer_init() < 0) {
		sys_info("failed init hard layer");
		return -1;
	}
	return 0;
}

int main(int argc, char* argv[])
{
	if (argc < 2) {
		sys_info("please input parameter-src_addr");
		exit(0);
	}
	SRC_ADDR = atoi(argv[1]);
	if (SRC_ADDR < 1 || SRC_ADDR > MAX_NODES) {
		sys_info("parameter-src_addr is invalid");
		exit(0);
	}
	if (net_system_init() < 0) {
		sys_info("net_system init failed");
		exit(0);
	}	
	
	while(1){
		sleep(60);	
	}
}


