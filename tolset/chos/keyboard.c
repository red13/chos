#include "bootpack.h"
#include "queue.h"
#include "int.h"

//extern Queue8_t keyq;
extern Queue16_t msg_queue;

/* PS/2 キーボードからの割り込み */
void inthandler21( int *esp )
{
    unsigned char key_data;
    unsigned short que_data;
    io_out8( PIC0_OCW2, 0x61 );  /* IRQ-01受付完了をPICに通知 */
    key_data = io_in8( PORT_KEYDAT );
    que_data = ((E_QUEUE_EVENT_TYPE_KEY & 0xFF) << 8) | (key_data & 0xFF);
    queue16_put( &msg_queue, que_data );
    return;
}


void wait_KBC_sendready( void )
{
    /* キーボードコントローラーがデータ送信可能になるのを待つ */
    for(;;)
    {
        if( (io_in8(PORT_KEYSTA) & KEYSTA_SEND_NOTREADY) == 0 )
        {
            break;
        }
    }
    return;
}

void init_keyboard( void )
{
    wait_KBC_sendready();
    io_out8( PORT_KEYCMD, KEYCMD_WRITE_MODE );
    wait_KBC_sendready();
    io_out8( PORT_KEYDAT, KBC_MODE );
    return;
}
