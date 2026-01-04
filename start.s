/* make g++ happy */
.section .note.GNU-stack,"",@progbits

.section .data
    .align 8
    .global __dso_handle
__dso_handle:
    .quad 0

.section .text
    .global _start
    .global mcall
    .global mstart_stub
    .global systemstack
    .global getCallerSP
    .global getCallerPC
    .global getCallerBP
    .global gogo
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
    /* I don't know why, but glibc does it */
    endbr64

    mov %rdx, %r9    /* move fini fn to %r9 (6th arg of __libc_start_main) */
    pop %rsi         /* pop argc to %rsi (2nd arg of __libc_start_main) */
    mov %rsp, %rdx   /* move rsp to rdx (3rd arg of __libc_start_main) */
    and $0xfffffffffffffff0, %rsp /* align stack to 16 bytes */
    push %rax        /* align on 16 bytes (we must have four 8-byte pushes) */
    /* save %r9, %rdx, %rsi */
    push %r9
    push %rdx
    push %rsi

    /* Initialize m0 and g0 */
    lea g0(%rip), %rdi /* load address of g0 to rdi (1st arg) */
    call setg          /* call setg(g0) to set g0 as current GPP in tls */
    lea m0(%rip), %rsi /* load address of m0 to rsi */
    mov %rsi, 0(%rdi)  /* g0->m = m0 */
    mov %rdi, 0(%rsi)  /* m0->g0 = g0 */

    /* Call schedinit() to initialize scheduler */
    call schedinit

    /* Call __libc_start_main() to initialize glibc, global variables
     * and start the program */
    pop %rsi         /* restore argc to %rsi (2nd arg) */
    pop %rdx         /* restore argv to %rdx (3rd arg) */
    pop %r9          /* restore r9 (6th arg, rtld_fini of __libc_start_main) */
    xor %ebp, %ebp   /* clear %ebp */
    push %rsp        /* push 7th arg (stack_end) */
    xor %r8d, %r8d   /* clear r8 (5th arg) */
    xor %ecx, %ecx   /* clear rcx (4th arg) */
    lea mstart_main(%rip), %rdi /* load address of main to rdi (1st arg) */
    call __libc_start_main /* call __libc_start_main, which does not return */
    hlt              /* halt the CPU */

getCallerSP:
    lea 16(%rbp), %rax
    ret

getCallerPC:
    mov 8(%rbp), %rax
    ret

getCallerBP:
    mov (%rbp), %rax
    ret

/* mstart_stub(M *m) */
mstart_stub:
    push %rbp
    mov %rsp, %rbp
    /* setg to g0 */
    mov 0(%rdi), %rdi /* g0 = m->g0 */
    call setg
    call mstart
    mov %rbp, %rsp
    pop %rbp
    ret

/* void mcall(void (*fn)(void))
 *
 * It's possible that a dead gpp routine
 * calls mcall to switch to another gpp.
 * we need to check whether getg() returns NULL
 * in this case.
 */
mcall:
    /* save callee-saved registers */
    push %rbx
    push %r12
    push %r13
    push %r14
    push %r15
    /* save pc/bp/sp to current g->sched */
    call getg
    lea .restore(%rip), %rcx
    mov %rcx, 24(%rax)
    mov %rbp, 16(%rax)
    mov %rsp, 8(%rax)
    /* switch to g0's stack */
    mov (%rax), %rcx /* m = g->m */
    mov 0(%rcx), %rdx /* g0 = m->g0 */
    mov 8(%rdx), %rsp /* rsp = g0->sched.sp */
    mov 16(%rdx), %rbp /* rbp = g0->sched.bp */
    /* setg to g0 */
    push %rdi
    mov %rdx, %rdi
    call setg
    pop %rdi
    /* call fn, fn should never return */
    call *%rdi
.restore:
    /* restore callee-saved registers */
    pop %r15
    pop %r14
    pop %r13
    pop %r12
    pop %rbx
    ret

/* void gogo(GPP *g) */
gogo:
    call setg
    mov 8(%rdi), %rsp   /* rsp = g->sched.sp */
    mov 16(%rdi), %rbp  /* rbp = g->sched.bp */
    mov 24(%rdi), %rcx  /* rcx = g->sched.pc */
    jmp *%rcx           /* jump to pc */

/* systemstack(void fn(void *arg), void *arg) */
systemstack:
    push %rbp
    mov %rsp, %rbp

    /* save sp/bp to g->sched */
    call getg
    mov %rsp, 8(%rax)    /* g->sched.sp = rsp */
    mov %rbp, 16(%rax)   /* g->sched.bp = rbp */
    /* switch to g0's stack */
    mov (%rax), %rcx     /* m = g->m */
    mov 0(%rcx), %rdx    /* g0 = m->g0 */
    mov 8(%rdx), %rsp    /* rsp = g0->sched.sp */
    mov 16(%rdx), %rbp   /* rbp = g0->sched.bp */
    /* setg to g0 */
    mov %rdi, %rcx
    mov %rdx, %rdi
    call setg
    /* call fn(arg) */
    mov %rsi, %rdi
    call *%rcx
    /* restore g->sched to sp/bp */
    call getg
    mov (%rax), %rcx     /* m = g0->m */
    mov 8(%rcx), %rcx    /* g = m->curg */
    mov 8(%rcx), %rsp    /* rsp = g->sched.sp */
    mov 16(%rcx), %rbp   /* rbp = g->sched.bp */
    /* setg to g */
    mov %rcx, %rdi
    call setg

    mov %rbp, %rsp
    pop %rbp
    ret
