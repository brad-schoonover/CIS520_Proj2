#include "userprog/syscall.h"
#include <stdio.h>
#include <syscall-nr.h>
#include "threads/interrupt.h"
#include "threads/thread.h"
#include "filesys/file.h"
#include "threads/synch.h"

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
static int get_user(const uint8_t *uaddr);
static bool put_user(uint8_t *udst, uint8_t byte)l

static struct lock file_lock;
static struct lock list_lock;
static struct open_filedescriptor fdheader;
static int next_desc = 3;

static int fd_open(const char *file);
static struct file* fd_retrieve(int fd);
static bool fd_close(int fd);

/* Function to initialize syscall_handler to handle system call
 * interrupts.
 */
void
syscall_init (void) 
{
	lock_init(file_lock);
	lock_init(list_lock);
	fdheader->fd = -1;
	fdheader->file_pointer = NULL;
	fdheader->next = NULL;
  intr_register_int (0x30, 3, INTR_ON, syscall_handler, "syscall");
}

/* Function to open a new file and add it to the list of open file
 * descriptors. Returns the file descriptor assigned to the file.
 */
static int
fd_open(const char *filename)
{
	struct open_filedescriptor *temp;
	struct file *newFile;
	struct open_filedescriptor *newcell;

	//Open a file with the given name
	newcell = malloc(sizeof(struct open_filedescriptor));
	lock_acquire(file_lock);
	if((newFile = filesys_open(filename))==NULL)
	{
		return -1;
	}	
	newcell->fd = next_desc;
	next_desc++;
	lock_release(file_lock);

	newcell->file_pointer = newFile;
	newcell->next = NULL;

	//Add new file to the list of open file descriptors
	lock_acquire(list_lock);
	temp = fdheader;
	while(temp->next != NULL)
	{
		temp = temp->next;
	}
	temp->next = newcell;
	lock_release(list_lock);
}

/* Function to retrieve a file pointer from the list of file 
 * descriptors. Returns the file pointer associated with the 
 * given file descriptor.
 */
static struct file*
fd_retrieve(int fd)
{
	lock_acquire(list_lock);
	struct open_filedescriptor *temp = fdheader;
	struct open_filedescriptor *result = NULL;
	while(temp != NULL)
	{
		if(temp->fd == fd)
		{
			result = temp->file;
			break;
		}
		temp = temp->next;
	}
	lock_release(list_lock);
	return result;
}

/* Function to close the given file descriptor. Returns a bool 
 * indicating the success of the close operation.
 */
static bool
fd_close(int fd)
{
	bool result;
	lock_acquire(list_lock);
	lock_acquire(file_lock);
	struct open_filedescriptor *temp = fdheader;
	struct open_filedescriptor *remove;
	while(temp->next != NULL)
	{
		if(temp->next->fd == fd)
		{
			remove = temp->next;
			temp->next = temp->next->next;
			file_close(remove->file_pointer);
			lock_release(file_lock);
			result = true;
		}
		temp = temp->next;
	}
	result =  false;
	lock_release(list_lock);
	return result;
}

/* Reads a byte at user virtual address UADDR.  
	 UADDR must be below PHYS_BASE.   
	 Returns the byte value if successful, -1 if a segfault   
	 occurred. */
static int
get_user (const uint8_t *uaddr)
{  
	int result;  
	asm ("movl $1f, %0; movzbl %1, %0; 1:"      
			 : "=&a" (result) : "m" (*uaddr));
	return result;
}

/* Writes BYTE to user address UDST.   
	 UDST must be below PHYS_BASE.   
	 Returns true if successful, false if a segfault occurred. */
static bool
put_user (uint8_t *udst, uint8_t byte)
{  
	int error_code;  
	asm ("movl $1f, %0; movb %b2, %1; 1:"
			 : "=&a" (error_code), "=m" (*udst) : "q" (byte));  
			 return error_code != -1;
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
	printf("Exiting with status %d\n", status);
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
	bool result;
	lock_acquire(file_lock);
	result = filesys_create(file, initialsize);
	lock_release(file_lock);
	return result;
}

static bool
remove(const char* file)
{
	bool result;
	lock_acquire(file_lock);
	result = filesys_remove(file);
	lock_release(file_lock);
	return result;
}

static int
open(const char* file)
{
	return fd_open(file);
}

static int
filesize(int fd)
{
  int result;
	lock_acquire(file_lock);
	result = file_length(fd_retrieve(fd));
	lock_release(file_lock);
	return result;
}

static int
read(int fd, void *buffer, unsigned size)
{
	int results;
	lock_acquire(file_lock);
	result = file_read(fd_retrieve(fd), buffer, size);
	lock_release(file_lock);
	return result;
}

static int
write(int fd, const void *buffer, unsigned size)
{
	//Counter to keep track of the number of bytes written
	char *tempbuff = (char*)buffer;
	
	//Handle writing to STDOUT
	if(fd == 1)
	{
		int i, put_size, counter;
		counter = 0;
		for(i=0; i<size, i+=300)
		{
			put_size = min(300, size-i);
			putbuf(tempbuff, put_size);
			counter += put_size;
			tempbuff += put_size;
		}
		return counter;
	}
	else
	{
		int result;
		lock_acquire(file_lock);
		result = file_write(fd_retrieve(fd), buffer, size);
		lock_release(file_lock);
		return result;
	}
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
	return fd_close(fd);
}
