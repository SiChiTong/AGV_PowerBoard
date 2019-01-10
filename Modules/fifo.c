/*
 *  Author: Kaka Xie
 *  brief: fifo operation
 */

#include "fifo.h"
#include "stdio.h"
#include "string.h"

static uint8_t is_fifo_full(fifo_t *head);         //

uint8_t init_fifo(fifo_t *head, fifo_data_struct *buf, uint32_t len)
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

void rst_fifo(fifo_t *head)
{
    if(head == NULL)
    {
        return;
    }
    head->front = 0;
    head->rear = 0;
}

uint8_t is_fifo_empty(fifo_t *head)
{
    return ((head->front == head->rear) ? TRUE : FALSE);
}

static uint8_t is_fifo_full(fifo_t *head)
{
    return ((head->front == ((head->rear + 1) % head->size)) ? TRUE : FALSE);
}


uint32_t get_fifo_valid_size(fifo_t *head)
{
    return ((head->rear < head->front)\
            ? (head->rear + head->size - head->front)\
            : (head->rear - head->front));
}



uint8_t put_data_to_fifo(fifo_t *head, const fifo_data_struct data)
{
    if(head == NULL)
    {
        return FALSE;
    }
    if(is_fifo_full(head) == TRUE)
    {
        return FALSE;
    }

    memcpy(&head->data[head->rear], &data, sizeof(fifo_data_struct));
    head->rear++;
    head->rear = head->rear % head->size;

    return TRUE;
}


uint8_t get_data_from_fifo(fifo_t *head, fifo_data_struct *data)
{
    if(head == NULL)
    {
        return FALSE;
    }
    if(is_fifo_empty(head) == TRUE)
    {
        return FALSE;
    }
    memcpy(data, &head->data[head->front], sizeof(fifo_data_struct));
    head->front++;
    head->front = head->front % head->size;

    return TRUE;
}

