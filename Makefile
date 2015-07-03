#CC = gcc
CC = arm-linux-gcc

objects = net_system.o net_sys_api.o transport_layer.o network_layer.o hard_layer.o common.o net_package.o

ap_net_1600:$(objects)
	$(CC) -g -o ap_net_1600 $(objects) -lpthread -lm
net_system.o:net_system.c
	$(CC) -c -g net_system.c -o net_system.o
net_sys_api.o:net_sys_api.c
	$(CC) -c -g net_sys_api.c -o net_sys_api.o
transport_layer.o:transport_layer.c
	$(CC) -c -g transport_layer.c -o transport_layer.o	
network_layer.o:network_layer.c
	$(CC) -c -g network_layer.c -o network_layer.o
hard_layer.o:hard_layer.c
	$(CC) -c -g hard_layer.c -o hard_layer.o
common.o:common.c
	$(CC) -c -g common.c -o common.o
net_package.o:net_package.c
	$(CC) -c -g net_package.c -o net_package.o
clean:
	rm *.o ap_net_1600
