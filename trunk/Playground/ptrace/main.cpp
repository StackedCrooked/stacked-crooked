#include <sys/prctl.h>
#include <sys/ptrace.h>
#include <sys/syscall.h>
#include <sys/types.h>
#include <sys/user.h>
#include <sys/wait.h>
#include <cstddef>
#include <stdio.h>
#include <unistd.h>


bool is_banned(unsigned call)
{
    static const unsigned banned_syscalls[] =
    {
        __NR_fork,
        __NR_mknod,
        __NR_chdir,
        __NR_chmod,
        __NR_ptrace,
        __NR_kill,
        __NR_rename,
        __NR_mkdir,
        __NR_rmdir,
        __NR_dup,
        __NR_pipe,
        __NR_setgid,
        __NR_getgid,
        // __NR_signal, // 32-bit
        __NR_geteuid,
        __NR_getegid,
        __NR_umask,
        __NR_chroot,
        __NR_dup2,
        // __NR_sigsuspend, // 32-bit
        __NR_symlink,
        __NR_reboot,
        __NR_clone,
        // __NR_socketcall, // 32-bit
        __NR_chown,
        __NR_getcwd,
        __NR_vfork
    };

    for (unsigned i = 0; i != sizeof(banned_syscalls) / sizeof(banned_syscalls[0]); ++i)
    {
        if (banned_syscalls[i] == call)
        {
            return true;
        }
    }
    return false;
}


unsigned GetAXOffset()
{
    return offsetof(user_regs_struct, orig_rax);
}

int main(int argc,char ** argv)
{
    int insyscall=0;
    if (argc!=2)
    {
        fprintf(stderr,"Usage: %s <prog name> ",argv[0]);
        return 1;
    }

    int amp;
    int pid = fork();

    if (!pid)
    {
        prctl(PR_SET_PDEATHSIG, SIGKILL);
        ptrace(PTRACE_TRACEME, 0, 0, 0);
        execlp(argv[1],argv[1], NULL);
    }
    else
    {
        ptrace(PTRACE_SETOPTIONS, pid, NULL, PTRACE_SYSCALL | PTRACE_O_TRACEFORK) ;
        while (1)
        {
            printf("Waiting\n");
            pid = wait(&amp);

            printf("Waited %d\n", amp);

            if (WIFEXITED(amp))
            {
                break;
            }


            if (WSTOPSIG(amp) == SIGTRAP)
            {
                int event = (amp >> 16) & 0xffff;
                if (event ==  PTRACE_EVENT_FORK)
                {
                    printf("fork caught\n");
                    pid_t newpid;
                    ptrace(PTRACE_GETEVENTMSG, pid, NULL, (long) &newpid);
                    kill(newpid, SIGKILL);
                    kill(pid, SIGKILL);
                    break;
                }
            }
            if (insyscall == 0)
            {
                struct user_regs_struct regs;
                ptrace(PTRACE_GETREGS, pid, 0, &regs);
                if (is_banned(regs.orig_rax))
                {
                    kill(pid, SIGKILL);
                    printf("%d killed due to illegal system call\n", pid);

                    return 1;
                }

                insyscall = 1;
            }
            else
            {
                insyscall = 0;
            }
            ptrace(PTRACE_CONT, pid, NULL, 0);
        }
    }
    return 0;
}
