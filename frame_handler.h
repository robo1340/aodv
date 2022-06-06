#ifndef frame_handler_h
#define frame_handler_h

#include <stdint.h>
#include <stdbool.h>


void handle_received_frame(uint64_t my_addr, uint64_t immediate_addr, zhashx_t * links, uint8_t * data, size_t len);

#endif // frame_handler_h