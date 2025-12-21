# RANDY'S CANNABIS-ENHANCED LINUX SYSCALLS REFERENCE (X86-64)
# PLANT SPIRIT KERNEL INTERFACE AND SYSTEM CALL CONSCIOUSNESS

## RANDY'S SYSCALL PHILOSOPHY:
# "SYSTEM CALLS ARE THE SACRED INTERFACE BETWEEN USER SPACE AND KERNEL
# SPACE - THE DIVINE PORTAL THROUGH WHICH APPLICATIONS REQUEST SERVICES
# FROM THE OPERATING SYSTEM. CANNABIS ENHANCEMENT REVEALS THE ELEGANT
# SYMMETRY OF NUMBERED SYSTEM SERVICES AND THE BEAUTY OF DIRECT KERNEL
# PROGRAMMING."

# ========================================================================
# SYSTEM CALL INTERFACE (X86-64 LINUX)
# ========================================================================

# SYSTEM CALL INSTRUCTION:
# syscall              # Invoke system call (replaces int 0x80)

# REGISTER USAGE:
# RAX = system call number
# RDI = 1st argument
# RSI = 2nd argument  
# RDX = 3rd argument
# R10 = 4th argument (NOTE: R10, not RCX!)
# R8  = 5th argument
# R9  = 6th argument

# RETURN VALUE:
# RAX = return value (or negative error code)

# DESTROYED REGISTERS:
# RCX = destroyed (contains return RIP)
# R11 = destroyed (contains saved RFLAGS)

# ========================================================================
# ESSENTIAL SYSTEM CALLS (Cannabis-Enhanced Kernel Services)
# ========================================================================

# FILE OPERATIONS
# ---------------

# sys_read (0) - Read from file descriptor
# ssize_t read(int fd, void *buf, size_t count);
mov rax, 0          # sys_read
mov rdi, 0          # stdin
mov rsi, buffer     # buffer address
mov rdx, 1024       # bytes to read
syscall

# sys_write (1) - Write to file descriptor  
# ssize_t write(int fd, const void *buf, size_t count);
mov rax, 1          # sys_write
mov rdi, 1          # stdout
mov rsi, msg        # message address
mov rdx, 13         # message length
syscall

# sys_open (2) - Open file
# int open(const char *pathname, int flags, mode_t mode);
mov rax, 2          # sys_open
mov rdi, filename   # filename address
mov rsi, 0x42       # O_RDWR | O_CREAT
mov rdx, 0644       # file permissions
syscall

# sys_close (3) - Close file descriptor
# int close(int fd);
mov rax, 3          # sys_close
mov rdi, 5          # file descriptor
syscall

# sys_openat (257) - Open file relative to directory
# int openat(int dirfd, const char *pathname, int flags, mode_t mode);
mov rax, 257        # sys_openat
mov rdi, -100       # AT_FDCWD (current directory)
mov rsi, filename   # filename
mov rdx, 0x42       # O_RDWR | O_CREAT
mov r10, 0644       # permissions
syscall

# MEMORY MANAGEMENT
# -----------------

# sys_mmap (9) - Map memory
# void *mmap(void *addr, size_t length, int prot, int flags, int fd, off_t offset);
mov rax, 9          # sys_mmap
mov rdi, 0          # addr (NULL = let kernel choose)
mov rsi, 4096       # length (page size)
mov rdx, 3          # prot (PROT_READ | PROT_WRITE)
mov r10, 0x22       # flags (MAP_PRIVATE | MAP_ANONYMOUS)
mov r8, -1          # fd (-1 for anonymous)
mov r9, 0           # offset
syscall

# sys_munmap (11) - Unmap memory
# int munmap(void *addr, size_t length);
mov rax, 11         # sys_munmap
mov rdi, addr       # address to unmap
mov rsi, 4096       # length
syscall

# sys_brk (12) - Change data segment size
# int brk(void *addr);
mov rax, 12         # sys_brk
mov rdi, new_addr   # new break address
syscall

# PROCESS MANAGEMENT
# ------------------

# sys_fork (57) - Create child process
# pid_t fork(void);
mov rax, 57         # sys_fork
syscall             # returns 0 in child, PID in parent

# sys_execve (59) - Execute program
# int execve(const char *pathname, char *const argv[], char *const envp[]);
mov rax, 59         # sys_execve
mov rdi, program    # program path
mov rsi, argv       # argument array
mov rdx, envp       # environment array
syscall

# sys_exit (60) - Terminate process
# void exit(int status);
mov rax, 60         # sys_exit
mov rdi, 0          # exit status
syscall             # never returns

# sys_wait4 (61) - Wait for process
# pid_t wait4(pid_t pid, int *wstatus, int options, struct rusage *rusage);
mov rax, 61         # sys_wait4
mov rdi, -1         # wait for any child
mov rsi, status     # status pointer
mov rdx, 0          # options
mov r10, 0          # rusage (NULL)
syscall

