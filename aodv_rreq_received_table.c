
#include <czmq.h>

#include "aodv_rreq_received_table.h"

#define RREQ_RECEIVED_BUFFER_LEN 5
#define RREQ_RECEIVED_RECORD_VALID_MS 30000 //the time to wait before invalidating a received_rreq_record_t


//a record of previously received route requests
typedef struct rreq_received_record_t {
	bool valid;
	uint64_t addr; //originator of the rreq
	uint32_t id; //the id of the rreq
	uint64_t time_received; //the time the rreq was received
} rreq_received_record_t;

	
rreq_received_record_t rreq_recv_table[RREQ_RECEIVED_BUFFER_LEN];

#define RECORD rreq_recv_table[i]

void update_rreq_received_record_table(void){
	int i;
	
	for (i=0; i<RREQ_RECEIVED_BUFFER_LEN; i++){
		if 	(!RECORD.valid) {continue;}
		else {
			if ((zclock_time() - RECORD.time_received) > RREQ_RECEIVED_RECORD_VALID_MS) {
				RECORD.valid = false;	
			}
		}
	}
}

/** @brief check if an rreq has been received previously
 *  return true the request has been received already
 *  if the rreq hasn't been received, silently add it to the
 *  table if a slot remains and return false
 *  @param addr the originator addr of the requester
 *  @param rreq_id the id of the rreq
*/
bool rreq_received_record_exists(const uint64_t addr, const uint32_t rreq_id){
	update_rreq_received_record_table();
	
	int i;
	bool record_found = false;
	
	for (i=0; i<RREQ_RECEIVED_BUFFER_LEN; i++){
		if (!RECORD.valid){continue;}
		else {
			if ((addr == RECORD.addr) && (rreq_id == RECORD.id)){
				return true;	
			}
		}
	}
	
	//a matching record wasn't found if program execution gets here
	for (i=0; i<RREQ_RECEIVED_BUFFER_LEN; i++){
		if (!RECORD.valid){
			RECORD.valid = true;
			RECORD.addr = addr;
			RECORD.id = rreq_id;
			RECORD.time_received = zclock_time();
			break;
		}
	}
	
	return false;
}