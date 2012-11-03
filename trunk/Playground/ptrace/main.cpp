#include <sys/ptrace.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <sys/user.h>
#include <sys/syscall.h>
#include <stddef.h>
#include <stdio.h>
#include <iostream>
#include <cassert>
#include <string>
#include <stdexcept>


//#define ISLINUX32(x)		(linux_call_type((x)->cs) == LINUX32)
//#define SYSCALL_NUM(x)		(x)->orig_rax
//#define SET_RETURN_CODE(x, v)	(x)->rax = (v)
//#define RETURN_CODE(x)		(ISLINUX32(x) ? (long)(int)(x)->rax : (x)->rax)
//#define ARGUMENT_0(x)		(ISLINUX32(x) ? (x)->rbx : (x)->rdi)
//#define ARGUMENT_1(x)		(ISLINUX32(x) ? (x)->rcx : (x)->rsi)
//#define ARGUMENT_2(x)		(ISLINUX32(x) ? (x)->rdx : (x)->rdx)
//#define ARGUMENT_3(x)		(ISLINUX32(x) ? (x)->rsi : (x)->rcx)
//#define ARGUMENT_4(x)		(ISLINUX32(x) ? (x)->rdi : (x)->r8)
//#define ARGUMENT_5(x)		(ISLINUX32(x) ? (x)->rbp : (x)->r9)
//#define SET_ARGUMENT_0(x, v)	if (ISLINUX32(x)) (x)->rbx = (v); else (x)->rdi = (v)
//#define SET_ARGUMENT_1(x, v)	if (ISLINUX32(x)) (x)->rcx = (v); else (x)->rsi = (v)
//#define SET_ARGUMENT_2(x, v)	if (ISLINUX32(x)) (x)->rdx = (v); else (x)->rdx = (v)
//#define SET_ARGUMENT_3(x, v)	if (ISLINUX32(x)) (x)->rsi = (v); else (x)->rcx = (v)
//#define SET_ARGUMENT_4(x, v)	if (ISLINUX32(x)) (x)->rdi = (v); else (x)->r8 = (v)
//#define SET_ARGUMENT_5(x, v)	if (ISLINUX32(x)) (x)->rbp = (v); else (x)->r9 = (v)


enum
{
    offset_orig_rax = offsetof(user_regs_struct, orig_rax),
    offset_arg0 = offsetof(user_regs_struct, rdi),
    offset_arg1 = offsetof(user_regs_struct, rsi),
    offset_arg2 = offsetof(user_regs_struct, rdi),
    offset_arg3 = offsetof(user_regs_struct, rcx),
    offset_ret = offsetof(user_regs_struct, rax)
};

