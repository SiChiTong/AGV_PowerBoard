#ifndef __CAN_FIFO_H
#define __CAN_FIFO_H
#include "Common.h"

#include "can_protocol.h"

typedef struct
{
    can_id_union    id;
    can_data_union  data;
    uint8_t len;
}can_pkg_t;

typedef struct{
    can_pkg_t   *data;
    uint32_t   size;
    uint32_t   front;
    uint32_t   rear;
}can_fifo_t;


#define TRUE    1
#define FALSE   0



extern can_fifo_t *can_fifo;


uint8_t can_fifo_init(can_fifo_t *head, can_pkg_t *buf, uint32_t len);
void can_fifo_rst(can_fifo_t *head);
uint8_t is_can_fifo_empty(can_fifo_t *head);
uint32_t get_can_fifo_valid_size(can_fifo_t *head);
uint8_t put_can_pkg_to_fifo(can_fifo_t *head, const can_pkg_t data);
uint8_t get_can_pkg_from_fifo(can_fifo_t *head, can_pkg_t *data);

#endif //queue.h end


