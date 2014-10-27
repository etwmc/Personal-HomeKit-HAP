#include "poly1305_defines.inc"
#define POLY1305_SUFFIX sse2

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
movl 4(%esp), %eax
movl 8(%esp), %edx
movl 12(%esp), %ecx
poly1305_init_ext_sse2_local:
pushl %esi
pushl %edi
pushl %ebx
pushl %ebp
movl 32(%esp), %edi
subl $76, %esp
movl $-1, %ebx
testl %ecx, %ecx
pxor %xmm0, %xmm0
cmove %ebx, %ecx
movdqa %xmm0, (%eax)
testl %edi, %edi
movdqa %xmm0, 16(%eax)
movdqa %xmm0, 32(%eax)
movdqa %xmm0, 48(%eax)
cmovnz %ebx, %ecx
movdqa %xmm0, 64(%eax)
movl %ecx, (%esp)
movl 4(%edx), %ecx
movl (%edx), %edi
movl %edi, %ebp
movl 8(%edx), %ebx
andl $67108863, %ebp
movl 12(%edx), %esi
movl %edx, 4(%esp)
movl %ecx, %edx
shrl $26, %edi
shll $6, %edx
orl %edx, %edi
movl %ebx, %edx
shrl $20, %ecx
andl $67108611, %edi
shll $12, %edx
orl %edx, %ecx
movl %esi, %edx
shrl $14, %ebx
andl $67092735, %ecx
shll $18, %edx
orl %edx, %ebx
movl 4(%esp), %edx
andl $66076671, %ebx
shrl $8, %esi
andl $1048575, %esi
movl %ebp, 84(%eax)
movl %edi, 92(%eax)
movl %ecx, 100(%eax)
movl %ebx, 108(%eax)
movl %esi, 116(%eax)
movl %ebp, 8(%esp)
movl 16(%edx), %ebp
movl %ebp, 124(%eax)
movl %edi, 12(%esp)
movl 20(%edx), %edi
movl %edi, 132(%eax)
movl 24(%edx), %ebp
movl %ebp, 140(%eax)
movl 28(%edx), %edx
movl (%esp), %ebp
movl %edx, 148(%eax)
cmpl $16, %ebp
jb poly1305_init_ext_sse2_7
xorl %edx, %edx
movl %edx, 16(%esp)
movl %esi, 20(%esp)
movl %ebx, 28(%esp)
movl %ecx, 24(%esp)
movl %ebp, (%esp)
movl %eax, 4(%esp)
poly1305_init_ext_sse2_3:
movl 8(%esp), %eax
mull %eax
movl 20(%esp), %esi
movl %edx, %edi
movl 12(%esp), %ebx
movl 28(%esp), %ebp
lea (%esi,%esi), %ecx
movl %eax, %esi
movl %ecx, 32(%esp)
lea (%ebx,%ebx,4), %eax
mull %ecx
movl 24(%esp), %ecx
addl %eax, %esi
movl %ecx, 48(%esp)
lea (%ebp,%ebp,4), %ebp
adcl %edx, %edi
movl %ebp, 36(%esp)
lea (%ecx,%ecx), %eax
mull %ebp
addl %eax, %esi
lea (%ebx,%ebx), %ebp
movl 28(%esp), %ebx
movl 8(%esp), %eax
adcl %edx, %edi
movl %ebp, 44(%esp)
movl %esi, 40(%esp)
addl %ebx, %ebx
movl %ebx, 52(%esp)
lea (%eax,%eax), %ebx
mull %ebp
movl %ebx, 56(%esp)
movl %eax, %ebx
movl %edx, %ebp
lea (%ecx,%ecx,4), %eax
mull 32(%esp)
addl %eax, %ebx
movl 28(%esp), %eax
movl 36(%esp), %ecx
adcl %edx, %ebp
mull %ecx
shll $6, %edi
shrl $26, %esi
orl %esi, %edi
addl %edi, %eax
adcl $0, %edx
addl %eax, %ebx
movl 12(%esp), %eax
adcl %edx, %ebp
mull %eax
movl %eax, %edi
movl %edx, %esi
movl 48(%esp), %eax
mull 56(%esp)
addl %eax, %edi
movl 32(%esp), %eax
adcl %edx, %esi
mull %ecx
movl %ebx, 60(%esp)
shll $6, %ebp
shrl $26, %ebx
orl %ebx, %ebp
addl %ebp, %eax
movl 48(%esp), %ecx
adcl $0, %edx
addl %eax, %edi
movl 44(%esp), %eax
movl %edi, %ebp
adcl %edx, %esi
andl $67108863, %ebp
mull %ecx
shll $6, %esi
shrl $26, %edi
movl 52(%esp), %ebx
orl %edi, %esi
movl %ebp, 24(%esp)
movl %eax, %ebp
movl 8(%esp), %eax
movl %edx, %edi
mull %ebx
addl %eax, %ebp
movl 20(%esp), %eax
adcl %edx, %edi
lea (%eax,%eax,4), %edx
mull %edx
addl %eax, %ebp
movl %ecx, %eax
adcl %edx, %edi
addl %ebp, %esi
movl %esi, %ebp
adcl $0, %edi
andl $67108863, %ebp
mull %ecx
shll $6, %edi
movl %edx, %ecx
shrl $26, %esi
orl %esi, %edi
movl %eax, %esi
movl %ebx, %eax
mull 12(%esp)
addl %eax, %esi
movl 56(%esp), %eax
adcl %edx, %ecx
mull 20(%esp)
addl %eax, %esi
movl 40(%esp), %ebx
adcl %edx, %ecx
addl %esi, %edi
movl %edi, %esi
adcl $0, %ecx
andl $67108863, %ebx
shll $6, %ecx
andl $67108863, %esi
shrl $26, %edi
orl %edi, %ecx
movl 60(%esp), %edx
andl $67108863, %edx
movl %ebp, 28(%esp)
movl %esi, 20(%esp)
lea (%ecx,%ecx,4), %edi
addl %edi, %ebx
movl %ebx, %eax
shrl $26, %ebx
andl $67108863, %eax
movl 4(%esp), %edi
movl %eax, 8(%esp)
addl %edx, %ebx
movl 16(%esp), %edx
movl %ebx, 12(%esp)
lea (%edx,%edx,4), %ecx
incl %edx
shll $4, %ecx
movl %edx, 16(%esp)
movl %eax, 88(%edi,%ecx)
movl %eax, 80(%edi,%ecx)
movl 24(%esp), %eax
movl %ebx, 104(%edi,%ecx)
movl %ebx, 96(%edi,%ecx)
movl %eax, 120(%edi,%ecx)
movl %eax, 112(%edi,%ecx)
movl %ebp, 136(%edi,%ecx)
movl %ebp, 128(%edi,%ecx)
movl %esi, 152(%edi,%ecx)
movl %esi, 144(%edi,%ecx)
cmpl $2, %edx
jae poly1305_init_ext_sse2_6
cmpl $0, 16(%esp)
je poly1305_init_ext_sse2_3
cmpl $64, (%esp)
jae poly1305_init_ext_sse2_3
poly1305_init_ext_sse2_6:
movl %edi, %eax
poly1305_init_ext_sse2_7:
movl $0, 240(%eax)
addl $76, %esp
popl %ebp
popl %ebx
popl %edi
popl %esi
ret
ENDFN(poly1305_init_ext)

