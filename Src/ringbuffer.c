#include "ringbuffer.h"

// 初始化环形buffer,size指的是buffer的大小 注:这里并没对数据地址对齐做处理 //
void rb_init(struct rb* rb, uint8_t *pool, uint16_t size)
{

    // 对读写指针清零 
    rb->read_index = rb->write_index = 0;

    // 设置环形buffer的内存数据块 [/i]/
    rb->buffer_ptr = pool;
    rb->buffer_size = size;
}
extern	uint16_t bl1,bl2,bl3,bl4,bl5;
// 向环形buffer中写入数据 //
uint8_t rb_put(struct rb* rb, const uint8_t *ptr, uint16_t length)
{
    uint16_t size;
	
    // 判断是否有足够的剩余空间 [/i]/
    if (rb->read_index > rb->write_index)
        size = rb->read_index - rb->write_index;
    else
        size = rb->buffer_size - rb->write_index + rb->read_index;

    // 没有多余的空间 [/i]/
    if (size < length) return 0;

    if (rb->read_index > rb->write_index)
    {
        // read_index - write_index 即为总的剩余空间 [/i]/
        memcpy(&rb->buffer_ptr, ptr, length);
        rb->write_index += length;
    }
    else
    {
        if (rb->buffer_size - rb->write_index > length)
        {
            // write_index 后面的剩余空间有足够的长度 [/i]/
            memcpy(&rb->buffer_ptr, ptr, length);
            rb->write_index += length;
        }
        else
        {
            /*
             * write_index 后面剩余的空间不存在足够的长度,需要把部分数据复制到
             * 前面的剩余空间中
             */
            memcpy(&rb->buffer_ptr, ptr,
                   rb->buffer_size - rb->write_index);
            memcpy(&rb->buffer_ptr, &ptr,
                   length - (rb->buffer_size - rb->write_index));
            rb->write_index = length - (rb->buffer_size - rb->write_index);
        }
    }

    return 1;
}

// 从环形buffer中读出数据 [/i]/
uint8_t rb_get(struct rb* rb, uint8_t *ptr, uint16_t length)
{
    uint16_t size;

    // 判断是否有足够的数据 [/i]/
    if (rb->read_index > rb->write_index)
        size = rb->buffer_size - rb->read_index + rb->write_index;
    else
        size = rb->write_index - rb->read_index;

    // 没有足够的数据 [/i]/
    if (size < length) return 0;

    if (rb->read_index > rb->write_index)
    {
        if (rb->buffer_size - rb->read_index > length)
        {
            // read_index的数据足够多,直接复制 [/i]/
            memcpy(ptr, &rb->buffer_ptr, length);
            rb->read_index += length;
        }
        else
        {
            // read_index的数据不够,需要分段复制 [/i]/
            memcpy(ptr, &rb->buffer_ptr,
                   rb->buffer_size - rb->read_index);
            memcpy(&ptr, &rb->buffer_ptr,
                   length - rb->buffer_size + rb->read_index);
            rb->read_index = length - rb->buffer_size + rb->read_index;
        }
    }
    else
    {
        /*
         * read_index要比write_index小,总的数据量够(前面已经有总数据量的判断),
         * 直接复制出数据
         */
        memcpy(ptr, &rb->buffer_ptr, length);
        rb->read_index += length;
    }

    return 1;
}


uint8_t rb_get_cmd(struct rb* rb, uint8_t *ptr)
{
    uint8_t result;
		uint8_t buff[BUFFER_SIZE]={0};
		uint16_t cmd_pose = 0;
		uint16_t cmd_state = 0;
		uint16_t cmd_size = 0;
		do
		{
			//取出一字节长度
			result = rb_get(rb,buff,1);
			if(result == 0) return 0;
			if((cmd_pose == 0) && (*buff != 0xEE))
				continue;
			if(cmd_pose < BUFFER_SIZE)
			{
				ptr = buff;
			}
			if(*buff == 0xFF)
			{
				switch (cmd_state)
				{			
					case 2:cmd_state = 3;break; //FF FC FF ??
					case 3:cmd_state = 4;break; //FF FC FF FF
					default:cmd_state = 1;break; //FF ?? ?? ??
				}
			}
			else if(*buff==0xFC)
			{
				switch (cmd_state)
				{
					case 1:cmd_state = 2;break; //FF FC ?? ??
					case 3:cmd_state = 2;break; //FF FC FF FC
					default:cmd_state = 0;break; //?? ?? ?? ??
				}
			}
			else
			cmd_state = 0;
			
			if(cmd_state==4)
			{
				cmd_size = cmd_pose;
				cmd_state = 0;
				cmd_pose = 0;
				return cmd_size;
			}	
			
		}while(result);
		return 0;
		
}
