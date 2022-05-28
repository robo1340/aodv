#ifndef frame_handler_h
#define frame_handler_h

#include <stdint.h>
#include <stdbool.h>


void handle_received_frame(uint8_t * data, size_t len);

#endif // frame_handler_h