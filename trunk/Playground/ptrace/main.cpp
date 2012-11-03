#include <sys/ptrace.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <sys/user.h>
#include <sys/syscall.h>
#include <stddef.h>
#include <stdio.h>
#include <iostream>


//  unsigned long int rax;
//  unsigned long int rcx;
//  unsigned long int rdx;
//  unsigned long int rsi;
//  unsigned long int rdi;
//  unsigned long int orig_rax;
//#define ARGUMENT_0(x)		(ISLINUX32(x) ? (x)->rbx : (x)->rdi)
//#define ARGUMENT_1(x)		(ISLINUX32(x) ? (x)->rcx : (x)->rsi)
//#define ARGUMENT_2(x)		(ISLINUX32(x) ? (x)->rdx : (x)->rdx)
//#define ARGUMENT_3(x)		(ISLINUX32(x) ? (x)->rsi : (x)->rcx)
//#define ARGUMENT_4(x)		(ISLINUX32(x) ? (x)->rdi : (x)->r8)
//#define ARGUMENT_5(x)		(ISLINUX32(x) ? (x)->rbp : (x)->r9)

enum
{
    offset_orig_rax = offsetof(user_regs_struct, orig_rax),
    offset_arg0 = offsetof(user_regs_struct, rdi),
    offset_arg1 = offsetof(user_regs_struct, rsi),
    offset_arg2 = offsetof(user_regs_struct, rdi),
    offset_arg3 = offsetof(user_regs_struct, rcx)
};


int main(int, char ** argv)
{

    pid_t child;
    long orig_eax, rax;
    long params[3];
    int status;
    int insyscall = 0;
    child = fork();
    if (child == 0)
    {
        ptrace(PTRACE_TRACEME, 0, NULL, NULL);
        execl(argv[1], argv[1], NULL);
    }
    else
    {
        while (1)
        {
            wait(&status);
            if (WIFEXITED(status))
            {
                break;
            }
            orig_eax = ptrace(PTRACE_PEEKUSER, child, offset_orig_rax, NULL);
            if (orig_eax == SYS_write)
            {
                if (insyscall == 0)
                {
                    /* Syscall entry */
                    insyscall = 1;
                    params[0] = ptrace(PTRACE_PEEKUSER, child, offset_arg0, NULL);
                    params[1] = ptrace(PTRACE_PEEKUSER, child, offset_arg1, NULL);
                    params[2] = ptrace(PTRACE_PEEKUSER, child, offset_arg2, NULL);
                    std::cout << "Write called with " << params[0] << ", " << params[1] << ", " << params[2] << std::endl;
                }
                else   /* Syscall exit */
                {
                    rax = ptrace(PTRACE_PEEKUSER, child, offset_orig_rax, NULL);
                    std::cout << "Write returned " << rax << std::endl;
                    insyscall = 0;
                }
            }
            ptrace(PTRACE_SYSCALL,
                   child, NULL, NULL);
        }
    }
    return 0;
}
