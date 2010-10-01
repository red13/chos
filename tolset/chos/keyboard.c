#include "bootpack.h"
#include "queue.h"
#include "int.h"

//extern Queue8_t keyq;
extern Queue16_t msg_queue;

const unsigned char key_table[0x80] = {
	KEY_MEAN_NONE,
	KEY_MEAN_ESCAPE,
	KEY_MEAN_FULL_KEY_1,
	KEY_MEAN_FULL_KEY_2,
	KEY_MEAN_FULL_KEY_3,
	KEY_MEAN_FULL_KEY_4,
	KEY_MEAN_FULL_KEY_5,
	KEY_MEAN_FULL_KEY_6,
	KEY_MEAN_FULL_KEY_7,
	KEY_MEAN_FULL_KEY_8,
	KEY_MEAN_FULL_KEY_9,
	KEY_MEAN_FULL_KEY_0,
	KEY_MEAN_HYPHEN,				/* - */
	KEY_MEAN_CIRCUMFLEX,			/* ^ */
	KEY_MEAN_BACKSPACE,
	KEY_MEAN_TAB,
	KEY_MEAN_Q,
	KEY_MEAN_W,
	KEY_MEAN_E,
	KEY_MEAN_R,
	KEY_MEAN_T,
	KEY_MEAN_Y,
	KEY_MEAN_U,
	KEY_MEAN_I,
	KEY_MEAN_O,
	KEY_MEAN_P,
	KEY_MEAN_ATMARK,				/* @ */
	KEY_MEAN_LEFT_BRACKET,		/* [ */
	KEY_MEAN_FULL_KEY_ENTER,
	KEY_MEAN_LEFT_CTRL,
	KEY_MEAN_A,
	KEY_MEAN_S,
	KEY_MEAN_D,
	KEY_MEAN_F,
	KEY_MEAN_G,
	KEY_MEAN_H,
	KEY_MEAN_J,
	KEY_MEAN_K,
	KEY_MEAN_L,
	KEY_MEAN_SEMICOLON,			/* ; */
	KEY_MEAN_COLON,				/* : */
	KEY_MEAN_HANKAKU_ZENKAKU,
	KEY_MEAN_LEFT_SHIFT,
	KEY_MEAN_RIGHT_BRACKET,		/* ] */
	KEY_MEAN_Z,
	KEY_MEAN_X,
	KEY_MEAN_C,
	KEY_MEAN_V,
	KEY_MEAN_B,
	KEY_MEAN_N,
	KEY_MEAN_M,
	KEY_MEAN_COMMA,				/* , */
	KEY_MEAN_PERIOD,				/* . */
	KEY_MEAN_SLASH,				/* / */
	KEY_MEAN_RIGHT_SHIFT,
	KEY_MEAN_TEN_KEY_STAR,
	KEY_MEAN_LEFT_ALT,
	KEY_MEAN_SPACE,
	KEY_MEAN_CAPSLOCK,
	KEY_MEAN_F1,
	KEY_MEAN_F2,
	KEY_MEAN_F3,
	KEY_MEAN_F4,
	KEY_MEAN_F5,
	KEY_MEAN_F6,
	KEY_MEAN_F7,
	KEY_MEAN_F8,
	KEY_MEAN_F9,
	KEY_MEAN_F10,
	KEY_MEAN_NUMLOCK,
	KEY_MEAN_SCROLLLOCK,
	KEY_MEAN_TEN_KEY_7,
	KEY_MEAN_TEN_KEY_8,
	KEY_MEAN_TEN_KEY_9,
	KEY_MEAN_TEN_KEY_MINUS,
	KEY_MEAN_TEN_KEY_4,
	KEY_MEAN_TEN_KEY_5,
	KEY_MEAN_TEN_KEY_6,
	KEY_MEAN_TEN_KEY_PLUS,
	KEY_MEAN_TEN_KEY_1,
	KEY_MEAN_TEN_KEY_2,
	KEY_MEAN_TEN_KEY_3,
	KEY_MEAN_TEN_KEY_0,
	KEY_MEAN_TEN_KEY_PERIOD,
	KEY_MEAN_SYS_REQ,
	KEY_MEAN_NONE,
	KEY_MEAN_NONE,
	KEY_MEAN_NONE,
	KEY_MEAN_NONE,
	KEY_MEAN_NONE,
	KEY_MEAN_NONE,
	KEY_MEAN_NONE,
	KEY_MEAN_NONE,
	KEY_MEAN_NONE,
	KEY_MEAN_NONE,
	KEY_MEAN_NONE,
	KEY_MEAN_NONE,
	KEY_MEAN_NONE,
	KEY_MEAN_NONE,
	KEY_MEAN_NONE,
	KEY_MEAN_NONE,
	KEY_MEAN_NONE,
	KEY_MEAN_NONE,
	KEY_MEAN_NONE,
	KEY_MEAN_NONE,
	KEY_MEAN_NONE,
	KEY_MEAN_NONE,
	KEY_MEAN_NONE,
	KEY_MEAN_HIRAGANA,
	KEY_MEAN_NONE,
	KEY_MEAN_NONE,
	KEY_MEAN_UNDERSCORE,			/* _ */
	KEY_MEAN_NONE,
	KEY_MEAN_NONE,
	KEY_MEAN_NONE,
	KEY_MEAN_NONE,
	KEY_MEAN_NONE,
	KEY_MEAN_HENKAN,
	KEY_MEAN_NONE,
	KEY_MEAN_MUHENKAN,
	KEY_MEAN_NONE,
	KEY_MEAN_BACKSLASH,			/* \ */
	KEY_MEAN_NONE,
	KEY_MEAN_NONE,
};


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
