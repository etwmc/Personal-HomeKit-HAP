#ifndef POLY1305_DEFINES_INC
#define POLY1305_DEFINES_INC

#define IS_LINUX_ELF (defined(__linux__) && defined(__ELF__))
#define IS_WIN64 (defined(_WIN64) || defined(__CYGWIN64__))

/* global */
#define SYSGLOBAL(n) .globl n; .globl _##n
#define GLOBAL3(n,s) .globl n##_##s; .globl _##n##_##s
#define GLOBAL2(n,s) GLOBAL3(n,s)
#define GLOBAL(n) GLOBAL2(n,POLY1305_SUFFIX)

/* function start */
#define SYSFN(n) .p2align 4,,15; n:; _##n:
#define FN3(n,s) .p2align 4,,15; n##_##s:; _##n##_##s:
#define FN2(n,s) FN3(n,s)
#define FN(n) FN2(n,POLY1305_SUFFIX)


/* end function annotations for linux/elf */
#if (IS_LINUX_ELF)
#define SYSENDFN(n) .size n, .-n; .type n, @function;
#define ENDFN3(n,s) .size n##_##s, .-n##_##s; .type n##_##s, @function;
#define ENDFN2(n,s) ENDFN3(n,s)
#define ENDFN(n) ENDFN2(n,POLY1305_SUFFIX)
#else
#define SYSENDFN(n)
#define ENDFN(n)
#endif


#if IS_WIN64

/* thunk to support win64. this assumes that there will be at most
   4 paramaters, aka no paramters passed on the stack! */
#undef FN3
#define FN3(n,s)                      \
.p2align 4,,15                       ;\
n##_##s:                             ;\
_##n##_##s:                          ;\
subq $184, %rsp                      ;\
movdqa %xmm6, 0(%rsp)                ;\
movdqa %xmm7, 16(%rsp)               ;\
movdqa %xmm8, 32(%rsp)               ;\
movdqa %xmm9, 48(%rsp)               ;\
movdqa %xmm10, 64(%rsp)              ;\
movdqa %xmm11, 80(%rsp)              ;\
movdqa %xmm12, 96(%rsp)              ;\
movdqa %xmm13, 112(%rsp)             ;\
movdqa %xmm14, 128(%rsp)             ;\
movdqa %xmm15, 144(%rsp)             ;\
movq %rdi, 160(%rsp)                 ;\
movq %rsi, 168(%rsp)                 ;\
movq %rcx, %rdi                      ;\
movq %rdx, %rsi                      ;\
movq %r8, %rdx                       ;\
movq %r9, %rcx                       ;\
call n##_thunk_##s                   ;\
movdqa 0(%rsp), %xmm6                ;\
movdqa 16(%rsp), %xmm7               ;\
movdqa 32(%rsp), %xmm8               ;\
movdqa 48(%rsp), %xmm9               ;\
movdqa 64(%rsp), %xmm10              ;\
movdqa 80(%rsp), %xmm11              ;\
movdqa 96(%rsp), %xmm12              ;\
movdqa 112(%rsp), %xmm13             ;\
movdqa 128(%rsp), %xmm14             ;\
movdqa 144(%rsp), %xmm15             ;\
movq 160(%rsp), %rdi                 ;\
movq 168(%rsp), %rsi                 ;\
addq $184, %rsp                      ;\
ret                                  ;\
.p2align 4,,15;                      ;\
n##_thunk_##s:

#endif /* IS_WIN64 */


/* global offset table support */
#if (defined(_WIN32) || defined(__CYGWIN__)) || (!defined(__pic__) && !defined(__PIC__))
#define LOAD_GOTREG(reg)
#define REF_GOTVAR(var, reg) var
#define CALL_PLT(function) call function
#else
#define LOAD_GOTREG(reg) call 1f; 1:; popl reg; leal _GLOBAL_OFFSET_TABLE_ + 1(reg), reg;
#define REF_GOTVAR(var, reg) var##@GOTOFF(reg)
#define CALL_PLT(function) call function@PLT
#endif


/* endfile */
/* NX indicator */
#if (IS_LINUX_ELF)
	#define SYSENDFILE() .section .note.GNU-stack,"",%progbits
#else
	#define SYSENDFILE()
#endif

#if !defined(POLY1305_ONEFILE)
	#define ENDFILE()
#else
	#define ENDFILE() SYSENDFILE()
#endif

#endif /* POLY1305_DEFINES_INC */



/* reset the suffix */
#undef POLY1305_SUFFIX
