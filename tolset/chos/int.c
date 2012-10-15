/**
 * @file
 * @brief ���荞�݊֌W
 */

#include <stdio.h>
#include "bootpack.h"
#include "timer.h"
#include "queue.h"
#include "int.h"

extern Queue16_t msg_queue;
extern timer_manager_t* kernel_timer_manager;

static TIME_t time;

/**
 * @brief PIC�̏�����
 */
void init_pic( void )
{
    /* PIC�̏����� */
    io_out8( PIC0_IMR, 0xff ); /* ���ׂĂ̊��荞�݂��󂯕t���Ȃ� */
    io_out8( PIC1_IMR, 0xff ); /* ���ׂĂ̊��荞�݂��󂯕t���Ȃ� */
    
    io_out8( PIC0_ICW1, 0x11 );   /* �G�b�W�g���K���[�h */
    io_out8( PIC0_ICW2, 0x20 );   /* IRQ0-7�́CINT20-27�Ŏ󂯂� */
    io_out8( PIC0_ICW3, 1 << 2 ); /* PIC1��IRQ2�ɂĐڑ� */
    io_out8( PIC0_ICW4, 0x01 );   /* �m���o�b�t�@���[�h */
    
    io_out8( PIC1_ICW1, 0x11 ); /* �G�b�W�g���K���[�h */
    io_out8( PIC1_ICW2, 0x28 ); /* IRQ8-15�́CINT28-2f�Ŏ󂯂� */
    io_out8( PIC1_ICW3, 2 );    /* PIC1��IRQ2�ɂĐڑ� */
    io_out8( PIC1_ICW4, 0x01 ); /* �m���o�b�t�@���[�h */
    
    io_out8( PIC0_IMR, 0xfb ); /* 11111011 PIC1�ȊO�͂��ׂċ֎~ */
    io_out8( PIC1_IMR, 0xff ); /* 11111111 ���ׂĂ̊��荞�݂��󂯕t���Ȃ� */
    
    return;
}

/**
 * @brief PIT������
 */
void init_pit( void )
{
    io_out8(PIT_CTRL, 0x34);
    io_out8(PIT_CNT0, PIT_CNT0_LOW);
    io_out8(PIT_CNT0, PIT_CNT0_HIGH);
    return;
}

/**
 * @brief �^�C�}���荞�݃n���h��
 */
void inthandler20(int *esp)
{
    unsigned short que_data;

    io_out8(PIC0_OCW2, 0x60); /* IRQ-00��t������PIC�ɒʒm */
    
    time.count++;

	/*@ �����͓����^�C�}�[�ɑ΂��ĉ�����L���[�C���O����̂�����̂�  */
	/*@ �t���O�𗧂Ă邾���ɂ��Ă��܂������D�D�D  */
	if( kernel_timer_manager != NULL ){
		kernel_timer_manager->list.tail->t.timeout++;
//		if( kernel_timer_manager->list.num > 0 ){
			if( (int)(time.count - kernel_timer_manager->list.head->t.timeout) >= 0 ){
				que_data = (E_QUEUE_EVENT_TYPE_TIMER & 0xFF) << 8;
				queue16_put( &msg_queue, que_data );
			}
//		}
	}
    
    return;
}

/**
 * @brief �V�X�e�����Ԓl�擾
 * @return �V�X�e������(unsigned long)
 */
unsigned long get_systime( void )
{
	return time.count;
}
