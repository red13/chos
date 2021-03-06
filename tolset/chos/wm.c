/**
 * @file
 * @brief ウインドウ管理処理
 */
#include <stdio.h>
#include <string.h>
#include "bootpack.h"

#include "wm.h"
#include "debug.h"


/* 唯一のWindowManager本体 */
static WindowManager* pWindowManager = NULL;

/* 内部関数プロトタイプ */
int show_window_abs( unsigned long window_id,
					 position_t* pos,
					 unsigned long width,
					 unsigned long height );


/*******************************/
/* 1画素あたりのバイト数を返す */
/*******************************/
int get_pixel_unit( WindowColorDepth depth )
{
    int unit = -1;
    switch( depth )
    {
    case WINDOW_COLOR_DEPTH_16:
        unit = 1;
        break;
    default:
        break;
    }
    return unit;
}

/**************************/
/* IDからWindowに変換する */
/**************************/
Window* get_window( unsigned long id )
{
    Window* window;

    if( pWindowManager == NULL )
    {
        return NULL;
    }

    window = pWindowManager->list_head;
    while( window != NULL )
    {
        if( window->window_id == id )
        {
            return window;
        }
        window = window->next;
    }

    return NULL;
}

/**********************************/
/* ウインドウマネージャを生成する */
/**********************************/
int create_window_manager( MEMMAN* mm,
                           unsigned char* vram,
                           WindowColorDepth color_depth,
                           unsigned long width,
                           unsigned long height)
{
    WindowManager* pwm = NULL;
    unsigned char* buf = NULL;
    unsigned char* map = NULL;
    int punit;

    if( pWindowManager != NULL ){
		push_debug_string("cre_wm(0)");
		assert();
        return -1;
    }
    
    /* WindowManager本体のための領域をアロケート */
    pwm = (WindowManager*) memman_alloc_4k( mm, sizeof(WindowManager) );
    if( pwm == NULL )
    {
		push_debug_string("cre_wm(1)");
		assert();
        /* メモリを獲得できなかった */
        return -1;
    }

    punit = get_pixel_unit(color_depth);

    /* 描画用のバッファをアロケート */
    buf = (unsigned char*) memman_alloc_4k( mm, punit * height * width );
    if( buf == NULL )
    {
        memman_free_4k( mm, (unsigned int)pwm, sizeof(WindowManager) );
		push_debug_string("cre_wm(2)");
		assert();
        return -1;
    }

    memset( buf, COL8_FF0000, punit * height * width );

	/* 描画更新ビットフラグをアロケート */
	map = (unsigned char*) memman_alloc_4k( mm, height * width );
	if( map == NULL )
	{
		memman_free_4k( mm, (unsigned int)pwm, sizeof(WindowManager) );
		memman_free_4k( mm, (unsigned int)buf, punit * height * width );
		push_debug_string("cre_wm(3)");
		assert();
		return -1;
	}
	memset( map, 0, height * width );

    pwm->list_head = NULL;
    pwm->last_window_id = 0;
    pwm->memory_manager = mm;
    pwm->buffer = buf;
    pwm->map = map;
    /* vramを取得 */
    pwm->vram = vram;
    /* 現在の色数を取得 */
    pwm->color_depth = color_depth;
    /* 現在の画面のサイズを取得 */
    pwm->height = height;
    pwm->width = width;
    
    pWindowManager = pwm;
    return 0;
}

/******************************************/
/* ウインドウマネージャを削除．           */
/*  普通はシャットダウンまで使う必要なし．*/
/******************************************/
void destroy_window_manager( void )
{
    /* すべてのウインドウを閉じる */
    while( pWindowManager->list_head !=NULL )
    {
        destroy_window( pWindowManager->list_head->window_id );
    }

    memman_free_4k( pWindowManager->memory_manager,
                    (unsigned int) pWindowManager->buffer,
                    get_pixel_unit(pWindowManager->color_depth) *
                    pWindowManager->height *
                    pWindowManager->width );
    memman_free_4k( pWindowManager->memory_manager,
                    (unsigned int)pWindowManager,
                    sizeof(WindowManager) );
    pWindowManager = NULL;
}


