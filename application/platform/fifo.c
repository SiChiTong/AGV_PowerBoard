
#include "fifo.h"


static uint32_t FifoSurplusSize(fifo_t *head); //
static uint8_t IsFifoFull(fifo_t *head);         //

fifo_data_struct fifo_data_in_ram[RCV_DATA_LEN_MAX];
fifo_t fifo_in_ram;
fifo_t *fifo = &fifo_in_ram;

uint8_t FifoInit(fifo_t *head, fifo_data_struct *buf, uint32_t len)
{
    if(head == NULL)
    {
        return FALSE;
    }
    head->data = buf;
    head->size = len;
    head->front = head->rear = 0;

    return TRUE;
}

void FifoRst(fifo_t *head)
{
    if(head == NULL)
    {
        return;
    }
    head->front = 0;
	head->rear = 0;
}

uint8_t IsFifoEmpty(fifo_t *head)
{    
    return ((head->front == head->rear) ? TRUE : FALSE);
}

static uint8_t IsFifoFull(fifo_t *head)
{   
    return ((head->front == ((head->rear + 1) % head->size)) ? TRUE : FALSE);
}


uint32_t FifoValidSize(fifo_t *head)
{
	return ((head->rear < head->front)
			? (head->rear + head->size - head->front)
			: (head->rear - head->front));
}



uint8_t FifoPut(fifo_t *head, const fifo_data_struct data)
{
    if(head == NULL)
    {
        return FALSE;
    }
    if(IsFifoFull(head) == TRUE)
    {
        return FALSE;
    }

    memcpy(&head->data[head->rear], &data, sizeof(fifo_data_struct));
    head->rear++;
    head->rear = head->rear % head->size;

    return TRUE;   
}


uint8_t FifoGet(fifo_t *head, fifo_data_struct *data)
{
    if(head == NULL)
    {
        return FALSE;
    }
    if(IsFifoEmpty(head) == TRUE)
    {
        return FALSE;
    }
    memcpy(data, &head->data[head->front], sizeof(fifo_data_struct));
    head->front++;
    head->front = head->front % head->size;

    return TRUE;   
}

