#include "poly1305_defines.inc"
#define POLY1305_SUFFIX avx

.text

GLOBAL(poly1305_block_size)
GLOBAL(poly1305_init_ext)
GLOBAL(poly1305_blocks)
GLOBAL(poly1305_finish_ext)
GLOBAL(poly1305_auth)

FN(poly1305_block_size)
movl $32, %eax
ret
ENDFN(poly1305_block_size)

FN(poly1305_init_ext)
poly1305_init_ext_avx_local:
pushq %r12
pushq %r13
pushq %r14
movq %rdx, %r10
movq $-1, %rcx
testq %r10, %r10
vpxor %xmm0, %xmm0, %xmm0
movq $0xfffffc0ffff, %r9
vmovdqu %xmm0, (%rdi)
cmove %rcx, %r10
vmovdqu %xmm0, 16(%rdi)
movq $0xffc0fffffff, %rcx
vmovdqu %xmm0, 32(%rdi)
vmovdqu %xmm0, 48(%rdi)
vmovdqu %xmm0, 64(%rdi)
movq 8(%rsi), %r11
movq %r11, %r8
movq (%rsi), %r12
andq %r12, %rcx
shrq $44, %r12
shlq $20, %r8
shrq $24, %r11
orq %r8, %r12
movq $0xffffffc0f, %r8
andq %r9, %r12
andq %r8, %r11
movl %ecx, %r8d
andl $67108863, %r8d
movq %rcx, %r9
movl %r8d, 84(%rdi)
movq %r12, %r8
shrq $26, %r9
shlq $18, %r8
orq %r8, %r9
movq %r12, %r8
shrq $8, %r8
andl $67108863, %r9d
andl $67108863, %r8d
movl %r9d, 92(%rdi)
movq %r12, %r9
movl %r8d, 100(%rdi)
movq %r11, %r8
shrq $34, %r9
shlq $10, %r8
orq %r8, %r9
movq %r11, %r8
shrq $16, %r8
andl $67108863, %r9d
movl %r9d, 108(%rdi)
movl %r8d, 116(%rdi)
movl 16(%rsi), %r8d
movl %r8d, 124(%rdi)
movl 20(%rsi), %r8d
movl %r8d, 132(%rdi)
movl 24(%rsi), %r8d
movl %r8d, 140(%rdi)
movl 28(%rsi), %esi
movl %esi, 148(%rdi)
cmpq $16, %r10
jbe poly1305_init_ext_avx_done
lea (%r11,%r11,4), %r14
shlq $2, %r14
lea (%r12,%r12), %rax
mulq %r14
movq %rax, %r13
movq %rcx, %rax
movq %rdx, %r8
mulq %rcx
addq %rax, %r13
lea (%rcx,%rcx), %rax
movq %r13, %r9
adcq %rdx, %r8
mulq %r12
shlq $20, %r8
movq %rax, %rsi
shrq $44, %r9
movq %r11, %rax
orq %r9, %r8
movq %rdx, %r9
mulq %r14
addq %rax, %rsi
movq %rcx, %rax
adcq %rdx, %r9
addq %r11, %r11
mulq %r11
addq %rsi, %r8
movq %rax, %r11
movq %r12, %rax
movq %rdx, %rcx
adcq $0, %r9
mulq %r12
addq %rax, %r11
movq %r8, %rsi
adcq %rdx, %rcx
shlq $20, %r9
shrq $44, %rsi
orq %rsi, %r9
movq $0xfffffffffff, %rsi
addq %r11, %r9
movq %r9, %r12
adcq $0, %rcx
andq %rsi, %r13
shlq $22, %rcx
andq %rsi, %r8
shrq $42, %r12
orq %r12, %rcx
movq %rsi, %r12
lea (%rcx,%rcx,4), %rcx
addq %rcx, %r13
movq %rsi, %rcx
andq %r13, %rcx
shrq $44, %r13
movq %rcx, %r14
addq %r13, %r8
movq $0x3ffffffffff, %r13
andq %r8, %r12
andq %r13, %r9
shrq $44, %r8
movq %r12, %r11
addq %r8, %r9
movq %r12, %rax
movq %r9, %r13
movl %ecx, %r8d
shrq $26, %r14
andl $67108863, %r8d
shlq $18, %r11
shrq $34, %rax
orq %r11, %r14
shlq $10, %r13
movq %r12, %r11
orq %r13, %rax
movq %r9, %r13
shrq $8, %r11
shrq $16, %r13
andl $67108863, %r14d
andl $67108863, %r11d
andl $67108863, %eax
movl %r8d, 88(%rdi)
movl %r8d, 80(%rdi)
movl %r14d, 104(%rdi)
movl %r14d, 96(%rdi)
movl %r11d, 120(%rdi)
movl %r11d, 112(%rdi)
movl %eax, 136(%rdi)
movl %eax, 128(%rdi)
movl %r13d, 152(%rdi)
movl %r13d, 144(%rdi)
cmpq $64, %r10
jbe poly1305_init_ext_avx_done
lea (%r9,%r9,4), %r14
shlq $2, %r14
lea (%r12,%r12), %rax
mulq %r14
movq %rax, %r8
movq %rcx, %rax
movq %rdx, %r10
mulq %rcx
addq %rax, %r8
lea (%rcx,%rcx), %rax
movq %r8, %r11
adcq %rdx, %r10
andq %rsi, %r8
mulq %r12
shlq $20, %r10
movq %rax, %r13
shrq $44, %r11
movq %r9, %rax
orq %r11, %r10
movq %rdx, %r11
mulq %r14
addq %rax, %r13
movq %rcx, %rax
adcq %rdx, %r11
addq %r9, %r9
mulq %r9
addq %r13, %r10
movq %rax, %r9
movq %r12, %rax
movq %rdx, %rcx
adcq $0, %r11
mulq %r12
addq %rax, %r9
movq %r10, %r13
adcq %rdx, %rcx
andq %rsi, %r10
shlq $20, %r11
shrq $44, %r13
orq %r13, %r11
addq %r9, %r11
movq %rsi, %r9
movq %r11, %r12
adcq $0, %rcx
shlq $22, %rcx
shrq $42, %r12
orq %r12, %rcx
lea (%rcx,%rcx,4), %rcx
addq %rcx, %r8
andq %r8, %r9
shrq $44, %r8
movl %r9d, %eax
addq %r8, %r10
movq $0x3ffffffffff, %r8
andq %r10, %rsi
andq %r8, %r11
shrq $44, %r10
movq %rsi, %r8
addq %r10, %r11
andl $67108863, %eax
shrq $26, %r9
movq %r11, %r10
shlq $18, %r8
shlq $10, %r10
orq %r8, %r9
movq %rsi, %r8
shrq $34, %rsi
andl $67108863, %r9d
shrq $8, %r8
orq %r10, %rsi
shrq $16, %r11
andl $67108863, %r8d
andl $67108863, %esi
movl %eax, 168(%rdi)
movl %eax, 160(%rdi)
movl %r9d, 184(%rdi)
movl %r9d, 176(%rdi)
movl %r8d, 200(%rdi)
movl %r8d, 192(%rdi)
movl %esi, 216(%rdi)
movl %esi, 208(%rdi)
movl %r11d, 232(%rdi)
movl %r11d, 224(%rdi)
poly1305_init_ext_avx_done:
movq $0, 240(%rdi)
popq %r14
popq %r13
popq %r12
ret
ENDFN(poly1305_init_ext)

