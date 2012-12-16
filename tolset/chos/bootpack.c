/**
 * @file
 * @brief 起動後，C言語のメイン関数
 */

#include <stdio.h>
#include <string.h>

#include "bootpack.h"
#include "mouse.h"
#include "queue.h"
#include "wm.h"
#include "int.h"
#include "debug.h"
#include "timer.h"
#include "task.h"

Queue16_t msg_queue;
#define MSG_BUF_LENGTH (256)
unsigned short msg_buf[MSG_BUF_LENGTH];
timer_manager_t* kernel_timer_manager = NULL;


/**
 * @brief マウスカーソルの初期化
 */
void init_mouse_cursor( unsigned long wnd_id, char bg_color )
{
    int x;
    int y;
    const char cursor[16][16] =	{
		"**************..",
		"*OOOOOOOOOOO*...",
		"*OOOOOOOOOO*....",
		"*OOOOOOOOO*.....",
        "*OOOOOOOO*......",
        "*OOOOOOO*.......",
        "*OOOOOOO*.......",
        "*OOOOOOOO*......",
        "*OOOO**OOO*.....",
        "*OOO*..*OOO*....",
        "*OO*....*OOO*...",
        "*O*......*OOO*..",
        "**........*OOO*.",
        "*..........*OOO*",
        "............*OO*",
        ".............***",
    };

    for( y = 0; y < 16; y++ )
    {
        for( x = 0;  x < 16; x++ )
        {
            if( cursor[y][x] == '*' )
            {
                print_dot( wnd_id, x, y, COL8_000000 );
            }
            if( cursor[y][x] == 'O' )
            {
                print_dot( wnd_id, x, y, COL8_FFFFFF );
            }
            if( cursor[y][x] == '.' )
            {
                print_dot( wnd_id, x, y, bg_color );
            }
        }
    }
    return;
}

/** タイマ通知用 */
unsigned long notify_param;

/**
 * @brief タイマーコールバック関数
 */
void notify_timer( void* param ){
	push_debug_string( "timer execution!" );
	assert();
}

/** サイクリックタイマパラメータ */
unsigned long ctimer_param;

/**
 * @brief サイクリックタイマハンドラ
 */
void ctimer_func( void* param ){
	unsigned long wid = *(unsigned long*)param;
	static unsigned char color = 0;
	char str[41];

	color = (color+1)%16;
	
    sprintf( str, "param(%x)", param );
    push_debug_string( str );
    sprintf( str, "ctimer param(%d)", ctimer_param );
    push_debug_string( str );
    sprintf( str, "ctimer done (desktop_wnd_id = %d)", wid );
    push_debug_string( str );

	if( color == 3 ){
		assert();
	}
}

/**
 * @brief タスクBのメイン関数
 */
void task_b_main(void)
{
	for(;;){
		io_hlt();
	}
}

/**
 * @brief エントリポイント
 */