/************************/
/* ウインドウを生成する */
/************************/
int create_window( MEMMAN* mm,
                   WindowColorDepth color_depth, 
                   unsigned long width,         /* クライアント領域の幅 */
                   unsigned long height,        /* クライアント領域の高さ */
                   unsigned long status,        /* 状態をビット列で指定 */
                   char* name,                  /* windowの名前 */
                   unsigned long* window_id )   /* 生成したウインドウのIDを返す */
{
    unsigned long tmp_id;
    unsigned char* buf = NULL;
    Window* window;
    Window* last_window = NULL;
    Window* tmp_window;


    if( pWindowManager == NULL )
    {
        return -1;
    }

    /* IDのチェック */
    tmp_id = (pWindowManager->last_window_id) + 1;
    while( tmp_id != pWindowManager->last_window_id ){
        last_window = NULL;
        window = pWindowManager->list_head;
        while( window != NULL )
        {
            if( window->window_id == tmp_id )
            {
                /* 既に使われているIDだった */
                break;
            }
            last_window = window;
            window = window->next;
        }
        if( window == NULL )
        {
            /* 未使用のため，tmp_idを使う */
            break;
        }
        tmp_id++;
    }
    if( tmp_id == pWindowManager->last_window_id )
    {
        /* すべてのIDが使われているため */
        /* 新しいウインドウは生成不可能 */
        /* そんな状況があるわけがないだろうけど */
        return -1;
    }

    /* last_window_idを更新しておく */
    pWindowManager->last_window_id = tmp_id;

    /* 新しいウインドウを生成 */
    tmp_window = (Window*) memman_alloc_4k( mm, sizeof(Window));
    if( tmp_window == NULL )
    {
        return -1;
    }

    /* 描画用のバッファを獲得する */
    buf = (unsigned char*) memman_alloc_4k( mm,
                                            get_pixel_unit(color_depth) *
                                            height * width );
    if( buf == NULL )
    {
        memman_free_4k( mm, (unsigned int)tmp_window, sizeof(Window) );
        return -1;
    }

    tmp_window->buffer = buf;
    tmp_window->prev = last_window;
    tmp_window->next = NULL;
    tmp_window->manager = pWindowManager;
    tmp_window->window_id = tmp_id;
    tmp_window->color_depth = color_depth;
    tmp_window->pos.x = 0;
    tmp_window->pos.y = 0;
    tmp_window->height = height;
    tmp_window->width = width;
    tmp_window->status = status;
    memset( tmp_window->name, 0, WM_WINDOW_NAME_LENGTH );
    strncpy( tmp_window->name, name, WM_WINDOW_NAME_LENGTH );
    
/*     if(( status & WM_WINDOW_STATUS_NOTITLE ) == 0 ) */
/*     { */
/*         tmp_window->height = height + WM_WINDOW_TITLE_HEIGHT + (WM_WINDOW_FRAME_WIDTH*2); */
/*     } */
/*     tmp_window->width = width + (WM_WINDOW_FRAME_WIDTH * 2); */
    
    if( tmp_window->prev != NULL )
    {
        /* リストにつなぐ */
        tmp_window->prev->next = tmp_window;
    }
    else
    {
        /* 先頭のウインドウならばマネージャに登録する */
        tmp_window->manager->list_head = tmp_window;
    }
    
    /* 返却するIDを設定する */
    *window_id = tmp_id;

    return 0;
}

/* ウインドウを破棄する */
void destroy_window( unsigned long id )
{
    Window* window = NULL;

    window = get_window( id );
    if( window == NULL )
    {
        /* 何もしない */
        return;
    }
    
    /* TODO: ウインドウを画面から消す */
    
    /* TODO: ウインドウを生成したプロセスに伝える．．． */
    
    if( window->prev != NULL )
    {
        window->prev->next = window->next;
    }
    if( window->next != NULL )
    {
        window->next->prev = window->prev;
    }
    memman_free_4k( pWindowManager->memory_manager,
                    (unsigned int)window->buffer,
                    get_pixel_unit(window->color_depth) *
                    window->height *
                    window->width );
    memman_free_4k( pWindowManager->memory_manager,
                    (unsigned int)window,
                    sizeof(Window) );
}


/* ウインドウの移動 */
/* 座標はグローバル */
int move_window( unsigned long window_id,
                 unsigned long x,
                 unsigned long y )
{
    Window* window;
    position_t prev;
	position_t cur;
    int ret;

    window = get_window( window_id );
    if( window == NULL )
    {
        return -1;
    }

    prev.x = window->pos.x;
    prev.y = window->pos.y;
	cur.x = x;
	cur.y = y;

    window->pos.x = x;
    window->pos.y = y;

    ret = show_window_abs( pWindowManager->list_head->window_id,
                           &prev,
                           window->width, window->height );
    if( ret != 0 ){
        return -1;
    }

    ret = show_window_abs( window_id,
                           &cur,
                           window->width, window->height );
    if( ret != 0 ){
        return -1;
    }

    return 0;
}

