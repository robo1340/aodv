
#include <czmq.h>
#include <stdio.h>

#include "frame_sender.h"
#include "state.h"
#include "print_util.h"

void send_frame(link_t * dst, uint8_t * data, size_t len){
	printf("send frame()\n");
	
	zframe_t * to_send = zframe_new(data,len);
	zframe_send(&to_send, dst->link , 0);
	
	//printf("send_frame()\n");
	//printArrHex(data,len);
	
	
		
}