FN(poly1305_blocks)
poly1305_blocks_avx_local:
pushq %rbp
movq %rsp, %rbp
pushq %rbx
andq $-64, %rsp
subq $264, %rsp
movl $((1<<26)-1), %ebx
movl $(1<<24), %eax
vmovd %eax, %xmm1
vmovd %ebx, %xmm0
vpshufd $0x44, %xmm1, %xmm1
vpshufd $0x44, %xmm0, %xmm0
vmovdqa %xmm1, 104(%rsp)
movq 240(%rdi), %rax
testb $4, %al
je poly1305_blocks_avx_11
vmovdqa 104(%rsp), %xmm2
vpsrldq $8, %xmm2, %xmm1
vmovdqa %xmm1, 104(%rsp)
poly1305_blocks_avx_11:
testb $8, %al
je poly1305_blocks_avx_12
vpxor %xmm5, %xmm5, %xmm5
vmovdqa %xmm5, 104(%rsp)
poly1305_blocks_avx_12:
testb $1, %al
jne poly1305_blocks_avx_13
vmovq (%rsi), %xmm3
vmovq 16(%rsi), %xmm1
vpunpcklqdq %xmm1, %xmm3, %xmm3
vmovq 8(%rsi), %xmm7
vmovq 24(%rsi), %xmm1
vpunpcklqdq %xmm1, %xmm7, %xmm7
vpand %xmm3, %xmm0, %xmm1
vpsrlq $26, %xmm3, %xmm2
vpand %xmm2, %xmm0, %xmm2
vpsllq $12, %xmm7, %xmm4
vpsrlq $52, %xmm3, %xmm3
vpor %xmm4, %xmm3, %xmm4
vpand %xmm4, %xmm0, %xmm3
vpsrlq $26, %xmm4, %xmm4
vpand %xmm4, %xmm0, %xmm4
vpsrlq $40, %xmm7, %xmm7
vpor 104(%rsp), %xmm7, %xmm7
addq $32, %rsi
subq $32, %rdx
orq $1, %rax
movq %rax, 240(%rdi)
jmp poly1305_blocks_avx_14
poly1305_blocks_avx_13:
vmovdqa (%rdi), %xmm1
vmovdqa 16(%rdi), %xmm2
vmovdqa 32(%rdi), %xmm3
vmovdqa 48(%rdi), %xmm4
vmovdqa 64(%rdi), %xmm7
poly1305_blocks_avx_14:
vmovdqa 80(%rdi), %xmm8
vmovdqa %xmm8, -72(%rsp)
movq $5, %rax
vmovd %rax, %xmm9
vpshufd $68, %xmm9, %xmm9
vmovdqa %xmm9, 72(%rsp)
vmovdqa 96(%rdi), %xmm11
vmovdqa %xmm11, 248(%rsp)
vpmuludq %xmm9, %xmm11, %xmm13
vmovdqa %xmm13, -24(%rsp)
vmovdqa 112(%rdi), %xmm5
vmovdqa %xmm5, 168(%rsp)
vpmuludq %xmm9, %xmm5, %xmm8
vmovdqa %xmm8, -88(%rsp)
vmovdqa 128(%rdi), %xmm9
vmovdqa %xmm9, 88(%rsp)
vpmuludq 72(%rsp), %xmm9, %xmm11
vmovdqa %xmm11, -104(%rsp)
vmovdqa 144(%rdi), %xmm13
vmovdqa %xmm13, -8(%rsp)
vpmuludq 72(%rsp), %xmm13, %xmm5
vmovdqa %xmm5, -120(%rsp)
cmpq $63, %rdx
jbe poly1305_blocks_avx_15
vmovdqa 176(%rdi), %xmm8
vmovdqa %xmm8, 232(%rsp)
vpmuludq 72(%rsp), %xmm8, %xmm9
vmovdqa %xmm9, -56(%rsp)
vmovdqa 192(%rdi), %xmm11
vmovdqa %xmm11, 152(%rsp)
vpmuludq 72(%rsp), %xmm11, %xmm6
vmovdqa 208(%rdi), %xmm13
vmovdqa %xmm13, 56(%rsp)
vpmuludq 72(%rsp), %xmm13, %xmm5
vmovdqa 224(%rdi), %xmm8
vmovdqa %xmm8, -40(%rsp)
vpmuludq 72(%rsp), %xmm8, %xmm8
leaq 32(%rsi), %rax
movq %rdx, %rcx
vmovdqa %xmm6, 40(%rsp)
vmovdqa %xmm5, 136(%rsp)
vmovdqa %xmm8, 216(%rsp)
vmovdqa 160(%rdi), %xmm6
vmovdqa -88(%rsp), %xmm9
vmovdqa %xmm9, 24(%rsp)
vmovdqa -104(%rsp), %xmm11
vmovdqa %xmm11, 120(%rsp)
vmovdqa -120(%rsp), %xmm13
vmovdqa %xmm13, 200(%rsp)
vmovdqa -72(%rsp), %xmm5
poly1305_blocks_avx_16:
vpmuludq -56(%rsp), %xmm7, %xmm10
vpmuludq 40(%rsp), %xmm4, %xmm9
vpmuludq 40(%rsp), %xmm7, %xmm11
vpmuludq 136(%rsp), %xmm4, %xmm8
vpmuludq 136(%rsp), %xmm7, %xmm15
vpaddq %xmm9, %xmm10, %xmm9
vpmuludq 136(%rsp), %xmm3, %xmm12
vpmuludq 216(%rsp), %xmm7, %xmm13
vpaddq %xmm8, %xmm11, %xmm11
vpmuludq 216(%rsp), %xmm2, %xmm10
vpmuludq 216(%rsp), %xmm3, %xmm8
vpaddq %xmm12, %xmm9, %xmm9
vpmuludq 216(%rsp), %xmm4, %xmm14
vpmuludq %xmm6, %xmm4, %xmm12
vpaddq %xmm10, %xmm9, %xmm9
vpmuludq %xmm6, %xmm7, %xmm7
vmovdqa %xmm7, 184(%rsp)
vpaddq %xmm8, %xmm11, %xmm11
vmovq -32(%rax), %xmm8
vmovq -16(%rax), %xmm7
vpunpcklqdq %xmm7, %xmm8, %xmm8
vpmuludq %xmm6, %xmm1, %xmm10
vpaddq %xmm14, %xmm15, %xmm15
vpmuludq %xmm6, %xmm2, %xmm7
vpaddq %xmm12, %xmm13, %xmm13
vpmuludq %xmm6, %xmm3, %xmm14
vpmuludq 232(%rsp), %xmm3, %xmm12
vpaddq %xmm10, %xmm9, %xmm9
vpmuludq 232(%rsp), %xmm4, %xmm4
vpaddq %xmm7, %xmm11, %xmm11
vmovq -24(%rax), %xmm10
vmovq -8(%rax), %xmm7
vpunpcklqdq %xmm7, %xmm10, %xmm7
vpmuludq 232(%rsp), %xmm1, %xmm10
vpaddq %xmm14, %xmm15, %xmm14
vpmuludq 232(%rsp), %xmm2, %xmm15
vpaddq %xmm12, %xmm13, %xmm12
vpmuludq 152(%rsp), %xmm2, %xmm13
vpaddq 184(%rsp), %xmm4, %xmm4
vpmuludq 152(%rsp), %xmm3, %xmm3
vpaddq %xmm10, %xmm11, %xmm10
vpand %xmm8, %xmm0, %xmm11
vmovdqa %xmm11, 184(%rsp)
vpsrlq $26, %xmm8, %xmm11
vpand %xmm11, %xmm0, %xmm11
vmovdqa %xmm11, 8(%rsp)
vpmuludq 152(%rsp), %xmm1, %xmm11
vpaddq %xmm15, %xmm14, %xmm14
vpmuludq 56(%rsp), %xmm1, %xmm15
vpaddq %xmm13, %xmm12, %xmm12
vpmuludq 56(%rsp), %xmm2, %xmm2
vpaddq %xmm3, %xmm4, %xmm3
vpsllq $12, %xmm7, %xmm4
vpsrlq $52, %xmm8, %xmm8
vpor %xmm4, %xmm8, %xmm8
vpsrlq $14, %xmm7, %xmm4
vpand %xmm4, %xmm0, %xmm4
vpmuludq -40(%rsp), %xmm1, %xmm1
vpaddq %xmm11, %xmm14, %xmm14
vpaddq %xmm15, %xmm12, %xmm15
vpaddq %xmm2, %xmm3, %xmm2
vpaddq %xmm1, %xmm2, %xmm1
vpand %xmm8, %xmm0, %xmm8
vpsrlq $40, %xmm7, %xmm7
vpor 104(%rsp), %xmm7, %xmm7
vmovdqu (%rax), %xmm12
vmovdqu 16(%rax), %xmm2
vpunpckldq %xmm2, %xmm12, %xmm11
vpxor %xmm13, %xmm13, %xmm13
vpunpckldq %xmm13, %xmm11, %xmm3
vpunpckhdq %xmm13, %xmm11, %xmm11
vpunpckhdq %xmm2, %xmm12, %xmm12
vpunpckldq %xmm13, %xmm12, %xmm2
vpunpckhdq %xmm13, %xmm12, %xmm12
vpsllq $6, %xmm11, %xmm11
vpsllq $12, %xmm2, %xmm2
vpsllq $18, %xmm12, %xmm12
vpaddq %xmm3, %xmm9, %xmm9
vpaddq %xmm11, %xmm10, %xmm10
vpaddq %xmm2, %xmm14, %xmm14
vpaddq %xmm12, %xmm15, %xmm12
vpaddq 104(%rsp), %xmm1, %xmm1
vpmuludq -24(%rsp), %xmm7, %xmm2
vpmuludq 24(%rsp), %xmm4, %xmm13
vpmuludq 24(%rsp), %xmm7, %xmm11
vpmuludq 120(%rsp), %xmm4, %xmm3
vpaddq %xmm2, %xmm9, %xmm9
vpmuludq 120(%rsp), %xmm7, %xmm2
vpaddq %xmm13, %xmm9, %xmm9
vpmuludq 120(%rsp), %xmm8, %xmm13
vpaddq %xmm11, %xmm10, %xmm10
vpmuludq 200(%rsp), %xmm7, %xmm15
vpaddq %xmm3, %xmm10, %xmm10
vmovdqa 8(%rsp), %xmm3
vpmuludq 200(%rsp), %xmm3, %xmm11
vpaddq %xmm2, %xmm14, %xmm2
vpmuludq 200(%rsp), %xmm8, %xmm14
vpaddq %xmm13, %xmm9, %xmm9
vpmuludq 200(%rsp), %xmm4, %xmm13
vpaddq %xmm15, %xmm12, %xmm12
vpmuludq %xmm5, %xmm4, %xmm15
vpaddq %xmm11, %xmm9, %xmm9
vpmuludq %xmm5, %xmm7, %xmm11
vpaddq %xmm14, %xmm10, %xmm10
vmovdqa 184(%rsp), %xmm7
vpmuludq %xmm5, %xmm7, %xmm14
vpaddq %xmm13, %xmm2, %xmm2
vpmuludq %xmm5, %xmm3, %xmm13
vpaddq %xmm15, %xmm12, %xmm15
vpmuludq %xmm5, %xmm8, %xmm12
vpaddq %xmm11, %xmm1, %xmm1
vpmuludq 248(%rsp), %xmm8, %xmm11
vpaddq %xmm14, %xmm9, %xmm9
vpmuludq 248(%rsp), %xmm4, %xmm4
vpaddq %xmm13, %xmm10, %xmm10
vpmuludq 248(%rsp), %xmm7, %xmm14
vpaddq %xmm12, %xmm2, %xmm2
vpmuludq 248(%rsp), %xmm3, %xmm13
vpaddq %xmm11, %xmm15, %xmm15
vpmuludq 168(%rsp), %xmm3, %xmm12
vpaddq %xmm4, %xmm1, %xmm1
vpmuludq 168(%rsp), %xmm8, %xmm8
vpaddq %xmm14, %xmm10, %xmm10
vpmuludq 168(%rsp), %xmm7, %xmm14
vpaddq %xmm13, %xmm2, %xmm2
vpmuludq 88(%rsp), %xmm7, %xmm11
vpaddq %xmm12, %xmm15, %xmm15
vpmuludq 88(%rsp), %xmm3, %xmm3
vpaddq %xmm8, %xmm1, %xmm1
vpmuludq -8(%rsp), %xmm7, %xmm7
vpaddq %xmm14, %xmm2, %xmm2
vpaddq %xmm11, %xmm15, %xmm15
vpaddq %xmm3, %xmm1, %xmm1
vpaddq %xmm7, %xmm1, %xmm7
vpsrlq $26, %xmm9, %xmm3
vpsrlq $26, %xmm15, %xmm12
vpand %xmm0, %xmm9, %xmm9
vpand %xmm0, %xmm15, %xmm15
vpaddq %xmm3, %xmm10, %xmm10
vpaddq %xmm12, %xmm7, %xmm7
vpsrlq $26, %xmm10, %xmm14
vpsrlq $26, %xmm7, %xmm1
vpand %xmm0, %xmm10, %xmm10
vpand %xmm0, %xmm7, %xmm7
vpaddq %xmm14, %xmm2, %xmm2
vpmuludq 72(%rsp), %xmm1, %xmm1
vpaddq %xmm1, %xmm9, %xmm1
vpsrlq $26, %xmm2, %xmm4
vpsrlq $26, %xmm1, %xmm8
vpand %xmm0, %xmm2, %xmm3
vpand %xmm0, %xmm1, %xmm1
vpaddq %xmm4, %xmm15, %xmm4
vpaddq %xmm8, %xmm10, %xmm2
vpsrlq $26, %xmm4, %xmm12
vpand %xmm0, %xmm4, %xmm4
vpaddq %xmm12, %xmm7, %xmm7
subq $64, %rcx
addq $64, %rax
cmpq $63, %rcx
ja poly1305_blocks_avx_16
leaq -64(%rdx), %rax
andq $-64, %rax
leaq 64(%rsi,%rax), %rsi
andl $63, %edx
poly1305_blocks_avx_15:
cmpq $31, %rdx
jbe poly1305_blocks_avx_17
vpmuludq -24(%rsp), %xmm7, %xmm6
vmovdqa -88(%rsp), %xmm9
vpmuludq %xmm9, %xmm4, %xmm5
vpmuludq %xmm9, %xmm7, %xmm9
vmovdqa -104(%rsp), %xmm10
vpmuludq %xmm10, %xmm4, %xmm8
vpmuludq %xmm10, %xmm7, %xmm15
vpaddq %xmm5, %xmm6, %xmm6
vpmuludq %xmm10, %xmm3, %xmm10
vmovdqa -120(%rsp), %xmm5
vpmuludq %xmm5, %xmm7, %xmm14
vpaddq %xmm8, %xmm9, %xmm13
vpmuludq %xmm5, %xmm2, %xmm8
vpmuludq %xmm5, %xmm3, %xmm9
vpaddq %xmm10, %xmm6, %xmm6
vpmuludq %xmm5, %xmm4, %xmm5
vmovdqa -72(%rsp), %xmm11
vpmuludq %xmm11, %xmm4, %xmm12
vpaddq %xmm8, %xmm6, %xmm8
vpmuludq %xmm11, %xmm7, %xmm7
vpaddq %xmm9, %xmm13, %xmm9
vpmuludq %xmm11, %xmm1, %xmm6
vpaddq %xmm5, %xmm15, %xmm5
vpmuludq %xmm11, %xmm2, %xmm13
vpaddq %xmm12, %xmm14, %xmm12
vpmuludq %xmm11, %xmm3, %xmm11
vpmuludq 248(%rsp), %xmm3, %xmm14
vpaddq %xmm6, %xmm8, %xmm6
vpmuludq 248(%rsp), %xmm4, %xmm4
vpaddq %xmm13, %xmm9, %xmm9
vpmuludq 248(%rsp), %xmm1, %xmm10
vpaddq %xmm11, %xmm5, %xmm5
vpmuludq 248(%rsp), %xmm2, %xmm11
vpaddq %xmm14, %xmm12, %xmm12
vpmuludq 168(%rsp), %xmm2, %xmm8
vpaddq %xmm4, %xmm7, %xmm4
vpmuludq 168(%rsp), %xmm3, %xmm3
vpaddq %xmm10, %xmm9, %xmm9
vpmuludq 168(%rsp), %xmm1, %xmm10
vpaddq %xmm11, %xmm5, %xmm5
vpmuludq 88(%rsp), %xmm1, %xmm13
vpaddq %xmm8, %xmm12, %xmm12
vpmuludq 88(%rsp), %xmm2, %xmm2
vpaddq %xmm3, %xmm4, %xmm3
vpmuludq -8(%rsp), %xmm1, %xmm1
vpaddq %xmm10, %xmm5, %xmm5
vpaddq %xmm13, %xmm12, %xmm12
vpaddq %xmm2, %xmm3, %xmm3
vpaddq %xmm1, %xmm3, %xmm3
testq %rsi, %rsi
je poly1305_blocks_avx_18
vmovdqu (%rsi), %xmm1
vmovdqu 16(%rsi), %xmm2
vpunpckldq %xmm2, %xmm1, %xmm4
vpxor %xmm8, %xmm8, %xmm8
vpunpckldq %xmm8, %xmm4, %xmm7
vpunpckhdq %xmm8, %xmm4, %xmm4
vpunpckhdq %xmm2, %xmm1, %xmm1
vpunpckldq %xmm8, %xmm1, %xmm2
vpunpckhdq %xmm8, %xmm1, %xmm1
vpsllq $6, %xmm4, %xmm4
vpsllq $12, %xmm2, %xmm2
vpsllq $18, %xmm1, %xmm1
vpaddq %xmm7, %xmm6, %xmm6
vpaddq %xmm4, %xmm9, %xmm9
vpaddq %xmm2, %xmm5, %xmm5
vpaddq %xmm1, %xmm12, %xmm12
vpaddq 104(%rsp), %xmm3, %xmm3
poly1305_blocks_avx_18:
vpsrlq $26, %xmm6, %xmm2
vpsrlq $26, %xmm12, %xmm7
vpand %xmm0, %xmm6, %xmm6
vpand %xmm0, %xmm12, %xmm12
vpaddq %xmm2, %xmm9, %xmm9
vpaddq %xmm7, %xmm3, %xmm7
vpsrlq $26, %xmm9, %xmm3
vpsrlq $26, %xmm7, %xmm1
vpand %xmm0, %xmm9, %xmm2
vpand %xmm0, %xmm7, %xmm7
vpaddq %xmm3, %xmm5, %xmm3
vpmuludq 72(%rsp), %xmm1, %xmm1
vpaddq %xmm1, %xmm6, %xmm1
vpsrlq $26, %xmm3, %xmm4
vpsrlq $26, %xmm1, %xmm5
vpaddq %xmm4, %xmm12, %xmm4
vpsrlq $26, %xmm4, %xmm6
vpaddq %xmm6, %xmm7, %xmm7
vpand %xmm0, %xmm4, %xmm4
vpand %xmm0, %xmm3, %xmm3
vpaddq %xmm5, %xmm2, %xmm2
vpand %xmm0, %xmm1, %xmm1
poly1305_blocks_avx_17:
testq %rsi, %rsi
je poly1305_blocks_avx_19
vmovdqa %xmm1, (%rdi)
vmovdqa %xmm2, 16(%rdi)
vmovdqa %xmm3, 32(%rdi)
vmovdqa %xmm4, 48(%rdi)
vmovdqa %xmm7, 64(%rdi)
jmp poly1305_blocks_avx_10
poly1305_blocks_avx_19:
vpsrldq $8, %xmm1, %xmm0
vpaddq %xmm0, %xmm1, %xmm1
vpsrldq $8, %xmm2, %xmm0
vpaddq %xmm0, %xmm2, %xmm2
vpsrldq $8, %xmm3, %xmm0
vpaddq %xmm0, %xmm3, %xmm3
vpsrldq $8, %xmm4, %xmm0
vpaddq %xmm0, %xmm4, %xmm4
vpsrldq $8, %xmm7, %xmm0
vpaddq %xmm0, %xmm7, %xmm7
vmovd %xmm1, %eax
vmovd %xmm2, %edx
movl %eax, %r9d
shrl $26, %r9d
addl %edx, %r9d
movl %r9d, %r8d
andl $67108863, %r8d
vmovd %xmm3, %edx
shrl $26, %r9d
addl %edx, %r9d
vmovd %xmm4, %edx
movl %r9d, %ecx
shrl $26, %ecx
addl %edx, %ecx
movl %ecx, %esi
andl $67108863, %esi
vmovd %xmm7, %r10d
movl %r8d, %r11d
salq $26, %r11
andl $67108863, %eax
orq %rax, %r11
movabsq $17592186044415, %rax
andq %rax, %r11
andl $67108863, %r9d
salq $8, %r9
shrl $18, %r8d
movl %r8d, %r8d
orq %r8, %r9
movq %rsi, %rdx
salq $34, %rdx
orq %rdx, %r9
andq %rax, %r9
shrl $26, %ecx
addl %r10d, %ecx
salq $16, %rcx
shrl $10, %esi
movl %esi, %esi
orq %rsi, %rcx
movabsq $4398046511103, %r10
movq %rcx, %r8
andq %r10, %r8
shrq $42, %rcx
leaq (%rcx,%rcx,4), %rdx
addq %r11, %rdx
movq %rdx, %rsi
andq %rax, %rsi
shrq $44, %rdx
addq %r9, %rdx
movq %rdx, %rcx
andq %rax, %rcx
shrq $44, %rdx
addq %r8, %rdx
andq %rdx, %r10
shrq $42, %rdx
leaq (%rsi,%rdx,4), %rsi
leaq (%rsi,%rdx), %r11
movq %r11, %rbx
andq %rax, %rbx
shrq $44, %r11
addq %rcx, %r11
leaq 5(%rbx), %r9
movq %r9, %r8
shrq $44, %r8
addq %r11, %r8
movabsq $-4398046511104, %rsi
addq %r10, %rsi
movq %r8, %rdx
shrq $44, %rdx
addq %rdx, %rsi
movq %rsi, %rdx
shrq $63, %rdx
subq $1, %rdx
movq %rdx, %rcx
notq %rcx
andq %rcx, %rbx
andq %rcx, %r11
andq %r10, %rcx
andq %rax, %r9
andq %rdx, %r9
orq %r9, %rbx
movq %rbx, (%rdi)
andq %r8, %rax
andq %rdx, %rax
orq %rax, %r11
movq %r11, 8(%rdi)
andq %rsi, %rdx
orq %rcx, %rdx
movq %rdx, 16(%rdi)
poly1305_blocks_avx_10:
movq -8(%rbp), %rbx
leave
ret
ENDFN(poly1305_blocks)