FN(poly1305_blocks)
movl 4(%esp), %eax
movl 8(%esp), %edx
movl 12(%esp), %ecx
poly1305_blocks_sse2_local:
pushl %ebp
movl %esp, %ebp
andl $~63, %ebp
subl $8, %ebp
subl $(568+8+63), %esp
movl %esi, -4(%ebp)
movl $16777216, %esi
movl %edi, -8(%ebp)
movl $67108863, %edi
movd %esi, %xmm0
movl $5, %esi
movd %edi, %xmm2
pshufd $68, %xmm0, %xmm1
pshufd $68, %xmm2, %xmm3
movd %esi, %xmm4
movl 240(%eax), %esi
pshufd $68, %xmm4, %xmm5
movdqa %xmm1, -456(%ebp)
movdqa %xmm3, -344(%ebp)
movdqa %xmm5, -552(%ebp)
testl $4, %esi
je poly1305_blocks_sse2_3
poly1305_blocks_sse2_2:
movdqa -456(%ebp), %xmm0
psrldq $8, %xmm0
movdqa %xmm0, -456(%ebp)
poly1305_blocks_sse2_3:
testl $8, %esi
je poly1305_blocks_sse2_5
poly1305_blocks_sse2_4:
pxor %xmm0, %xmm0
movdqa %xmm0, -456(%ebp)
poly1305_blocks_sse2_5:
btsl $0, %esi
jc poly1305_blocks_sse2_7
poly1305_blocks_sse2_6:
movq 8(%edx), %xmm0
addl $-32, %ecx
movq (%edx), %xmm3
movhpd 24(%edx), %xmm0
movdqa -344(%ebp), %xmm6
movaps %xmm0, %xmm4
movhpd 16(%edx), %xmm3
movdqa %xmm6, %xmm1
pand %xmm3, %xmm1
movaps %xmm3, %xmm2
psrlq $52, %xmm3
psllq $12, %xmm4
por %xmm4, %xmm3
movdqa %xmm6, %xmm5
psrlq $26, %xmm2
pand %xmm3, %xmm5
psrlq $26, %xmm3
psrlq $40, %xmm0
pand %xmm6, %xmm2
pand %xmm6, %xmm3
movdqa %xmm1, -520(%ebp)
addl $32, %edx
movdqa %xmm2, -504(%ebp)
movdqa %xmm5, -488(%ebp)
por -456(%ebp), %xmm0
movl %esi, 240(%eax)
jmp poly1305_blocks_sse2_8
poly1305_blocks_sse2_7:
movdqa (%eax), %xmm0
movdqa 16(%eax), %xmm1
movdqa 32(%eax), %xmm2
movdqa %xmm0, -520(%ebp)
movdqa %xmm1, -504(%ebp)
movdqa %xmm2, -488(%ebp)
movdqa 48(%eax), %xmm3
movdqa 64(%eax), %xmm0
poly1305_blocks_sse2_8:
movdqa 80(%eax), %xmm4
movdqa -552(%ebp), %xmm1
movdqa %xmm4, -360(%ebp)
movdqa %xmm1, %xmm4
movdqa 96(%eax), %xmm5
movdqa 112(%eax), %xmm6
movdqa %xmm5, -392(%ebp)
pmuludq %xmm5, %xmm4
movdqa %xmm1, %xmm5
pmuludq %xmm6, %xmm5
movdqa %xmm6, -424(%ebp)
movdqa %xmm1, %xmm6
movdqa 128(%eax), %xmm7
movdqa 144(%eax), %xmm2
movdqa %xmm7, -472(%ebp)
movdqa %xmm2, -568(%ebp)
pmuludq %xmm7, %xmm6
pmuludq %xmm2, %xmm1
cmpl $64, %ecx
jb poly1305_blocks_sse2_12
poly1305_blocks_sse2_9:
movdqa %xmm6, -408(%ebp)
movdqa %xmm1, -376(%ebp)
movdqa 160(%eax), %xmm6
movdqa -552(%ebp), %xmm1
movdqa %xmm6, -312(%ebp)
movdqa %xmm1, %xmm6
movdqa 176(%eax), %xmm7
movaps %xmm0, -168(%ebp)
movdqa %xmm7, -232(%ebp)
movdqa 192(%eax), %xmm0
pmuludq %xmm7, %xmm6
movdqa %xmm1, %xmm7
pmuludq %xmm0, %xmm7
movdqa %xmm5, -440(%ebp)
movdqa %xmm0, -216(%ebp)
movdqa %xmm1, %xmm0
movdqa 208(%eax), %xmm2
movdqa 224(%eax), %xmm5
pmuludq %xmm2, %xmm0
pmuludq %xmm5, %xmm1
movdqa %xmm5, -184(%ebp)
movdqa %xmm6, -248(%ebp)
movdqa %xmm0, -280(%ebp)
movdqa %xmm1, -296(%ebp)
movdqa %xmm2, -200(%ebp)
movdqa %xmm7, -264(%ebp)
movaps -168(%ebp), %xmm0
movdqa %xmm4, -536(%ebp)
movaps %xmm3, -328(%ebp)
jmp poly1305_blocks_sse2_10
.p2align 5,,31
poly1305_blocks_sse2_10:
movq 8(%edx), %xmm2
addl $-64, %ecx
movq (%edx), %xmm4
movhpd 24(%edx), %xmm2
movdqa -344(%ebp), %xmm1
movaps %xmm2, %xmm7
movhpd 16(%edx), %xmm4
movdqa %xmm1, %xmm6
pand %xmm4, %xmm6
movaps %xmm4, %xmm5
psrlq $52, %xmm4
psllq $12, %xmm7
movaps %xmm2, %xmm3
psrlq $26, %xmm5
por %xmm7, %xmm4
psrlq $14, %xmm3
pand %xmm1, %xmm5
pand %xmm1, %xmm4
pand %xmm1, %xmm3
psrlq $40, %xmm2
movdqu 32(%edx), %xmm1
movdqu 48(%edx), %xmm7
movdqa %xmm6, -152(%ebp)
movdqa %xmm1, %xmm6
punpckldq %xmm7, %xmm6
addl $64, %edx
punpckhdq %xmm7, %xmm1
movaps -328(%ebp), %xmm7
movaps %xmm0, -168(%ebp)
pmuludq -248(%ebp), %xmm0
pmuludq -264(%ebp), %xmm7
movdqa %xmm1, -72(%ebp)
movdqa -488(%ebp), %xmm1
pmuludq -280(%ebp), %xmm1
paddq %xmm7, %xmm0
movdqa -504(%ebp), %xmm7
pmuludq -296(%ebp), %xmm7
paddq %xmm1, %xmm0
movdqa -520(%ebp), %xmm1
paddq %xmm7, %xmm0
movdqa -312(%ebp), %xmm7
pmuludq %xmm7, %xmm1
por -456(%ebp), %xmm2
paddq %xmm1, %xmm0
movdqa -536(%ebp), %xmm1
pmuludq %xmm2, %xmm1
paddq %xmm1, %xmm0
movdqa -440(%ebp), %xmm1
pmuludq %xmm3, %xmm1
paddq %xmm1, %xmm0
movdqa -408(%ebp), %xmm1
pmuludq %xmm4, %xmm1
paddq %xmm1, %xmm0
movdqa -376(%ebp), %xmm1
movaps %xmm4, -120(%ebp)
movdqa %xmm1, %xmm4
pmuludq %xmm5, %xmm4
paddq %xmm4, %xmm0
movdqa -360(%ebp), %xmm4
movdqa %xmm5, -136(%ebp)
movdqa %xmm4, %xmm5
pmuludq -152(%ebp), %xmm5
pmuludq %xmm3, %xmm4
paddq %xmm5, %xmm0
pxor %xmm5, %xmm5
movdqa %xmm6, -88(%ebp)
punpckldq %xmm5, %xmm6
paddq %xmm6, %xmm0
movdqa %xmm0, -56(%ebp)
movaps -168(%ebp), %xmm6
movaps -328(%ebp), %xmm0
pmuludq -296(%ebp), %xmm6
pmuludq %xmm7, %xmm0
movdqa -488(%ebp), %xmm7
pmuludq -232(%ebp), %xmm7
paddq %xmm0, %xmm6
movdqa -504(%ebp), %xmm0
pmuludq -216(%ebp), %xmm0
paddq %xmm7, %xmm6
paddq %xmm0, %xmm6
movdqa -520(%ebp), %xmm0
movdqa %xmm0, %xmm7
pmuludq -200(%ebp), %xmm7
paddq %xmm7, %xmm6
movdqa %xmm1, %xmm7
pmuludq %xmm2, %xmm7
pmuludq -120(%ebp), %xmm1
paddq %xmm7, %xmm6
movdqa -392(%ebp), %xmm7
pmuludq -120(%ebp), %xmm7
paddq %xmm4, %xmm6
movdqa -424(%ebp), %xmm4
pmuludq -136(%ebp), %xmm4
paddq %xmm7, %xmm6
movdqa -472(%ebp), %xmm7
paddq %xmm4, %xmm6
movdqa -152(%ebp), %xmm4
pmuludq %xmm4, %xmm7
paddq %xmm7, %xmm6
movdqa -72(%ebp), %xmm7
punpckhdq %xmm5, %xmm7
psllq $18, %xmm7
movaps -328(%ebp), %xmm5
paddq %xmm7, %xmm6
pmuludq -280(%ebp), %xmm5
movaps -168(%ebp), %xmm7
pmuludq -264(%ebp), %xmm7
movdqa %xmm6, -40(%ebp)
movdqa -488(%ebp), %xmm6
pmuludq -296(%ebp), %xmm6
paddq %xmm5, %xmm7
movdqa -504(%ebp), %xmm5
paddq %xmm6, %xmm7
movdqa -312(%ebp), %xmm6
pmuludq %xmm6, %xmm5
paddq %xmm5, %xmm7
movdqa -232(%ebp), %xmm5
pmuludq %xmm5, %xmm0
paddq %xmm0, %xmm7
movdqa -440(%ebp), %xmm0
pmuludq %xmm2, %xmm0
paddq %xmm0, %xmm7
movdqa -408(%ebp), %xmm0
pmuludq %xmm3, %xmm0
movdqa %xmm3, -104(%ebp)
movdqa -360(%ebp), %xmm3
paddq %xmm0, %xmm7
movdqa %xmm3, %xmm0
pmuludq -136(%ebp), %xmm0
pmuludq %xmm2, %xmm3
pmuludq -408(%ebp), %xmm2
paddq %xmm1, %xmm7
movdqa -392(%ebp), %xmm1
paddq %xmm0, %xmm7
movdqa %xmm1, %xmm0
pmuludq %xmm4, %xmm0
paddq %xmm0, %xmm7
movdqa -88(%ebp), %xmm0
pxor %xmm4, %xmm4
punpckhdq %xmm4, %xmm0
psllq $6, %xmm0
movdqa -56(%ebp), %xmm4
paddq %xmm0, %xmm7
psrlq $26, %xmm4
paddq %xmm4, %xmm7
movdqa %xmm7, -24(%ebp)
movaps -168(%ebp), %xmm7
movaps %xmm7, %xmm0
pmuludq %xmm6, %xmm0
pmuludq -280(%ebp), %xmm7
movaps -328(%ebp), %xmm6
movaps %xmm6, %xmm4
pmuludq %xmm5, %xmm4
pmuludq -296(%ebp), %xmm6
paddq %xmm4, %xmm0
paddq %xmm6, %xmm7
movdqa -488(%ebp), %xmm4
movdqa %xmm4, %xmm5
pmuludq -216(%ebp), %xmm5
pmuludq -312(%ebp), %xmm4
paddq %xmm5, %xmm0
paddq %xmm4, %xmm7
movdqa -504(%ebp), %xmm5
pmuludq -200(%ebp), %xmm5
paddq %xmm5, %xmm0
movdqa -520(%ebp), %xmm5
pmuludq -184(%ebp), %xmm5
paddq %xmm5, %xmm0
movdqa -104(%ebp), %xmm5
pmuludq %xmm5, %xmm1
pmuludq -376(%ebp), %xmm5
paddq %xmm3, %xmm0
movdqa -424(%ebp), %xmm3
movdqa -504(%ebp), %xmm6
pmuludq -120(%ebp), %xmm3
pmuludq -232(%ebp), %xmm6
paddq %xmm1, %xmm0
paddq %xmm6, %xmm7
paddq %xmm3, %xmm0
movdqa -472(%ebp), %xmm1
movdqa -520(%ebp), %xmm4
pmuludq -136(%ebp), %xmm1
pmuludq -216(%ebp), %xmm4
paddq %xmm1, %xmm0
paddq %xmm4, %xmm7
movdqa -152(%ebp), %xmm3
pxor %xmm4, %xmm4
movdqa -568(%ebp), %xmm1
pmuludq %xmm3, %xmm1
pmuludq -424(%ebp), %xmm3
paddq %xmm2, %xmm7
paddq %xmm1, %xmm0
paddq %xmm5, %xmm7
paddq -456(%ebp), %xmm0
movaps -120(%ebp), %xmm2
pmuludq -360(%ebp), %xmm2
movdqa -136(%ebp), %xmm6
pmuludq -392(%ebp), %xmm6
paddq %xmm2, %xmm7
movdqa -40(%ebp), %xmm1
psrlq $26, %xmm1
paddq %xmm1, %xmm0
paddq %xmm6, %xmm7
movdqa -72(%ebp), %xmm1
movdqa %xmm0, %xmm5
punpckldq %xmm4, %xmm1
psrlq $26, %xmm5
paddq %xmm3, %xmm7
pmuludq -552(%ebp), %xmm5
psllq $12, %xmm1
paddq %xmm1, %xmm7
movdqa -24(%ebp), %xmm1
movdqa %xmm1, %xmm3
movdqa -344(%ebp), %xmm6
psrlq $26, %xmm3
movdqa -56(%ebp), %xmm2
pand %xmm6, %xmm1
pand %xmm6, %xmm2
pand %xmm6, %xmm0
paddq %xmm3, %xmm7
paddq %xmm5, %xmm2
movdqa %xmm6, %xmm5
movaps %xmm7, %xmm3
movdqa -40(%ebp), %xmm4
pand %xmm2, %xmm5
psrlq $26, %xmm2
pand %xmm6, %xmm4
psrlq $26, %xmm3
paddq %xmm2, %xmm1
paddq %xmm3, %xmm4
movdqa %xmm6, %xmm2
pand %xmm7, %xmm2
movdqa %xmm6, %xmm7
pand %xmm4, %xmm7
psrlq $26, %xmm4
movdqa %xmm5, -520(%ebp)
movdqa %xmm1, -504(%ebp)
movdqa %xmm2, -488(%ebp)
movaps %xmm7, -328(%ebp)
paddq %xmm4, %xmm0
cmpl $64, %ecx
jae poly1305_blocks_sse2_10
poly1305_blocks_sse2_11:
movdqa -376(%ebp), %xmm1
movdqa -408(%ebp), %xmm6
movdqa -440(%ebp), %xmm5
movdqa -536(%ebp), %xmm4
movaps -328(%ebp), %xmm3
poly1305_blocks_sse2_12:
cmpl $32, %ecx
jb poly1305_blocks_sse2_16
poly1305_blocks_sse2_13:
movaps %xmm3, -328(%ebp)
pmuludq %xmm0, %xmm4
pmuludq %xmm5, %xmm3
pmuludq %xmm0, %xmm5
paddq %xmm3, %xmm4
movdqa -488(%ebp), %xmm3
movdqa %xmm3, %xmm7
pmuludq %xmm6, %xmm7
movdqa -504(%ebp), %xmm2
pmuludq %xmm1, %xmm2
paddq %xmm7, %xmm4
movdqa -360(%ebp), %xmm7
paddq %xmm2, %xmm4
movdqa -520(%ebp), %xmm2
pmuludq %xmm7, %xmm2
paddq %xmm2, %xmm4
movdqa %xmm4, -536(%ebp)
movaps -328(%ebp), %xmm4
movaps %xmm4, %xmm2
pmuludq %xmm6, %xmm2
pmuludq %xmm0, %xmm6
paddq %xmm2, %xmm5
movdqa %xmm3, %xmm2
pmuludq %xmm1, %xmm2
paddq %xmm2, %xmm5
movdqa -504(%ebp), %xmm2
pmuludq %xmm7, %xmm2
movdqa -520(%ebp), %xmm7
paddq %xmm2, %xmm5
movdqa -392(%ebp), %xmm2
pmuludq %xmm2, %xmm7
paddq %xmm7, %xmm5
movdqa %xmm5, -440(%ebp)
movaps %xmm4, %xmm5
pmuludq %xmm1, %xmm5
movdqa %xmm3, %xmm7
pmuludq %xmm0, %xmm1
paddq %xmm5, %xmm6
movdqa -360(%ebp), %xmm5
pmuludq %xmm5, %xmm7
pmuludq %xmm5, %xmm0
paddq %xmm7, %xmm6
movdqa -504(%ebp), %xmm7
pmuludq %xmm2, %xmm7
movdqa -520(%ebp), %xmm2
paddq %xmm7, %xmm6
movdqa -424(%ebp), %xmm7
pmuludq %xmm7, %xmm2
paddq %xmm2, %xmm6
movdqa %xmm6, -408(%ebp)
movaps %xmm4, %xmm6
pmuludq %xmm5, %xmm6
paddq %xmm6, %xmm1
movdqa -392(%ebp), %xmm2
movdqa %xmm3, %xmm6
pmuludq %xmm2, %xmm6
pmuludq %xmm2, %xmm4
pmuludq -424(%ebp), %xmm3
paddq %xmm6, %xmm1
paddq %xmm4, %xmm0
movdqa -504(%ebp), %xmm6
pmuludq %xmm7, %xmm6
paddq %xmm3, %xmm0
paddq %xmm6, %xmm1
movdqa -472(%ebp), %xmm6
movdqa -504(%ebp), %xmm4
pmuludq %xmm6, %xmm4
movdqa -520(%ebp), %xmm7
movdqa -520(%ebp), %xmm2
pmuludq -568(%ebp), %xmm2
pmuludq %xmm6, %xmm7
paddq %xmm4, %xmm0
paddq %xmm7, %xmm1
paddq %xmm2, %xmm0
movdqa -536(%ebp), %xmm4
movdqa -440(%ebp), %xmm5
movdqa -408(%ebp), %xmm6
testl %edx, %edx
je poly1305_blocks_sse2_15
poly1305_blocks_sse2_14:
movdqu (%edx), %xmm2
movdqu 16(%edx), %xmm7
movdqa %xmm2, %xmm3
movaps %xmm0, -168(%ebp)
punpckldq %xmm7, %xmm3
pxor %xmm0, %xmm0
punpckhdq %xmm7, %xmm2
movdqa %xmm3, %xmm7
punpckhdq %xmm0, %xmm3
psllq $6, %xmm3
paddq %xmm3, %xmm5
movdqa %xmm2, %xmm3
punpckldq %xmm0, %xmm3
punpckhdq %xmm0, %xmm2
psllq $12, %xmm3
punpckldq %xmm0, %xmm7
psllq $18, %xmm2
movaps -168(%ebp), %xmm0
paddq -456(%ebp), %xmm0
paddq %xmm7, %xmm4
paddq %xmm3, %xmm6
paddq %xmm2, %xmm1
poly1305_blocks_sse2_15:
movdqa %xmm1, %xmm7
movdqa %xmm4, %xmm3
psrlq $26, %xmm7
psrlq $26, %xmm3
paddq %xmm7, %xmm0
paddq %xmm3, %xmm5
movaps %xmm0, %xmm3
movdqa %xmm5, %xmm2
psrlq $26, %xmm3
psrlq $26, %xmm2
pmuludq -552(%ebp), %xmm3
paddq %xmm2, %xmm6
movdqa -344(%ebp), %xmm2
movdqa %xmm6, %xmm7
pand %xmm2, %xmm4
pand %xmm2, %xmm1
paddq %xmm3, %xmm4
psrlq $26, %xmm7
movdqa %xmm2, %xmm3
paddq %xmm7, %xmm1
pand %xmm4, %xmm3
pand %xmm2, %xmm5
psrlq $26, %xmm4
pand %xmm2, %xmm0
movdqa %xmm3, -520(%ebp)
movdqa %xmm2, %xmm3
paddq %xmm4, %xmm5
movdqa %xmm2, %xmm4
pand %xmm1, %xmm3
psrlq $26, %xmm1
pand %xmm6, %xmm4
movdqa %xmm5, -504(%ebp)
movdqa %xmm4, -488(%ebp)
paddq %xmm1, %xmm0
poly1305_blocks_sse2_16:
testl %edx, %edx
je poly1305_blocks_sse2_19
poly1305_blocks_sse2_17:
movdqa -520(%ebp), %xmm1
movdqa -504(%ebp), %xmm2
movdqa -488(%ebp), %xmm4
movdqa %xmm1, (%eax)
movdqa %xmm2, 16(%eax)
movdqa %xmm4, 32(%eax)
movdqa %xmm3, 48(%eax)
movdqa %xmm0, 64(%eax)
poly1305_blocks_sse2_18:
movl -4(%ebp), %esi
movl -8(%ebp), %edi
addl $(568+8+63), %esp
popl %ebp
ret
poly1305_blocks_sse2_19:
movdqa -520(%ebp), %xmm2
movdqa %xmm2, %xmm1
movdqa -504(%ebp), %xmm5
psrldq $8, %xmm1
movdqa %xmm5, %xmm4
paddq %xmm1, %xmm2
psrldq $8, %xmm4
movaps %xmm3, %xmm1
movd %xmm2, %ecx
movdqa -488(%ebp), %xmm7
paddq %xmm4, %xmm5
movdqa %xmm7, %xmm6
movl %ecx, %edx
movd %xmm5, %esi
andl $67108863, %ecx
psrldq $8, %xmm6
paddq %xmm6, %xmm7
shrl $26, %edx
psrldq $8, %xmm1
addl %edx, %esi
movd %xmm7, %edx
movl %esi, %edi
paddq %xmm1, %xmm3
shrl $26, %edi
andl $67108863, %esi
movl %eax, -568(%ebp)
addl %edi, %edx
movd %xmm3, %eax
movaps %xmm0, %xmm3
psrldq $8, %xmm3
paddq %xmm3, %xmm0
movl %edx, %edi
andl $67108863, %edx
shrl $26, %edi
addl %edi, %eax
movd %xmm0, %edi
movl %eax, -564(%ebp)
shrl $26, %eax
addl %eax, %edi
movl %edi, %eax
andl $67108863, %edi
shrl $26, %eax
lea (%eax,%eax,4), %eax
addl %eax, %ecx
movl %ecx, %eax
andl $67108863, %ecx
shrl $26, %eax
addl %eax, %esi
movl %esi, %eax
andl $67108863, %esi
shrl $26, %eax
addl %eax, %edx
movl %edx, %eax
andl $67108863, %eax
movl %eax, -560(%ebp)
movl -564(%ebp), %eax
shrl $26, %edx
andl $67108863, %eax
addl %edx, %eax
movl %eax, %edx
shrl $26, %eax
andl $67108863, %edx
addl %eax, %edi
movl %edi, %eax
shrl $26, %edi
andl $67108863, %eax
movl %eax, -552(%ebp)
movl %edx, -556(%ebp)
lea (%edi,%edi,4), %edi
addl %edi, %ecx
movl %ecx, %edi
andl $67108863, %edi
shrl $26, %ecx
addl %ecx, %esi
movl %esi, -548(%ebp)
lea 5(%edi), %ecx
movl %ecx, -544(%ebp)
shrl $26, %ecx
addl %esi, %ecx
movl %ecx, -540(%ebp)
shrl $26, %ecx
movl -560(%ebp), %esi
addl %esi, %ecx
movl %ecx, -536(%ebp)
shrl $26, %ecx
addl %edx, %ecx
movl %ecx, -532(%ebp)
shrl $26, %ecx
movl -544(%ebp), %edx
andl $67108863, %edx
lea -67108864(%ecx,%eax), %eax
movl %eax, -528(%ebp)
shrl $31, %eax
decl %eax
movl %eax, %ecx
andl %eax, %edx
notl %ecx
andl %ecx, %edi
orl %edx, %edi
movl -568(%ebp), %edx
movl %ecx, -524(%ebp)
movl %edi, (%edx)
movl -548(%ebp), %edi
andl %ecx, %edi
movl -540(%ebp), %ecx
andl $67108863, %ecx
andl %eax, %ecx
orl %ecx, %edi
movl -536(%ebp), %ecx
andl $67108863, %ecx
movl %edi, 4(%edx)
andl %eax, %ecx
movl -524(%ebp), %edi
andl %edi, %esi
orl %ecx, %esi
movl %esi, 8(%edx)
movl -532(%ebp), %esi
andl $67108863, %esi
movl -556(%ebp), %ecx
andl %eax, %esi
andl %edi, %ecx
orl %esi, %ecx
movl -528(%ebp), %esi
andl -552(%ebp), %edi
andl %eax, %esi
orl %esi, %edi
movl %ecx, 12(%edx)
movl %edi, 16(%edx)
jmp poly1305_blocks_sse2_18
ENDFN(poly1305_blocks)


