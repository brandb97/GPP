/* make g++ happy */
.section .note.GNU-stack,"",@progbits

.section .text
    .global _start
    /* _start need to call __libc_start_main()
     * _libc_start_main arguments:
     * - int main(int argc, char **argv, char **envp) -> in %rdi (1st arg)
     * - int argc -> in %rsi (2nd arg)
     * - char **argv -> in %rdx (3rd arg)
     * - void (*init)(void) -> in %rcx (4th arg), null in glibc
     *  - void (*fini)(void) -> in %r8 (5th arg), null in glibc
     *  - void (*rtld_fini)(void) -> in %r9 (6th arg)
     *  - char *stack_end -> in stack (7th arg)
     */
_start:
    endbr64
    xor %ebp, %ebp   /* clear %ebp */
    mov %rdx, %r9    /* move fini fn to %r9 (6th arg) */
    pop %rsi         /* pop argc to %rsi (2nd arg) */
    mov %rsp, %rdx   /* move rsp to rdx (3rd arg) */
    and $0xfffffffffffffff0, %rsp /* align stack to 16 bytes */
    push %rax        /* I don't know why, but glibc does it */
    push %rsp        /* push 7th arg (stack_end) */
    xor %r8d, %r8d   /* clear r8 (5th arg) */
    xor %ecx, %ecx   /* clear rcx (4th arg) */
    lea sched(%rip), %rdi /* load address of main to rdi (1st arg) */
    call __libc_start_main /* call __libc_start_main, which does not return */
    hlt              /* halt the CPU */
