#include <stdio.h>
#include <string.h>
#include "bootpack.h"
#include "int.h"
#include "timer.h"

int init_timer_manager( MEMMAN* mm, timer_manager_t** tm )
{
	timer_manager_t* t;

	t = (timer_manager_t*)memman_alloc_4k( mm, sizeof(timer_manager_t) );
	if( t == 0 ){
		return -1;
	}

	memset( t->buf, 0, sizeof(timer_list_t) * TIMER_MAX );

	/* 番兵を置く */
	t->buf[0].prev = NULL;
	t->buf[0].next = NULL;
	t->buf[0].valid = 1;
	t->buf[0].t.timeout = get_systime() + 0x80000000 - 1;
	t->buf[0].t.func = NULL;
	t->buf[0].t.param = NULL;
	t->list.head = &t->buf[0];
	t->list.tail = &t->buf[0];
	t->list.num = 1;

	*tm = t;

	return 0;
}

int add_timer( timer_manager_t* tm, timer_t* t, unsigned long* id )
{
	timer_list_t* cur;
	int i;
	unsigned long now;

	if( tm->list.num >= TIMER_MAX ){
		return -1;
	}

	now = get_systime();

	for( i = 0; i < TIMER_MAX; i++ ){
		if( tm->buf[i].valid == 0 ){
			/* 挿入する場所をみつけた */
			break;
		}
	}
	if( i == TIMER_MAX ){
		return -1;
	}
	io_cli();
	for( cur = tm->list.head; cur != NULL; cur = cur->next ){
		if((int)(t->timeout - now) < (int)(cur->t.timeout - now)){
				
			tm->buf[i].prev = cur->prev;
			tm->buf[i].next = cur;
			tm->buf[i].valid = 1;
			tm->buf[i].t.timeout = t->timeout;
			tm->buf[i].t.func = t->func;
			tm->buf[i].t.param = t->param;
			if( cur->prev == NULL ){
				tm->list.head = &tm->buf[i];
			}else{
				cur->prev->next = &tm->buf[i];
			}
			tm->list.num++;
		}
	}
	if( cur->next == NULL ){
		/* 終端に追加するとき */
		tm->buf[i].prev = cur;
		tm->buf[i].next = NULL;
		tm->buf[i].valid = 1;
		tm->buf[i].t.timeout = t->timeout;
		tm->buf[i].t.func = t->func;
		tm->buf[i].t.param = t->param;
		cur->next = &tm->buf[i];
		tm->list.tail = &tm->buf[i];
		tm->list.num++;
	}
	io_sti();

	*id = i;

	return 0;
}

int delete_timer( timer_manager_t* tm, unsigned long id )
{
	timer_list_t* cur;
	int ret = 0;

	if( tm == NULL ){
		return -1;
	}

	if( (tm->list.num <= 1) || (id == 0) ){
		/* 番兵をdeleteすることはできない */
		return -1;
	}

	io_cli();
	/* for( cur = tm->list.head; cur != NULL; cur = cur->next ){ */
	/* 	if( (cur->t.timeout == t->timeout) && */
	/* 		(cur->t.func == t->func) && */
	/* 		(cur->t.param == t->param) ){ */
	/* 		cur->valid = 0; */
	/* 		if( cur->prev != NULL ){ */
	/* 			cur->prev->next = cur->next; */
	/* 		}else{ */
	/* 			tm->list.head = cur->next; */
	/* 		} */
	/* 		if( cur->next != NULL ){ */
	/* 			cur->next->prev = cur->prev; */
	/* 		}else{ */
	/* 			tm->list.tail = cur->prev; */
	/* 		} */
	/* 		tm->list.num--; */

	/* 		ret = 1; */
	/* 	} */
	/* } */
	cur = &(tm->buf[id]);
	cur->valid = 0;
	if( cur->prev != NULL ){
		cur->prev->next = cur->next;
	}else{
		tm->list.head = cur->next;
	}
	if( cur->next != NULL ){
		cur->next->prev = cur->prev;
	}else{
		tm->list.tail = cur->prev;
	}
	tm->list.num--;
	io_sti();
	
	return ret;
}

static int reentry_cyclic_timer( timer_manager_t* tm, timer_t* t )
{
	/*@ 未実装 */
	return 0;
}

int add_cyclic_timer( timer_manager_t* tm, timer_t* t )
{
	/*@ 未実装 */
	return 0;
}
