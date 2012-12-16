/**
 * @file
 * @brief �N����CC����̃��C���֐�
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
 * @brief �}�E�X�J�[�\���̏�����
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

/** �^�C�}�ʒm�p */
unsigned long notify_param;

/**
 * @brief �^�C�}�[�R�[���o�b�N�֐�
 */
void notify_timer( void* param ){
	push_debug_string( "timer execution!" );
	assert();
}

/** �T�C�N���b�N�^�C�}�p�����[�^ */
unsigned long ctimer_param;

/**
 * @brief �T�C�N���b�N�^�C�}�n���h��
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
 * @brief �^�X�NB�̃��C���֐�
 */
void task_b_main(void)
{
	for(;;){
		io_hlt();
	}
}

/**
 * @brief �G���g���|�C���g
 */
void HariMain( void )
{
    /* boot�̂Ƃ��ɃZ�b�g������� */
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
    /* IDT/PIC�̏��������I������̂ŁCCPU�̊��荞�݋֎~������ */
    io_sti();

    queue16_init( &msg_queue, MSG_BUF_LENGTH, msg_buf );

    init_pit();


    /* PIT��PIC1�ƃL�[�{�[�h������(11111000) */
    io_out8( PIC0_IMR, 0xf8 );

    /* �}�E�X������(11101111) */
    io_out8( PIC1_IMR, 0xef );

    /* �L�[�{�[�h�̏����� */
    init_keyboard();

    /* �}�E�X�������� */
    enable_mouse(&mdec);

    /* �������������炠�邩�e�X�g���� */
    memtotal = memtest( 0x00400000, 0xbfffffff );

    /* �������}�l�[�W���̏����� */
    memman_init( memman );
    memman_free( memman, 0x00001000, 0x0009e000 );
    memman_free( memman, 0x00400000, memtotal - 0x00400000 );

	/* �^�C�}�[�Ǘ����������� */
	init_timer_manager( memman, &kernel_timer_manager );

	/* �^�X�N�������������� */
	init_task();

	/* �^�X�N�𐶐����� */
	create_task( memman, task_b_main, 64 * 1024, &task_b_id );

#if 0
	/* �^�C�}�[��ݒ� */
	notify_param = &desktop_wnd_id;
	timer.timeout = get_systime() + 200;
	timer.func = notify_timer;
	timer.param = &notify_param;
	add_timer( kernel_timer_manager, &timer, &timer_id );
#endif
    /* ��ʂ̃p���b�g��ݒ肷�� */
    init_palette();

    /* �f�o�b�O�o�͗p�ɉ�ʂ̏���ݒ肵�Ă����D */
    set_debug_info( binfo->vram, binfo->scrnx, binfo->scrny );
    
    /* WindowManager�̐��� */
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

    /* �w�i����� */
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

    /* �w�i(��) */
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
	/* �^�C�}�[��ݒ� */
	ctimer_param = desktop_wnd_id;
	timer.timeout = get_systime() + 200;
	timer.func = ctimer_func;
	timer.param = &ctimer_param;
	add_cyclic_timer( kernel_timer_manager, &timer, &ctimer_id );
#endif

    /* �}�E�X�J�[�\���p�̃E�C���h�E����� */
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

    /* ���̓f�[�^�\���p�̃E�C���h�E����� */
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
    /* �w�i(��) */
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
	/* �^�C�}�[�폜�̃e�X�g */
	if( delete_timer( kernel_timer_manager, timer_id ) ){
		push_debug_string( "delete_timer returns TRUE" );
		assert();
	}
#endif

    /* ���b�Z�[�W�������C�����[�v */
    while(1)
    {
        /* ���荞�݋֎~ */
        io_cli();

		/* ��ʂɃ^�C�}�̃J�E���g�l��\������ */
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
		
		/* �`�揈��(�o�b�t�@�̃f�[�^��VRAM�ɓ]��) */
		display_window();
		
		que_status = queue16_status(&msg_queue);
		if( que_status == 0 )
		{
			/* msg_queue�ɓ��͂��Ȃ��Ƃ��͉������Ȃ� */
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
		
            /* �L�[�{�[�h���荞�� */
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
			/* �}�E�X���荞�� */
			else if( msg_type == E_QUEUE_EVENT_TYPE_MOUSE )
			{
				if( mouse_decode( &mdec, msg_value ) != 0 )
				{
					/* �f�[�^��3�o�C�g��������̂ŕ\�� */
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
				
					/* �}�E�X�J�[�\���̈ړ� */
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
				
					/* �}�E�X�J�[�\����`�� */
					move_window( cursor_wnd_id, mx, my );
				}
			}
			/* �^�C�}�[�C�x���g */
			else if( msg_type == E_QUEUE_EVENT_TYPE_TIMER )
			{
				timer_event_procedure(kernel_timer_manager);
			}
			else
			{
				/* �s���ȃC�x���g */
				/* �o�O? */
			}
		}
    }
}

