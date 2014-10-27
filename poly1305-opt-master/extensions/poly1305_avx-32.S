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
movl 4(%esp), %eax
movl 8(%esp), %edx
movl 12(%esp), %ecx
poly1305_init_ext_avx_local:
pushl %esi
pushl %edi
pushl %ebx
pushl %ebp
movl 32(%esp), %edi
subl $76, %esp
movl $-1, %ebx
testl %ecx, %ecx
vpxor %xmm0, %xmm0, %xmm0
cmove %ebx, %ecx
vmovdqu %xmm0, (%eax)
testl %edi, %edi
vmovdqu %xmm0, 16(%eax)
vmovdqu %xmm0, 32(%eax)
vmovdqu %xmm0, 48(%eax)
cmovnz %ebx, %ecx
vmovdqu %xmm0, 64(%eax)
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
jb poly1305_init_ext_avx_7
xorl %edx, %edx
movl %edx, 16(%esp)
movl %esi, 20(%esp)
movl %ebx, 28(%esp)
movl %ecx, 24(%esp)
movl %ebp, (%esp)
movl %eax, 4(%esp)
poly1305_init_ext_avx_3:
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
jae poly1305_init_ext_avx_6
cmpl $0, 16(%esp)
je poly1305_init_ext_avx_3
cmpl $64, (%esp)
jae poly1305_init_ext_avx_3
poly1305_init_ext_avx_6:
movl %edi, %eax
poly1305_init_ext_avx_7:
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
poly1305_blocks_avx_local:
pushl %ebp
movl %esp, %ebp
subl $584, %esp
movl %esi, -4(%ebp)
movl $16777216, %esi
movl %edi, -8(%ebp)
movl $67108863, %edi
vmovd %esi, %xmm3
movl $5, %esi
vmovd %edi, %xmm0
vpshufd $68, %xmm0, %xmm1
vpshufd $68, %xmm3, %xmm3
vmovd %esi, %xmm2
movl 240(%eax), %esi
vpshufd $68, %xmm2, %xmm4
vmovdqu %xmm1, -360(%ebp)
vmovdqu %xmm4, -536(%ebp)
testl $4, %esi
je poly1305_blocks_avx_3
poly1305_blocks_avx_2:
vpsrldq $8, %xmm3, %xmm3
poly1305_blocks_avx_3:
testl $8, %esi
je poly1305_blocks_avx_5
poly1305_blocks_avx_4:
vpxor %xmm3, %xmm3, %xmm3
poly1305_blocks_avx_5:
btsl $0, %esi
jc poly1305_blocks_avx_7
poly1305_blocks_avx_6:
vmovq (%edx), %xmm6
addl $-32, %ecx
vmovhpd 16(%edx), %xmm6, %xmm5
vmovq 8(%edx), %xmm2
vpsrlq $26, %xmm5, %xmm1
vmovhpd 24(%edx), %xmm2, %xmm0
vpsrlq $52, %xmm5, %xmm7
vmovdqu -360(%ebp), %xmm4
addl $32, %edx
vpand %xmm1, %xmm4, %xmm2
vpsllq $12, %xmm0, %xmm1
vpand %xmm5, %xmm4, %xmm6
vpor %xmm1, %xmm7, %xmm5
vpsrlq $26, %xmm5, %xmm7
vpsrlq $40, %xmm0, %xmm0
movl %esi, 240(%eax)
vpand %xmm5, %xmm4, %xmm1
vpand %xmm7, %xmm4, %xmm4
vpor %xmm3, %xmm0, %xmm0
jmp poly1305_blocks_avx_8
poly1305_blocks_avx_7:
vmovdqu (%eax), %xmm6
vmovdqu 16(%eax), %xmm2
vmovdqu 32(%eax), %xmm1
vmovdqu 48(%eax), %xmm4
vmovdqu 64(%eax), %xmm0
poly1305_blocks_avx_8:
vmovdqu 80(%eax), %xmm7
vmovdqu %xmm1, -344(%ebp)
vmovdqu %xmm4, -200(%ebp)
vmovdqu %xmm0, -184(%ebp)
vmovdqu %xmm7, -376(%ebp)
vmovdqu 128(%eax), %xmm0
vmovdqu 144(%eax), %xmm1
vmovdqu -536(%ebp), %xmm4
vmovdqu 96(%eax), %xmm7
vmovdqu 112(%eax), %xmm5
vmovdqu %xmm0, -472(%ebp)
vmovdqu %xmm1, -520(%ebp)
vpmuludq %xmm4, %xmm0, %xmm0
vpmuludq %xmm4, %xmm1, %xmm1
vmovdqu %xmm7, -392(%ebp)
vmovdqu %xmm5, -440(%ebp)
vpmuludq %xmm4, %xmm7, %xmm7
vpmuludq %xmm4, %xmm5, %xmm5
vmovdqu %xmm0, -424(%ebp)
vmovdqu %xmm1, -408(%ebp)
vmovdqu %xmm7, -504(%ebp)
vmovdqu %xmm5, -456(%ebp)
vmovdqu -184(%ebp), %xmm0
vmovdqu -200(%ebp), %xmm4
vmovdqu -344(%ebp), %xmm1
cmpl $64, %ecx
jb poly1305_blocks_avx_12
poly1305_blocks_avx_9:
vmovdqu 160(%eax), %xmm7
vmovdqu %xmm1, -344(%ebp)
vmovdqu %xmm4, -200(%ebp)
vmovdqu %xmm0, -184(%ebp)
vmovdqu %xmm7, -280(%ebp)
vmovdqu 208(%eax), %xmm0
vmovdqu 224(%eax), %xmm1
vmovdqu -536(%ebp), %xmm4
vmovdqu 176(%eax), %xmm7
vmovdqu 192(%eax), %xmm5
vmovdqu %xmm0, -248(%ebp)
vmovdqu %xmm1, -264(%ebp)
vpmuludq %xmm4, %xmm0, %xmm0
vpmuludq %xmm4, %xmm1, %xmm1
vmovdqu %xmm7, -216(%ebp)
vmovdqu %xmm5, -232(%ebp)
vpmuludq %xmm4, %xmm7, %xmm7
vpmuludq %xmm4, %xmm5, %xmm5
vmovdqu %xmm0, -568(%ebp)
vmovdqu %xmm1, -584(%ebp)
vmovdqu %xmm7, -296(%ebp)
vmovdqu %xmm5, -552(%ebp)
vmovdqu -184(%ebp), %xmm0
vmovdqu -200(%ebp), %xmm4
vmovdqu -344(%ebp), %xmm1
vmovdqu %xmm3, -488(%ebp)
poly1305_blocks_avx_10:
vmovdqu %xmm6, -312(%ebp)
addl $-64, %ecx
vmovq (%edx), %xmm6
vmovhpd 16(%edx), %xmm6, %xmm7
vmovq 8(%edx), %xmm3
vmovdqu -360(%ebp), %xmm6
vmovhpd 24(%edx), %xmm3, %xmm5
vpand %xmm7, %xmm6, %xmm3
vmovdqu %xmm3, -168(%ebp)
vpsrlq $26, %xmm7, %xmm3
vpand %xmm3, %xmm6, %xmm3
vpsrlq $52, %xmm7, %xmm7
vmovdqu %xmm3, -152(%ebp)
vpsllq $12, %xmm5, %xmm3
vpor %xmm3, %xmm7, %xmm7
vpsrlq $14, %xmm5, %xmm3
vpand %xmm7, %xmm6, %xmm7
vpsrlq $40, %xmm5, %xmm5
vmovdqu %xmm7, -136(%ebp)
vpand %xmm3, %xmm6, %xmm7
vmovdqu 32(%edx), %xmm3
vpunpckldq 48(%edx), %xmm3, %xmm6
vpunpckhdq 48(%edx), %xmm3, %xmm3
addl $64, %edx
vmovdqu %xmm0, -184(%ebp)
vmovdqu %xmm3, -72(%ebp)
vpmuludq -296(%ebp), %xmm0, %xmm0
vpmuludq -552(%ebp), %xmm4, %xmm3
vmovdqu %xmm1, -344(%ebp)
vpmuludq -568(%ebp), %xmm1, %xmm1
vpaddq %xmm3, %xmm0, %xmm0
vpaddq %xmm1, %xmm0, %xmm1
vmovdqu -584(%ebp), %xmm0
vmovdqu %xmm2, -328(%ebp)
vpmuludq %xmm0, %xmm2, %xmm2
vpaddq %xmm2, %xmm1, %xmm2
vmovdqu -280(%ebp), %xmm1
vpmuludq -312(%ebp), %xmm1, %xmm3
vpmuludq %xmm1, %xmm4, %xmm1
vpaddq %xmm3, %xmm2, %xmm2
vpor -488(%ebp), %xmm5, %xmm5
vpmuludq -504(%ebp), %xmm5, %xmm3
vmovdqu %xmm7, -120(%ebp)
vpmuludq -456(%ebp), %xmm7, %xmm7
vpaddq %xmm3, %xmm2, %xmm2
vpaddq %xmm7, %xmm2, %xmm7
vmovdqu -136(%ebp), %xmm2
vpmuludq -424(%ebp), %xmm2, %xmm3
vmovdqu -152(%ebp), %xmm2
vpaddq %xmm3, %xmm7, %xmm7
vpmuludq -408(%ebp), %xmm2, %xmm3
vmovdqu -376(%ebp), %xmm2
vpaddq %xmm3, %xmm7, %xmm7
vpmuludq -168(%ebp), %xmm2, %xmm3
vpmuludq -120(%ebp), %xmm2, %xmm2
vpaddq %xmm3, %xmm7, %xmm7
vpxor %xmm3, %xmm3, %xmm3
vmovdqu %xmm6, -88(%ebp)
vpunpckldq %xmm3, %xmm6, %xmm6
vpaddq %xmm6, %xmm7, %xmm6
vmovdqu -184(%ebp), %xmm7
vpmuludq %xmm0, %xmm7, %xmm0
vmovdqu %xmm6, -56(%ebp)
vmovdqu -344(%ebp), %xmm6
vpaddq %xmm1, %xmm0, %xmm0
vpmuludq -216(%ebp), %xmm6, %xmm1
vmovdqu -328(%ebp), %xmm6
vpaddq %xmm1, %xmm0, %xmm0
vpmuludq -232(%ebp), %xmm6, %xmm1
vmovdqu -312(%ebp), %xmm6
vpaddq %xmm1, %xmm0, %xmm0
vpmuludq -248(%ebp), %xmm6, %xmm1
vpaddq %xmm1, %xmm0, %xmm0
vpmuludq -408(%ebp), %xmm5, %xmm1
vpaddq %xmm1, %xmm0, %xmm0
vmovdqu -136(%ebp), %xmm1
vpaddq %xmm2, %xmm0, %xmm2
vpmuludq -392(%ebp), %xmm1, %xmm0
vmovdqu -152(%ebp), %xmm1
vpaddq %xmm0, %xmm2, %xmm2
vpmuludq -440(%ebp), %xmm1, %xmm0
vpaddq %xmm0, %xmm2, %xmm1
vmovdqu -168(%ebp), %xmm2
vpmuludq -472(%ebp), %xmm2, %xmm0
vpaddq %xmm0, %xmm1, %xmm1
vmovdqu -72(%ebp), %xmm0
vpunpckhdq %xmm3, %xmm0, %xmm0
vpsllq $18, %xmm0, %xmm0
vpaddq %xmm0, %xmm1, %xmm1
vmovdqu %xmm4, -200(%ebp)
vmovdqu %xmm1, -40(%ebp)
vpmuludq -552(%ebp), %xmm7, %xmm1
vpmuludq -568(%ebp), %xmm4, %xmm4
vpaddq %xmm4, %xmm1, %xmm4
vmovdqu -344(%ebp), %xmm1
vpmuludq -584(%ebp), %xmm1, %xmm0
vpaddq %xmm0, %xmm4, %xmm1
vmovdqu -280(%ebp), %xmm4
vpmuludq -328(%ebp), %xmm4, %xmm0
vpaddq %xmm0, %xmm1, %xmm0
vmovdqu -216(%ebp), %xmm1
vpmuludq %xmm1, %xmm6, %xmm6
vmovdqu %xmm5, -104(%ebp)
vpmuludq -456(%ebp), %xmm5, %xmm5
vpaddq %xmm6, %xmm0, %xmm6
vpaddq %xmm5, %xmm6, %xmm0
vmovdqu -120(%ebp), %xmm6
vpmuludq -424(%ebp), %xmm6, %xmm5
vmovdqu -136(%ebp), %xmm6
vpaddq %xmm5, %xmm0, %xmm0
vpmuludq -408(%ebp), %xmm6, %xmm5
vmovdqu -376(%ebp), %xmm6
vpaddq %xmm5, %xmm0, %xmm0
vpmuludq -152(%ebp), %xmm6, %xmm5
vpaddq %xmm5, %xmm0, %xmm0
vmovdqu -392(%ebp), %xmm5
vpmuludq %xmm5, %xmm2, %xmm2
vpmuludq -120(%ebp), %xmm5, %xmm5
vpaddq %xmm2, %xmm0, %xmm2
vmovdqu -88(%ebp), %xmm0
vpunpckhdq %xmm3, %xmm0, %xmm3
vpsllq $6, %xmm3, %xmm3
vpaddq %xmm3, %xmm2, %xmm0
vmovdqu -56(%ebp), %xmm2
vpsrlq $26, %xmm2, %xmm2
vpaddq %xmm2, %xmm0, %xmm3
vpmuludq %xmm4, %xmm7, %xmm0
vpmuludq -568(%ebp), %xmm7, %xmm7
vmovdqu -200(%ebp), %xmm2
vpmuludq %xmm1, %xmm2, %xmm1
vpmuludq -584(%ebp), %xmm2, %xmm2
vpaddq %xmm1, %xmm0, %xmm1
vpaddq %xmm2, %xmm7, %xmm7
vmovdqu -344(%ebp), %xmm0
vmovdqu %xmm3, -24(%ebp)
vpmuludq -232(%ebp), %xmm0, %xmm3
vpmuludq %xmm4, %xmm0, %xmm4
vpaddq %xmm3, %xmm1, %xmm1
vmovdqu -328(%ebp), %xmm3
vpmuludq -248(%ebp), %xmm3, %xmm3
vpaddq %xmm3, %xmm1, %xmm1
vmovdqu -312(%ebp), %xmm3
vpmuludq -264(%ebp), %xmm3, %xmm3
vpaddq %xmm3, %xmm1, %xmm3
vmovdqu -104(%ebp), %xmm1
vpmuludq %xmm6, %xmm1, %xmm6
vpmuludq -424(%ebp), %xmm1, %xmm1
vpaddq %xmm6, %xmm3, %xmm6
vpaddq %xmm5, %xmm6, %xmm3
vmovdqu -136(%ebp), %xmm6
vpmuludq -440(%ebp), %xmm6, %xmm5
vpmuludq -376(%ebp), %xmm6, %xmm6
vpaddq %xmm5, %xmm3, %xmm3
vmovdqu -152(%ebp), %xmm5
vpmuludq -472(%ebp), %xmm5, %xmm5
vpaddq %xmm5, %xmm3, %xmm5
vmovdqu -168(%ebp), %xmm3
vpmuludq -520(%ebp), %xmm3, %xmm3
vpaddq %xmm3, %xmm5, %xmm5
vpaddq -488(%ebp), %xmm5, %xmm3
vmovdqu -40(%ebp), %xmm5
vmovdqu -328(%ebp), %xmm0
vpsrlq $26, %xmm5, %xmm5
vpaddq %xmm5, %xmm3, %xmm3
vpaddq %xmm4, %xmm7, %xmm5
vpmuludq -216(%ebp), %xmm0, %xmm4
vmovdqu -312(%ebp), %xmm2
vpmuludq -232(%ebp), %xmm2, %xmm0
vpaddq %xmm4, %xmm5, %xmm7
vpaddq %xmm0, %xmm7, %xmm5
vpaddq %xmm1, %xmm5, %xmm4
vmovdqu -120(%ebp), %xmm1
vpmuludq -408(%ebp), %xmm1, %xmm2
vmovdqu -152(%ebp), %xmm1
vpaddq %xmm2, %xmm4, %xmm7
vpmuludq -392(%ebp), %xmm1, %xmm4
vpaddq %xmm6, %xmm7, %xmm6
vmovdqu -168(%ebp), %xmm2
vpxor %xmm1, %xmm1, %xmm1
vpmuludq -440(%ebp), %xmm2, %xmm0
vpaddq %xmm4, %xmm6, %xmm7
vmovdqu -72(%ebp), %xmm5
vpunpckldq %xmm1, %xmm5, %xmm6
vpsrlq $26, %xmm3, %xmm5
vpaddq %xmm0, %xmm7, %xmm4
vpsllq $12, %xmm6, %xmm2
vpaddq %xmm2, %xmm4, %xmm7
vpmuludq -536(%ebp), %xmm5, %xmm4
vmovdqu -24(%ebp), %xmm2
vpsrlq $26, %xmm2, %xmm0
vpaddq %xmm0, %xmm7, %xmm1
vmovdqu -360(%ebp), %xmm0
vpsrlq $26, %xmm1, %xmm5
vpand -56(%ebp), %xmm0, %xmm6
vpand %xmm0, %xmm2, %xmm2
vpand -40(%ebp), %xmm0, %xmm7
vpand %xmm0, %xmm1, %xmm1
vpaddq %xmm4, %xmm6, %xmm4
vpaddq %xmm5, %xmm7, %xmm7
vpand %xmm0, %xmm4, %xmm6
vpsrlq $26, %xmm4, %xmm4
vpaddq %xmm4, %xmm2, %xmm2
vpand %xmm0, %xmm7, %xmm4
vpand %xmm0, %xmm3, %xmm3
vpsrlq $26, %xmm7, %xmm0
vpaddq %xmm0, %xmm3, %xmm0
cmpl $64, %ecx
jae poly1305_blocks_avx_10
poly1305_blocks_avx_11:
vmovdqu -488(%ebp), %xmm3
poly1305_blocks_avx_12:
cmpl $32, %ecx
jb poly1305_blocks_avx_16
poly1305_blocks_avx_13:
vmovdqu %xmm3, -488(%ebp)
vmovdqu -456(%ebp), %xmm3
vpmuludq -504(%ebp), %xmm0, %xmm5
vpmuludq %xmm3, %xmm4, %xmm7
vpmuludq %xmm3, %xmm0, %xmm3
vpaddq %xmm7, %xmm5, %xmm7
vmovdqu -424(%ebp), %xmm5
vmovdqu %xmm1, -344(%ebp)
vpmuludq %xmm5, %xmm1, %xmm1
vpaddq %xmm1, %xmm7, %xmm7
vmovdqu -408(%ebp), %xmm1
vmovdqu %xmm2, -328(%ebp)
vpmuludq %xmm1, %xmm2, %xmm2
vpaddq %xmm2, %xmm7, %xmm7
vmovdqu -376(%ebp), %xmm2
vmovdqu %xmm6, -312(%ebp)
vpmuludq %xmm2, %xmm6, %xmm6
vpaddq %xmm6, %xmm7, %xmm7
vmovdqu %xmm7, -584(%ebp)
vpmuludq %xmm5, %xmm4, %xmm7
vpmuludq %xmm5, %xmm0, %xmm5
vpaddq %xmm7, %xmm3, %xmm6
vpmuludq -344(%ebp), %xmm1, %xmm3
vpaddq %xmm3, %xmm6, %xmm7
vpmuludq -328(%ebp), %xmm2, %xmm6
vpaddq %xmm6, %xmm7, %xmm3
vmovdqu -392(%ebp), %xmm6
vpmuludq -312(%ebp), %xmm6, %xmm7
vpaddq %xmm7, %xmm3, %xmm3
vpmuludq %xmm1, %xmm4, %xmm7
vpmuludq %xmm1, %xmm0, %xmm1
vpmuludq %xmm2, %xmm0, %xmm0
vpaddq %xmm7, %xmm5, %xmm5
vmovdqu %xmm3, -568(%ebp)
vpmuludq -344(%ebp), %xmm2, %xmm3
vpaddq %xmm3, %xmm5, %xmm7
vpmuludq -328(%ebp), %xmm6, %xmm5
vpaddq %xmm5, %xmm7, %xmm3
vmovdqu -440(%ebp), %xmm5
vpmuludq -312(%ebp), %xmm5, %xmm7
vpaddq %xmm7, %xmm3, %xmm3
vpmuludq %xmm2, %xmm4, %xmm7
vpmuludq %xmm6, %xmm4, %xmm4
vpmuludq -344(%ebp), %xmm5, %xmm2
vpaddq %xmm7, %xmm1, %xmm1
vpaddq %xmm4, %xmm0, %xmm0
vpmuludq -328(%ebp), %xmm5, %xmm7
vmovdqu %xmm3, -552(%ebp)
vpmuludq -344(%ebp), %xmm6, %xmm3
vpaddq %xmm3, %xmm1, %xmm1
vpaddq %xmm7, %xmm1, %xmm3
vmovdqu -472(%ebp), %xmm7
vpmuludq -312(%ebp), %xmm7, %xmm1
vpmuludq -328(%ebp), %xmm7, %xmm4
vpaddq %xmm1, %xmm3, %xmm1
vpaddq %xmm2, %xmm0, %xmm3
vmovdqu -312(%ebp), %xmm5
vpmuludq -520(%ebp), %xmm5, %xmm7
vpaddq %xmm4, %xmm3, %xmm6
vmovdqu -488(%ebp), %xmm3
vpaddq %xmm7, %xmm6, %xmm5
testl %edx, %edx
je poly1305_blocks_avx_15
poly1305_blocks_avx_14:
vmovdqu (%edx), %xmm6
vpaddq %xmm3, %xmm5, %xmm5
vpunpckldq 16(%edx), %xmm6, %xmm4
vpxor %xmm2, %xmm2, %xmm2
vpunpckldq %xmm2, %xmm4, %xmm7
vpunpckhdq 16(%edx), %xmm6, %xmm0
vpunpckhdq %xmm2, %xmm4, %xmm4
vpaddq -584(%ebp), %xmm7, %xmm6
vpsllq $6, %xmm4, %xmm7
vmovdqu %xmm6, -584(%ebp)
vpunpckldq %xmm2, %xmm0, %xmm6
vpaddq -568(%ebp), %xmm7, %xmm4
vpunpckhdq %xmm2, %xmm0, %xmm0
vpsllq $12, %xmm6, %xmm7
vmovdqu %xmm4, -568(%ebp)
vpsllq $18, %xmm0, %xmm2
vpaddq -552(%ebp), %xmm7, %xmm4
vpaddq %xmm2, %xmm1, %xmm1
vmovdqu %xmm4, -552(%ebp)
poly1305_blocks_avx_15:
vmovdqu -584(%ebp), %xmm6
vpsrlq $26, %xmm1, %xmm0
vpsrlq $26, %xmm6, %xmm4
vpaddq -568(%ebp), %xmm4, %xmm2
vpaddq %xmm0, %xmm5, %xmm3
vpsrlq $26, %xmm2, %xmm5
vpaddq -552(%ebp), %xmm5, %xmm4
vmovdqu -360(%ebp), %xmm5
vpand %xmm5, %xmm6, %xmm7
vpsrlq $26, %xmm3, %xmm6
vpmuludq -536(%ebp), %xmm6, %xmm6
vpand %xmm5, %xmm1, %xmm1
vpaddq %xmm6, %xmm7, %xmm7
vpsrlq $26, %xmm4, %xmm6
vpand %xmm5, %xmm3, %xmm3
vpaddq %xmm6, %xmm1, %xmm0
vpand %xmm5, %xmm2, %xmm1
vpsrlq $26, %xmm7, %xmm2
vpaddq %xmm2, %xmm1, %xmm2
vpand %xmm5, %xmm4, %xmm1
vpand %xmm5, %xmm0, %xmm4
vpsrlq $26, %xmm0, %xmm0
vpand %xmm5, %xmm7, %xmm6
vpaddq %xmm0, %xmm3, %xmm0
poly1305_blocks_avx_16:
testl %edx, %edx
je poly1305_blocks_avx_19
poly1305_blocks_avx_17:
vmovdqu %xmm6, (%eax)
vmovdqu %xmm2, 16(%eax)
vmovdqu %xmm1, 32(%eax)
vmovdqu %xmm4, 48(%eax)
vmovdqu %xmm0, 64(%eax)
poly1305_blocks_avx_18:
movl -4(%ebp), %esi
movl -8(%ebp), %edi
movl %ebp, %esp
popl %ebp
ret
poly1305_blocks_avx_19:
vpsrldq $8, %xmm6, %xmm3
vpaddq %xmm3, %xmm6, %xmm5
vpsrldq $8, %xmm2, %xmm6
vmovd %xmm5, %ecx
vpaddq %xmm6, %xmm2, %xmm2
vmovd %xmm2, %esi
movl %ecx, %edx
vpsrldq $8, %xmm1, %xmm7
vpaddq %xmm7, %xmm1, %xmm1
shrl $26, %edx
andl $67108863, %ecx
addl %edx, %esi
vmovd %xmm1, %edx
movl %esi, %edi
vpsrldq $8, %xmm4, %xmm1
vpaddq %xmm1, %xmm4, %xmm4
shrl $26, %edi
andl $67108863, %esi
addl %edi, %edx
vpsrldq $8, %xmm0, %xmm1
movl %edx, %edi
movl %eax, -584(%ebp)
andl $67108863, %edx
vmovd %xmm4, %eax
vpaddq %xmm1, %xmm0, %xmm0
shrl $26, %edi
addl %edi, %eax
vmovd %xmm0, %edi
movl %eax, -580(%ebp)
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
movl %eax, -576(%ebp)
movl -580(%ebp), %eax
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
movl %eax, -568(%ebp)
movl %edx, -572(%ebp)
lea (%edi,%edi,4), %edi
addl %edi, %ecx
movl %ecx, %edi
andl $67108863, %edi
shrl $26, %ecx
addl %ecx, %esi
movl %esi, -564(%ebp)
lea 5(%edi), %ecx
movl %ecx, -560(%ebp)
shrl $26, %ecx
addl %esi, %ecx
movl %ecx, -556(%ebp)
shrl $26, %ecx
movl -576(%ebp), %esi
addl %esi, %ecx
movl %ecx, -552(%ebp)
shrl $26, %ecx
addl %edx, %ecx
movl %ecx, -548(%ebp)
shrl $26, %ecx
movl -560(%ebp), %edx
andl $67108863, %edx
lea -67108864(%ecx,%eax), %eax
movl %eax, -544(%ebp)
shrl $31, %eax
decl %eax
movl %eax, %ecx
andl %eax, %edx
notl %ecx
andl %ecx, %edi
orl %edx, %edi
movl -584(%ebp), %edx
movl %ecx, -540(%ebp)
movl %edi, (%edx)
movl -564(%ebp), %edi
andl %ecx, %edi
movl -556(%ebp), %ecx
andl $67108863, %ecx
andl %eax, %ecx
orl %ecx, %edi
movl -552(%ebp), %ecx
andl $67108863, %ecx
movl %edi, 4(%edx)
andl %eax, %ecx
movl -540(%ebp), %edi
andl %edi, %esi
orl %ecx, %esi
movl %esi, 8(%edx)
movl -548(%ebp), %esi
andl $67108863, %esi
movl -572(%ebp), %ecx
andl %eax, %esi
andl %edi, %ecx
orl %esi, %ecx
movl -544(%ebp), %esi
andl -568(%ebp), %edi
andl %eax, %esi
orl %esi, %edi
movl %ecx, 12(%edx)
movl %edi, 16(%edx)
jmp poly1305_blocks_avx_18
ENDFN(poly1305_blocks)

