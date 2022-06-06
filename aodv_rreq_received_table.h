#ifndef aodv_rreq_received_table_h
#define aodv_rreq_received_table_h

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include <string.h>

//mark any expired entries in the table as invalid
void update_rreq_received_record_table(void);

/** @brief check if an rreq has been received previously
 *  return true the request has been received already
 *  if the rreq hasn't been received, silently add it to the
 *  table if a slot remains and return false
 *  @param addr the originator addr of the requester
 *  @param rreq_id the id of the rreq
*/
bool rreq_received_record_exists(const uint64_t addr, const uint32_t rreq_id);


#endif // aodv_rreq_received_table_h