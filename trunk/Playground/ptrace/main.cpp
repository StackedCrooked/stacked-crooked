#include <sys/ptrace.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <sys/user.h>
#include <iostream>
#include <cstddef>
#include <asm/unistd.h>


int main()
{
    std::cout << "GetAXOffset: " << offsetof(user_regs_struct, orig_rax) << std::endl;

    pid_t child;
    long orig_eax;
    child = fork();
    if(child == 0)
    {
        ptrace(PTRACE_TRACEME, 0, NULL, NULL);
        execl("/bin/ls", "ls", NULL);
    }
    else {

        wait(NULL);
        orig_eax = ptrace(PTRACE_PEEKUSER, child, offsetof(user_regs_struct, orig_rax), NULL);
        std::cout << "The child made a system call: " << orig_eax << std::endl;
        ptrace(PTRACE_CONT, child, NULL, NULL);
    }
    return 0;
}