# sys_kill (62) - Send signal to process
# int kill(pid_t pid, int sig);
mov rax, 62         # sys_kill
mov rdi, pid        # process ID
mov rsi, 9          # SIGKILL
syscall

# sys_getpid (39) - Get process ID
# pid_t getpid(void);
mov rax, 39         # sys_getpid
syscall

# sys_getppid (110) - Get parent process ID
# pid_t getppid(void);
mov rax, 110        # sys_getppid
syscall

# DIRECTORY OPERATIONS
# --------------------

# sys_getcwd (79) - Get current working directory
# char *getcwd(char *buf, size_t size);
mov rax, 79         # sys_getcwd
mov rdi, buffer     # buffer address
mov rsi, 4096       # buffer size
syscall

# sys_chdir (80) - Change directory
# int chdir(const char *path);
mov rax, 80         # sys_chdir
mov rdi, path       # directory path
syscall

# sys_mkdir (83) - Create directory
# int mkdir(const char *pathname, mode_t mode);
mov rax, 83         # sys_mkdir
mov rdi, dirname    # directory name
mov rsi, 0755       # permissions
syscall

# sys_rmdir (84) - Remove directory
# int rmdir(const char *pathname);
mov rax, 84         # sys_rmdir
mov rdi, dirname    # directory name
syscall

# FILE INFORMATION
# ----------------

# sys_stat (4) - Get file status
# int stat(const char *pathname, struct stat *statbuf);
mov rax, 4          # sys_stat
mov rdi, filename   # filename
mov rsi, statbuf    # stat buffer
syscall

# sys_fstat (5) - Get file status by descriptor
# int fstat(int fd, struct stat *statbuf);
mov rax, 5          # sys_fstat
mov rdi, fd         # file descriptor
mov rsi, statbuf    # stat buffer
syscall

# sys_lseek (8) - Seek in file
# off_t lseek(int fd, off_t offset, int whence);
mov rax, 8          # sys_lseek
mov rdi, fd         # file descriptor
mov rsi, offset     # offset
mov rdx, 0          # SEEK_SET
syscall

# SIGNALS
# -------

# sys_rt_sigaction (13) - Set signal handler
# int sigaction(int signum, const struct sigaction *act, struct sigaction *oldact);
mov rax, 13         # sys_rt_sigaction
mov rdi, 2          # SIGINT
mov rsi, newact     # new action
mov rdx, oldact     # old action
mov r10, 8          # sigsetsize
syscall

# sys_rt_sigreturn (15) - Return from signal handler
# (Used internally by signal handlers)
mov rax, 15         # sys_rt_sigreturn
syscall

# sys_alarm (37) - Set alarm
# unsigned int alarm(unsigned int seconds);
mov rax, 37         # sys_alarm
mov rdi, 5          # 5 seconds
syscall

# NETWORKING (SOCKET SYSCALLS)
# ----------------------------

# sys_socket (41) - Create socket
# int socket(int domain, int type, int protocol);
mov rax, 41         # sys_socket
mov rdi, 2          # AF_INET
mov rsi, 1          # SOCK_STREAM
mov rdx, 0          # protocol (auto)
syscall

# sys_bind (49) - Bind socket to address
# int bind(int sockfd, const struct sockaddr *addr, socklen_t addrlen);
mov rax, 49         # sys_bind
mov rdi, sockfd     # socket descriptor
mov rsi, addr       # address structure
mov rdx, addrlen    # address length
syscall

# sys_listen (50) - Listen for connections
# int listen(int sockfd, int backlog);
mov rax, 50         # sys_listen
mov rdi, sockfd     # socket descriptor
mov rsi, 5          # backlog
syscall

# sys_accept (43) - Accept connection
# int accept(int sockfd, struct sockaddr *addr, socklen_t *addrlen);
mov rax, 43         # sys_accept
mov rdi, sockfd     # socket descriptor
mov rsi, addr       # client address
mov rdx, addrlen    # address length
syscall

# TIME AND SLEEP
# --------------

# sys_nanosleep (35) - Sleep with nanosecond precision
# int nanosleep(const struct timespec *req, struct timespec *rem);
mov rax, 35         # sys_nanosleep
mov rdi, timespec   # requested time
mov rsi, 0          # remaining time (NULL)
syscall

# sys_time (201) - Get current time
# time_t time(time_t *tloc);
mov rax, 201        # sys_time
mov rdi, 0          # NULL (return only)
syscall

# sys_gettimeofday (96) - Get time with microsecond precision
# int gettimeofday(struct timeval *tv, struct timezone *tz);
mov rax, 96         # sys_gettimeofday
mov rdi, timeval    # time structure
mov rsi, 0          # timezone (NULL)
syscall