const char *linux_syscallnames_64[] = {
    "read", /* 0 */
    "write", /* 1 */
    "open", /* 2 */
    "close", /* 3 */
    "stat", /* 4 */
    "fstat", /* 5 */
    "lstat", /* 6 */
    "poll", /* 7 */
    "lseek", /* 8 */
    "mmap", /* 9 */
    "mprotect", /* 10 */
    "munmap", /* 11 */
    "brk", /* 12 */
    "rt_sigaction", /* 13 */
    "rt_sigprocmask", /* 14 */
    "rt_sigreturn", /* 15 */
    "ioctl", /* 16 */
    "pread64", /* 17 */
    "pwrite64", /* 18 */
    "readv", /* 19 */
    "writev", /* 20 */
    "access", /* 21 */
    "pipe", /* 22 */
    "select", /* 23 */
    "sched_yield", /* 24 */
    "mremap", /* 25 */
    "msync", /* 26 */
    "mincore", /* 27 */
    "madvise", /* 28 */
    "shmget", /* 29 */
    "shmat", /* 30 */
    "shmctl", /* 31 */
    "dup", /* 32 */
    "dup2", /* 33 */
    "pause", /* 34 */
    "nanosleep", /* 35 */
    "getitimer", /* 36 */
    "alarm", /* 37 */
    "setitimer", /* 38 */
    "getpid", /* 39 */
    "sendfile", /* 40 */
    "socket", /* 41 */
    "connect", /* 42 */
    "accept", /* 43 */
    "sendto", /* 44 */
    "recvfrom", /* 45 */
    "sendmsg", /* 46 */
    "recvmsg", /* 47 */
    "shutdown", /* 48 */
    "bind", /* 49 */
    "listen", /* 50 */
    "getsockname", /* 51 */
    "getpeername", /* 52 */
    "socketpair", /* 53 */
    "setsockopt", /* 54 */
    "getsockopt", /* 55 */
    "clone", /* 56 */
    "fork", /* 57 */
    "vfork", /* 58 */
    "execve", /* 59 */
    "exit", /* 60 */
    "wait4", /* 61 */
    "kill", /* 62 */
    "uname", /* 63 */
    "semget", /* 64 */
    "semop", /* 65 */
    "semctl", /* 66 */
    "shmdt", /* 67 */
    "msgget", /* 68 */
    "msgsnd", /* 69 */
    "msgrcv", /* 70 */
    "msgctl", /* 71 */
    "fcntl", /* 72 */
    "flock", /* 73 */
    "fsync", /* 74 */
    "fdatasync", /* 75 */
    "truncate", /* 76 */
    "ftruncate", /* 77 */
    "getdents", /* 78 */
    "getcwd", /* 79 */
    "chdir", /* 80 */
    "fchdir", /* 81 */
    "rename", /* 82 */
    "mkdir", /* 83 */
    "rmdir", /* 84 */
    "creat", /* 85 */
    "link", /* 86 */
    "unlink", /* 87 */
    "symlink", /* 88 */
    "readlink", /* 89 */
    "chmod", /* 90 */
    "fchmod", /* 91 */
    "chown", /* 92 */
    "fchown", /* 93 */
    "lchown", /* 94 */
    "umask", /* 95 */
    "gettimeofday", /* 96 */
    "getrlimit", /* 97 */
    "getrusage", /* 98 */
    "sysinfo", /* 99 */
    "times", /* 100 */
    "ptrace", /* 101 */
    "getuid", /* 102 */
    "syslog", /* 103 */
    "getgid", /* 104 */
    "setuid", /* 105 */
    "setgid", /* 106 */
    "geteuid", /* 107 */
    "getegid", /* 108 */
    "setpgid", /* 109 */
    "getppid", /* 110 */
    "getpgrp", /* 111 */
    "setsid", /* 112 */
    "setreuid", /* 113 */
    "setregid", /* 114 */
    "getgroups", /* 115 */
    "setgroups", /* 116 */
    "setresuid", /* 117 */
    "getresuid", /* 118 */
    "setresgid", /* 119 */
    "getresgid", /* 120 */
    "getpgid", /* 121 */
    "setfsuid", /* 122 */
    "setfsgid", /* 123 */
    "getsid", /* 124 */
    "capget", /* 125 */
    "capset", /* 126 */
    "rt_sigpending", /* 127 */
    "rt_sigtimedwait", /* 128 */
    "rt_sigqueueinfo", /* 129 */
    "rt_sigsuspend", /* 130 */
    "sigaltstack", /* 131 */
    "utime", /* 132 */
    "mknod", /* 133 */
    "uselib", /* 134 */
    "personality", /* 135 */
    "ustat", /* 136 */
    "statfs", /* 137 */
    "fstatfs", /* 138 */
    "sysfs", /* 139 */
    "getpriority", /* 140 */
    "setpriority", /* 141 */
    "sched_setparam", /* 142 */
    "sched_getparam", /* 143 */
    "sched_setscheduler", /* 144 */
    "sched_getscheduler", /* 145 */
    "sched_get_priority_max", /* 146 */
    "sched_get_priority_min", /* 147 */
    "sched_rr_get_interval", /* 148 */
    "mlock", /* 149 */
    "munlock", /* 150 */
    "mlockall", /* 151 */
    "munlockall", /* 152 */
    "vhangup", /* 153 */
    "modify_ldt", /* 154 */
    "pivot_root", /* 155 */
    "_sysctl", /* 156 */
    "prctl", /* 157 */
    "arch_prctl", /* 158 */
    "adjtimex", /* 159 */
    "setrlimit", /* 160 */
    "chroot", /* 161 */
    "sync", /* 162 */
    "acct", /* 163 */
    "settimeofday", /* 164 */
    "mount", /* 165 */
    "umount2", /* 166 */
    "swapon", /* 167 */
    "swapoff", /* 168 */
    "reboot", /* 169 */
    "sethostname", /* 170 */
    "setdomainname", /* 171 */
    "iopl", /* 172 */
    "ioperm", /* 173 */
    "create_module", /* 174 */
    "init_module", /* 175 */
    "delete_module", /* 176 */
    "get_kernel_syms", /* 177 */
    "query_module", /* 178 */
    "quotactl", /* 179 */
    "nfsservctl", /* 180 */
    "getpmsg", /* 181 */
    "putpmsg", /* 182 */
    "afs_syscall", /* 183 */
    "tuxcall", /* 184 */
    "security", /* 185 */
    "gettid", /* 186 */
    "readahead", /* 187 */
    "setxattr", /* 188 */
    "lsetxattr", /* 189 */
    "fsetxattr", /* 190 */
    "getxattr", /* 191 */
    "lgetxattr", /* 192 */
    "fgetxattr", /* 193 */
    "listxattr", /* 194 */
    "llistxattr", /* 195 */
    "flistxattr", /* 196 */
    "removexattr", /* 197 */
    "lremovexattr", /* 198 */
    "fremovexattr", /* 199 */
    "tkill", /* 200 */
    "time", /* 201 */
    "futex", /* 202 */
    "sched_setaffinity", /* 203 */
    "sched_getaffinity", /* 204 */
    "set_thread_area", /* 205 */
    "io_setup", /* 206 */
    "io_destroy", /* 207 */
    "io_getevents", /* 208 */
    "io_submit", /* 209 */
    "io_cancel", /* 210 */
    "get_thread_area", /* 211 */
    "lookup_dcookie", /* 212 */
    "epoll_create", /* 213 */
    "epoll_ctl_old", /* 214 */
    "epoll_wait_old", /* 215 */
    "remap_file_pages", /* 216 */
    "getdents64", /* 217 */
    "set_tid_address", /* 218 */
    "restart_syscall", /* 219 */
    "semtimedop", /* 220 */
    "fadvise64", /* 221 */
    "timer_create", /* 222 */
    "timer_settime", /* 223 */
    "timer_gettime", /* 224 */
    "timer_getoverrun", /* 225 */
    "timer_delete", /* 226 */
    "clock_settime", /* 227 */
    "clock_gettime", /* 228 */
    "clock_getres", /* 229 */
    "clock_nanosleep", /* 230 */
    "exit_group", /* 231 */
    "epoll_wait", /* 232 */
    "epoll_ctl", /* 233 */
    "tgkill", /* 234 */
    "utimes", /* 235 */
    "vserver", /* 236 */
    "mbind", /* 237 */
    "set_mempolicy", /* 238 */
    "get_mempolicy", /* 239 */
    "mq_open", /* 240 */
    "mq_unlink", /* 241 */
    "mq_timedsend", /* 242 */
    "mq_timedreceive", /* 243 */
    "mq_notify", /* 244 */
    "mq_getsetattr", /* 245 */
    "kexec_load", /* 246 */
    "waitid", /* 247 */
    "add_key", /* 248 */
    "request_key", /* 249 */
    "keyctl", /* 250 */
    "ioprio_set", /* 251 */
    "ioprio_get", /* 252 */
    "inotify_init", /* 253 */
    "inotify_add_watch", /* 254 */
    "inotify_rm_watch", /* 255 */
    "migrate_pages", /* 256 */
    "openat", /* 257 */
    "mkdirat", /* 258 */
    "mknodat", /* 259 */
    "fchownat", /* 260 */
    "futimesat", /* 261 */
    "newfstatat", /* 262 */
    "unlinkat", /* 263 */
    "renameat", /* 264 */
    "linkat", /* 265 */
    "symlinkat", /* 266 */
    "readlinkat", /* 267 */
    "fchmodat", /* 268 */
    "faccessat", /* 269 */
    "pselect6", /* 270 */
    "ppoll", /* 271 */
    "unshare", /* 272 */
    "set_robust_list", /* 273 */
    "get_robust_list", /* 274 */
    "splice", /* 275 */
    "tee", /* 276 */
    "sync_file_range", /* 277 */
    "vmsplice", /* 278 */
    "move_pages", /* 279 */
    "utimensat", /* 280 */
    "ORE_getcpu", /* 0 */
    "epoll_pwait", /* 281 */
    "signalfd", /* 282 */
    "timerfd_create", /* 283 */
    "eventfd", /* 284 */
    "fallocate", /* 285 */
    "timerfd_settime", /* 286 */
    "timerfd_gettime", /* 287 */
    "paccept", /* 288 */
    "signalfd4", /* 289 */
    "eventfd2", /* 290 */
    "epoll_create1", /* 291 */
    "dup3", /* 292 */
    "pipe2", /* 293 */
    "inotify_init1", /* 294 */
    NULL
};