/* ウインドウの描画用バッファを */
/* ウインドウマネージャの描画用バッファにコピーする */
/* 座標は絶対座標 */
int show_window_abs( unsigned long window_id,
					 position_t* pos,
                     unsigned long width,
                     unsigned long height )
{
    unsigned long nx;
    unsigned long ny;
    unsigned long srcx;
    unsigned long srcy;
    unsigned long src_left;
    unsigned long src_top;
    unsigned long src_right;
    unsigned long src_bottom;
    unsigned long dst_left;
    unsigned long dst_top;
    unsigned long dst_right;
    unsigned long dst_bottom;
    Window* window;

/* 	if(window_id == 1){ */
/* 		char str[41]; */
/* 		memset(str, 0, 41); */
/* 		sprintf(str, "show_window_abs(%x, %x, %x, %x, %x)", */
/* 			    window_id, left, top, right, bottom ); */
/* 		push_debug_string(str); */
/* 	} */

	if( (width == 0) || (height == 0) ){
		/* 描画するところなし． */
		return 0;
	}

    /* window取得 */
    window = get_window( window_id );
    if( window == NULL )
    {
        push_debug_string( "window is NULL" );
        return -1;
    }

/* 	if(window_id == 1){ */
/* 		char str[41]; */
/* 		memset(str, 0, 41); */
/* 		sprintf(str, "show_window_abs(%x, %x, %x, %x, %x)", */
/* 			    window_id, left, top, right, bottom ); */
/* 		push_debug_string(str); */
/* 	} */

    /* 描画位置が画面の中に納まっているかどうか確認する */
    src_left = pos->x;
    src_top = pos->y;

    if( (src_left + width) >= window->manager->width ){
        src_right = window->manager->width - 1;
    }else{
        src_right = (src_left + width) - 1;
    }

    if( (src_top + height) >= window->manager->height ){
        src_bottom = window->manager->height - 1;
    }else{
        src_bottom = (src_top + height) - 1;
    }

    while( window != NULL )
    {
        if( (window->status & WM_WINDOW_STATUS_INVISIBLE) == 0 )
        {
            if( src_left < window->pos.x ){
                dst_left = window->pos.x;
            }else{
                dst_left = src_left;
            }

            if( src_top < window->pos.y ){
                dst_top = window->pos.y;
            }else{
                dst_top = src_top;
            }

            if( src_right >= (window->pos.x + window->width) ){
                dst_right = (window->pos.x + window->width) -1;
            }else{
                dst_right = src_right;
            }

            if( src_bottom >= (window->pos.y + window->height) ){
                dst_bottom = (window->pos.y + window->height) -1;
            }else{
                dst_bottom = src_bottom;
            }

/*             if(0){ */
/*                 char dbg[40]; */
/*                 sprintf( dbg, "show(%d %d %d %d %d %d)", */
/*                          dst_left, dst_top, dst_right, dst_bottom, */
/*                          window->pos.x, window->pos.y); */
/*                 push_debug_string( dbg ); */
/*             } */

            for( ny = dst_top; ny <= dst_bottom; ny++ )
            {
                for( nx = dst_left; nx <= dst_right; nx++ )
                {
                    srcx = nx - window->pos.x;
                    srcy = ny - window->pos.y;
                    window->manager->buffer[(ny*window->manager->width)+nx] =
                        window->buffer[(srcy*window->width)+srcx];
					window->manager->map[(ny*window->manager->width)+nx] = 1;
                }
            }

// VRAMへの転送は，最後の1回だけ
// というか，別のサービスコールにしてしまったほうがいいかも．
//            display_window( dst_left, dst_top, dst_right, dst_bottom );
        }

        window = window->next;
    }

    return 0;
}

/* 相対アドレス版 */
int show_window( unsigned long wnd_id,
                 unsigned long x,
                 unsigned long y,
                 unsigned long width,
                 unsigned long height )
{
    Window* wnd;
    int ret;
	position_t pos;

    wnd = get_window( wnd_id );
    if(wnd == NULL){
        return -1;
    }

	pos.x = wnd->pos.x + x;
	pos.y = wnd->pos.y + y;

    ret = show_window_abs( wnd_id,
						   &pos,
                           width,
                           height );
	if(ret != 0){
		return -1;
	}
	
	return 0;
}

/* Window全部を再描画する */
int show_whole_window( unsigned long wnd_id )
{
    int ret;
    Window* wnd;
	position_t pos;

    wnd = get_window( wnd_id );
    if(wnd != NULL){
		pos.x = wnd->pos.x;
		pos.y = wnd->pos.y;
        ret = show_window_abs( wnd_id,
                               &pos,
                               wnd->width,
                               wnd->height );
		if(ret != 0){
			return -1;
		}
		
    }
    return 0;
}

/* 描画する */
void display_window( void )
{
	int nx;
	int ny;
	unsigned char* vram;
	unsigned char* buff;
	unsigned char* map;
	unsigned long width;
	unsigned long height;

    if( pWindowManager == NULL )
    {
        char dbg[40];
        sprintf( dbg, "display_window pWindowManager=NULL" );
        push_debug_string( dbg );
        return;
    }

	vram = pWindowManager->vram;
	buff = pWindowManager->buffer;
	map = pWindowManager->map;
	width = pWindowManager->width;
	height = pWindowManager->height;

    for( ny = 0; ny < height; ny++ )
    {
        for( nx = 0; nx < width; nx++ )
        {
			if(map[(ny*width)+nx] != 0){
	            vram[(ny*width)+nx] = buff[(ny*width)+nx];
				map[(ny*width)+nx] = 0;
	        }
        }
    }
    return;
}