FN(poly1305_finish_ext)
poly1305_finish_ext_sse2_local:
pushl %ebp
movl %esp, %ebp
andl $-64, %esp
pushl %esi
pushl %edi
pushl %ebx
subl $116, %esp
pxor %xmm0, %xmm0
movaps %xmm0, 64(%esp)
movaps %xmm0, 80(%esp)
poly1305_finish_ext_sse2_2:
movl 16(%ebp), %ebx
movl 8(%ebp), %esi
testl %ebx, %ebx
je poly1305_finish_ext_sse2_19
poly1305_finish_ext_sse2_3:
movl 12(%ebp), %edx
testb $16, %bl
je poly1305_finish_ext_sse2_5
poly1305_finish_ext_sse2_4:
movl $16, %eax
movdqu (%edx), %xmm0
movdqa %xmm0, 64(%esp)
jmp poly1305_finish_ext_sse2_6
poly1305_finish_ext_sse2_5:
xorl %eax, %eax
poly1305_finish_ext_sse2_6:
testb $8, %bl
je poly1305_finish_ext_sse2_8
poly1305_finish_ext_sse2_7:
movl (%eax,%edx), %ecx
movl 4(%eax,%edx), %edi
movl %ecx, 64(%esp,%eax)
movl %edi, 68(%esp,%eax)
addl $8, %eax
poly1305_finish_ext_sse2_8:
testb $4, %bl
je poly1305_finish_ext_sse2_10
poly1305_finish_ext_sse2_9:
movl (%eax,%edx), %ecx
movl %ecx, 64(%esp,%eax)
addl $4, %eax
poly1305_finish_ext_sse2_10:
testb $2, %bl
je poly1305_finish_ext_sse2_12
poly1305_finish_ext_sse2_11:
movzwl (%eax,%edx), %ecx
movw %cx, 64(%esp,%eax)
addl $2, %eax
poly1305_finish_ext_sse2_12:
testb $1, %bl
je poly1305_finish_ext_sse2_14
poly1305_finish_ext_sse2_13:
movzbl (%eax,%edx), %edx
movb %dl, 64(%esp,%eax)
poly1305_finish_ext_sse2_14:
cmpl $16, %ebx
je poly1305_finish_ext_sse2_17
poly1305_finish_ext_sse2_15:
movb $1, 64(%esp,%ebx)
jae poly1305_finish_ext_sse2_17
poly1305_finish_ext_sse2_16:
movl $8, %eax
jmp poly1305_finish_ext_sse2_18
poly1305_finish_ext_sse2_17:
movl $4, %eax
poly1305_finish_ext_sse2_18:
orl %eax, 240(%esi)
movl %esi, %eax
movl $32, %ecx
lea 64(%esp), %edx
call poly1305_blocks_sse2_local
poly1305_finish_ext_sse2_19:
testb $1, 240(%esi)
je poly1305_finish_ext_sse2_25
poly1305_finish_ext_sse2_20:
testl %ebx, %ebx
je poly1305_finish_ext_sse2_22
poly1305_finish_ext_sse2_21:
cmpl $16, %ebx
jbe poly1305_finish_ext_sse2_23
poly1305_finish_ext_sse2_22:
movl 84(%esi), %eax
movl 92(%esi), %edx
movl 100(%esi), %ecx
movl 108(%esi), %ebx
movl 116(%esi), %edi
movl %eax, 88(%esi)
movl %edx, 104(%esi)
movl %ecx, 120(%esi)
movl %ebx, 136(%esi)
movl %edi, 152(%esi)
jmp poly1305_finish_ext_sse2_24
poly1305_finish_ext_sse2_23:
movl 84(%esi), %eax
movl 92(%esi), %edx
movl 100(%esi), %ecx
movl 108(%esi), %ebx
movl 116(%esi), %edi
movl %eax, 80(%esi)
xorl %eax, %eax
movl $1, 88(%esi)
movl %edx, 96(%esi)
movl %eax, 104(%esi)
movl %ecx, 112(%esi)
movl %eax, 120(%esi)
movl %ebx, 128(%esi)
movl %eax, 136(%esi)
movl %edi, 144(%esi)
movl %eax, 152(%esi)
poly1305_finish_ext_sse2_24:
movl %esi, %eax
xorl %edx, %edx
movl $32, %ecx
call poly1305_blocks_sse2_local
poly1305_finish_ext_sse2_25:
movl 20(%ebp), %edi
movl 8(%esi), %eax
movl %edi, 96(%esp)
movl %eax, %edi
movl 4(%esi), %ecx
movl %ecx, %edx
shrl $6, %ecx
shll $20, %edi
movl 12(%esi), %ebx
orl %edi, %ecx
movl %ebx, %edi
shrl $12, %eax
shll $14, %edi
pxor %xmm0, %xmm0
orl %edi, %eax
movl 16(%esi), %edi
shll $26, %edx
shrl $18, %ebx
shll $8, %edi
orl (%esi), %edx
orl %edi, %ebx
movdqa %xmm0, 0(%esi)
movdqa %xmm0, 16(%esi)
movdqa %xmm0, 32(%esi)
movdqa %xmm0, 48(%esi)
movdqa %xmm0, 64(%esi)
movdqa %xmm0, 80(%esi)
movdqa %xmm0, 96(%esi)
addl 124(%esi), %edx
movdqa %xmm0, 112(%esi)
adcl 132(%esi), %ecx
adcl 140(%esi), %eax
movdqa %xmm0, 128(%esi)
adcl 148(%esi), %ebx
movdqa %xmm0, 144(%esi)
movdqa %xmm0, 160(%esi)
movdqa %xmm0, 176(%esi)
movdqa %xmm0, 192(%esi)
movdqa %xmm0, 208(%esi)
movdqa %xmm0, 224(%esi)
movl 96(%esp), %edi
movl %ebx, 12(%edi)
movl %eax, 8(%edi)
movl %ecx, 4(%edi)
movl %edx, (%edi)
addl $116, %esp
popl %ebx
popl %edi
popl %esi
movl %ebp, %esp
popl %ebp
ret
ENDFN(poly1305_finish_ext)


FN(poly1305_auth)
pushl %ebp
pushl %edi
movl %esp, %ebp
subl $244, %esp
andl $~63, %esp
movl %esp, %edi
movl %edi, %eax
movl 24(%ebp), %edx
movl 20(%ebp), %ecx
subl $16, %esp
movl $0, 12(%esp)
call poly1305_init_ext_sse2_local
movl 20(%ebp), %ecx
andl $~31, %ecx
jz poly1305_auth_sse2_no_data
movl %edi, %eax
movl 16(%ebp), %edx
addl %ecx, 16(%ebp)
call poly1305_blocks_sse2_local
poly1305_auth_sse2_no_data:
pushl 12(%ebp)
movl 20(%ebp), %ecx
andl $31, %ecx
pushl %ecx
pushl 16(%ebp)
pushl %edi
call poly1305_finish_ext_sse2_local
movl %ebp, %esp
popl %edi
popl %ebp
ret
ENDFN(poly1305_auth)

ENDFILE()
