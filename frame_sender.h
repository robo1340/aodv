#ifndef frame_sender_h
#define frame_sender_h

#include <stdint.h>
#include <stdbool.h>

#include "state.h"

void send_frame(link_t * dst, uint8_t * data, size_t len);

#endif // frame_sender_h