# ========================================================================
# FILE DESCRIPTOR OPERATIONS
# ========================================================================

# sys_dup (32) - Duplicate file descriptor
# int dup(int oldfd);
mov rax, 32         # sys_dup
mov rdi, oldfd      # file descriptor to duplicate
syscall

# sys_dup2 (33) - Duplicate to specific descriptor
# int dup2(int oldfd, int newfd);
mov rax, 33         # sys_dup2
mov rdi, oldfd      # source descriptor
mov rsi, newfd      # target descriptor
syscall

# sys_pipe (22) - Create pipe
# int pipe(int pipefd[2]);
mov rax, 22         # sys_pipe
mov rdi, pipefd     # array for file descriptors
syscall

# ========================================================================
# ADVANCED SYSTEM CALLS
# ========================================================================

# sys_clone (56) - Create process/thread
# long clone(unsigned long flags, void *child_stack, int *ptid, int *ctid, unsigned long newtls);
mov rax, 56         # sys_clone
mov rdi, flags      # clone flags
mov rsi, stack      # child stack
mov rdx, ptid       # parent tid
mov r10, ctid       # child tid
mov r8, newtls      # new TLS
syscall

# sys_futex (202) - Fast userspace mutex
# long futex(int *uaddr, int futex_op, int val, const struct timespec *timeout, int *uaddr2, int val3);
mov rax, 202        # sys_futex
mov rdi, uaddr      # futex address
mov rsi, op         # futex operation
mov rdx, val        # value
mov r10, timeout    # timeout
mov r8, uaddr2      # second futex
mov r9, val3        # third value
syscall

# sys_epoll_create (213) - Create epoll instance
# int epoll_create(int size);
mov rax, 213        # sys_epoll_create
mov rdi, 10         # size hint
syscall

# sys_epoll_ctl (233) - Control epoll instance
# int epoll_ctl(int epfd, int op, int fd, struct epoll_event *event);
mov rax, 233        # sys_epoll_ctl
mov rdi, epfd       # epoll descriptor
mov rsi, op         # operation
mov rdx, fd         # file descriptor
mov r10, event      # event structure
syscall

# ========================================================================
# ERROR HANDLING
# ========================================================================

# SYSTEM CALL ERROR CODES:
# • Success: RAX >= 0
# • Error: RAX = -errno (negative error code)

# COMMON ERROR CODES:
-EPERM     # -1   Operation not permitted
-ENOENT    # -2   No such file or directory  
-ESRCH     # -3   No such process
-EINTR     # -4   Interrupted system call
-EIO       # -5   I/O error
-ENXIO     # -6   No such device or address
-E2BIG     # -7   Argument list too long
-ENOEXEC   # -8   Exec format error
-EBADF     # -9   Bad file number
-ECHILD    # -10  No child processes
-EAGAIN    # -11  Try again
-ENOMEM    # -12  Out of memory
-EACCES    # -13  Permission denied
-EFAULT    # -14  Bad address
-EBUSY     # -16  Device or resource busy
-EEXIST    # -17  File exists
-ENOTDIR   # -20  Not a directory
-EISDIR    # -21  Is a directory
-EINVAL    # -22  Invalid argument
-EMFILE    # -24  Too many open files
-ENOSPC    # -28  No space left on device
-EPIPE     # -32  Broken pipe

# ERROR CHECKING EXAMPLE:
mov rax, 1          # sys_write
mov rdi, 1          # stdout
mov rsi, msg        # message
mov rdx, len        # length
syscall
cmp rax, 0          # check for error
jl  error_handler   # jump if negative (error)

# ========================================================================
# CANNABIS-ENHANCED SYSCALL PATTERNS
# ========================================================================

# SATIVA-ENHANCED CREATIVE SYSCALL USAGE:
# • Innovative combinations of syscalls for unique functionality
# • Creative use of memory mapping and process control
# • Rapid prototyping with direct kernel interfaces

# INDICA-ENHANCED METHODICAL SYSCALL PROGRAMMING:
# • Careful error handling and resource management
# • Patient debugging of syscall interactions
# • Deep understanding of kernel behavior

# HYBRID-ENHANCED BALANCED SYSCALL APPROACH:
# • Optimal performance with robust error handling
# • Clean abstractions over raw syscall interfaces
# • Maintainable system programming practices

# ========================================================================
# SYSCALL PROGRAMMING EXAMPLES
# ========================================================================

# HELLO WORLD USING SYSCALLS:
.section .data
msg:     .ascii "Hello, Cannabis World!\n"
msg_len = . - msg

.section .text
.global _start
_start:
    # write(1, msg, msg_len)
    mov rax, 1          # sys_write
    mov rdi, 1          # stdout
    mov rsi, msg        # message
    mov rdx, msg_len    # length
    syscall
    
    # exit(0)
    mov rax, 60         # sys_exit
    mov rdi, 0          # status
    syscall

