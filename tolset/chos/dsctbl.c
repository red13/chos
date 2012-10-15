/**
 * @file
 * @brief ディスクリプタテーブル設定
 */
#include "bootpack.h"

void init_gdtidt( void )
{
    SEGMENT_DESCRIPTOR* gdt = (SEGMENT_DESCRIPTOR*) ADR_GDT;
    GATE_DESCRIPTOR* idt = (GATE_DESCRIPTOR*) ADR_IDT;
    int i;
    
    /* initial GDT */
    for( i = 0; i <= (LIMIT_GDT/8); i++ )
    {
        set_segment_descriptor( gdt+i, 0, 0, 0 );
    }
    set_segment_descriptor( gdt+1, 0xffffffff, 0x00000000, AR_DATA32_RW );
    set_segment_descriptor( gdt+2, LIMIT_BOTPAK, ADR_BOTPAK, AR_CODE32_ER );
    load_gdtr( LIMIT_GDT, ADR_GDT );
    
    /* initial IDT */
    for( i = 0; i <= (LIMIT_IDT/8); i++ )
    {
        set_gate_descriptor( idt+i, 0, 0, 0 );
    }
    load_idtr( LIMIT_IDT, ADR_IDT );
    
    /* IDTの設定 */
    set_segment_descriptor( (SEGMENT_DESCRIPTOR*)idt+0x20, (int)asm_inthandler20, 2*8, AR_INTGATE32 );
    set_segment_descriptor( (SEGMENT_DESCRIPTOR*)idt+0x21, (int)asm_inthandler21, 2*8, AR_INTGATE32 );
    set_segment_descriptor( (SEGMENT_DESCRIPTOR*)idt+0x2c, (int)asm_inthandler2c, 2*8, AR_INTGATE32 );
    
    return;
}

void set_segment_descriptor( SEGMENT_DESCRIPTOR* sd, unsigned int limit, int base, int ar )
{
    if( limit > 0x0fffff )
    {
        ar |= 0x8000; /* G_bit = 1 */
        limit /= 0x1000;
    }
    sd->limit_low = limit & 0xffff;
    sd->base_low  = base & 0xffff;
    sd->base_mid  = (base >> 16) & 0xff;
    sd->access_right = ar & 0xff;
    sd->limit_high = ((limit >> 16) & 0x0f) | ((ar >> 8) & 0xf0);
    sd->base_high = (base >> 24) & 0xff;
    return;
}

void set_gate_descriptor( GATE_DESCRIPTOR* gd, int offset, int selector, int ar )
{
    gd->offset_low = offset & 0xffff;
    gd->selector = selector;
    gd->dw_count = (ar >> 8) & 0xff;
    gd->access_right = ar & 0xff;
    gd->offset_high = (offset >> 16) & 0xffff;
    return ;
}

