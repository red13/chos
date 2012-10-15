/**
 * @file
 * @brief デバッグ処理
 */
#ifndef DEBUG_H_
#define DEBUG_H_

/* 定数 */
#define DEBUG_STR_LENGTH (40)	/**< 最大表示文字数  */
#define DEBUG_STR_MAX    (8)	/**< 最大表示行数  */

/* 関数 */
void set_debug_info( char* vram, short width, short height );
void debug_print( char* str );
void push_debug_string( char* str );
void dump_debug_string( void );
void assert( void );

#endif /* DEBUG_H_ */

