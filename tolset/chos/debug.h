/**
 * @file
 * @brief �f�o�b�O����
 */
#ifndef DEBUG_H_
#define DEBUG_H_

/* �萔 */
#define DEBUG_STR_LENGTH (40)	/**< �ő�\��������  */
#define DEBUG_STR_MAX    (8)	/**< �ő�\���s��  */

/* �֐� */
void set_debug_info( char* vram, short width, short height );
void debug_print( char* str );
void push_debug_string( char* str );
void dump_debug_string( void );
void assert( void );

#endif /* DEBUG_H_ */

