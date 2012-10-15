/**
 * @file
 * @brief タイマー処理
 */
#define TIMER_MAX (100)

typedef unsigned long timer_id_t;

typedef struct tag_timer_t{
	unsigned long	timeout;
	void (*func)(void *);
	void*			param;
} timer_t;

typedef struct tag_timer_list_t{
	struct tag_timer_list_t* 	prev;
	struct tag_timer_list_t* 	next;
	unsigned long 				valid;
	unsigned long				is_cyclic;
	timer_t 					t;
} timer_list_t;

typedef struct tag_timer_list_head_t{
	timer_list_t* head;
	timer_list_t* tail;
	unsigned long num;
} timer_list_head_t;

typedef struct tag_timer_manager_t{
	timer_list_head_t 	list;
	timer_list_t 		buf[TIMER_MAX];
} timer_manager_t;

int init_timer_manager( MEMMAN* mm, timer_manager_t** tm );
void timer_event_procedure( timer_manager_t* tm );

int add_timer( timer_manager_t* tm, timer_t* t, timer_id_t* id );
int delete_timer( timer_manager_t* tm, timer_id_t id );

int add_cyclic_timer( timer_manager_t* tm, timer_t* t, timer_id_t* id );
