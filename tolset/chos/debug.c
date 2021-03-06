/**
 * @file
 * @brief デバッグ処理
 */
#include <stdio.h>
#include <string.h>
#include "debug.h"
#include "bootpack.h"


/* 変数 */
static char* debug_vram;
static short debug_scrn_width;
static short debug_scrn_height;
static char debug_string[DEBUG_STR_LENGTH];
static char debug_buffer[DEBUG_STR_LENGTH * DEBUG_STR_MAX];
static unsigned int debug_write_num = 0;


static void print_debug_string( unsigned int num, unsigned int place )
{
    if((num < DEBUG_STR_MAX)&&(place <= (debug_scrn_height/8))){
        boxfill8( debug_vram, debug_scrn_width, COL8_FFFFFF,
                  0, 16*place, 8*DEBUG_STR_LENGTH-1, 16*place+15 );
        putfonts8_asc( debug_vram, debug_scrn_width,
                       0, 16*place, COL8_000000,
                       &debug_buffer[DEBUG_STR_LENGTH*num] );
    }
}

void set_debug_info( char* vram, short width, short height )
{
    debug_vram		  = vram;
    debug_scrn_width  = width;
    debug_scrn_height = height;
}

void debug_print( char* str )
{
    unsigned int length;
    length = strlen( str );
    if( length > DEBUG_STR_LENGTH )
    {
        length = DEBUG_STR_LENGTH;
    }
    memcpy( debug_string, str, length );
    boxfill8( debug_vram, debug_scrn_width, COL8_000000, 0, 0, 8*DEBUG_STR_LENGTH-1, 15 );
    putfonts8_asc( debug_vram, debug_scrn_width, 0, 0, COL8_FFFFFF, str );
}

void push_debug_string( char* str )
{
    unsigned int length;

    length = strlen( str );
    if( length > DEBUG_STR_LENGTH )
    {
        length = DEBUG_STR_LENGTH;
    }
    memset( &debug_buffer[DEBUG_STR_LENGTH*debug_write_num], 0, DEBUG_STR_LENGTH );
    memcpy( &debug_buffer[DEBUG_STR_LENGTH*debug_write_num], str, length );
    debug_write_num++;
    if(debug_write_num >= DEBUG_STR_MAX){
        debug_write_num = 0;
    }
}

void dump_debug_string( void )
{
    int p;
    int n;

    n = debug_write_num;

    for( p = 0; p < DEBUG_STR_MAX; p++ ){
        print_debug_string(n, p);
        n++;
        if( n >= DEBUG_STR_MAX ){
            n = 0;
        }
    }
}

void assert( void )
{
    dump_debug_string();
    io_stihlt();
	for(;;){}
}

