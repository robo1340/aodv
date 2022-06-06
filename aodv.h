#ifndef aodv_h
#define aodv_h

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include <string.h>

#define AODV_RREQ_TYPE 0x01
#define AODV_RREP_TYPE 0x02

typedef struct rreq_t {
	uint8_t type;		///<always set to 0x01 for RREQ messages
	uint8_t flags;		///<flags associated with RREQ messages, moving from msb down (join,repair,gratuitous,destination,unknown seq)
	uint8_t dst_seq_valid;
	uint8_t hop_cnt;	///<the number of hops taken from the the src address, initially zero and incremented by every node touching the request
	uint32_t rreq_id;	///<sequence number set by the source uniquely identifying the RREQ in conjunction with the src_addr
	uint64_t dst_addr;	///<address of the node a route is desired for
	uint32_t dst_seq;	///<the latest sequence number received by the originator for any route to the destination, set to 0 and assert the unknown seq bit flag if no seq is known
	uint64_t src_addr;	///<originating node sending the RREQ
	uint32_t src_seq;	///<the current sequence number of the src node
} rreq_t;

typedef struct rrep_t {
	uint8_t type;		///<always set to 0x02 for RREP messages
	uint8_t flags;		///<flags associated with RREP messages, moving from msb down (repair, ack required)
	uint8_t prefix_size;
	uint8_t hop_cnt;	///<number of hops from src address to the destination
	uint64_t dst_addr;	///<the destination node address
	uint32_t dst_seq;	///<the destination sequence number associated with the route
	uint64_t src_addr;	///<the address of the node that created the RREQ message that caused this RREP to be sent in response
	uint32_t lifetime_ms; ///<the time in milliseconds this route is valid
} rrep_t;


typedef struct route_table_entry_t {
	uint64_t dst_addr;
	uint32_t dst_seq;
	uint8_t dst_seq_valid;
	uint8_t valid; ///<set to one if the entry is valid at all
	uint8_t buf2;
	uint8_t hop_cnt;
	uint64_t next_hop_addr;
} route_table_entry_t;

typedef enum {
	FORWARD,
	DISCARD,
	RREP
} aodv_handle_rreq_return_t;

void aodv_init(uint64_t my_addr);

/** @brief search through the routing table for a valid next hop address
 *  @param next_hop_addr pointer to a variable to store the next hop address
 *  @return returns true if a valid next hop exists in the routing table, returns false otherwise
 */
bool aodv_get_next_hop_address(uint64_t * next_hop_addr, const uint64_t dst_addr);

/** @brief get the sequence number for a routing table entry if it exists
 *  @param addr the address that owns the sequence number being searched for
 *  @param seq_num pointer to the sequence number where it will be written to if it is found
 *  @return  return true if the sequence number was found, return false otherwise
 */
bool aodv_get_dst_seq(uint64_t addr, uint32_t * seq_num);

/** @brief create a route request message that is ready to be broadcast
 *  @rreq pointer to a rreq_t struct that will hold the rreq message
 *  @dst_addr address of the node a route is being requested for
 *  @return return true if a rreq message could be generated, return false otherwise
 */
bool aodv_generate_rreq(rreq_t * rreq, const uint64_t dst_addr);

/** @brief handle a new RREQ frame that was received, return type determines whether or not it should be forwarded
 *  @brief rreq pointer to the RREQ frame
 *  @brief immediate_addr address of the node one hop away that sent this RREQ but didn't necessarily originate it
 *  @return enum typedef indicating what action should be taken by the calling function
 */
aodv_handle_rreq_return_t aodv_handle_rreq(uint64_t immediate_addr, rreq_t * rreq);



///////// print functions //////////

void print_rreq(rreq_t * rreq);

#endif // aodv_h