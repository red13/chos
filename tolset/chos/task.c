/**
 * @file
 * @brief ƒ^ƒXƒNˆ—
 */
#include <stdio.h>
#include <string.h>
#include "bootpack.h"
#include "task.h"

static tss32_manager_t gtsk;

void init_task( void )
{

	/*@@ test  */
	return;

	memset(&gtsk, 0, sizeof(tss32_manager_t));

	gtsk.buf[0].valid = 1;
	gtsk.buf[0].prev = NULL;
	gtsk.buf[0].next = NULL;
	memset(&(gtsk.buf[0].tss), 0, sizeof(tss32_t));
	gtsk.buf[0].tss.iomap = 0x40000000;

	gtsk.list.head = &gtsk.buf[0];
	gtsk.list.tail = &gtsk.buf[0];
	gtsk.list.num  = 1;
}

int create_task( MEMMAN* mm, task_entry_func_t func, unsigned long stacksize, task_id_t* id )
{
	int				i;
	tss32_list_t*	list;
	void*			stack_bottom;

	if( gtsk.list.num >= TASK_MAX ){
		return -1;
	}

	/* ‹ó‚«—Ìˆæ‚ğ’T‚· */
	for( i = 0; i < TASK_MAX; i++ ){
		if( gtsk.buf[i].valid == 0 ){
			list = &(gtsk.buf[i]);
			list->prev = gtsk.list.tail;

			gtsk.list.tail = list;

			list->next = NULL;
			list->valid = 1;
			memset( &list->tss, 0, sizeof(tss32_t));
			list->tss.eip = (unsigned long)func;
			list->tss.eflags = 0x00000202;
			stacksize = (stacksize & 0x03ff)?(stacksize&0xFFFFFC00)+0x400:stacksize;
			stack_bottom = memman_alloc( mm, stacksize );
			list->tss.esp = (unsigned long)stack_bottom + stacksize;
			list->tss.es = 1*8;
			list->tss.cs = 2*8;
			list->tss.ss = 1*8;
			list->tss.ds = 1*8;
			list->tss.fs = 1*8;
			list->tss.gs = 1*8;
			list->tss.iomap = 0x40000000;
		}
	}
}