void HariMain( void )
{
    /* bootのときにセットした情報 */
    BOOTINFO_t* 	binfo 		   = (BOOTINFO_t*)ADR_BOOTINFO;
    MEMMAN* 		memman 		   = (MEMMAN*)MEMMAN_ADDR;
    MouseDec_t 		mdec;
    char 			str[32];
    int 			mx 			   = binfo->scrnx / 2;
    int 			my 			   = binfo->scrny / 2;
    int 			msg;
    int 			msg_type;
    int 			msg_value;
    unsigned int 	memtotal;
    unsigned long 	desktop_wnd_id;
    unsigned long 	cursor_wnd_id;
    unsigned long 	input_wnd_id;
    /* unsigned long 	counter_wnd_id; */
    int 			top_wnd_toggle = 0;
    int 			ret;
    unsigned long 	que_status 	   = 0;
	timer_t timer;
	timer_id_t timer_id;
	timer_id_t ctimer_id;
	task_id_t	task_b_id;
    SEGMENT_DESCRIPTOR* gdt = (SEGMENT_DESCRIPTOR*) ADR_GDT;

    init_gdtidt();
    init_pic();
    /* IDT/PICの初期化が終わったので，CPUの割り込み禁止を解除 */
    io_sti();

    queue16_init( &msg_queue, MSG_BUF_LENGTH, msg_buf );

    init_pit();


    /* PITとPIC1とキーボードを許可(11111000) */
    io_out8( PIC0_IMR, 0xf8 );

    /* マウスを許可(11101111) */
    io_out8( PIC1_IMR, 0xef );

    /* キーボードの初期化 */
    init_keyboard();

    /* マウスを初期化 */
    enable_mouse(&mdec);

    /* メモリがいくらあるかテストする */
    memtotal = memtest( 0x00400000, 0xbfffffff );

    /* メモリマネージャの初期化 */
    memman_init( memman );
    memman_free( memman, 0x00001000, 0x0009e000 );
    memman_free( memman, 0x00400000, memtotal - 0x00400000 );

	/* タイマー管理情報を初期化 */
	init_timer_manager( memman, &kernel_timer_manager );

	/* タスク情報を初期化する */
	init_task();

	/* タスクを生成する */
	create_task( memman, task_b_main, 64 * 1024, &task_b_id );

#if 0
	/* タイマーを設定 */
	notify_param = &desktop_wnd_id;
	timer.timeout = get_systime() + 200;
	timer.func = notify_timer;
	timer.param = &notify_param;
	add_timer( kernel_timer_manager, &timer, &timer_id );
#endif
    /* 画面のパレットを設定する */
    init_palette();

    /* デバッグ出力用に画面の情報を設定しておく． */
    set_debug_info( binfo->vram, binfo->scrnx, binfo->scrny );
    
    /* WindowManagerの生成 */
    ret = create_window_manager( memman,
                                 binfo->vram,
                                 WINDOW_COLOR_DEPTH_16,
                                 binfo->scrnx,
                                 binfo->scrny );
    if ( ret != 0 ) {
        sprintf( str, "create window manager error(%d)", ret );
        push_debug_string( str );
        assert();
    }

    /* 背景を作る */
    ret = create_window( memman,
                         WINDOW_COLOR_DEPTH_16,
                         binfo->scrnx,
                         binfo->scrny,
                         WM_WINDOW_STATUS_NORMAL,
                         "desktop",
                         &desktop_wnd_id );
    if ( ret != 0 ) {
        sprintf( str, "create window error(%d)", ret );
        push_debug_string( str );
        assert();
    }
    sprintf( str, "desktop_wnd_id = %d", desktop_wnd_id );
    push_debug_string( str );

    /* 背景(青) */
    ret = fill_rect( desktop_wnd_id,
					 0, 0,
					 binfo->scrnx, binfo->scrny,
					 COL8_0000FF );
    if ( ret != 0 ) {
        sprintf( str, "fill_rect error(%d)", ret );
        push_debug_string( str );
        assert();
    }

    ret = show_window( desktop_wnd_id, 0, 0, binfo->scrnx, binfo->scrny );
    if ( ret != 0 ){
        sprintf( str, "show_window error(%d)", ret );
        push_debug_string( str );
        assert();
    }

#if 0
	/* タイマーを設定 */
	ctimer_param = desktop_wnd_id;
	timer.timeout = get_systime() + 200;
	timer.func = ctimer_func;
	timer.param = &ctimer_param;
	add_cyclic_timer( kernel_timer_manager, &timer, &ctimer_id );
#endif

    /* マウスカーソル用のウインドウを作る */
    ret = create_window( memman,
                         WINDOW_COLOR_DEPTH_16,
                         PXL_CURSOR_WIDTH,
                         PXL_CURSOR_HEIGHT,
                         WM_WINDOW_STATUS_NORMAL,
                         "cursor",
                         &cursor_wnd_id );
    if ( ret != 0 ){
        sprintf( str, "create window error(%d)", ret );
        push_debug_string(str);
        assert();
    }

    sprintf( str, "cursor_wnd_id = %d", cursor_wnd_id );
    push_debug_string( str );

    init_mouse_cursor( cursor_wnd_id, COL8_0000FF );
    move_window( cursor_wnd_id, mx, my );

    /* 入力データ表示用のウインドウを作る */
    ret = create_window( memman,
                         WINDOW_COLOR_DEPTH_16,
                         binfo->scrnx,
                         16,
                         WM_WINDOW_STATUS_NORMAL,
                         "input",
                         &input_wnd_id );
    if ( ret != 0 ){
        sprintf( str, "create window error(%d)", ret );
        push_debug_string(str);
        assert();
    }
    /* 背景(黒) */
    ret = fill_rect( input_wnd_id,
					 0, 0,
					 binfo->scrnx, 16,
					 COL8_000000 );
    if ( ret != 0 ) {
        sprintf( str, "fill_rect error(%d)", ret );
        push_debug_string( str );
        assert();
    }
    move_window( input_wnd_id, binfo->scrny-16, 0 );

#if 0
	/* タイマー削除のテスト */
	if( delete_timer( kernel_timer_manager, timer_id ) ){
		push_debug_string( "delete_timer returns TRUE" );
		assert();
	}
#endif

    /* メッセージ処理メインループ */
    while(1)
    {
        /* 割り込み禁止 */
        io_cli();

		/* 画面にタイマのカウント値を表示する */
		fill_rect( desktop_wnd_id,
				   0, 0,
				   64, /* width */
				   32, /* height */
				   COL8_0000FF );
		sprintf( str, "t=%d", get_systime() );
		print_string( desktop_wnd_id,
					  str,
					  0, 0,
					  COL8_FFFFFF );
		sprintf( str, "q_st=%d", que_status );
		print_string( desktop_wnd_id,
					  str,
					  0, 16,
					  COL8_FFFFFF );
//	show_whole_window( desktop_wnd_id );
		show_window( desktop_wnd_id, 0, 0, 64-1, 32-1 );
		
		/* 描画処理(バッファのデータをVRAMに転送) */
		display_window();
		
		que_status = queue16_status(&msg_queue);
		if( que_status == 0 )
		{
			/* msg_queueに入力がないときは何もしない */
			io_stihlt();
/*            io_sti(); */
		}
		else
		{
			msg = queue16_get( &msg_queue );
			if( msg == -1){
				io_sti();
				continue;
			}
			msg_type = (msg >> 8) & 0xFF;
			msg_value = msg & 0xFF;

			io_sti();
		
            /* キーボード割り込み */
			if( msg_type == E_QUEUE_EVENT_TYPE_KEY )
			{
				unsigned char val;

				fill_rect( input_wnd_id, 0, 0, binfo->scrnx, 16, COL8_000000);
				val = get_key_meaning(msg_value);
				if( (KEY_MEAN_FULL_KEY_1 <= val) && (val <= KEY_MEAN_FULL_KEY_9)){
					str[0] = '1' + (val - KEY_MEAN_FULL_KEY_1);
					str[1] = '\0';
					print_string( input_wnd_id,
								  str,
								  0, 0,
								  COL8_FFFFFF );
					
				}
				
				/* show_window( input_wnd_id, */
				/* 			 0, 0, binfo->scrnx, 16); */
				
				show_whole_window( desktop_wnd_id );

				/*@@@ test  */
				taskswitch4();
			}
			/* マウス割り込み */
			else if( msg_type == E_QUEUE_EVENT_TYPE_MOUSE )
			{
				if( mouse_decode( &mdec, msg_value ) != 0 )
				{
					/* データが3バイトそろったので表示 */
					sprintf( str, "[lcr %4d %4d]", mdec.x, mdec.y );
					if( ( mdec.button & MOUSE_BUTTON_LEFT ) != 0 )
					{
						str[1] = 'L';
					}
					if( ( mdec.button & MOUSE_BUTTON_RIGHT ) != 0 )
					{
						str[3] = 'R';
					}
					if( ( mdec.button & MOUSE_BUTTON_CENTER ) != 0 )
					{
						str[2] = 'C';
					}
					fill_rect( desktop_wnd_id, 0, 16*3, 120, 16*4 -1,
							   COL8_0000FF );
					print_string( desktop_wnd_id,
								  str,
								  0, 16*3,
								  COL8_FFFFFF );
					show_window( desktop_wnd_id,
								 0, 16*3, 120-1, 16-1);
				
					/* マウスカーソルの移動 */
					mx += mdec.x;
					my += mdec.y;
					if( mx < 0 )
					{
						mx = 0;
					}
					if( my < 0 )
					{
						my = 0;
					}
					if( mx > binfo->scrnx -1 )
					{
						mx = binfo->scrnx -1;
					}
					if( my > binfo->scrny -1 )
					{
						my = binfo->scrny -1;
					}
				
					/* マウスカーソルを描画 */
					move_window( cursor_wnd_id, mx, my );
				}
			}
			/* タイマーイベント */
			else if( msg_type == E_QUEUE_EVENT_TYPE_TIMER )
			{
				timer_event_procedure(kernel_timer_manager);
			}
			else
			{
				/* 不明なイベント */
				/* バグ? */
			}
		}
    }
}

