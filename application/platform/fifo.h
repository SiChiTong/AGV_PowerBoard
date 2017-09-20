#ifndef __FIFO_H
#define __FIFO_H
#include "Common.h"





#define RCV_DATA_LEN_MAX    200

#pragma pack(1)

#pragma pack()

typedef  uint8_t fifo_data_struct;

typedef struct{
    fifo_data_struct   *data;
    uint32_t   size;
    uint32_t   front;
    uint32_t   rear;
}fifo_t;


                              
#define TRUE    1
#define FALSE   0

extern fifo_data_struct fifo_data_in_ram[RCV_DATA_LEN_MAX];
extern fifo_t fifo_in_ram;
extern fifo_t *fifo;                               
                              
//extern fifo_t *can_fifo;


uint8_t FifoInit(fifo_t *head, fifo_data_struct *buf, uint32_t len);
void FifoRst(fifo_t *head);
uint8_t IsFifoEmpty(fifo_t *head);
static uint32_t FifoSurplusSize(fifo_t *head);
uint32_t FifoValidSize(fifo_t *head);
uint8_t FifoPuts(fifo_t *head, uint8_t *data, uint32_t len);
uint8_t FifoGets(fifo_t *head, uint8_t *data, uint32_t len);
uint8_t FifoPut(fifo_t *head, const fifo_data_struct data);
uint8_t FifoGet(fifo_t *head, fifo_data_struct *data);
							
#endif //queue.h end
/**************************Copyright BestFu 2014-05-14*************************/
