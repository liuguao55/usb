#ifndef __RINGBUFFER__H__
#define __RINGBUFFER__H__
#ifdef __cplusplus
 extern "C" {
#endif
// Includes ------------------------------------------------------------------[/i]/
#include "stm32f1xx_hal.h"
#include "usbd_cdc_if.h"

// 环形buffer的内存块(用数组体现出来) [/i]/
#define BUFFER_SIZE        256
#define BUFFER_ITEM        32	 
	 
// 一个环形buffer的实现 [/i]/
struct rb
{
    uint16_t read_index, write_index;
    uint8_t *buffer_ptr;
    uint16_t buffer_size;
};


extern uint8_t working_buffer[BUFFER_SIZE];
extern struct rb working_rb;

// 初始化环形buffer,size指的是buffer的大小 注:这里并没对数据地址对齐做处理 [/i]/
void rb_init(struct rb* rb, uint8_t *pool, uint16_t size);	
// 向环形buffer中写入数据 [/i]/
uint8_t rb_put(struct rb* rb, const uint8_t *ptr, uint16_t length);
// 从环形buffer中读出数据 [/i]/
uint8_t rb_get(struct rb* rb, uint8_t *ptr, uint16_t length);	
 uint8_t rb_get_cmd(struct rb* rb, uint8_t *ptr);
#ifdef __cplusplus
}
#endif

#endif //__RINGBUFFER__H__