FN(poly1305_finish_ext)
poly1305_finish_ext_avx_local:
pushq %r13
pushq %r14
pushq %r15
subq $32, %rsp
movq %rdx, %r14
movq %rcx, %r15
movq %rdi, %r13
testq %r14, %r14
je poly1305_finish_ext_avx_18
poly1305_finish_ext_avx_2:
vpxor %xmm0, %xmm0, %xmm0
vmovups %xmm0, 16(%rsp)
vmovups %xmm0, (%rsp)
poly1305_finish_ext_avx_3:
lea (%rsp), %rax
subq %rax, %rsi
testq $16, %r14
je poly1305_finish_ext_avx_5
poly1305_finish_ext_avx_4:
lea 16(%rsp), %rax
vmovdqu (%rsp,%rsi), %xmm0
vmovdqu %xmm0, (%rsp)
poly1305_finish_ext_avx_5:
testq $8, %r14
je poly1305_finish_ext_avx_7
poly1305_finish_ext_avx_6:
movq (%rax,%rsi), %rdx
movq %rdx, (%rax)
addq $8, %rax
poly1305_finish_ext_avx_7:
testq $4, %r14
je poly1305_finish_ext_avx_9
poly1305_finish_ext_avx_8:
movl (%rax,%rsi), %edx
movl %edx, (%rax)
addq $4, %rax
poly1305_finish_ext_avx_9:
testq $2, %r14
je poly1305_finish_ext_avx_11
poly1305_finish_ext_avx_10:
movzwl (%rax,%rsi), %edx
movw %dx, (%rax)
addq $2, %rax
poly1305_finish_ext_avx_11:
testq $1, %r14
je poly1305_finish_ext_avx_13
poly1305_finish_ext_avx_12:
movb (%rax,%rsi), %dl
movb %dl, (%rax)
poly1305_finish_ext_avx_13:
cmpq $16, %r14
je poly1305_finish_ext_avx_16
poly1305_finish_ext_avx_14:
movb $1, (%rsp,%r14)
jae poly1305_finish_ext_avx_16
poly1305_finish_ext_avx_15:
movl $8, %eax
jmp poly1305_finish_ext_avx_17
poly1305_finish_ext_avx_16:
movl $4, %eax
poly1305_finish_ext_avx_17:
movq %r13, %rdi
lea (%rsp), %rsi
movl $32, %edx
orq %rax, 240(%r13)
call poly1305_blocks_avx_local
poly1305_finish_ext_avx_18:
testq $1, 240(%r13)
je poly1305_finish_ext_avx_24
poly1305_finish_ext_avx_19:
testq %r14, %r14
je poly1305_finish_ext_avx_21
poly1305_finish_ext_avx_20:
cmpq $16, %r14
jbe poly1305_finish_ext_avx_22
poly1305_finish_ext_avx_21:
movl 84(%r13), %eax
movl 92(%r13), %edx
movl 100(%r13), %ecx
movl 108(%r13), %esi
movl 116(%r13), %edi
movl %eax, 88(%r13)
movl %edx, 104(%r13)
movl %ecx, 120(%r13)
movl %esi, 136(%r13)
movl %edi, 152(%r13)
jmp poly1305_finish_ext_avx_23
poly1305_finish_ext_avx_22:
movl 84(%r13), %eax
xorl %r8d, %r8d
movl 92(%r13), %edx
movl 100(%r13), %ecx
movl 108(%r13), %esi
movl 116(%r13), %edi
movl %eax, 80(%r13)
movl $1, 88(%r13)
movl %edx, 96(%r13)
movl %r8d, 104(%r13)
movl %ecx, 112(%r13)
movl %r8d, 120(%r13)
movl %esi, 128(%r13)
movl %r8d, 136(%r13)
movl %edi, 144(%r13)
movl %r8d, 152(%r13)
poly1305_finish_ext_avx_23:
movq %r13, %rdi
xorl %esi, %esi
movl $32, %edx
call poly1305_blocks_avx_local
poly1305_finish_ext_avx_24:
movq 8(%r13), %r9
movq %r9, %r8
movq 16(%r13), %rcx
movl 132(%r13), %esi
vpxor %xmm0, %xmm0, %xmm0
movl 148(%r13), %edi
shlq $32, %rsi
shlq $32, %rdi
shlq $44, %r8
shrq $20, %r9
shlq $24, %rcx
movl 124(%r13), %eax
orq %rax, %rsi
movl 140(%r13), %edx
orq %rdx, %rdi
orq (%r13), %r8
orq %rcx, %r9
addq %rsi, %r8
adcq %rdi, %r9
vmovdqu %xmm0, (%r13)
vmovdqu %xmm0, 16(%r13)
vmovdqu %xmm0, 32(%r13)
vmovdqu %xmm0, 48(%r13)
vmovdqu %xmm0, 64(%r13)
vmovdqu %xmm0, 80(%r13)
vmovdqu %xmm0, 96(%r13)
vmovdqu %xmm0, 112(%r13)
vmovdqu %xmm0, 128(%r13)
vmovdqu %xmm0, 144(%r13)
vmovdqu %xmm0, 160(%r13)
vmovdqu %xmm0, 176(%r13)
vmovdqu %xmm0, 192(%r13)
vmovdqu %xmm0, 208(%r13)
vmovdqu %xmm0, 224(%r13)
movq %r8, (%r15)
movq %r9, 8(%r15)
addq $32, %rsp
popq %r15
popq %r14
popq %r13
ret
ENDFN(poly1305_finish_ext)


FN(poly1305_auth)
pushq %rbp
movq %rsp, %rbp
subq $272, %rsp
andq $~63, %rsp
movq %rdi, 248(%rsp)
movq %rsi, 256(%rsp)
movq %rdx, 264(%rsp)
movq %rsp, %rdi
movq %rcx, %rsi
callq poly1305_init_ext_avx_local
movq %rsp, %rdi
movq 256(%rsp), %rsi
movq 264(%rsp), %rdx
andq $~31, %rdx
jz poly1305_auth_avx_no_data
callq poly1305_blocks_avx_local
poly1305_auth_avx_no_data:
movq %rsp, %rdi
movq 256(%rsp), %rsi
movq 264(%rsp), %rdx
movq %rdx, %rax
andq $~31, %rax
andq $31, %rdx
addq %rax, %rsi
movq 248(%rsp), %rcx
callq poly1305_finish_ext_avx_local
movq %rbp, %rsp
popq %rbp
ret
ENDFN(poly1305_auth)


ENDFILE()
