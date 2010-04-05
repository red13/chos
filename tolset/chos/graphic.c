#include "bootpack.h"

void init_palette( void )
{
    static unsigned char table_rgb[16*3] = 
    {
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
    set_palette( 0, 15, table_rgb );
    return;
    
    /* 'static char' equal DB in assembler */
}


void set_palette( int start, int end, unsigned char* rgb )
{
    int i;
    int eflags;
    const int port = 0x03c8; /* パレット設定 */
    unsigned char* cur_rgb = rgb;

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

#if 0
void init_screen( char* vram, int x, int y )
{
    boxfill8( vram, x, COL8_008484,     0,    0,  x-1, y-29 );
    boxfill8( vram, x, COL8_C6C6C6,     0, y-28,  x-1, y-28 );
    boxfill8( vram, x, COL8_FFFFFF,     0, y-27,  x-1, y-27 );
    boxfill8( vram, x, COL8_C6C6C6,     0, y-26,  x-1, y- 1 );

    boxfill8( vram, x, COL8_FFFFFF,     3, y-24,   59, y-24 );
    boxfill8( vram, x, COL8_FFFFFF,     2, y-24,    2, y- 4 );
    boxfill8( vram, x, COL8_848484,     3, y- 4,   59, y- 4 );
    boxfill8( vram, x, COL8_848484,    59, y-23,   59, y- 5 );
    boxfill8( vram, x, COL8_000000,     2, y- 3,   59, y- 3 );
    boxfill8( vram, x, COL8_000000,    60, y-24,   60, y- 3 );

    boxfill8( vram, x, COL8_848484,  x-47, y-24, x- 4, y-24 );
    boxfill8( vram, x, COL8_848484,  x-47, y-23, x-47, y- 4 );
    boxfill8( vram, x, COL8_FFFFFF,  x-47, y- 3, x- 4, y- 3 );
    boxfill8( vram, x, COL8_FFFFFF,  x- 3, y-24, x- 3, y- 3 );
}
#endif

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