FN(poly1305_finish_ext)
poly1305_finish_ext_avx_local:
pushl %ebp
movl %esp, %ebp
andl $-64, %esp
pushl %esi
pushl %edi
pushl %ebx
subl $116, %esp
vpxor %xmm1, %xmm1, %xmm1
vmovups %xmm1, 80(%esp)
vmovups %xmm1, 64(%esp)
poly1305_finish_ext_avx_2:
movl 16(%ebp), %ebx
movl 8(%ebp), %esi
testl %ebx, %ebx
je poly1305_finish_ext_avx_19
poly1305_finish_ext_avx_3:
movl 12(%ebp), %edx
testb $16, %bl
je poly1305_finish_ext_avx_5
poly1305_finish_ext_avx_4:
movl $16, %eax
vmovdqu (%edx), %xmm0
vmovdqu %xmm0, 64(%esp)
jmp poly1305_finish_ext_avx_6
poly1305_finish_ext_avx_5:
xorl %eax, %eax
poly1305_finish_ext_avx_6:
testb $8, %bl
je poly1305_finish_ext_avx_8
poly1305_finish_ext_avx_7:
movl (%eax,%edx), %ecx
movl 4(%eax,%edx), %edi
movl %ecx, 64(%esp,%eax)
movl %edi, 68(%esp,%eax)
addl $8, %eax
poly1305_finish_ext_avx_8:
testb $4, %bl
je poly1305_finish_ext_avx_10
poly1305_finish_ext_avx_9:
movl (%eax,%edx), %ecx
movl %ecx, 64(%esp,%eax)
addl $4, %eax
poly1305_finish_ext_avx_10:
testb $2, %bl
je poly1305_finish_ext_avx_12
poly1305_finish_ext_avx_11:
movzwl (%eax,%edx), %ecx
movw %cx, 64(%esp,%eax)
addl $2, %eax
poly1305_finish_ext_avx_12:
testb $1, %bl
je poly1305_finish_ext_avx_14
poly1305_finish_ext_avx_13:
movzbl (%eax,%edx), %edx
movb %dl, 64(%esp,%eax)
poly1305_finish_ext_avx_14:
cmpl $16, %ebx
je poly1305_finish_ext_avx_17
poly1305_finish_ext_avx_15:
movb $1, 64(%esp,%ebx)
jae poly1305_finish_ext_avx_17
poly1305_finish_ext_avx_16:
movl $8, %eax
jmp poly1305_finish_ext_avx_18
poly1305_finish_ext_avx_17:
movl $4, %eax
poly1305_finish_ext_avx_18:
orl %eax, 240(%esi)
movl %esi, %eax
movl $32, %ecx
lea 64(%esp), %edx
call poly1305_blocks_avx_local
poly1305_finish_ext_avx_29:
vpxor %xmm1, %xmm1, %xmm1
poly1305_finish_ext_avx_19:
testb $1, 240(%esi)
je poly1305_finish_ext_avx_25
poly1305_finish_ext_avx_20:
testl %ebx, %ebx
je poly1305_finish_ext_avx_22
poly1305_finish_ext_avx_21:
cmpl $16, %ebx
jbe poly1305_finish_ext_avx_23
poly1305_finish_ext_avx_22:
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
jmp poly1305_finish_ext_avx_24
poly1305_finish_ext_avx_23:
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
poly1305_finish_ext_avx_24:
movl %esi, %eax
xorl %edx, %edx
movl $32, %ecx
call poly1305_blocks_avx_local
poly1305_finish_ext_avx_28:
vpxor %xmm1, %xmm1, %xmm1
poly1305_finish_ext_avx_25:
movl 20(%ebp), %edi
movl 8(%esi), %eax
movl %edi, 96(%esp)
movl %eax, %edi
movl 4(%esi), %edx
movl %edx, %ebx
shrl $6, %edx
shll $20, %edi
movl 12(%esi), %ecx
orl %edi, %edx
movl %ecx, %edi
shrl $12, %eax
shll $14, %edi
orl %edi, %eax
movl 16(%esi), %edi
shll $26, %ebx
shrl $18, %ecx
shll $8, %edi
orl (%esi), %ebx
orl %edi, %ecx
addl 124(%esi), %ebx
adcl 132(%esi), %edx
adcl 140(%esi), %eax
adcl 148(%esi), %ecx
vmovdqu %xmm1, (%esi)
vmovdqu %xmm1, 16(%esi)
vmovdqu %xmm1, 32(%esi)
vmovdqu %xmm1, 48(%esi)
vmovdqu %xmm1, 64(%esi)
vmovdqu %xmm1, 80(%esi)
vmovdqu %xmm1, 96(%esi)
vmovdqu %xmm1, 112(%esi)
vmovdqu %xmm1, 128(%esi)
vmovdqu %xmm1, 144(%esi)
vmovdqu %xmm1, 160(%esi)
vmovdqu %xmm1, 176(%esi)
vmovdqu %xmm1, 192(%esi)
vmovdqu %xmm1, 208(%esi)
vmovdqu %xmm1, 224(%esi)
movl 96(%esp), %esi
movl %ebx, (%esi)
movl %edx, 4(%esi)
movl %eax, 8(%esi)
movl %ecx, 12(%esi)
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
call poly1305_init_ext_avx_local
movl 20(%ebp), %ecx
andl $~31, %ecx
jz poly1305_auth_avx_no_data
movl %edi, %eax
movl 16(%ebp), %edx
addl %ecx, 16(%ebp)
call poly1305_blocks_avx_local
poly1305_auth_avx_no_data:
pushl 12(%ebp)
movl 20(%ebp), %ecx
andl $31, %ecx
pushl %ecx
pushl 16(%ebp)
pushl %edi
call poly1305_finish_ext_avx_local
movl %ebp, %esp
popl %edi
popl %ebp
ret
ENDFN(poly1305_auth)

ENDFILE()
