/**
 * @file
 * @brief �E�C���h�E�Ǘ�����
 */
#ifndef __WM_H_
#define __WM_H_

/*
 * LOCAL DEFINE
 */
#define WM_WINDOW_NAME_LENGTH 32

/* �E�C���h�E�̏�� */
/* �ʏ��0�ł悢 */
#define WM_WINDOW_STATUS_NORMAL     0
#define WM_WINDOW_STATUS_NOTITLE    0x00000001
#define WM_WINDOW_STATUS_INVISIBLE  0x00000002

/* �^�C�g���s�̍��� */
#define WM_WINDOW_TITLE_HEIGHT (12)
/* �E�C���h�E�̘g�̕� */
#define WM_WINDOW_FRAME_WIDTH  (2)

/* �F�̐[�� */
typedef enum tag_WindowColorDepth
{
    WINDOW_COLOR_DEPTH_DEFAULT,  /* Window�������̃f�t�H���g�w�� */
    WINDOW_COLOR_DEPTH_16,       /* 16�F */
    WINDOW_COLOR_DEPTH_256,      /* 256�F */
    WINDOW_COLOR_DEPTH_65536,    /* 65536�F */
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

/* �E�C���h�E�}�l�[�W�� */
struct tag_WindowManager{
    struct tag_Window* 	list_head;	/* �E�C���h�E�̃��X�g */
    unsigned long 		last_window_id;	/* �Ō�ɂ���id */
    MEMMAN*  			memory_manager;
    WindowColorDepth 	color_depth;	/* ���̕\�����[�h */
    unsigned char* 		vram;   /* vram�̐擪�ւ̃|�C���^ */
    unsigned char* 		buffer; /* �`��p�̃o�b�t�@ */
    unsigned char* 		map;    /* �`��o�b�t�@�̍X�V�r�b�g�t���O�� */
    unsigned long 		height; /* �o�b�t�@�̍��� */
    unsigned long 		width;  /* �o�b�t�@�̕� */
};

/* �E�C���h�E */
struct tag_Window{
    unsigned char* 				buffer;	/* �E�C���h�E�`��p�o�b�t�@ */
    struct tag_Window* 			prev;	/* ���X�g�̑O�̃E�C���h�E�FNULL�Ȃ�擪 */
    struct tag_Window* 			next;	/* ���X�g�̎��̃E�C���h�E�FNULL�Ȃ�I�[ */
    struct tag_WindowManager* 	manager;	/* �e�̃E�C���h�E�}�l�[�W�� */
    unsigned long 				window_id;
    WindowColorDepth 			color_depth;	/* �E�C���h�E�̕\�����[�h */
    position_t 					pos;	/* �X�N���[���ɑ΂���E�C���h�E�̍���ʒu */
    unsigned long 				height;	/* �E�C���h�E�S�̂̍��� */
    unsigned long 				width;	/* �E�C���h�E�S�̂̕� */
    unsigned long 				status;	/* ��Ԃ��r�b�g��ŕێ� */
    char 						name[WM_WINDOW_NAME_LENGTH];	/* ���O */
};

typedef struct tag_Window Window;
typedef struct tag_WindowManager WindowManager;

/********************/
/* �v���g�^�C�v�錾 */
/********************/
extern int create_window_manager( MEMMAN* mm,
                                  unsigned char* vram,
                                  WindowColorDepth color_depth,
                                  unsigned long width,
                                  unsigned long height );
extern void destroy_window_manager( void );

extern int create_window( MEMMAN* mm,
                          WindowColorDepth color_depth, 
                          unsigned long width,  /* �N���C�A���g�̈�̕� */
                          unsigned long height, /* �N���C�A���g�̈�̍��� */
                          unsigned long status, /* ��Ԃ��r�b�g��Ŏw�� */
                          char* name,           /* window�̖��O */
                          unsigned long* window_id ); /* ���������E�C���h�E��ID��Ԃ� */
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
