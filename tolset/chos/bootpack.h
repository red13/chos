#ifndef BOOTPACK_H_
#define BOOTPACK_H_

/* 定数定義 */
#define ADR_BOOTINFO (0x00000ff0)

#define ADR_IDT      (0x0026f800)
#define LIMIT_IDT    (0x000007ff)
#define ADR_GDT      (0x00270000)
#define LIMIT_GDT    (0x0000ffff)
#define ADR_BOTPAK   (0x00280000)
#define LIMIT_BOTPAK (0x0007ffff)
#define AR_DATA32_RW (0x4092)
#define AR_CODE32_ER (0x409a)
#define AR_INTGATE32 (0x008e)

#define PORT_KEYDAT (0x0060)

#define COL8_000000   (0)
#define COL8_FF0000   (1)
#define COL8_00FF00   (2)
#define COL8_FFFF00   (3)
#define COL8_0000FF   (4)
#define COL8_FF00FF   (5)
#define COL8_00FFFF   (6)
#define COL8_FFFFFF   (7)
#define COL8_C6C6C6   (8)
#define COL8_840000   (9)
#define COL8_008400   (10)
#define COL8_848400   (11)
#define COL8_000084   (12)
#define COL8_840084   (13)
#define COL8_008484   (14)
#define COL8_848484   (15)


#define PORT_KEYDAT           (0x0060)
#define PORT_KEYSTA           (0x0064)
#define PORT_KEYCMD           (0x0064)
#define KEYSTA_SEND_NOTREADY  (0x02)
#define KEYCMD_WRITE_MODE     (0x60)
#define KEYCMD_SENDTO_MOUSE   (0xd4)
#define MOUSECMD_ENABLE       (0xf4)
#define KBC_MODE              (0x47)

#define EFLAGS_AC_BIT      (0x00040000)
#define CR0_CACHE_DISABLE (0x60000000)

#define MEMMAN_FREES (4090)
#define MEMMAN_ADDR  (0x003c0000)

#define MAX_SHEETS (256)
#define SHEET_USE  (1)

#define PXL_STRING_WIDTH  (8)
#define PXL_STRING_HEIGHT (16)
#define PXL_CURSOR_WIDTH  (16)
#define PXL_CURSOR_HEIGHT (16)

#define PXL_INFO_WND_WIDTH  (160)
#define PXL_INFO_WND_HEIGHT (64)

/* enum */
typedef enum tag_E_QUEUE_EVENT_TYPE
{
    E_QUEUE_EVENT_TYPE_KEY = 0,
    E_QUEUE_EVENT_TYPE_MOUSE,
    E_QUEUE_EVENT_TYPE_TIMER,
    E_QUEUE_EVENT_TYPE_MAX,
} E_QUEUE_EVENT_TYPE;


/* 構造体定義 */
/* bootpack.c */
typedef struct tag_BOOTINFO
{
    char cyls;
    char leds;
    char vmode;
    char reserve;
    short scrnx;
    short scrny;
    char* vram;
} BOOTINFO_t;

typedef struct tag_FREEINFO
{
    unsigned int addr;
    unsigned int size;
} FREEINFO;

typedef struct tag_MEMMAN
{
    int free_num;
    int maxfree_num;
    int lostsize;
    int losts;
    FREEINFO free[MEMMAN_FREES];
} MEMMAN;


/* DGTやIDTなどの，description table 関係 */
/* dsctbl.c */
typedef struct tag_SEGMENT_DESCRIPTOR
{
    short limit_low;
    short base_low;
    char base_mid;
    char access_right;
    char limit_high;
    char base_high;
} SEGMENT_DESCRIPTOR;

typedef struct tag_GATE_DESCRIPTOR
{
    short offset_low;
    short selector;
    char dw_count;
    char access_right;
    short offset_high;
} GATE_DESCRIPTOR;


/* sheet.c */
struct tag_SHEET_CTRL;

typedef struct tag_SHEET
{
    unsigned char* buf;
    int bxsize;
    int bysize;
    int vx0;
    int vy0;
    int color;
    int height;
    int flags;
    struct tag_SHEET_CTRL* ctrl;
} SHEET;

typedef struct tag_SHEET_CTRL
{
    unsigned char* vram;
    unsigned char* map;
    int xsize;
    int ysize;
    int top;
    SHEET* p_sheet[MAX_SHEETS];
    SHEET  sheet[MAX_SHEETS];
} SHEET_CTRL;

