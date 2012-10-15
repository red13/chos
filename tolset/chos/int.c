/**
 * @file
 * @brief 割り込み関係
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
 * @brief PICの初期化
 */
void init_pic( void )
{
    /* PICの初期化 */
    io_out8( PIC0_IMR, 0xff ); /* すべての割り込みを受け付けない */
    io_out8( PIC1_IMR, 0xff ); /* すべての割り込みを受け付けない */
    
    io_out8( PIC0_ICW1, 0x11 );   /* エッジトリガモード */
    io_out8( PIC0_ICW2, 0x20 );   /* IRQ0-7は，INT20-27で受ける */
    io_out8( PIC0_ICW3, 1 << 2 ); /* PIC1はIRQ2にて接続 */
    io_out8( PIC0_ICW4, 0x01 );   /* ノンバッファモード */
    
    io_out8( PIC1_ICW1, 0x11 ); /* エッジトリガモード */
    io_out8( PIC1_ICW2, 0x28 ); /* IRQ8-15は，INT28-2fで受ける */
    io_out8( PIC1_ICW3, 2 );    /* PIC1はIRQ2にて接続 */
    io_out8( PIC1_ICW4, 0x01 ); /* ノンバッファモード */
    
    io_out8( PIC0_IMR, 0xfb ); /* 11111011 PIC1以外はすべて禁止 */
    io_out8( PIC1_IMR, 0xff ); /* 11111111 すべての割り込みを受け付けない */
    
    return;
}

/**
 * @brief PIT初期化
 */
void init_pit( void )
{
    io_out8(PIT_CTRL, 0x34);
    io_out8(PIT_CNT0, PIT_CNT0_LOW);
    io_out8(PIT_CNT0, PIT_CNT0_HIGH);
    return;
}

/**
 * @brief タイマ割り込みハンドラ
 */
void inthandler20(int *esp)
{
    unsigned short que_data;

    io_out8(PIC0_OCW2, 0x60); /* IRQ-00受付完了をPICに通知 */
    
    time.count++;

	/*@ ここは同じタイマーに対して何回もキューイングするのも困るので  */
	/*@ フラグを立てるだけにしてしまいたい．．．  */
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
 * @brief システム時間値取得
 * @return システム時間(unsigned long)
 */
unsigned long get_systime( void )
{
	return time.count;
}