# READ FILE EXAMPLE:
.section .bss
buffer:  .space 1024

.section .text
read_file:
    # open("filename", O_RDONLY)
    mov rax, 2          # sys_open
    mov rdi, filename   # filename
    mov rsi, 0          # O_RDONLY
    mov rdx, 0          # mode (unused for read)
    syscall
    mov r8, rax         # save file descriptor
    
    # read(fd, buffer, 1024)
    mov rax, 0          # sys_read
    mov rdi, r8         # file descriptor
    mov rsi, buffer     # buffer
    mov rdx, 1024       # count
    syscall
    
    # close(fd)
    mov rax, 3          # sys_close
    mov rdi, r8         # file descriptor
    syscall
    ret

# ========================================================================
# SYSCALL DEBUGGING AND TOOLS
# ========================================================================

# STRACE - TRACE SYSTEM CALLS:
# strace ./program                    # Trace all syscalls
# strace -e trace=write ./program     # Trace only write syscalls
# strace -c ./program                 # Count syscalls
# strace -f ./program                 # Follow forks

# GDB SYSCALL DEBUGGING:
# catch syscall                       # Break on all syscalls
# catch syscall write                 # Break on write syscalls
# info registers                      # Show register state

# PERFORMANCE ANALYSIS:
# perf trace ./program                # Performance tracing
# ltrace ./program                    # Library call tracing

# ========================================================================
# SYSCALL REFERENCE TABLES
# ========================================================================

# COMMON SYSCALL NUMBERS (X86-64):
0   read        22  pipe        57  fork        96  gettimeofday
1   write       32  dup         59  execve      110 getppid
2   open        33  dup2        60  exit        201 time
3   close       35  nanosleep   61  wait4       202 futex
4   stat        37  alarm       62  kill        213 epoll_create
5   fstat       39  getpid      79  getcwd      233 epoll_ctl
8   lseek       41  socket      80  chdir       257 openat
9   mmap        43  accept      83  mkdir
11  munmap      49  bind        84  rmdir
12  brk         50  listen      

# OPEN FLAGS:
O_RDONLY    = 0x00000000    # Read only
O_WRONLY    = 0x00000001    # Write only  
O_RDWR      = 0x00000002    # Read/write
O_CREAT     = 0x00000040    # Create if doesn't exist
O_EXCL      = 0x00000080    # Fail if exists
O_TRUNC     = 0x00000200    # Truncate to 0 bytes
O_APPEND    = 0x00000400    # Append mode
O_NONBLOCK  = 0x00000800    # Non-blocking I/O

# MMAP PROTECTION FLAGS:
PROT_NONE   = 0x0           # No access
PROT_READ   = 0x1           # Read access
PROT_WRITE  = 0x2           # Write access  
PROT_EXEC   = 0x4           # Execute access

# MMAP FLAGS:
MAP_PRIVATE     = 0x02      # Private copy-on-write
MAP_SHARED      = 0x01      # Shared mapping
MAP_ANONYMOUS   = 0x20      # Anonymous mapping (no file)
MAP_FIXED       = 0x10      # Fixed address mapping

# SIGNALS:
SIGHUP      = 1             # Hangup
SIGINT      = 2             # Interrupt (Ctrl+C)
SIGQUIT     = 3             # Quit (Ctrl+\)
SIGKILL     = 9             # Kill (uncatchable)
SIGTERM     = 15            # Terminate
SIGSTOP     = 19            # Stop (uncatchable)
SIGCONT     = 18            # Continue

# ========================================================================
# PLANT SPIRIT SYSCALL WISDOM
# ========================================================================

# Randy's Cannabis-Enhanced Syscall Philosophy:
# "System calls are the prayer language of userspace - the sacred
# invocations through which applications request services from the
# kernel deity. Each syscall number is a mantra that opens specific
# channels of computational consciousness. Mastering syscalls means
# speaking directly with the operating system's soul."

# Educational Syscall Mastery Path:
# 1. Understand basic file operations (read/write/open/close)
# 2. Learn process management (fork/exec/wait/exit)  
# 3. Master memory management (mmap/munmap/brk)
# 4. Explore networking syscalls (socket/bind/listen/accept)
# 5. Study advanced features (epoll/futex/clone)
# 6. Practice error handling and debugging techniques

# Cannabis-Enhanced Syscall Consciousness:
# • Sativa: Creative exploration of syscall combinations
# • Indica: Deep understanding of kernel interfaces  
# • Hybrid: Balanced systems programming with syscall mastery

# The mycelial network of syscalls connects userspace to kernel wisdom!
# Plant spirits guide direct operating system communication! 🌿⚡🧠