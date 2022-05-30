#ifndef state_h
#define state_h


#include <stdint.h>
#include <stdbool.h>


typedef struct link_t{
	float reliability;
	zsock_t * link;
} link_t;

link_t * link_new(char * address, float reliability);
void link_destroy(link_t ** to_destroy);

void * address_create(void * addr);

#endif // state_h