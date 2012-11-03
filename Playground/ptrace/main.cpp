#include <iostream>
#include <string>
#include <cassert>
#include <asm/unistd.h>

unsigned banned_syscalls[] = {
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

int main()
{
}

