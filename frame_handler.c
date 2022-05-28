
#include <czmq.h>
#include <stdio.h>

#include "frame_handler.h"
#include "print_util.h"

void handle_received_frame(uint8_t * data, size_t len){
	printf("handle_received_frame()\n");
	printArrHex(data, len);
		
	
}