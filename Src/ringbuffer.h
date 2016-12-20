#ifndef __RINGBUFFER__H__
#define __RINGBUFFER__H__
#ifdef __cplusplus
 extern "C" {
#endif
// Includes ------------------------------------------------------------------[/i]/
#include "stm32f1xx_hal.h"
#include "usbd_cdc_if.h"

// ����buffer���ڴ��(���������ֳ���) [/i]/
#define BUFFER_SIZE        256
#define BUFFER_ITEM        32	 
	 
// һ������buffer��ʵ�� [/i]/
struct rb
{
    uint16_t read_index, write_index;
    uint8_t *buffer_ptr;
    uint16_t buffer_size;
};


extern uint8_t working_buffer[BUFFER_SIZE];
extern struct rb working_rb;

// ��ʼ������buffer,sizeָ����buffer�Ĵ�С ע:���ﲢû�����ݵ�ַ���������� [/i]/
void rb_init(struct rb* rb, uint8_t *pool, uint16_t size);	
// ����buffer��д������ [/i]/
uint8_t rb_put(struct rb* rb, const uint8_t *ptr, uint16_t length);
// �ӻ���buffer�ж������� [/i]/
uint8_t rb_get(struct rb* rb, uint8_t *ptr, uint16_t length);	
 uint8_t rb_get_cmd(struct rb* rb, uint8_t *ptr);
#ifdef __cplusplus
}
#endif

#endif //__RINGBUFFER__H__