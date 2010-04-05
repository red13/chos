#ifndef MOUSE_H
#define MOUSE_H

#define PORT_KEYDAT           (0x0060)
#define PORT_KEYSTA           (0x0064)
#define PORT_KEYCMD           (0x0064)
#define KEYSTA_SEND_NOTREADY  (0x02)
#define KEYCMD_WRITE_MODE     (0x60)
#define KEYCMD_SENDTO_MOUSE   (0xd4)
#define MOUSECMD_ENABLE       (0xf4)
#define KBC_MODE              (0x47)

#define MOUSE_BUTTON_LEFT     (0x01)
#define MOUSE_BUTTON_RIGHT    (0x02)
#define MOUSE_BUTTON_CENTER   (0x04)

typedef struct tag_MouseDec_t
{
    unsigned char buf[3];
    unsigned char phase;
    int x;
    int y;
    int button;
} MouseDec_t;


extern void inthandler2c( int *esp );
extern void enable_mouse( MouseDec_t* mdec );
extern int mouse_decode( MouseDec_t* mdec, unsigned char data );


#endif /* MOUSE_H */


