
#include <czmq.h>
#include <stdio.h>

#include "state.h"


link_t * link_new(char * address, float reliability){
	link_t * new_link = (link_t*)malloc(sizeof(link_t));
	//printf("creating link_t 0x%p\n", (void *)new_link);
	
	if ((reliability > 1.0) || (reliability < 0.0)) {
		new_link->reliability = 1.0;	
	} else {
		new_link->reliability = reliability;
	}
	
	char endpoint[30];
	sprintf(endpoint, "ipc:///tmp/%s\n",address);
	new_link->link = zsock_new_push(endpoint);
	return new_link;
}


void link_destroy(link_t ** to_destroy){
	//printf("destroying link_t at 0x%p\n", (void *)*to_destroy);
	
	zsock_destroy ( &((*to_destroy)->link) );
	free(*to_destroy);
}

void * address_create(void * addr){
	uint64_t * to_return = (uint64_t *)malloc(sizeof(uint64_t));
	*to_return = *(uint64_t *)addr;
	return (void *)to_return;
}

//void address_destroy(void ** addr){
//	free(*addr);
//}