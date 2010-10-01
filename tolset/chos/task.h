#define TASK_MAX (32)

/* task status segment (32bit) */
typedef struct tag_tss32_t{
	unsigned long backlink;
	unsigned long esp0;
	unsigned long ss0;
	unsigned long esp1;
	unsigned long ss1;
	unsigned long esp2;
	unsigned long ss2;
	unsigned long cr3;

	unsigned long eip;
	unsigned long eflags;
	unsigned long eax;
	unsigned long ecx;
	unsigned long edx;
	unsigned long ebx;
	unsigned long esp;
	unsigned long ebp;
	unsigned long esi;
	unsigned long edi;

	unsigned long es;
	unsigned long cs;
	unsigned long ss;
	unsigned long ds;
	unsigned long fs;
	unsigned long gs;

	unsigned long ldtr;
	unsigned long iomap;
} tss32_t;

typedef tag_tss32_list_t{
	tag_tss32_list_t*	prev;
	tag_tss32_list_t*	next;
	unsigned long		valid;
	tss32_list_t		tss;
} tss32_list_t;

typedef tag_tss32_list_head_t{
	tss32_list_t*	head;
	tss32_list_t*	tail;
	unsigned long	num;
} tss32_list_head_t;

typedef struct tag_tss32_manager_t{
	tss32_list_head_t	list;
	tss32_list_t		buf[TASK_MAX];
} tss32_manager_t;

typedef unsigned long task_id_t;

typedef int (*TASK_ENTRY_FUNC)(void*);


void init_task(void);
int create_task( task_id* id );


