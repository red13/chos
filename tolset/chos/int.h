#ifndef INT_H_
#define INT_H_

/* function macro */
#define jiffies (global_time.count)


/*******/
/* PIC */
/*******/
#define PIC0_ICW1  (0x0020)
#define PIC0_OCW2  (0x0020)
#define PIC0_IMR   (0x0021)
#define PIC0_ICW2  (0x0021)
#define PIC0_ICW3  (0x0021)
#define PIC0_ICW4  (0x0021)
#define PIC1_ICW1  (0x00a0)
#define PIC1_OCW2  (0x00a0)
#define PIC1_IMR   (0x00a1)
#define PIC1_ICW2  (0x00a1)
#define PIC1_ICW3  (0x00a1)
#define PIC1_ICW4  (0x00a1)

/************************************/
/* PIT: Programmable Interval Timer */
/************************************/
#define PIT_CTRL (0x0043)
#define PIT_CNT0 (0x0040)

/* 1�b������̃^�C�}���荞�݉� */
#define TIMER_IRQ_PER_SECOND (100)

/* AT�݊��@(Hz) */
#define CLOCK_FREQUENCY    (1193180)
/* FM-TOWNS(ch0-2)(Hz) */
//#define CLOCK_FREQUENCY    (307200)
/* FM-TOWNS(ch4)(Hz) */
//#define CLOCK_FREQUENCY    (1228800)
/* PC-9801(�V�X�e���N���b�N5MHz�n) */
//#define CLOCK_FREQUENCY    (2457600)
/* PC-9801(�V�X�e���N���b�N8MHz�n) */
//#define CLOCK_FREQUENCY    (1996800)

/* �l�̌ܓ������l�ɋ߂��ق����悢�Ǝv����̂ŁC�Œ�l�ɂ��Ƃ� */
//#include CNT0_VALUE (CLOCK_FREQUENCY/TIMER_IRQ_PER_SECOND)
#define PIT_CNT0_VALUE (11932)

#define PIT_CNT0_HIGH ((PIT_CNT0_VALUE & 0xff00) >> 8)
#define PIT_CNT0_LOW  (PIT_CNT0_VALUE & 0x00ff)


typedef struct tag_TIME_t{
	unsigned long count;
}TIME_t;

/* �v���g�^�C�v�錾 */
void init_pic( void );
void init_pit( void );
unsigned long get_systime( void );


#endif /* INT_H_ */
