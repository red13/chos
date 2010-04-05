#include "task.h"

static tss32_manager_t gtsk;

void init_task(void){

	memset(gtsk, 0, sizeof(tss32_manager_t));

	gtsk.buf[0].valid = 1;
	gtsk.buf[0].prev = NULL;
	gtsk.buf[0].next = NULL;
	memset(&(gtsk.buf[0].tss), 0, sizeof(tss32_t));
	gtsk.buf[0].tss.iomap = 0x40000000;

	gtsk.list.head = &gtsk.buf[0];
	gtsk.list.tail = &gtsk.buf[0];
	gtsk.list.num  = 1;
}

int create_task( task_id* id ){
	int i;
	tss32_list_t*	list;

	if( gtsk.list.num >= TASK_MAX ){
		return -1;
	}

	/* ‹ó‚«—Ìˆæ‚ð’T‚· */
	for( i = 0; i < TASK_MAX; i++ ){
		if( gtsk.buf[i].valid == 0 ){
			list = &(gtsk.buf[i]);
			list->prev = gtsk.list.tail;

			gtsk.list.tail = list;

			list->next = NULL;
			list->valid = 1;
			memset( list->tss, 0, sizeof(tss32_t));
			list->tss.iomap = 0x40000000;
		}
	}
}
