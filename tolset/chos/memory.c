/**
 * @file
 * @brief ����������
 */
#include "bootpack.h"

unsigned int memtest( unsigned int start, unsigned int end )
{
    char flg486 = 0;
    unsigned int eflg;
    unsigned int cr0;
    unsigned int i;
    
    /* 386���C486�ȍ~�Ȃ̂����m�F���� */
    eflg = io_load_eflags();
    eflg |= EFLAGS_AC_BIT;
    io_store_eflags( eflg );
    eflg = io_load_eflags();
    if( ( eflg & EFLAGS_AC_BIT ) != 0 )
    {
        flg486 = 1;
    }
    eflg &= ~EFLAGS_AC_BIT;
    io_store_eflags( eflg );
    
    if( flg486 != 0 )
    {
        cr0 = load_cr0();
        cr0 |= CR0_CACHE_DISABLE;
        store_cr0( cr0 );
    }
    
    i = memtest_sub( start, end );
    
    if( flg486 != 0 )
    {
        cr0 = load_cr0();
        cr0 &= ~CR0_CACHE_DISABLE;
        store_cr0( cr0 );
    }
    
    return i;
}

void memman_init( MEMMAN* man )
{
    man->free_num = 0;      /* �󂫏��̌� */
    man->maxfree_num = 0;   /* �󋵊ώ@�p */
    man->lostsize = 0;   /* ����Ɏ��s�������v�T�C�Y */
    man->losts = 0;      /* ����Ɏ��s������ */
    return;
}

unsigned int memman_total( MEMMAN* man )
{
    unsigned int i;
    unsigned int t = 0;
    for( i = 0; i < man->free_num; i++ )
    {
        t += man->free[i].size;
    }
    return t;
}

unsigned int memman_alloc( MEMMAN* man, unsigned int size )
{
    unsigned int i;
    unsigned int a;
    for( i = 0; i < man->free_num; i++ )
    {
        if( man->free[i].size >= size )
        {
            /* �\���ȍL���̋󂫂𔭌� */
            a = man->free[i].addr;
            man->free[i].addr += size;
            man->free[i].size -= size;
            if( man->free[i].size == 0 )
            {
                /* free[i]���Ȃ��Ȃ����̂ŁC�O�ւ߂� */
                man->free_num--;
                for( ; i < man->free_num; i++ )
                {
                    /* �\���̂̑�� */
                    man->free[i] = man->free[i+1];
                }
            }
            return a;
        }
    }
    /* �󂫂��Ȃ� */
    return 0;
}

int memman_free( MEMMAN* man, unsigned int addr, unsigned int size )
{
    int i;
    int j;
    /* �܂Ƃ߂₷�����l����ƁCfree[]��addr���ɕ���ł���ق������� */
    /* ������܂��C�ǂ��ɓ����ׂ��������߂� */
    for( i = 0; i < man->free_num; i++ )
    {
        if( man->free[i].addr > addr )
        {
            break;
        }
    }
    /* free[i-1].addr < addr < free[i].addr */
    if( i > 0 )
    {
        /* �O������ */
        if( man->free[i-1].addr + man->free[i-1].size == addr )
        {
            /* �O�̋󂫗̈�ɂ܂Ƃ߂��� */
            man->free[i-1].size += size;
            if( i < man->free_num )
            {
                /* �������� */
                if( addr + size == man->free[i].addr )
                {
                    /* �����܂Ƃ߂��� */
                    man->free[i-1].size += man->free[i].size;
                    /* man->free[i]���폜 */
                    /* free[i]���Ȃ��Ȃ����̂ŁC�O�ւ߂� */
                    man->free_num--;
                    for( ; i < man->free_num; i++ )
                    {
                        man->free[i] = man->free[i+1];
                    }
                }
            }
            /* �����I�� */
            return 0;
        }
    }
    /* �O�Ƃ͂܂Ƃ߂��Ȃ����� */
    if( i < man->free_num )
    {
        /* ��낪���� */
        if( addr + size == man->free[i].addr )
        {
            /* ���Ƃ͂܂Ƃ߂��� */
            man->free[i].addr = addr;
            man->free[i].size += size;
            /* �����I�� */
            return 0;
        }
    }
    /* �O�ɂ����ɂ��܂Ƃ߂��Ȃ� */
    if( man->free_num < MEMMAN_FREES )
    {
        /* free[i]�������C���ւ��炵�āC���Ԃ���� */
        for( j = man->free_num; j > i; j-- )
        {
            man->free[j] = man->free[j-1];
        }
        man->free_num++;
        if( man->maxfree_num < man->free_num )
        {
            /* �ő�l���X�V */
            man->maxfree_num = man->free_num;
        }
        man->free[i].addr = addr;
        man->free[i].size = size;
        /* �����I�� */
        return 0;
    }
    /* ���ɂ����点�Ȃ����� */
    man->losts++;
    man->lostsize += size;
    /* ���s�I�� */
    return -1;
}

unsigned int memman_alloc_4k( MEMMAN* man, unsigned int size )
{
    unsigned int ret;
    size = ( size + 0x0fff ) & 0xfffff000;
    ret = memman_alloc( man, size );
    return ret;
}

int memman_free_4k( MEMMAN* man, unsigned int addr, unsigned int size )
{
    int i;
    size = ( size + 0x0fff ) & 0xfffff000;
    i = memman_free( man, addr, size );
    return i;
}

