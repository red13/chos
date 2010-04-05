#ifndef QUEUE_H
#define QUEUE_H

#define FLAGS_OVERRUN  (0x00000001)


typedef struct tag_Queue8_t
{
    unsigned char *buf;  
    unsigned long write;     /* ���ɏ������ނƂ��� */
    unsigned long read;     /* ���ɓǂݏo���Ƃ��� */
    unsigned long size;  /* buf�̃T�C�Y */
    unsigned long free;  /* �o�b�t�@�̋� */
    unsigned long flags; /* �I�[�o�[�t���[�̗L�� */
} Queue8_t;

typedef struct tag_Queue16_t
{
    unsigned short* buf;
    unsigned long write;
    unsigned long read;
    unsigned long size;
    unsigned long free;
    unsigned long flags;
} Queue16_t;

typedef struct tag_Queue32_t
{
    unsigned long* buf;
    unsigned long write;
    unsigned long read;
    unsigned long size;
    unsigned long free;
    unsigned long flags;
} Queue32_t;

#if defined(queue_c_)
#define EXTERN
#else
#define EXTERN extern
#endif

EXTERN void queue8_init( Queue8_t* que, int size, unsigned char* buf );
EXTERN int queue8_put( Queue8_t* que, unsigned char data );
EXTERN int queue8_get( Queue8_t* que );
EXTERN int queue8_status( Queue8_t* que );

EXTERN void queue16_init( Queue16_t* q, int size, unsigned short* buf );
EXTERN int queue16_put( Queue16_t* q, unsigned short data );
EXTERN int queue16_get( Queue16_t* q );
EXTERN int queue16_status( Queue16_t* q );


#endif /* QUEUE_ */
