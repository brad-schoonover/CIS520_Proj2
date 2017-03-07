#include "userprog/syscall.h"
#include <stdio.h>
#include <syscall-nr.h>
#include "threads/interrupt.h"
#include "threads/thread.h"

static void syscall_handler (struct intr_frame *);
static void halt(void);
static void exit(int status);
static pid_t exec(const char *cmd_line);
static int wait(pid_t pid);
static bool create(const char *file, unsigned initialsize);
static bool remove(const char* file);
static int open(const char* file);
static int filesize(int fd);
static int read(int fd, void *buffer, unsigned size);
static int write(int fd, const void *buffer, unsigned size);
static int seek(int fd, unsigned position);
static unsigned tell(int fd);
static void close(int fd);

/* Function to initialize syscall_handler to handle system call
 * interrupts.
 */
void
syscall_init (void) 
{
  intr_register_int (0x30, 3, INTR_ON, syscall_handler, "syscall");
}

/* Function to retrieve a 4 byte entry starting at the given 
 * location. Used to retrieve a full entry from the user stack
 * instead of forcing the caller to retrieve the information
 * one byte at a time
 */
static int get_usr_stack_entry(void *entry)
{
	int result = 0;
	for(int i = 0; i<4, i++)
	{
		result << 8;
		result += get_user(entry+i);
	}
	result;
}

/* Function used to handle an request for a system call, function
 * first reads the system call number from the requesters stack
 * to determine which system call to perform. The function then
 * retrieves all of the needed arguments for the specified system 
 * call from the requesters stack and makes the system call.
 */
static void
syscall_handler (struct intr_frame *f) 
{
  printf ("system call!\n");
	int call_number = get_user(f->esp+3);
	printf("System call number: %d\n", call_number);

	switch(call_number)
	{
		case SYS_HALT:
			halt();
			break;
		
		case SYS_EXIT:
			int status = get_usr_stack_entry(t->esp + 4)
			printf("Exit status: %d\n", status);
			exit(status);
			break;

		case SYS_EXEC:
			char *commandline = get_usr_stack_entry(t->esp + 4);
			printf("Command line ptr: %s\n",commandline);
			exec(commandline);
			break;
		
		case SYS_WAIT:
			pid_t wait_on = get_usr_stack_entry(t->esp + 4);
			printf("Waiting on thread: %d\n",wait_on);
			wait(wait_on);
			break;
		
		case SYS_CREATE:
			const char *filename = get_usr_stack_entry(t->esp + 4);
			unsigned init_size = get_usr_stack_entry(t->esp + 8);
			printf("Creating %s with size %d\n", filename, init_size);
			create(filename, init_size);
			break
		
		case SYS_REMOVE:
			const char *filename = get_usr_stack_entry(t->esp + 4);
			printf("Removing file: %s\n",filename);
			remove(filename);
			break;
		
		case SYS_OPEN:
			const char *filename = get_usr_stack_entry(t->esp + 4);
			printf("Opening file: %s\n", filename);
			open(filename);
		break;
		
		case SYS_FILESIZE:
			int fd = get_usr_stack_entry(t->esp + 4);
			printf("Getting filesize of filedescriptor: %d\n", fd);
			filesize(fd);
			break;
		
		case SYS_READ:
			int fd = get_usr_stack_entry(t->esp + 4);
			void* buf = get_usr_stack_entry(t->esp + 8);
			unsigned size = get_usr_stack_entry(t->esp + 12);
			printf("Reading %d bytes from %d to %d buffer\n", size, fd, buf);
			read(fd, buf, size);
			break;
		
		case SYS_WRITE:
			int fd = get_usr_stack_entry(t->esp + 4);
			void* buf = get_usr_stack_entry(t->esp + 8);
			unsigned size = get_usr_stack_entry(t->esp + 12);
			printf("Writing first %d bytes from \"%s\" into %d\n", size, buf, fd);
			write(fd, buf, size);
			break;
		
		case SYS_SEEK:
			int fd = get_usr_stack_entry(t->esp + 4);
			unsigned position = get_usr_stack_entry(t->esp + 8);
			printf("Seeking for position %d in %d\n", position, fd);
			seek(fd, position);
			break;
		
		case SYS_TELL:
			int fd = get_usr_stack_entry(t->esp + 4);
			printf("Finding position of next byte to read in %d\n", fd);
			tell(fd);
			break;
		
		case SYS_CLOSE:
			int fd = get_usr_stack_entry(t->esp + 4);
			printf("Closing file %d\n", fd);
			close(fd);
			break;
		
		default:
  		thread_exit ();
		  break;
	}
}

static void
halt(void)
{
	//TODO Implement halt system call
}

static void
exit(int status)
{
	//TODO Implement exit system call
}

static pid_t
exec(const char *cmd_line)
{
	//TODO Implement exec system call
}

static int
wait(pid_t pid)
{
	//TODO Implement wait system call
}

static bool
create(const char *file, unsigned initialsize)
{
	//TODO Implement create system call
}

static bool
remove(const char* file)
{
	//TODO Implement remove system call
}

static int
open(const char* file)
{
	//TODO Implement open system call
}

static int
filesize(int fd)
{
	//TODO Implement filesize system call
}

static int
read(int fd, void *buffer, unsigned size)
{
	//TODO Implement read system call
}

static int
write(int fd, const void *buffer, unsigned size)
{
	//TODO Implement write system call
}

static int
seek(int fd, unsigned position)
{
	//TODO Implement seek system call
}

static unsigned
tell(int fd)
{
	//TODO Implement tell system call
}

static void
close(int fd)
{
	//TODO Implement close system call
}
