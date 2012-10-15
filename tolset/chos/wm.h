/**
 * @file
 * @brief ウインドウ管理処理
 */
#ifndef __WM_H_
#define __WM_H_

/*
 * LOCAL DEFINE
 */
#define WM_WINDOW_NAME_LENGTH 32

/* ウインドウの状態 */
/* 通常は0でよい */
#define WM_WINDOW_STATUS_NORMAL     0
#define WM_WINDOW_STATUS_NOTITLE    0x00000001
#define WM_WINDOW_STATUS_INVISIBLE  0x00000002

/* タイトル行の高さ */
#define WM_WINDOW_TITLE_HEIGHT (12)
/* ウインドウの枠の幅 */
#define WM_WINDOW_FRAME_WIDTH  (2)

/* 色の深さ */
typedef enum tag_WindowColorDepth
{
    WINDOW_COLOR_DEPTH_DEFAULT,  /* Window生成時のデフォルト指定 */
    WINDOW_COLOR_DEPTH_16,       /* 16色 */
    WINDOW_COLOR_DEPTH_256,      /* 256色 */
    WINDOW_COLOR_DEPTH_65536,    /* 65536色 */
    WINDOW_COLOR_DEPTH_RGB_8,    /* RGB 8bit */
    WINDOW_COLOR_DEPTH_RGB_16,   /* RGB 16bit */
    WINDOW_COLOR_DEPTH_RGB_32,   /* RGB 32bit */
} WindowColorDepth;

typedef struct tag_position_t{
    unsigned long x;			/* left -> right */
    unsigned long y;			/* top -> bottom */
} position_t;

typedef struct tag_rect_t{
	position_t pos;				/* left & top */
	unsigned long width;
	unsigned long height;
} rect_t;

/* ウインドウマネージャ */
struct tag_WindowManager{
    struct tag_Window* 	list_head;	/* ウインドウのリスト */
    unsigned long 		last_window_id;	/* 最後につけたid */
    MEMMAN*  			memory_manager;
    WindowColorDepth 	color_depth;	/* 今の表示モード */
    unsigned char* 		vram;   /* vramの先頭へのポインタ */
    unsigned char* 		buffer; /* 描画用のバッファ */
    unsigned char* 		map;    /* 描画バッファの更新ビットフラグ列 */
    unsigned long 		height; /* バッファの高さ */
    unsigned long 		width;  /* バッファの幅 */
};

/* ウインドウ */
struct tag_Window{
    unsigned char* 				buffer;	/* ウインドウ描画用バッファ */
    struct tag_Window* 			prev;	/* リストの前のウインドウ：NULLなら先頭 */
    struct tag_Window* 			next;	/* リストの次のウインドウ：NULLなら終端 */
    struct tag_WindowManager* 	manager;	/* 親のウインドウマネージャ */
    unsigned long 				window_id;
    WindowColorDepth 			color_depth;	/* ウインドウの表示モード */
    position_t 					pos;	/* スクリーンに対するウインドウの左上位置 */
    unsigned long 				height;	/* ウインドウ全体の高さ */
    unsigned long 				width;	/* ウインドウ全体の幅 */
    unsigned long 				status;	/* 状態をビット列で保持 */
    char 						name[WM_WINDOW_NAME_LENGTH];	/* 名前 */
};

typedef struct tag_Window Window;
typedef struct tag_WindowManager WindowManager;

/********************/
/* プロトタイプ宣言 */
/********************/
extern int create_window_manager( MEMMAN* mm,
                                  unsigned char* vram,
                                  WindowColorDepth color_depth,
                                  unsigned long width,
                                  unsigned long height );
extern void destroy_window_manager( void );

extern int create_window( MEMMAN* mm,
                          WindowColorDepth color_depth, 
                          unsigned long width,  /* クライアント領域の幅 */
                          unsigned long height, /* クライアント領域の高さ */
                          unsigned long status, /* 状態をビット列で指定 */
                          char* name,           /* windowの名前 */
                          unsigned long* window_id ); /* 生成したウインドウのIDを返す */
extern void destroy_window( unsigned long id );
extern int show_window( unsigned long window_id,
						unsigned long x,
						unsigned long y,
						unsigned long width,
						unsigned long height );
extern int show_whole_window( unsigned long wnd_id );
extern void display_window( void );
extern int move_window( unsigned long window_id,
                        unsigned long x,
                        unsigned long y );
extern void exchange_window_priority( unsigned long a,
                                      unsigned long b );

extern int print_dot( unsigned long window_id,
                      unsigned long x,
                      unsigned long y,
                      unsigned char color );
extern int print_string( unsigned long window_id,
                         unsigned char* str,
                         unsigned long x,
                         unsigned long y,
                         unsigned char color );
extern int fill_rect( unsigned long window_id,
					  unsigned long left,
					  unsigned long top,
					  unsigned long right,
					  unsigned long bottom,
					  unsigned char color );

extern void window_force_display( unsigned long id );
extern void debug_print_window_manager( void );

#endif /* __WM_H_ */
