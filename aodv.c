
#include <stdio.h>

#include "aodv.h"
#include "aodv_rreq_received_table.h"


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

#define ROUTE_TABLE_LEN 50
static route_table_entry_t routing_table[ROUTE_TABLE_LEN];

////////////////
static route_table_entry_t * find_route_entry_by_addr(const uint64_t addr);
static route_table_entry_t * get_next_empty_route_entry(void);

///////////////

void aodv_init(uint64_t my_addr){
	aodv_state.my_addr = my_addr;
	aodv_state.my_seq = 0;
}

bool aodv_get_next_hop_address(uint64_t * next_hop_addr, const uint64_t dst_addr){
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

bool aodv_get_dst_seq(uint64_t addr, uint32_t * seq_num){
	route_table_entry_t * route = find_route_entry_by_addr(addr);
	if (route != NULL){
		if (route->dst_seq_valid == 1){
			*seq_num = route->dst_seq;
			return true;
		}
	}
	return false;
}

bool aodv_generate_rreq(rreq_t * rreq, const uint64_t dst_addr){
	if (rreq_state.rreq_pending == true) {return false;}
	
	rreq_state.rreq_pending = true;
	
	memset(rreq,0,sizeof(rreq_t));
	
	rreq->type = AODV_RREQ_TYPE;
	rreq->rreq_id = ++rreq_state.rreq_id;
	rreq->dst_addr = dst_addr;
	rreq->dst_seq_valid = (aodv_get_dst_seq(dst_addr, &rreq->dst_seq) == true) ? 1 : 0;
	rreq->src_addr = aodv_state.my_addr;
	rreq->src_seq = ++aodv_state.my_seq;
	
	return true;
}

aodv_handle_rreq_return_t aodv_handle_rreq(uint64_t immediate_addr, rreq_t * rreq){
	route_table_entry_t * route;
	
	//update/create route to previous hop
	if (find_route_entry_by_addr(immediate_addr) == NULL){
		if ((route = get_next_empty_route_entry()) != NULL){
			route->dst_addr = immediate_addr;
			route->valid = 1;
			route->hop_cnt = 1;
			route->next_hop_addr = immediate_addr;
		} else {
			printf("routing table full\n");
		}
	}
	
	//check if RREQ IP and RREQ ID is already in cache
		//discard if found
	if (rreq_received_record_exists(rreq->src_addr, rreq->rreq_id) == true){
		return DISCARD;
	}
	
	//if the rreq hasn't been received yet
	// 1. increment hop count value in RREQ
	rreq->hop_cnt++;
	// 2. search for reverse route to source of RREQ and create/update if possible
	// 		update only if src_seq in rreq is greater than seq in the table entry
	route = find_route_entry_by_addr(rreq->src_addr);
	if (route == NULL){
		if ((route = get_next_empty_route_entry()) != NULL){
			route->dst_addr = rreq->src_addr;
			route->valid = 1;
			route->dst_seq = rreq->dst_seq;
			route->dst_seq_valid = 1;
			route->hop_cnt = rreq->hop_cnt;
			route->next_hop_addr = immediate_addr;
			//route to source updated
		} else {
			printf("routing table full\n");	
		}
	}
	else { //a route to the source address already exists
		if (rreq->dst_seq > route->dst_seq) {
			route->dst_addr = rreq->src_addr;
			route->valid = 1;
			route->dst_seq = rreq->dst_seq;
			route->dst_seq_valid = 1;
			route->hop_cnt = rreq->hop_cnt;
			route->next_hop_addr = immediate_addr;		
			//route to src updated
		}
		else if (rreq->dst_seq == route->dst_seq) {
			if (rreq->hop_cnt < route->hop_cnt){
				route->dst_addr = rreq->src_addr;
				route->valid = 1;
				route->dst_seq = rreq->dst_seq;
				route->dst_seq_valid = 1;
				route->hop_cnt = rreq->hop_cnt;
				route->next_hop_addr = immediate_addr;		
				//route to src updated
			}
		}
		//else do nothing
		
	}
	
	printf("aodv_state.my_addr %lu\n", aodv_state.my_addr);
	printf("rreq->dst_addr %lu\n", rreq->dst_addr);
	
	if (aodv_state.my_addr == rreq->dst_addr) {
		return RREP;
	}
	else {
		return FORWARD;
	}
	
	
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

static route_table_entry_t * get_next_empty_route_entry(void){
	int i;
	for (i=0; i<ROUTE_TABLE_LEN; i++){
		if (routing_table[i].valid == 0){
			return &routing_table[i];	
		}
	}
	return NULL;
}