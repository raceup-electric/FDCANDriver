#ifndef FDCAN_H
#define FDCAN_H

#include <stdint.h>
#include <stdbool.h>

typedef enum {
    CAN_ID_STD = 0,
    CAN_ID_EXT = 1
} can_id_type_t;

typedef struct {
    uint32_t id;
    can_id_type_t id_type;
    uint8_t dlc;       // Data Length Code (0-15)
    uint8_t data[64];  // Max 64 bytes for FDCAN
    bool is_fd;        // True for FDCAN frames
} can_frame_t;

void fdcan_init(void);
bool fdcan_send(const can_frame_t *frame);
bool fdcan_receive(can_frame_t *frame);
bool fdcan_is_available(void); // Check if RX FIFO has data

#endif
