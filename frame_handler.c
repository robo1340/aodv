
#include <czmq.h>
#include <stdio.h>

#include "frame_handler.h"
#include "frame_sender.h"
#include "print_util.h"
#include "aodv.h"

void handle_received_frame(uint64_t my_addr, uint64_t immediate_addr, zhashx_t * links, uint8_t * data, size_t len){
	printf("handle_received_frame()\n");
	
	if (data[0] == AODV_RREQ_TYPE){
		printf("RREQ Message\n");
		
		//printf("len %lu\n", len);
		//printArrHex(data,len);
		print_rreq((rreq_t*) data);
		
		aodv_handle_rreq_return_t rc = aodv_handle_rreq(immediate_addr, (rreq_t*)data);
		switch(rc){
			case FORWARD:
				printf("forward RREQ\n");
				broadcast_frame(my_addr, links, data, len);
				break;
			case DISCARD:
				printf("discard RREQ\n");
				break;
			case RREP:
				printf("send RREP in response to RREQ\n");
				break;
			default: 
				printf("WARNING: unknown response to RREQ\n");
				break;
		}
		
	}
	else {
		
		printArrHex(data, len);
	}
	
}