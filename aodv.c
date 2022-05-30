


#include "aodv.h"


#define RREQ_RECEIVED_BUFFER_LEN 10

typedef struct rreq_received_t {
	uint32_t rreq_id;
	uint64_t addr;
	uint32_t time_received;
} rreq_received_t;


#define RREQ_MAX_TX_RATE_MS 10000
//#define RREQ_ROUTE_DISCOVERY_TIME_MS 10000 ///<the maximum amount of time to wait before sending another route request

typedef struct rreq_state_t {
	bool rreq_pending;
	uint32_t rreq_id;
	uint32_t last_send_time;
	
} rreq_state_t;
static rreq_state_t rreq_state;

typedef struct aodv_state_t {
	uint64_t my_addr;	
	uint32_t my_seq;
} aodv_state_t;
static aodv_state_t aodv_state;

	
static rreq_received_t rreq_received_buffer[RREQ_RECEIVED_BUFFER_LEN];

#define ROUTE_TABLE_LEN 50
static route_table_entry_t routing_table[ROUTE_TABLE_LEN];

////////////////
static route_table_entry_t * find_route_entry_by_addr(const uint64_t addr);


///////////////

void aodv_init(uint64_t my_addr){
	aodv_state.my_addr = my_addr;
	aodv_state.my_seq = 0;
}

bool get_next_hop_address(uint64_t * next_hop_addr, const uint64_t dst_addr){
	int i;
	for (i=0; i<ROUTE_TABLE_LEN; i++){
		if (routing_table[i].valid == 1){
			if (routing_table[i].dst_addr == dst_addr){
				*next_hop_addr = routing_table[i].next_hop_addr;	
				return true;
			}
		}
	}
	return false;
}

bool get_dst_seq(uint64_t addr, uint32_t * seq_num){
	route_table_entry_t * route = find_route_entry_by_addr(addr);
	if (route != NULL){
		if (route->dst_seq_valid == 1){
			*seq_num = route->dst_seq;
			return true;
		}
	}
	return false;
}

bool generate_rreq(rreq_t * rreq, const uint64_t dst_addr){
	if (rreq_state.rreq_pending == true) {return false;}
	
	rreq_state.rreq_pending = true;
	
	memset(rreq,0,sizeof(rreq_t));
	
	rreq->type = RREQ_TYPE;
	rreq->rreq_id = ++rreq_state.rreq_id;
	rreq->dst_addr = dst_addr;
	rreq->dst_seq_valid = (get_dst_seq(dst_addr, &rreq->dst_seq) == true) ? 1 : 0;
	rreq->src_addr = aodv_state.my_addr;
	rreq->src_seq = ++aodv_state.my_seq;
	
	return false;
}
		
static route_table_entry_t * find_route_entry_by_addr(const uint64_t addr){
	int i;
	for (i=0; i<ROUTE_TABLE_LEN; i++){
		if (routing_table[i].valid == 1){
			if (routing_table[i].dst_addr == addr){
				return &routing_table[i];	
			}
		}
	}
	return NULL;
}