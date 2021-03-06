/**
 * @file
 * @brief マウス処理
 */

#include "bootpack.h"
#include "mouse.h"
#include "queue.h"
#include "int.h"

//extern Queue8_t mouseq;
extern Queue16_t msg_queue;

/**
 * @brief マウスの割り込みハンドラ
 */
void inthandler2c( int *esp )
{
    unsigned char mouse_data;
    unsigned short que_data;
    io_out8( PIC1_OCW2, 0x64 ); /* IRQ-12受付完了をPIC1に通知 */
    io_out8( PIC0_OCW2, 0x62 ); /* IRQ-02受付完了をPIC0に通知 */
    mouse_data = io_in8( PORT_KEYDAT );
    que_data = (E_QUEUE_EVENT_TYPE_MOUSE << 8) | mouse_data;
    queue16_put( &msg_queue, que_data );
    return;
}

/**
 * @brief マウス有効化
 */
void enable_mouse( MouseDec_t* mdec )
{
    /* マウス有効 */
    wait_KBC_sendready();
    io_out8( PORT_KEYCMD, KEYCMD_SENDTO_MOUSE );

    wait_KBC_sendready();
    io_out8( PORT_KEYDAT, MOUSECMD_ENABLE );

    /* うまくいくとACK(0xfa)が返ってくる */
    mdec->phase = 0;
    return;
}

/**
 * @brief マウスからのデータを解析する
 */
int mouse_decode( MouseDec_t* mdec, unsigned char data )
{
    if( mdec->phase == 0 )
    {
        /* マウスの0xfaを待っている状態 */
        if( data == 0xfa )
        {
            mdec->phase = 1;
        }
        return 0;
    }
    else if( mdec->phase == 1 )
    {
        /* マウスの1バイト目を待っている状態 */
        if( (data & 0xc8) == 0x08 )
        {
            /* 正しい1バイト目 */
            mdec->buf[0] = data;
            mdec->phase = 2;
        }
        return 0;
    }
    else if( mdec->phase == 2 )
    {
        /* マウスの2バイト目を待っている状態 */
        mdec->buf[1] = data;
        mdec->phase = 3;
        return 0;
    }
    else if( mdec->phase == 3 )
    {
        /* マウスの3バイト目を待っている状態 */
        mdec->buf[2] = data;
        mdec->phase = 1;
        mdec->button = mdec->buf[0] & 0x07;
        mdec->x = mdec->buf[1];
        mdec->y = mdec->buf[2];
        if( ( mdec->buf[0] & 0x10 ) != 0 )
        {
            mdec->x |= 0xffffff00;
        }
        if( ( mdec->buf[0] & 0x20 ) != 0 )
        {
            mdec->y |= 0xffffff00;
        }
        /* マウスではy方向の符号が画面と逆 */
        mdec->y = -mdec->y;
        return 1;
    }
    /* ここにくることはないはず */
    return -1;
}

