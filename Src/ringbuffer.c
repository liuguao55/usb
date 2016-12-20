#include "ringbuffer.h"

// ��ʼ������buffer,sizeָ����buffer�Ĵ�С ע:���ﲢû�����ݵ�ַ���������� //
void rb_init(struct rb* rb, uint8_t *pool, uint16_t size)
{

    // �Զ�дָ������ 
    rb->read_index = rb->write_index = 0;

    // ���û���buffer���ڴ����ݿ� [/i]/
    rb->buffer_ptr = pool;
    rb->buffer_size = size;
}
extern	uint16_t bl1,bl2,bl3,bl4,bl5;
// ����buffer��д������ //
uint8_t rb_put(struct rb* rb, const uint8_t *ptr, uint16_t length)
{
    uint16_t size;
	
    // �ж��Ƿ����㹻��ʣ��ռ� [/i]/
    if (rb->read_index > rb->write_index)
        size = rb->read_index - rb->write_index;
    else
        size = rb->buffer_size - rb->write_index + rb->read_index;

    // û�ж���Ŀռ� [/i]/
    if (size < length) return 0;

    if (rb->read_index > rb->write_index)
    {
        // read_index - write_index ��Ϊ�ܵ�ʣ��ռ� [/i]/
        memcpy(&rb->buffer_ptr, ptr, length);
        rb->write_index += length;
    }
    else
    {
        if (rb->buffer_size - rb->write_index > length)
        {
            // write_index �����ʣ��ռ����㹻�ĳ��� [/i]/
            memcpy(&rb->buffer_ptr, ptr, length);
            rb->write_index += length;
        }
        else
        {
            /*
             * write_index ����ʣ��Ŀռ䲻�����㹻�ĳ���,��Ҫ�Ѳ������ݸ��Ƶ�
             * ǰ���ʣ��ռ���
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

// �ӻ���buffer�ж������� [/i]/
uint8_t rb_get(struct rb* rb, uint8_t *ptr, uint16_t length)
{
    uint16_t size;

    // �ж��Ƿ����㹻������ [/i]/
    if (rb->read_index > rb->write_index)
        size = rb->buffer_size - rb->read_index + rb->write_index;
    else
        size = rb->write_index - rb->read_index;

    // û���㹻������ [/i]/
    if (size < length) return 0;

    if (rb->read_index > rb->write_index)
    {
        if (rb->buffer_size - rb->read_index > length)
        {
            // read_index�������㹻��,ֱ�Ӹ��� [/i]/
            memcpy(ptr, &rb->buffer_ptr, length);
            rb->read_index += length;
        }
        else
        {
            // read_index�����ݲ���,��Ҫ�ֶθ��� [/i]/
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
         * read_indexҪ��write_indexС,�ܵ���������(ǰ���Ѿ��������������ж�),
         * ֱ�Ӹ��Ƴ�����
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
			//ȡ��һ�ֽڳ���
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