/* プロトタイプ宣言 */
/* naskfunc.nas */
void io_hlt( void );
void io_cli( void );
void io_sti( void );
void io_stihlt( void );
int io_in8( int port );
void io_out8( int port, int data );
int io_load_eflags( void );
void io_store_eflags( int eflags );
void load_gdtr( int limit, int addr );
void load_idtr( int limit, int addr );
int load_cr0( void );
void store_cr0( int cr0 );
unsigned int memtest_sub( unsigned int start, unsigned int end );
void asm_inthandler20( void );
void asm_inthandler21( void );
void asm_inthandler2c( void );

/* keyboard.c */
#define KEY_UP_MASK (0x80)

typedef enum tag_key_signal {
	KEY_SIG_NONE = 0,
	KEY_SIG_ESCAPE,
	KEY_SIG_FULL_KEY_1,
	KEY_SIG_FULL_KEY_2,
	KEY_SIG_FULL_KEY_3,
	KEY_SIG_FULL_KEY_4,
	KEY_SIG_FULL_KEY_5,
	KEY_SIG_FULL_KEY_6,
	KEY_SIG_FULL_KEY_7,
	KEY_SIG_FULL_KEY_8,
	KEY_SIG_FULL_KEY_9,
	KEY_SIG_FULL_KEY_0,
	KEY_SIG_HYPHEN,				/* - */
	KEY_SIG_CIRCUMFLEX,			/* ^ */
	KEY_SIG_BACKSPACE,
	KEY_SIG_TAB,
	KEY_SIG_Q,
	KEY_SIG_W,
	KEY_SIG_E,
	KEY_SIG_R,
	KEY_SIG_T,
	KEY_SIG_Y,
	KEY_SIG_U,
	KEY_SIG_I,
	KEY_SIG_O,
	KEY_SIG_P,
	KEY_SIG_ATMARK,				/* @ */
	KEY_SIG_LEFT_BRACKET,		/* [ */
	KEY_SIG_FULL_KEY_ENTER,
	KEY_SIG_LEFT_CTRL,
	KEY_SIG_A,
	KEY_SIG_S,
	KEY_SIG_D,
	KEY_SIG_F,
	KEY_SIG_G,
	KEY_SIG_H,
	KEY_SIG_J,
	KEY_SIG_K,
	KEY_SIG_L,
	KEY_SIG_SEMICOLON,			/* ; */
	KEY_SIG_COLON,				/* : */
	KEY_SIG_HANKAKU_ZENKAKU,
	KEY_SIG_LEFT_SHIFT,
	KEY_SIG_RIGHT_BRACKET,		/* ] */
	KEY_SIG_Z,
	KEY_SIG_X,
	KEY_SIG_C,
	KEY_SIG_V,
	KEY_SIG_B,
	KEY_SIG_N,
	KEY_SIG_M,
	KEY_SIG_COMMA,				/* , */
	KEY_SIG_PERIOD,				/* . */
	KEY_SIG_SLASH,				/* / */
	KEY_SIG_RIGHT_SHIFT,
	KEY_SIG_TEN_KEY_STAR,
	KEY_SIG_LEFT_ALT,
	KEY_SIG_SPACE,
	KEY_SIG_CAPSLOCK,
	KEY_SIG_F1,
	KEY_SIG_F2,
	KEY_SIG_F3,
	KEY_SIG_F4,
	KEY_SIG_F5,
	KEY_SIG_F6,
	KEY_SIG_F7,
	KEY_SIG_F8,
	KEY_SIG_F9,
	KEY_SIG_F10,
	KEY_SIG_NUMLOCK,
	KEY_SIG_SCROLLLOCK,
	KEY_SIG_TEN_KEY_7,
	KEY_SIG_TEN_KEY_8,
	KEY_SIG_TEN_KEY_9,
	KEY_SIG_TEN_KEY_MINUS,
	KEY_SIG_TEN_KEY_4,
	KEY_SIG_TEN_KEY_5,
	KEY_SIG_TEN_KEY_6,
	KEY_SIG_TEN_KEY_PLUS,
	KEY_SIG_TEN_KEY_1,
	KEY_SIG_TEN_KEY_2,
	KEY_SIG_TEN_KEY_3,
	KEY_SIG_TEN_KEY_0,
	KEY_SIG_TEN_KEY_PERIOD,
	KEY_SIG_SYS_REQ,
	KEY_SIG_RESERVED_59,
	KEY_SIG_RESERVED_5A,
	KEY_SIG_RESERVED_5B,
	KEY_SIG_RESERVED_5C,
	KEY_SIG_RESERVED_5D,
	KEY_SIG_RESERVED_5E,
	KEY_SIG_RESERVED_5F,
	KEY_SIG_RESERVED_60,
	KEY_SIG_RESERVED_61,
	KEY_SIG_RESERVED_62,
	KEY_SIG_RESERVED_63,
	KEY_SIG_RESERVED_64,
	KEY_SIG_RESERVED_65,
	KEY_SIG_RESERVED_66,
	KEY_SIG_RESERVED_67,
	KEY_SIG_RESERVED_68,
	KEY_SIG_RESERVED_69,
	KEY_SIG_RESERVED_6A,
	KEY_SIG_RESERVED_6B,
	KEY_SIG_RESERVED_6C,
	KEY_SIG_RESERVED_6D,
	KEY_SIG_RESERVED_6E,
	KEY_SIG_RESERVED_6F,
	KEY_SIG_HIRAGANA,
	KEY_SIG_RESERVED_71,
	KEY_SIG_RESERVED_72,
	KEY_SIG_UNDERSCORE,			/* _ */
	KEY_SIG_RESERVED_74,
	KEY_SIG_RESERVED_75,
	KEY_SIG_RESERVED_76,
	KEY_SIG_RESERVED_77,
	KEY_SIG_RESERVED_78,
	KEY_SIG_HENKAN,
	KEY_SIG_RESERVED_7A,
	KEY_SIG_MUHENKAN,
	KEY_SIG_RESERVED_7C,
	KEY_SIG_BACKSLASH,			/* \ */
	KEY_SIG_RESERVED_7E,
	KEY_SIG_RESERVED_7F,
} key_signal_t;