#define SET_RETURN_CODE(x, v)	(x)->rax = (v)


struct ptrace_exception : std::runtime_error
{
    ptrace_exception(const char * oper, int err) :
        std::runtime_error(oper),
        _err(err)
    {
    }

    ~ptrace_exception() throw() {}

    int get_err() const { return _err; }

    int _err;
};

#define THROW_PTRACE_EXCEPTION(oper, errno) \
    throw ptrace_exception(#oper, errno);

static void
linux_write_returncode(pid_t pid, struct user_regs_struct* regs, long code)
{
    SET_RETURN_CODE(regs, code);
    int res = ptrace(PTRACE_SETREGS, pid, NULL, regs);
    THROW_PTRACE_EXCEPTION(PTRACE_SETREGS, res);
}



int main(int, char ** argv)
{
    pid_t child = fork();
    if (child == 0)
    {
        ptrace(PTRACE_TRACEME, 0, NULL, NULL);
        execl(argv[1], argv[1], NULL);
    }
    else
    {
        int insyscall = 0;
        while (1)
        {
            pid_t status;
            wait(&status);
            if (WIFEXITED(status))
            {
                break;
            }

            long orig_eax = ptrace(PTRACE_PEEKUSER, child, offset_orig_rax, NULL);
            if (orig_eax == SYS_execve)
            {
                static bool first_execve = true;
                if (first_execve)
                {
                    ptrace(PTRACE_SYSCALL, child, NULL, NULL);
                    first_execve = false;
                    continue;
                }
                break;
            }

            if (insyscall == 0)
            {
                std::cout << "<" << linux_syscallnames_64[orig_eax] << ">" << std::endl;
                insyscall = 1;
                switch (orig_eax)
                {
                    case SYS_read:
                    case SYS_write:
                    {
                        break;
                    }
                    default:
                    {
                        struct user_regs_struct regs;
                        int res = ptrace(PTRACE_GETREGS, child, NULL, &regs);
                        assert(res == 0);
                        linux_write_returncode(child, &regs, 0);
                        break;
                    }
                }
            }
            else
            {
                struct user_regs_struct regs;
                int res = ptrace(PTRACE_GETREGS, child, NULL, &regs);
                assert(res == 0);
                std::cout << "Return: " << regs.rax << std::endl;
                std::cout << "</" << linux_syscallnames_64[orig_eax] << ">" << std::endl;
                insyscall = 0;
            }
            ptrace(PTRACE_SYSCALL, child, NULL, NULL);
        }
    }
    return 0;
}