/* Windowに点を描画する */
/* 16色の時だけしか使わないこと!! */
int print_dot( unsigned long window_id,
               unsigned long x,
               unsigned long y,
               unsigned char color )
{
    Window* window;

    window = get_window( window_id );
    if( window == NULL )
    {
        push_debug_string("print_dot wnd_id is NULL");
        return -1;
    }

    if( ( x < 0 ) || ( x >= window->width ) )
    {
        return 0;
    }
    if( ( y < 0 ) || ( y >= window->height ) )
    {
        return 0;
    }
    window->buffer[y * window->width + x] = color;
    return 0;
}

/* 四角に塗りつぶす */
int fill_rect( unsigned long window_id,
               unsigned long left,
               unsigned long top,
               unsigned long right,
               unsigned long bottom,
               unsigned char color )
{
    Window* window;
    unsigned long x;
    unsigned long y;
    int ret;

    window = get_window( window_id );
    if( window == NULL )
    {
        push_debug_string("fill_rect wnd is NULL");
        return -1;
    }

    for( y = top; y <= bottom; y++ )
    {
        for( x = left; x <= right; x++ )
        {
            ret = print_dot( window_id, x, y, color );
            if ( ret != 0 ) {
                char dbg[40];
                sprintf( dbg, "print_dot error" );
                push_debug_string( dbg );
            }
        }
    }
    return 0;
}

/* 文字列を表示する */
int print_string( unsigned long window_id,
				  unsigned char* str,
				  unsigned long x,
				  unsigned long y,
				  unsigned char color )
{
    Window* window;
    if( pWindowManager != NULL )
    {
        window = get_window( window_id );
        if( window != NULL )
        {
            putfonts8_asc( window->buffer,
                           window->width,
                           x, y, color, str );
        }
    }
    return 0;
}

int print_string_bg( unsigned long window_id,
					 unsigned char* str,
					 unsigned long x,
					 unsigned long y,
					 unsigned char textcolor,
					 unsigned char bgcolor )
{
    Window* window;
    if( pWindowManager != NULL )
    {
        window = get_window( window_id );
        if( window != NULL )
        {
            putfonts8_ascbg( window->buffer,
							 window->width,
							 x, y, textcolor, bgcolor, str );
        }
    }
    return 0;
}
					 

/* Windowの表示順を入れ替える */
void exchange_window_priority( unsigned long a, unsigned long b )
{
    Window* wnd_a;
    Window* wnd_b;
    Window* tmp;

    wnd_a = get_window( a );
    if( wnd_a == NULL ){
        return;
    }

    wnd_b = get_window( b );
    if( wnd_b == NULL ){
        return;
    }
    
    tmp = wnd_a->prev;
    wnd_a->prev = wnd_b->prev;
    wnd_b->prev = tmp;

    if(wnd_a->prev != NULL){
        wnd_a->prev->next = wnd_a;
    }
    if(wnd_b->prev != NULL){
        wnd_b->prev->next = wnd_b;
    }

    tmp = wnd_a->next;
    wnd_a->next = wnd_b->next;
    wnd_b->next = tmp;

    if(wnd_a->next != NULL){
        wnd_a->next->prev = wnd_a;
    }
    if(wnd_b->next != NULL){
        wnd_b->next->prev = wnd_b;
    }
}

/* windowの内容を直接VRAMに描画する */
/* for debug */
void window_force_display( unsigned long id )
{
    unsigned long x;
    unsigned long y;
    Window* window;
	unsigned char* vram;

    window = get_window( id );
    if( window == NULL ){
        return;
    }

	vram = window->manager->vram;

    for( y = 0; (y < window->manager->height)&&(y < window->height); y++ ){
        for( x = 0; (x < window->manager->width)&&(x < window->width); x++ )
        {
            vram[y*window->manager->width + x] = window->buffer[y*window->width + x];
        }
    }
}

/* windowマネージャの情報をデバッグ情報にのせる */
/* for debug */
void debug_print_window_manager( void )
{
    char str[40];
    sprintf( str, "wm=%x buf=%x",
             pWindowManager,
             pWindowManager->buffer );
    push_debug_string(str);
}


void set_rect( rect_t* rect,
			   unsigned long x,
			   unsigned long y,
			   unsigned long width,
			   unsigned long height )
{
	rect->pos.x = x;
	rect->pos.y = y;
	rect->width = width;
	rect->height = height;
}
