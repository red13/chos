/**
 * @file
 * @brief 描画系処理
 */
#include "bootpack.h"

/** 色テーブル */
static const unsigned char table_rgb[16*3] = {
	0x00, 0x00, 0x00, /* 0:black   */
	0xff, 0x00, 0x00, /* 1:red     */
	0x00, 0xff, 0x00, /* 2:green   */
	0xff, 0xff, 0x00, /* 3:yellow  */
	0x00, 0x00, 0xff, /* 4:blue    */
	0xff, 0x00, 0xff, /* 5:magenta */
	0x00, 0xff, 0xff, /* 6:cyan    */
	0xff, 0xff, 0xff, /* 7:white   */
	0xc6, 0xc6, 0xc6, /* 8:gray    */
	0x84, 0x00, 0x00, /* 9:dark red      */
	0x00, 0x84, 0x00, /* 10:dark green   */
	0x84, 0x84, 0x00, /* 11:dark yellow  */
	0x00, 0x00, 0x84, /* 12:dark blue    */
	0x84, 0x00, 0x84, /* 13:dark magenta */
	0x00, 0x84, 0x84, /* 13:dark cyan    */
	0x84, 0x84, 0x84, /* 14:dark gray    */
};

/**
 * @brief パレット初期化
 */
void init_palette( void )
{
    set_palette( 0, 15, table_rgb );
    return;
}


/**
 * @brief パレット設定
 */
void set_palette( int start, int end, unsigned char* rgb )
{
    int				i;
    int				eflags;
    const int		port	= 0x03c8;	/* パレット設定 */
    unsigned char*	cur_rgb = rgb;

    /* 割り込み許可フラグの値を記録 */
    eflags = io_load_eflags();
    io_cli();
    io_out8( port, start );
    for( i = start; i <= end; i++ )
    {
        io_out8( 0x03c9, cur_rgb[0]/4 );
        io_out8( 0x03c9, cur_rgb[1]/4 );
        io_out8( 0x03c9, cur_rgb[2]/4 );
        cur_rgb += 3;
    }
    /* 割り込み許可フラグの値をもとに戻す */
    io_store_eflags( eflags );
    return;
}

/**
 * @brief 矩形に色を塗りつぶす
 * @param[in] vram VRAMのアドレス
 * @param[in] xsize x座標の最大サイズ
 * @param[in] c 色番号
 * @param[in] x0 X座標開始位置
 * @param[in] y0 Y座標開始位置
 * @param[in] x1 X座標終了位置
 * @param[in] y1 Y座標終了位置
 */
void boxfill8( unsigned char* vram, int xsize, unsigned char c, int x0, int y0, int x1, int y1 )
{
    int x;
    int y;
    for( y = y0; y <= y1; y++ )
    {
        for( x = x0; x <= x1; x++ )
        {
            vram[y*xsize + x] = c;
        }
    }
    return;
}

void putfont8( char *vram, int xsize, int x, int y, char color, char *font )
{
    int i;
    char* p;
    char data;
    for( i = 0; i < 16; i++ )
    {
        p = vram + (y + i) * xsize + x;
        data = font[i];
        if( ( data & 0x80) != 0 ){ p[0] = color; }
        if( ( data & 0x40) != 0 ){ p[1] = color; }
        if( ( data & 0x20) != 0 ){ p[2] = color; }
        if( ( data & 0x10) != 0 ){ p[3] = color; }
        if( ( data & 0x08) != 0 ){ p[4] = color; }
        if( ( data & 0x04) != 0 ){ p[5] = color; }
        if( ( data & 0x02) != 0 ){ p[6] = color; }
        if( ( data & 0x01) != 0 ){ p[7] = color; }
    }
    return;
}

void putfonts8_asc( char* vram, int xsize, int x, int y, char color, unsigned char* str )
{
    extern char hankaku[4096];
    for( ; *str != '\0'; str++ )
    {
        putfont8( vram, xsize, x, y, color, hankaku + *str * 16 );
        x += 8;
    }
    return;
}

void putfonts8_ascbg( char* vram,
					  int xsize,
					  int x, int y,
					  char fgcolor, char bgcolor,
					  unsigned char* str )
{
    extern char hankaku[4096];
    for( ; *str != '\0'; str++ )
    {
		boxfill8( vram, xsize, bgcolor, x, y, (x + 8) -1, (y + 16) -1 );
        putfont8( vram, xsize, x, y, fgcolor, hankaku + *str * 16 );
        x += 8;
    }
    return;
}

void putblock8_8( char* vram, int vxsize, int pxsize, int pysize,
    int px0, int py0, char* buf, int bxsize )
{
    int x;
    int y;
    for( y = 0; y < pysize; y++ )
    {
        for( x = 0; x < pxsize; x++ )
        {
            vram[(py0 + y) * vxsize + (px0 + x)] = buf[y * bxsize + x];
        }
    }
    return;
}

