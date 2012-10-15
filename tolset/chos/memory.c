/**
 * @file
 * @brief メモリ処理
 */
#include "bootpack.h"

unsigned int memtest( unsigned int start, unsigned int end )
{
    char flg486 = 0;
    unsigned int eflg;
    unsigned int cr0;
    unsigned int i;
    
    /* 386か，486以降なのかを確認する */
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
    man->free_num = 0;      /* 空き情報の個数 */
    man->maxfree_num = 0;   /* 状況観察用 */
    man->lostsize = 0;   /* 解放に失敗した合計サイズ */
    man->losts = 0;      /* 解放に失敗した回数 */
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
            /* 十分な広さの空きを発見 */
            a = man->free[i].addr;
            man->free[i].addr += size;
            man->free[i].size -= size;
            if( man->free[i].size == 0 )
            {
                /* free[i]がなくなったので，前へつめる */
                man->free_num--;
                for( ; i < man->free_num; i++ )
                {
                    /* 構造体の代入 */
                    man->free[i] = man->free[i+1];
                }
            }
            return a;
        }
    }
    /* 空きがない */
    return 0;
}

int memman_free( MEMMAN* man, unsigned int addr, unsigned int size )
{
    int i;
    int j;
    /* まとめやすさを考えると，free[]がaddr順に並んでいるほうがいい */
    /* だからまず，どこに入れるべきかを決める */
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
        /* 前がある */
        if( man->free[i-1].addr + man->free[i-1].size == addr )
        {
            /* 前の空き領域にまとめられる */
            man->free[i-1].size += size;
            if( i < man->free_num )
            {
                /* 後ろもある */
                if( addr + size == man->free[i].addr )
                {
                    /* 後ろもまとめられる */
                    man->free[i-1].size += man->free[i].size;
                    /* man->free[i]を削除 */
                    /* free[i]がなくなったので，前へつめる */
                    man->free_num--;
                    for( ; i < man->free_num; i++ )
                    {
                        man->free[i] = man->free[i+1];
                    }
                }
            }
            /* 成功終了 */
            return 0;
        }
    }
    /* 前とはまとめられなかった */
    if( i < man->free_num )
    {
        /* 後ろがある */
        if( addr + size == man->free[i].addr )
        {
            /* 後ろとはまとめられる */
            man->free[i].addr = addr;
            man->free[i].size += size;
            /* 成功終了 */
            return 0;
        }
    }
    /* 前にも後ろにもまとめられない */
    if( man->free_num < MEMMAN_FREES )
    {
        /* free[i]より後ろを，後ろへずらして，隙間を作る */
        for( j = man->free_num; j > i; j-- )
        {
            man->free[j] = man->free[j-1];
        }
        man->free_num++;
        if( man->maxfree_num < man->free_num )
        {
            /* 最大値を更新 */
            man->maxfree_num = man->free_num;
        }
        man->free[i].addr = addr;
        man->free[i].size = size;
        /* 成功終了 */
        return 0;
    }
    /* 後ろにもずらせなかった */
    man->losts++;
    man->lostsize += size;
    /* 失敗終了 */
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