typedef enum tag_key_meaning{
	KEY_MEAN_NONE = 0,

	/* システム */
	KEY_MEAN_ESCAPE,
	KEY_MEAN_TAB,
	KEY_MEAN_FULL_KEY_ENTER,
	KEY_MEAN_BACKSPACE,
	KEY_MEAN_HANKAKU_ZENKAKU,
	KEY_MEAN_LEFT_CTRL,
	KEY_MEAN_LEFT_SHIFT,
	KEY_MEAN_RIGHT_SHIFT,
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
	KEY_MEAN_SYS_REQ,
	KEY_MEAN_HIRAGANA,
	KEY_MEAN_HENKAN,
	KEY_MEAN_MUHENKAN,

	/* 記号 */
	KEY_MEAN_TEN_KEY_STAR,
	KEY_MEAN_TEN_KEY_PLUS,
	KEY_MEAN_TEN_KEY_MINUS,
	KEY_MEAN_TEN_KEY_PERIOD,

	KEY_MEAN_COMMA,				/* , */
	KEY_MEAN_HYPHEN,				/* - */
	KEY_MEAN_PERIOD,				/* . */
	KEY_MEAN_SLASH,				/* / */
	KEY_MEAN_ATMARK,				/* @ */

	/* 数字 */
	KEY_MEAN_FULL_KEY_0,
	KEY_MEAN_FULL_KEY_1,
	KEY_MEAN_FULL_KEY_2,
	KEY_MEAN_FULL_KEY_3,
	KEY_MEAN_FULL_KEY_4,
	KEY_MEAN_FULL_KEY_5,
	KEY_MEAN_FULL_KEY_6,
	KEY_MEAN_FULL_KEY_7,
	KEY_MEAN_FULL_KEY_8,
	KEY_MEAN_FULL_KEY_9,
	KEY_MEAN_SEMICOLON,			/* ; */

	/* 記号 */
	KEY_MEAN_COLON,				/* : */

	/* アルファベット */
	KEY_MEAN_A,
	KEY_MEAN_B,
	KEY_MEAN_C,
	KEY_MEAN_D,
	KEY_MEAN_E,
	KEY_MEAN_F,
	KEY_MEAN_G,
	KEY_MEAN_H,
	KEY_MEAN_I,
	KEY_MEAN_J,
	KEY_MEAN_K,
	KEY_MEAN_L,
	KEY_MEAN_M,
	KEY_MEAN_N,
	KEY_MEAN_O,
	KEY_MEAN_P,
	KEY_MEAN_Q,
	KEY_MEAN_R,
	KEY_MEAN_S,
	KEY_MEAN_T,
	KEY_MEAN_U,
	KEY_MEAN_V,
	KEY_MEAN_W,
	KEY_MEAN_X,
	KEY_MEAN_Y,
	KEY_MEAN_Z,

	/* 記号 */
	KEY_MEAN_LEFT_BRACKET,		/* [ */
	KEY_MEAN_BACKSLASH,			/* \ */
	KEY_MEAN_RIGHT_BRACKET,		/* ] */
	KEY_MEAN_CIRCUMFLEX,			/* ^ */
	KEY_MEAN_UNDERSCORE,			/* _ */

	KEY_MEAN_TEN_KEY_0,
	KEY_MEAN_TEN_KEY_1,
	KEY_MEAN_TEN_KEY_2,
	KEY_MEAN_TEN_KEY_3,
	KEY_MEAN_TEN_KEY_4,
	KEY_MEAN_TEN_KEY_5,
	KEY_MEAN_TEN_KEY_6,
	KEY_MEAN_TEN_KEY_7,
	KEY_MEAN_TEN_KEY_8,
	KEY_MEAN_TEN_KEY_9,
} key_meaning_t;
extern const unsigned char key_table[0x80];

