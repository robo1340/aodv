
#include <czmq.h>
#include <stdio.h>

#include "frame_sender.h"
#include "state.h"
#include "print_util.h"

void send_frame(uint64_t my_addr, link_t * dst, uint8_t * data, size_t len){
	//printf("send frame() len: %lu\n", len);
	
	
	zmsg_t * to_send = zmsg_new();
	zmsg_addmem (to_send, &my_addr, sizeof(uint64_t));
	zmsg_addmem (to_send, data, len);
	//zframe_t * to_send = zframe_new(data,len);
	zmsg_send(&to_send, dst->link);
	
	//printf("send_frame()\n");
	//printArrHex(data,len);	
}

void broadcast_frame(uint64_t my_addr, zhashx_t * links, uint8_t * data, size_t len){
	printf("broadcast_frame() -> ");
	//printArrHex(data,len);
	
	zlistx_t * addrs = zhashx_keys(links);

	
	uint64_t * addr = (uint64_t *)zlistx_first(addrs);
	//iterate first just to print off the destination addresses
	while (addr != NULL){
		printf("%lu | ", *addr);
		addr = (uint64_t *)zlistx_next(addrs);	
	}	
	printf("\n");
	
	addr = (uint64_t *)zlistx_first(addrs);
	while (addr != NULL){
		link_t * link = (link_t *)zhashx_lookup(links, addr);
		if (link != NULL){
			send_frame(my_addr, link, data, len);
		} else {
			printf("warning: couldn't find neighbor in known links\n");	
		}	
		addr = (uint64_t *)zlistx_next(addrs);	
	}	
}