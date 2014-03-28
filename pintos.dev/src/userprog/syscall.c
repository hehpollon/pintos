#include "userprog/syscall.h"
#include <stdio.h>
#include <syscall-nr.h>
#include "threads/interrupt.h"
#include "threads/thread.h"
#include <devices/shutdown.h>
#include <filesys/filesys.h>
#include <threads/thread.h>

static void syscall_handler (struct intr_frame *);
void check_address(void*);
void get_argument(void*,int*,int);
void halt(void);
void exit(int);
bool create(const char*,unsigned);
bool remove(const char*);

void
syscall_init (void) 
{
  intr_register_int (0x30, 3, INTR_ON, syscall_handler, "syscall");
}

/*Syscall_handler copy the esp(stack) constructed by syscall
 *First of esp is syscall number
 *By syscall number, it executes each syscall function & often checks
 *whether the address is in the user stack.*/
static void
syscall_handler (struct intr_frame *f UNUSED) 
{
	int syscall_number=*(int *)f->esp;
	int* arg;
	int i=0;
	int arg_count=0;
  printf ("system call!: %d\n",syscall_number);
	check_address(f->esp);
	switch(syscall_number)
	{
		case SYS_HALT:
			arg_count=0;
			halt();
			break;
		case SYS_EXIT:
			arg_count=1;
			arg=(int*)malloc(sizeof(int)*arg_count);
			get_argument(f->esp,arg,arg_count);
			exit(arg[0]);
			break;
		case SYS_CREATE:
			arg_count=2;
			arg=(int*)malloc(sizeof(int)*arg_count);
			get_argument(f->esp,arg,arg_count);
			check_address(arg[0]);
			create((char*)arg[0],arg[1]);
			break;
		case SYS_REMOVE:
			arg_count=1;
			arg=(int*)malloc(sizeof(int)*arg_count);
			get_argument(f->esp,arg,arg_count);
			check_address(arg[0]);
			remove((char*)arg[0]);
			break;
	}
	if(arg_count)
		free(arg);
  thread_exit ();
}

void check_address(void *addr)
{
	uint32_t address=addr;
	if(address<0x08048000||address>0xc0000000)
	{
		printf("check address_denied : %x\n",address);
		exit(-1);
	}
	else
		printf("check address_OK : %x\n",address);
}

void get_argument(void *esp,int *arg,int count)
{
	int i;
	for(i=1;i<count+1;i++)
	{
		arg[i]=((int*)esp)[i];
		//printf("%x/",arg[i]);
	}
}

void halt()
{
	shutdown_power_off();
}

void exit(int status)
{
	struct thread* thread_now=thread_current();
	printf("thread name: %s\n",thread_now->name);
	printf("exit status: %d\n",status);
	thread_exit();
}

bool create(const char* file,unsigned initial_size)
{
	printf("syscall_create\n");
	return filesys_create(file,initial_size);
}

bool remove(const char* file)
{
	printf("syscall_remove\n");
	return filesys_remove(file);
}