void wait_KBC_sendready( void );
void init_keyboard( void );
inline unsigned char get_key_meaning( unsigned char sig )
{
	return key_table[sig];
}


/* dsctbl.c */
void init_gdtidt( void );
void set_segment_descriptor( SEGMENT_DESCRIPTOR* sd, unsigned int limit, int base, int ar );
void set_gate_descriptor( GATE_DESCRIPTOR* gd, int offset, int selector, int ar );
void load_gdtr(int limit, int addr);
void load_idtr(int limit, int addr);

/* graphic.c */
void init_palette( void );
void set_palette( int start, int end, unsigned char* rgb );
void boxfill8( unsigned char* vram, int xsize, unsigned char c, int x0, int y0, int x1, int y1 );
void init_screen( char* vram, int x, int y );
void putfont8( char *vram, int xsize, int x, int y, char color, char *font );
void putfonts8_asc( char* vram, int xsize, int x, int y, char color, unsigned char* str );
void putfonts8_ascbg( char* vram, int xsize, int x, int y, char fgcolor, char bgcolor, unsigned char* str );
void putblock8_8( char* vram, int vxsize, int pxsize, int pysize,
    int px0, int py0, char* buf, int bxsize );

/* int.c */
void init_pic( void );

/* memory.c */
unsigned int memtest( unsigned int start, unsigned int end );
void memman_init( MEMMAN* man );
unsigned int memman_total( MEMMAN* man );
unsigned int memman_alloc( MEMMAN* man, unsigned int size );
int memman_free( MEMMAN* man, unsigned int addr, unsigned int size );
unsigned int memman_alloc_4k( MEMMAN* man, unsigned int size );
int memman_free_4k( MEMMAN* man, unsigned int addr, unsigned int size );

/* sheet.c */
SHEET_CTRL* sheet_ctrl_init( MEMMAN* memman, unsigned char* vram, int xsize, int ysize );
SHEET* sheet_alloc( SHEET_CTRL* ctrl );
void sheet_setbuf( SHEET* sheet, unsigned char* buf, int xsize, int ysize, int color );
void sheet_updown( SHEET* sheet, int height );
/* void sheet_refresh( SHEET_CTRL* ctrl ); */
void sheet_refresh( SHEET* sheet, int bx0, int by0, int bx1, int by1 );
void sheet_slide( SHEET* sheet, int vx0, int vy0 );
void sheet_free( SHEET* sheet );
void sheet_refreshsub( SHEET_CTRL* ctrl, int vx0, int vy0, int vx1, int vy1, int h0, int h1 );
void sheet_refreshmap( SHEET_CTRL* ctrl, int vx0, int vy0, int vx1, int vy1, int height );

/* main.c */
#if 0 /* 未使用のため削除 */
void make_window8( unsigned char* buf, int xsize, int ysize, char* title );
#endif

#endif /* BOOTPACK_H_ */
