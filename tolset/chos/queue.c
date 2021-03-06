/**
 * @file
 * @brief キュー処理
 */

#include "queue.h"

/* キューの初期化 */
void queue8_init( Queue8_t* que, int size, unsigned char* buf )
{
    que->size = size;
    que->buf = buf;
    que->free = size;
    que->flags = 0;
    que->write = 0; /* 書き込み位置 */
    que->read = 0; /* 読み出し位置 */
    return;
}

void queue16_init( Queue16_t* q, int size, unsigned short* buf )
{
    q->size = size;
    q->buf = buf;
    q->free = size;
    q->flags = 0;
    q->write = 0;
    q->read = 0;
    return;
}

/* QUEUEへデータをenqueueする */
/* 成功したら0を返す */
/* 失敗したら-1を返す */
int queue8_put( Queue8_t* que, unsigned char data )
{
    if( que->free == 0 )
    {
        /* オーバーフロー */
        que->flags |= FLAGS_OVERRUN;
        return -1;
    }
    que->buf[que->write] = data;
    que->write++;
    if( que->write >= que->size )
    {
        que->write = 0;
    }
    que->free--;
    return 0;
}

int queue16_put( Queue16_t* q, unsigned short data )
{
    if( q->free == 0 )
    {
        /* オーバーフロー */
        q->flags |= FLAGS_OVERRUN;
        return -1;
    }
    q->buf[q->write] = data;
    q->write++;
    if( q->write >= q->size )
    {
        q->write = 0;
    }
    q->free--;
    return 0;
}

/* QUEUEから1バイトデータをdequeueする */
/* 取得できたら0を返す */
/* 取得できなかったら-1を返す */
int queue8_get( Queue8_t* que )
{
    int data;
    if( que->free == que->size )
    {
        /* バッファが空のときは，とりあえず-1を返却 */
        return -1;
    }
    data = que->buf[que->read];
    que->read++;
    if( que->read == que->size )
    {
        que->read = 0;
    }
    que->free++;
    return data;
}

int queue16_get( Queue16_t* q )
{
    int data;
    if( q->free == q->size )
    {
        /* バッファが空の時は，とりあえず-1を返却する */
        return -1;
    }
    data = q->buf[q->read];
    q->read++;
    if( q->read == q->size )
    {
        q->read = 0;
    }
    q->free++;
    return data;
}

/* QUEUEの使用中の状態を取得する */
int queue8_status( Queue8_t* que )
{
    return(que->size - que->free);
}

int queue16_status( Queue16_t* q )
{
    return(q->size - q->free);
}
