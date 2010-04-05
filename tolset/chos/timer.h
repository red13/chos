#define TIMER_MAX (100)

typedef struct tag_timer_t{
	unsigned long	timeout;
	void (*func)(void *);
	void*			param;
} timer_t;

typedef struct tag_timer_list_t{
	struct tag_timer_list_t* 	prev;
	struct tag_timer_list_t* 	next;
	unsigned long 				valid;
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
int add_timer( timer_manager_t* tm, timer_t* t, unsigned long* id );
int delete_timer( timer_manager_t* tm, unsigned long id );
