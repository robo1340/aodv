
#include <stdio.h>

#include "aodv.h"
//#include "aodv_rreq_received_table.h"

void print_rreq(rreq_t * rreq) {
	
	if (rreq->dst_seq_valid == 1) {
		printf("{ dst_addr : %lu, dst_seq : %u, src_addr : %lu, src_seq : %u, hop_cnt : %u, id : %u}\n",\
		   rreq->dst_addr, rreq->dst_seq, rreq->src_addr, rreq->src_seq, rreq->hop_cnt, rreq->rreq_id);
	} else {
		printf("{ dst_addr : %lu src_addr : %lu, src_seq : %u, hop_cnt : %u, id : %u}\n",\
		   rreq->dst_addr, rreq->src_addr, rreq->src_seq, rreq->hop_cnt, rreq->rreq_id);	
	}
	
}
