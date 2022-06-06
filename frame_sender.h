#ifndef frame_sender_h
#define frame_sender_h

#include <stdint.h>
#include <stdbool.h>

#include "state.h"

void send_frame(uint64_t my_addr, link_t * dst, uint8_t * data, size_t len);

void broadcast_frame(uint64_t my_addr, zhashx_t * links, uint8_t * data, size_t len);

#endif // frame_sender_h