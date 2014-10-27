#include "poly1305_defines.inc"
#define POLY1305_SUFFIX avx2

.text

GLOBAL(poly1305_block_size)
GLOBAL(poly1305_init_ext)
GLOBAL(poly1305_blocks)
GLOBAL(poly1305_finish_ext)
GLOBAL(poly1305_auth)

FN(poly1305_block_size)
movl $64, %eax
ret
ENDFN(poly1305_block_size)


FN(poly1305_init_ext)
movl 4(%esp), %eax
movl 8(%esp), %edx
movl 12(%esp), %ecx
poly1305_init_ext_avx2_local:
pushl %esi
pushl %edi
pushl %ebx
movl 28(%esp), %edi
subl $96, %esp
movl $-1, %ebx
testl %ecx, %ecx
vpxor %ymm0, %ymm0, %ymm0
vmovdqu %ymm0, (%eax)
cmovz %ebx, %ecx
vmovdqu %ymm0, 32(%eax)
vmovdqu %ymm0, 64(%eax)
testl %edi, %edi
vmovdqu %ymm0, 96(%eax)
vmovdqu %ymm0, 128(%eax)
cmovnz %ebx, %ecx
movl %ecx, 76(%esp)
movl (%edx), %ecx
movl %ecx, %edi
movl 4(%edx), %esi
andl $67108863, %edi
movl %edi, 64(%esp)
movl %esi, %edi
shrl $26, %ecx
shll $6, %edi
movl 8(%edx), %ebx
orl %edi, %ecx
movl %ebx, %edi
andl $67108611, %ecx
shrl $20, %esi
shll $12, %edi
movl %edx, 68(%esp)
orl %edi, %esi
movl 12(%edx), %edx
movl %edx, %edi
shrl $14, %ebx
andl $67092735, %esi
shll $18, %edi
shrl $8, %edx
orl %edi, %ebx
movl 64(%esp), %edi
andl $66076671, %ebx
andl $1048575, %edx
movl %eax, 48(%esp)
movl %ecx, 72(%esp)
movl %esi, 88(%esp)
movl %ebx, 84(%esp)
movl %edx, 80(%esp)
movl %edi, 164(%eax)
movl %ecx, 172(%eax)
movl %esi, 180(%eax)
movl %ebx, 188(%eax)
movl %edx, 196(%eax)
cmpl $16, 76(%esp)
jbe poly1305_init_ext_avx2_4
poly1305_init_ext_avx2_2:
movl %edx, %ecx
movl %edi, %eax
mull %eax
movl %eax, %esi
lea (%ecx,%ecx), %edi
movl 72(%esp), %ecx
movl %edi, 4(%esp)
movl %edx, %edi
movl 4(%esp), %eax
lea (%ecx,%ecx,4), %edx
mull %edx
addl %eax, %esi
lea (%ebx,%ebx,4), %ebx
movl 88(%esp), %eax
adcl %edx, %edi
movl %ebx, (%esp)
addl %eax, %eax
mull %ebx
addl %eax, %esi
lea (%ecx,%ecx), %ebx
movl 64(%esp), %eax
adcl %edx, %edi
mull %ebx
movl %ebx, 8(%esp)
movl %edx, %ebx
movl 88(%esp), %edx
movl %eax, %ecx
movl 4(%esp), %eax
movl %esi, 60(%esp)
shll $6, %edi
lea (%edx,%edx,4), %edx
mull %edx
addl %eax, %ecx
movl (%esp), %eax
adcl %edx, %ebx
mull 84(%esp)
addl %eax, %ecx
adcl %edx, %ebx
movl 64(%esp), %edx
shrl $26, %esi
orl %esi, %edi
addl %edi, %ecx
lea (%edx,%edx), %eax
movl %eax, 16(%esp)
movl 72(%esp), %eax
adcl $0, %ebx
mull %eax
movl %eax, %esi
movl %edx, %edi
movl 88(%esp), %eax
mull 16(%esp)
addl %eax, %esi
movl (%esp), %eax
adcl %edx, %edi
mull 4(%esp)
movl %ecx, 12(%esp)
addl %eax, %esi
movl 84(%esp), %eax
adcl %edx, %edi
shll $6, %ebx
shrl $26, %ecx
orl %ecx, %ebx
lea (%eax,%eax), %ecx
addl %ebx, %esi
movl %esi, %edx
movl 8(%esp), %eax
adcl $0, %edi
andl $67108863, %edx
movl %edx, 44(%esp)
mull 88(%esp)
movl %ecx, 20(%esp)
movl %eax, %ebx
movl 64(%esp), %eax
movl %edx, %ecx
mull 20(%esp)
addl %eax, %ebx
movl 80(%esp), %eax
adcl %edx, %ecx
shll $6, %edi
shrl $26, %esi
lea (%eax,%eax,4), %edx
mull %edx
addl %eax, %ebx
movl 88(%esp), %eax
adcl %edx, %ecx
orl %esi, %edi
mull %eax
addl %edi, %ebx
movl %eax, %esi
movl %ebx, %edi
movl 72(%esp), %eax
adcl $0, %ecx
andl $67108863, %edi
movl %edi, 56(%esp)
movl %edx, %edi
mull 20(%esp)
addl %eax, %esi
movl 16(%esp), %eax
adcl %edx, %edi
mull 80(%esp)
shll $6, %ecx
addl %eax, %esi
movl 60(%esp), %eax
adcl %edx, %edi
andl $67108863, %eax
shrl $26, %ebx
orl %ebx, %ecx
addl %ecx, %esi
movl %esi, %edx
adcl $0, %edi
andl $67108863, %edx
shll $6, %edi
shrl $26, %esi
orl %esi, %edi
movl 12(%esp), %ecx
andl $67108863, %ecx
movl %edx, 40(%esp)
lea (%edi,%edi,4), %ebx
addl %ebx, %eax
movl %eax, %esi
shrl $26, %eax
andl $67108863, %esi
addl %ecx, %eax
movl 48(%esp), %ecx
movl %esi, 52(%esp)
movl %eax, 60(%esp)
movl %esi, 204(%ecx)
movl %eax, 212(%ecx)
movl 44(%esp), %eax
movl 56(%esp), %esi
movl %eax, 220(%ecx)
movl %esi, 228(%ecx)
movl %edx, 236(%ecx)
cmpl $32, 76(%esp)
jbe poly1305_init_ext_avx2_5
poly1305_init_ext_avx2_3:
movl %eax, %edx
movl 52(%esp), %eax
movl 40(%esp), %edi
lea (%edx,%edx,4), %ecx
mull 64(%esp)
movl %ecx, 4(%esp)
lea (%esi,%esi,4), %ebx
movl %ebx, (%esp)
lea (%edi,%edi,4), %esi
movl %esi, 8(%esp)
movl %edx, %esi
movl 60(%esp), %edx
movl %eax, %edi
movl 80(%esp), %eax
lea (%edx,%edx,4), %edx
mull %edx
addl %eax, %edi
movl 84(%esp), %eax
adcl %edx, %esi
mull %ecx
addl %eax, %edi
movl %ebx, %eax
adcl %edx, %esi
mull 88(%esp)
movl %eax, %ebx
movl %edx, %ecx
movl 72(%esp), %eax
mull 8(%esp)
addl %eax, %ebx
movl 52(%esp), %eax
adcl %edx, %ecx
addl %ebx, %edi
movl %edi, 12(%esp)
adcl %ecx, %esi
mull 72(%esp)
movl %eax, %ecx
movl %edx, %edi
movl 60(%esp), %eax
mull 64(%esp)
addl %eax, %ecx
movl 80(%esp), %eax
adcl %edx, %edi
mull 4(%esp)
addl %eax, %ecx
movl (%esp), %eax
adcl %edx, %edi
mull 84(%esp)
movl %esi, 16(%esp)
movl %eax, %esi
movl 88(%esp), %eax
movl %edx, %ebx
mull 8(%esp)
addl %eax, %esi
movl 52(%esp), %eax
adcl %edx, %ebx
addl %esi, %ecx
movl 16(%esp), %esi
adcl %ebx, %edi
mull 88(%esp)
movl 12(%esp), %ebx
shll $6, %esi
shrl $26, %ebx
orl %ebx, %esi
movl %edx, %ebx
addl %esi, %ecx
movl %ecx, 20(%esp)
movl %eax, %ecx
movl 60(%esp), %eax
adcl $0, %edi
mull 72(%esp)
addl %eax, %ecx
movl 44(%esp), %eax
adcl %edx, %ebx
mull 64(%esp)
addl %eax, %ecx
movl (%esp), %eax
adcl %edx, %ebx
mull 80(%esp)
movl %eax, %esi
movl 84(%esp), %eax
movl %edi, 24(%esp)
movl %edx, %edi
mull 8(%esp)
addl %eax, %esi
movl 52(%esp), %eax
adcl %edx, %edi
addl %esi, %ecx
movl 24(%esp), %esi
adcl %edi, %ebx
mull 84(%esp)
movl 20(%esp), %edi
shll $6, %esi
shrl $26, %edi
orl %edi, %esi
movl %edx, %edi
addl %esi, %ecx
movl %eax, %esi
movl 60(%esp), %eax
adcl $0, %ebx
mull 88(%esp)
addl %eax, %esi
movl 44(%esp), %eax
adcl %edx, %edi
mull 72(%esp)
addl %eax, %esi
movl 64(%esp), %eax
adcl %edx, %edi
mull 56(%esp)
addl %eax, %esi
movl 80(%esp), %eax
adcl %edx, %edi
mull 8(%esp)
addl %eax, %esi
movl 52(%esp), %eax
adcl %edx, %edi
mull 80(%esp)
movl %ecx, 28(%esp)
shll $6, %ebx
shrl $26, %ecx
orl %ecx, %ebx
addl %ebx, %esi
movl %esi, 32(%esp)
movl %eax, %esi
movl 60(%esp), %eax
adcl $0, %edi
movl %edi, 36(%esp)
movl %edx, %edi
mull 84(%esp)
addl %eax, %esi
movl 44(%esp), %eax
adcl %edx, %edi
mull 88(%esp)
addl %eax, %esi
movl 56(%esp), %eax
adcl %edx, %edi
mull 72(%esp)
movl %eax, %ebx
movl %edx, %ecx
movl 64(%esp), %eax
mull 40(%esp)
addl %eax, %ebx
adcl %edx, %ecx
addl %ebx, %esi
movl 36(%esp), %edx
adcl %ecx, %edi
movl 32(%esp), %ecx
movl %ecx, %ebx
shll $6, %edx
andl $67108863, %ecx
shrl $26, %ebx
orl %ebx, %edx
addl %edx, %esi
movl %esi, %eax
adcl $0, %edi
andl $67108863, %esi
shll $6, %edi
shrl $26, %eax
orl %eax, %edi
movl 12(%esp), %ebx
andl $67108863, %ebx
movl 48(%esp), %edx
lea (%edi,%edi,4), %edi
addl %edi, %ebx
movl 20(%esp), %edi
movl %ebx, %eax
shrl $26, %ebx
andl $67108863, %edi
addl %ebx, %edi
andl $67108863, %eax
movl 28(%esp), %ebx
andl $67108863, %ebx
movl %eax, 244(%edx)
movl %edi, 252(%edx)
movl %ebx, 260(%edx)
movl %ecx, 268(%edx)
movl %esi, 276(%edx)
jmp poly1305_init_ext_avx2_5
poly1305_init_ext_avx2_4:
movl %eax, 52(%esp)
movl %edx, 60(%esp)
movl %ecx, 44(%esp)
movl %ebx, 56(%esp)
movl %esi, 40(%esp)
poly1305_init_ext_avx2_5:
movl 68(%esp), %ebx
movl 48(%esp), %edi
cmpl $48, 76(%esp)
movl 16(%ebx), %eax
movl %eax, 284(%edi)
movl 20(%ebx), %edx
movl %edx, 292(%edi)
movl 24(%ebx), %ecx
movl %ecx, 300(%edi)
movl 28(%ebx), %esi
movl %esi, 308(%edi)
jbe poly1305_init_ext_avx2_7
poly1305_init_ext_avx2_6:
movl 40(%esp), %ebx
movl 56(%esp), %esi
movl 52(%esp), %eax
mull %eax
movl %eax, %ecx
lea (%ebx,%ebx), %edi
movl %edi, (%esp)
lea (%esi,%esi,4), %ebx
movl 60(%esp), %esi
movl %edx, %edi
movl (%esp), %eax
movl %ebx, 4(%esp)
lea (%esi,%esi,4), %edx
mull %edx
addl %eax, %ecx
movl 44(%esp), %eax
adcl %edx, %edi
addl %eax, %eax
mull %ebx
addl %eax, %ecx
movl 52(%esp), %eax
movl %ecx, 8(%esp)
lea (%esi,%esi), %ecx
adcl %edx, %edi
movl %ecx, 12(%esp)
lea (%eax,%eax), %edx
movl %edx, 20(%esp)
mull %ecx
movl %edx, %ecx
movl 44(%esp), %edx
movl 56(%esp), %ebx
shll $6, %edi
lea (%edx,%edx,4), %edx
lea (%ebx,%ebx), %esi
movl %esi, 16(%esp)
movl %eax, %esi
movl (%esp), %eax
mull %edx
addl %eax, %esi
movl %ebx, %eax
movl 4(%esp), %ebx
adcl %edx, %ecx
mull %ebx
addl %eax, %esi
movl 8(%esp), %eax
adcl %edx, %ecx
shrl $26, %eax
orl %eax, %edi
movl 60(%esp), %eax
addl %edi, %esi
movl %esi, 24(%esp)
adcl $0, %ecx
mull %eax
movl %eax, %esi
movl %edx, %edi
movl 44(%esp), %eax
mull 20(%esp)
addl %eax, %esi
movl (%esp), %eax
adcl %edx, %edi
mull %ebx
addl %eax, %esi
movl 12(%esp), %eax
adcl %edx, %edi
mull 44(%esp)
movl 24(%esp), %ebx
shll $6, %ecx
shrl $26, %ebx
orl %ebx, %ecx
movl %edx, %ebx
addl %ecx, %esi
movl %esi, %ecx
adcl $0, %edi
andl $67108863, %ecx
movl %ecx, 28(%esp)
movl %eax, %ecx
movl 52(%esp), %eax
mull 16(%esp)
addl %eax, %ecx
movl 40(%esp), %eax
adcl %edx, %ebx
shll $6, %edi
shrl $26, %esi
lea (%eax,%eax,4), %edx
mull %edx
addl %eax, %ecx
movl 44(%esp), %eax
adcl %edx, %ebx
orl %esi, %edi
mull %eax
addl %edi, %ecx
movl %edx, %esi
movl %ecx, %edi
adcl $0, %ebx
andl $67108863, %edi
movl %edi, 32(%esp)
movl %eax, %edi
movl 16(%esp), %eax
mull 60(%esp)
addl %eax, %edi
movl 20(%esp), %eax
adcl %edx, %esi
mull 40(%esp)
shll $6, %ebx
addl %eax, %edi
movl 24(%esp), %eax
adcl %edx, %esi
andl $67108863, %eax
shrl $26, %ecx
orl %ecx, %ebx
addl %ebx, %edi
movl %edi, %edx
adcl $0, %esi
andl $67108863, %edx
shll $6, %esi
shrl $26, %edi
orl %edi, %esi
movl 8(%esp), %ecx
andl $67108863, %ecx
movl 32(%esp), %edi
lea (%esi,%esi,4), %ebx
addl %ebx, %ecx
movl 48(%esp), %ebx
movl %ecx, %esi
shrl $26, %ecx
andl $67108863, %esi
addl %ecx, %eax
movl 28(%esp), %ecx
movl %esi, 184(%ebx)
movl %esi, 176(%ebx)
movl %esi, 168(%ebx)
movl %esi, 160(%ebx)
movl %eax, 216(%ebx)
movl %eax, 208(%ebx)
movl %eax, 200(%ebx)
movl %eax, 192(%ebx)
movl %ecx, 248(%ebx)
movl %ecx, 240(%ebx)
movl %ecx, 232(%ebx)
movl %ecx, 224(%ebx)
movl %edi, 280(%ebx)
movl %edi, 272(%ebx)
movl %edi, 264(%ebx)
movl %edi, 256(%ebx)
movl %edx, 312(%ebx)
movl %edx, 304(%ebx)
movl %edx, 296(%ebx)
movl %edx, 288(%ebx)
poly1305_init_ext_avx2_7:
movl 48(%esp), %eax
vzeroupper
movl $0, 320(%eax)
addl $96, %esp
popl %ebx
popl %edi
popl %esi
ret
ENDFN(poly1305_init_ext)

FN(poly1305_blocks)
movl 4(%esp), %eax
movl 8(%esp), %edx
movl 12(%esp), %ecx
poly1305_blocks_avx2_local:
pushl %ebp
movl %esp, %ebp
andl $-64, %esp
pushl %esi
pushl %edi
pushl %ebx
subl $628, %esp
movl $67108863, %ebx
movl $5, %esi
movl $16777216, %edi
vmovd %ebx, %xmm0
vmovd %esi, %xmm2
vmovd %edi, %xmm4
vpbroadcastq %xmm0, %ymm1
vpbroadcastq %xmm2, %ymm3
movl 320(%eax), %ebx
vmovdqu %ymm1, 384(%esp)
vmovdqu %ymm3, 416(%esp)
vpbroadcastq %xmm4, %ymm5
testb $60, %bl
je poly1305_blocks_avx2_11
poly1305_blocks_avx2_2:
vpsrldq $8, %ymm5, %ymm5
testb $4, %bl
je poly1305_blocks_avx2_4
poly1305_blocks_avx2_3:
vpermq $192, %ymm5, %ymm5
poly1305_blocks_avx2_4:
testb $8, %bl
je poly1305_blocks_avx2_6
poly1305_blocks_avx2_5:
vpermq $240, %ymm5, %ymm5
poly1305_blocks_avx2_6:
testb $16, %bl
je poly1305_blocks_avx2_8
poly1305_blocks_avx2_7:
vpermq $252, %ymm5, %ymm5
poly1305_blocks_avx2_8:
testb $32, %bl
je poly1305_blocks_avx2_11
poly1305_blocks_avx2_9:
vpxor %ymm5, %ymm5, %ymm5
poly1305_blocks_avx2_11:
movl %ebx, %esi
btsl $0, %esi
jc poly1305_blocks_avx2_13
poly1305_blocks_avx2_12:
vmovdqu (%edx), %ymm4
movl %esi, %ebx
vmovdqu 32(%edx), %ymm6
vmovdqu 384(%esp), %ymm2
vpunpcklqdq %ymm6, %ymm4, %ymm3
vpunpckhqdq %ymm6, %ymm4, %ymm7
vpermq $216, %ymm3, %ymm1
addl $64, %edx
vpermq $216, %ymm7, %ymm0
addl $-64, %ecx
vpand %ymm1, %ymm2, %ymm3
vpsrlq $26, %ymm1, %ymm4
vpsrlq $52, %ymm1, %ymm1
vpsllq $12, %ymm0, %ymm6
vpsrlq $40, %ymm0, %ymm0
vpand %ymm4, %ymm2, %ymm4
vpor %ymm6, %ymm1, %ymm7
vpor %ymm5, %ymm0, %ymm0
vpsrlq $26, %ymm7, %ymm6
vpand %ymm7, %ymm2, %ymm1
vpand %ymm6, %ymm2, %ymm2
movl %ebx, 320(%eax)
jmp poly1305_blocks_avx2_14
poly1305_blocks_avx2_13:
vmovdqu (%eax), %ymm3
vmovdqu 32(%eax), %ymm4
vmovdqu 64(%eax), %ymm1
vmovdqu 96(%eax), %ymm2
vmovdqu 128(%eax), %ymm0
poly1305_blocks_avx2_14:
cmpl $64, %ecx
jb poly1305_blocks_avx2_18
poly1305_blocks_avx2_15:
vmovdqu 160(%eax), %ymm7
vmovdqu %ymm1, (%esp)
vmovdqu %ymm2, 32(%esp)
vmovdqu %ymm0, 64(%esp)
vmovdqu 256(%eax), %ymm0
vmovdqu 288(%eax), %ymm1
vmovdqu %ymm7, 224(%esp)
vmovdqu 416(%esp), %ymm2
vmovdqu 192(%eax), %ymm7
vmovdqu 224(%eax), %ymm6
vmovdqu %ymm0, 288(%esp)
vmovdqu %ymm1, 256(%esp)
vmovdqu %ymm5, 448(%esp)
vmovdqu %ymm7, 352(%esp)
vmovdqu %ymm6, 320(%esp)
vpmuludq %ymm2, %ymm0, %ymm0
vpmuludq %ymm2, %ymm1, %ymm1
vpmuludq %ymm2, %ymm7, %ymm7
vpmuludq %ymm2, %ymm6, %ymm6
vmovdqu %ymm0, 128(%esp)
vmovdqu %ymm1, 96(%esp)
vmovdqu %ymm7, 192(%esp)
vmovdqu %ymm6, 160(%esp)
vmovdqu 64(%esp), %ymm0
vmovdqu 32(%esp), %ymm2
vmovdqu (%esp), %ymm1
poly1305_blocks_avx2_16:
vmovdqu (%edx), %ymm6
addl $-64, %ecx
vmovdqu %ymm4, 32(%esp)
vmovdqu %ymm3, (%esp)
vperm2i128 $32, 32(%edx), %ymm6, %ymm7
vperm2i128 $49, 32(%edx), %ymm6, %ymm5
addl $64, %edx
vpunpckldq %ymm5, %ymm7, %ymm6
vpunpckhdq %ymm5, %ymm7, %ymm7
vmovdqu %ymm6, 64(%esp)
vmovdqu %ymm7, 480(%esp)
vpmuludq 192(%esp), %ymm0, %ymm7
vpmuludq 160(%esp), %ymm2, %ymm5
vpaddq %ymm5, %ymm7, %ymm7
vpmuludq 128(%esp), %ymm1, %ymm5
vpaddq %ymm5, %ymm7, %ymm5
vmovdqu 96(%esp), %ymm7
vpmuludq %ymm7, %ymm4, %ymm4
vpaddq %ymm4, %ymm5, %ymm5
vmovdqu 224(%esp), %ymm4
vpmuludq %ymm4, %ymm3, %ymm3
vpaddq %ymm3, %ymm5, %ymm3
vpxor %ymm5, %ymm5, %ymm5
vpunpckldq %ymm5, %ymm6, %ymm6
vpaddq %ymm6, %ymm3, %ymm3
vpmuludq %ymm7, %ymm0, %ymm6
vpmuludq %ymm4, %ymm2, %ymm4
vmovdqu %ymm3, 512(%esp)
vpaddq %ymm4, %ymm6, %ymm3
vmovdqu 32(%esp), %ymm6
vpmuludq 352(%esp), %ymm1, %ymm4
vpaddq %ymm4, %ymm3, %ymm3
vpmuludq 320(%esp), %ymm6, %ymm4
vmovdqu (%esp), %ymm6
vpaddq %ymm4, %ymm3, %ymm3
vpmuludq 288(%esp), %ymm6, %ymm4
vpaddq %ymm4, %ymm3, %ymm3
vmovdqu 480(%esp), %ymm4
vpunpckhdq %ymm5, %ymm4, %ymm4
vpsllq $18, %ymm4, %ymm4
vpaddq %ymm4, %ymm3, %ymm3
vmovdqu %ymm3, 544(%esp)
vpmuludq 160(%esp), %ymm0, %ymm3
vpmuludq 128(%esp), %ymm2, %ymm4
vpaddq %ymm4, %ymm3, %ymm3
vpmuludq %ymm7, %ymm1, %ymm7
vpaddq %ymm7, %ymm3, %ymm4
vmovdqu 224(%esp), %ymm3
vpmuludq 32(%esp), %ymm3, %ymm7
vpaddq %ymm7, %ymm4, %ymm7
vmovdqu 352(%esp), %ymm4
vpmuludq %ymm4, %ymm6, %ymm6
vpaddq %ymm6, %ymm7, %ymm7
vmovdqu 64(%esp), %ymm6
vpunpckhdq %ymm5, %ymm6, %ymm5
vpsllq $6, %ymm5, %ymm5
vpaddq %ymm5, %ymm7, %ymm6
vmovdqu 512(%esp), %ymm7
vpsrlq $26, %ymm7, %ymm7
vpaddq %ymm7, %ymm6, %ymm5
vpmuludq %ymm3, %ymm0, %ymm6
vpmuludq %ymm4, %ymm2, %ymm7
vmovdqu %ymm5, 576(%esp)
vpaddq %ymm7, %ymm6, %ymm5
vmovdqu 32(%esp), %ymm7
vpmuludq 320(%esp), %ymm1, %ymm6
vpaddq %ymm6, %ymm5, %ymm5
vpmuludq 288(%esp), %ymm7, %ymm6
vpaddq %ymm6, %ymm5, %ymm6
vmovdqu (%esp), %ymm5
vpmuludq 256(%esp), %ymm5, %ymm5
vpaddq %ymm5, %ymm6, %ymm6
vpaddq 448(%esp), %ymm6, %ymm5
vmovdqu 544(%esp), %ymm6
vpmuludq 128(%esp), %ymm0, %ymm0
vpmuludq 96(%esp), %ymm2, %ymm2
vpsrlq $26, %ymm6, %ymm6
vpaddq %ymm2, %ymm0, %ymm2
vmovdqu 480(%esp), %ymm0
vpaddq %ymm6, %ymm5, %ymm5
vpxor %ymm6, %ymm6, %ymm6
vpmuludq %ymm3, %ymm1, %ymm1
vpaddq %ymm1, %ymm2, %ymm2
vmovdqu (%esp), %ymm1
vpmuludq %ymm4, %ymm7, %ymm3
vpaddq %ymm3, %ymm2, %ymm4
vpunpckldq %ymm6, %ymm0, %ymm2
vpsrlq $26, %ymm5, %ymm6
vpmuludq 320(%esp), %ymm1, %ymm7
vpaddq %ymm7, %ymm4, %ymm3
vpsllq $12, %ymm2, %ymm1
vmovdqu 576(%esp), %ymm4
vpaddq %ymm1, %ymm3, %ymm7
vpsrlq $26, %ymm4, %ymm0
vpaddq %ymm0, %ymm7, %ymm2
vmovdqu 384(%esp), %ymm0
vpsrlq $26, %ymm2, %ymm7
vpand 512(%esp), %ymm0, %ymm3
vpand %ymm0, %ymm4, %ymm4
vpand %ymm0, %ymm5, %ymm5
vpmuludq 416(%esp), %ymm6, %ymm1
vpaddq %ymm1, %ymm3, %ymm1
vpand 544(%esp), %ymm0, %ymm3
vpaddq %ymm7, %ymm3, %ymm7
vpand %ymm0, %ymm1, %ymm3
vpsrlq $26, %ymm1, %ymm1
vpaddq %ymm1, %ymm4, %ymm4
vpand %ymm0, %ymm2, %ymm1
vpand %ymm0, %ymm7, %ymm2
vpsrlq $26, %ymm7, %ymm0
vpaddq %ymm0, %ymm5, %ymm0
cmpl $64, %ecx
jae poly1305_blocks_avx2_16
poly1305_blocks_avx2_18:
testb $64, %bl
jne poly1305_blocks_avx2_20
poly1305_blocks_avx2_19:
vmovdqu %ymm3, (%eax)
vmovdqu %ymm4, 32(%eax)
vmovdqu %ymm1, 64(%eax)
vmovdqu %ymm2, 96(%eax)
vmovdqu %ymm0, 128(%eax)
jmp poly1305_blocks_avx2_21
poly1305_blocks_avx2_20:
vpermq $245, %ymm3, %ymm5
vpaddq %ymm5, %ymm3, %ymm3
vpermq $245, %ymm4, %ymm6
vpaddq %ymm6, %ymm4, %ymm4
vpermq $245, %ymm1, %ymm7
vpaddq %ymm7, %ymm1, %ymm1
vpermq $170, %ymm3, %ymm7
vpermq $245, %ymm2, %ymm5
vpaddq %ymm7, %ymm3, %ymm3
vpaddq %ymm5, %ymm2, %ymm2
vpermq $170, %ymm4, %ymm5
vpaddq %ymm5, %ymm4, %ymm4
movl %eax, (%esp)
vpermq $245, %ymm0, %ymm6
vpaddq %ymm6, %ymm0, %ymm0
vmovd %xmm3, %eax
vpermq $170, %ymm1, %ymm3
movl %eax, %edx
vpaddq %ymm3, %ymm1, %ymm1
andl $67108863, %eax
vpermq $170, %ymm2, %ymm3
shrl $26, %edx
vpaddq %ymm3, %ymm2, %ymm2
vmovd %xmm4, %ebx
addl %edx, %ebx
movl %ebx, %edi
andl $67108863, %ebx
shrl $26, %edi
vmovd %xmm1, %ecx
vpermq $170, %ymm0, %ymm1
addl %edi, %ecx
vpaddq %ymm1, %ymm0, %ymm0
movl %ecx, %edx
shrl $26, %edx
andl $67108863, %ecx
vmovd %xmm2, %esi
addl %edx, %esi
movl %esi, %edx
andl $67108863, %esi
shrl $26, %edx
vmovd %xmm0, %edi
addl %edx, %edi
movl %edi, %edx
andl $67108863, %edi
shrl $26, %edx
lea (%edx,%edx,4), %edx
addl %edx, %eax
movl %eax, %edx
andl $67108863, %eax
shrl $26, %edx
addl %edx, %ebx
movl %ebx, %edx
andl $67108863, %ebx
shrl $26, %edx
addl %edx, %ecx
movl %ecx, %edx
shrl $26, %ecx
andl $67108863, %edx
addl %ecx, %esi
movl %esi, %ecx
shrl $26, %esi
andl $67108863, %ecx
addl %esi, %edi
movl %edi, %esi
shrl $26, %edi
andl $67108863, %esi
movl %esi, 4(%esp)
lea (%edi,%edi,4), %edi
addl %edi, %eax
movl %eax, %edi
andl $67108863, %edi
shrl $26, %eax
addl %eax, %ebx
lea 5(%edi), %eax
movl %eax, 8(%esp)
shrl $26, %eax
addl %ebx, %eax
movl %eax, 12(%esp)
shrl $26, %eax
addl %edx, %eax
movl %eax, 16(%esp)
shrl $26, %eax
addl %ecx, %eax
movl %eax, 20(%esp)
shrl $26, %eax
lea -67108864(%eax,%esi), %eax
movl %eax, 24(%esp)
shrl $31, %eax
decl %eax
andn %edi, %eax, %esi
movl 8(%esp), %edi
andl $67108863, %edi
andl %eax, %edi
orl %edi, %esi
movl (%esp), %edi
movl %esi, (%edi)
andn %ebx, %eax, %esi
movl 12(%esp), %ebx
andl $67108863, %ebx
andl %eax, %ebx
orl %ebx, %esi
andn %edx, %eax, %ebx
movl 16(%esp), %edx
andl $67108863, %edx
andl %eax, %edx
orl %edx, %ebx
andn %ecx, %eax, %edx
movl 20(%esp), %ecx
andl $67108863, %ecx
movl %ebx, 8(%edi)
andl %eax, %ecx
movl %esi, 4(%edi)
orl %ecx, %edx
movl 24(%esp), %ebx
andn 4(%esp), %eax, %esi
andl %eax, %ebx
orl %ebx, %esi
movl %edx, 12(%edi)
movl %esi, 16(%edi)
poly1305_blocks_avx2_21:
vzeroupper
addl $628, %esp
popl %ebx
popl %edi
popl %esi
movl %ebp, %esp
popl %ebp
ret
ENDFN(poly1305_blocks)

FN(poly1305_finish_ext)
poly1305_finish_ext_avx2_local:
pushl %ebp
movl %esp, %ebp
andl $-64, %esp
pushl %esi
pushl %edi
pushl %ebx
subl $180, %esp
movl 16(%ebp), %esi
movl 8(%ebp), %ebx
testl %esi, %esi
je poly1305_finish_ext_avx2_29
poly1305_finish_ext_avx2_2:
movl 12(%ebp), %ecx
lea 64(%esp), %edx
vpxor %ymm0, %ymm0, %ymm0
subl %edx, %ecx
vmovdqu %ymm0, 64(%esp)
vmovdqu %ymm0, 96(%esp)
testl $32, %esi
je poly1305_finish_ext_avx2_4
poly1305_finish_ext_avx2_3:
vmovdqu 64(%esp,%ecx), %ymm0
lea 96(%esp), %edx
vmovdqu %ymm0, 64(%esp)
poly1305_finish_ext_avx2_4:
testl $16, %esi
je poly1305_finish_ext_avx2_6
poly1305_finish_ext_avx2_5:
vmovdqu (%edx,%ecx), %xmm0
vmovdqu %xmm0, (%edx)
addl $16, %edx
poly1305_finish_ext_avx2_6:
testl $8, %esi
je poly1305_finish_ext_avx2_8
poly1305_finish_ext_avx2_7:
movl (%edx,%ecx), %edi
movl 4(%edx,%ecx), %eax
movl %edi, (%edx)
movl %eax, 4(%edx)
addl $8, %edx
poly1305_finish_ext_avx2_8:
testl $4, %esi
je poly1305_finish_ext_avx2_10
poly1305_finish_ext_avx2_9:
movl (%edx,%ecx), %edi
movl %edi, (%edx)
addl $4, %edx
poly1305_finish_ext_avx2_10:
testl $2, %esi
je poly1305_finish_ext_avx2_12
poly1305_finish_ext_avx2_11:
movzwl (%edx,%ecx), %edi
movw %di, (%edx)
addl $2, %edx
poly1305_finish_ext_avx2_12:
testl $1, %esi
je poly1305_finish_ext_avx2_14
poly1305_finish_ext_avx2_13:
movzbl (%edx,%ecx), %ecx
movb %cl, (%edx)
poly1305_finish_ext_avx2_14:
testl $15, %esi
je poly1305_finish_ext_avx2_16
poly1305_finish_ext_avx2_15:
movb $1, 64(%esp,%esi)
poly1305_finish_ext_avx2_16:
cmpl $48, %esi
jb poly1305_finish_ext_avx2_18
poly1305_finish_ext_avx2_17:
movl 320(%ebx), %edx
orl $4, %edx
movl %edx, 320(%ebx)
jmp poly1305_finish_ext_avx2_23
poly1305_finish_ext_avx2_18:
cmpl $32, %esi
jb poly1305_finish_ext_avx2_20
poly1305_finish_ext_avx2_19:
movl 320(%ebx), %edx
orl $8, %edx
movl %edx, 320(%ebx)
jmp poly1305_finish_ext_avx2_23
poly1305_finish_ext_avx2_20:
cmpl $16, %esi
jb poly1305_finish_ext_avx2_22
poly1305_finish_ext_avx2_21:
movl 320(%ebx), %edx
orl $16, %edx
movl %edx, 320(%ebx)
jmp poly1305_finish_ext_avx2_23
poly1305_finish_ext_avx2_22:
movl 320(%ebx), %edx
orl $32, %edx
movl %edx, 320(%ebx)
poly1305_finish_ext_avx2_23:
testb $1, %dl
je poly1305_finish_ext_avx2_28
poly1305_finish_ext_avx2_24:
cmpl $32, %esi
ja poly1305_finish_ext_avx2_28
poly1305_finish_ext_avx2_25:
movl $2, %edx
movl $3, %edi
movl $1, %ecx
cmpl $16, %esi
cmovbe %edx, %edi
cmovbe %edx, %ecx
movl $10, %edx
xorl %eax, %eax
poly1305_finish_ext_avx2_26:
movl 164(%ebx,%edx,8), %esi
incl %eax
movl %esi, 160(%ebx,%edi,8)
movl 172(%ebx,%edx,8), %esi
movl %esi, 192(%ebx,%edi,8)
movl 180(%ebx,%edx,8), %esi
movl %esi, 224(%ebx,%edi,8)
movl 188(%ebx,%edx,8), %esi
movl %esi, 256(%ebx,%edi,8)
movl 196(%ebx,%edx,8), %esi
addl $-5, %edx
movl %esi, 288(%ebx,%edi,8)
incl %edi
cmpl %ecx, %eax
jb poly1305_finish_ext_avx2_26
poly1305_finish_ext_avx2_27:
movl 16(%ebp), %esi
poly1305_finish_ext_avx2_28:
movl %ebx, %eax
lea 64(%esp), %edx
movl $64, %ecx
call poly1305_blocks_avx2_local
poly1305_finish_ext_avx2_29:
movl 320(%ebx), %edx
testb $1, %dl
je poly1305_finish_ext_avx2_43
poly1305_finish_ext_avx2_30:
testl %esi, %esi
je poly1305_finish_ext_avx2_32
poly1305_finish_ext_avx2_31:
cmpl $48, %esi
jbe poly1305_finish_ext_avx2_33
poly1305_finish_ext_avx2_32:
movl $10, %ecx
movl $3, %eax
movl $1, %edi
jmp poly1305_finish_ext_avx2_36
poly1305_finish_ext_avx2_33:
xorl %edi, %edi
cmpl $32, %esi
ja poly1305_finish_ext_avx2_35
poly1305_finish_ext_avx2_34:
movl $5, %ecx
xorl %eax, %eax
cmpl $16, %esi
cmovbe %eax, %ecx
movl $16, %eax
cmpl %esi, %eax
movl $1, %eax
adcl $0, %eax
jmp poly1305_finish_ext_avx2_36
poly1305_finish_ext_avx2_35:
movl $10, %ecx
movl $3, %eax
poly1305_finish_ext_avx2_36:
movl %edx, 128(%esp)
xorl %esi, %esi
poly1305_finish_ext_avx2_37:
movl 164(%ebx,%ecx,8), %edx
incl %esi
movl %edx, 160(%ebx,%edi,8)
movl 172(%ebx,%ecx,8), %edx
movl %edx, 192(%ebx,%edi,8)
movl 180(%ebx,%ecx,8), %edx
movl %edx, 224(%ebx,%edi,8)
movl 188(%ebx,%ecx,8), %edx
movl %edx, 256(%ebx,%edi,8)
movl 196(%ebx,%ecx,8), %edx
addl $-5, %ecx
movl %edx, 288(%ebx,%edi,8)
incl %edi
cmpl %eax, %esi
jb poly1305_finish_ext_avx2_37
poly1305_finish_ext_avx2_38:
movl 128(%esp), %edx
cmpl $4, %edi
jae poly1305_finish_ext_avx2_42
poly1305_finish_ext_avx2_39:
xorl %ecx, %ecx
poly1305_finish_ext_avx2_40:
movl $1, 160(%ebx,%edi,8)
movl %ecx, 192(%ebx,%edi,8)
movl %ecx, 224(%ebx,%edi,8)
movl %ecx, 256(%ebx,%edi,8)
movl %ecx, 288(%ebx,%edi,8)
incl %edi
cmpl $4, %edi
jb poly1305_finish_ext_avx2_40
poly1305_finish_ext_avx2_42:
orl $96, %edx
movl %ebx, %eax
vpxor %ymm0, %ymm0, %ymm0
movl $64, %ecx
movl %edx, 320(%ebx)
lea 64(%esp), %edx
vmovdqu %ymm0, 64(%esp)
vmovdqu %ymm0, 96(%esp)
call poly1305_blocks_avx2_local
poly1305_finish_ext_avx2_43:
movl 20(%ebp), %edx
movl 8(%ebx), %edi
movl %edi, %eax
movl %edx, 128(%esp)
movl 4(%ebx), %edx
movl %edx, %esi
shrl $6, %edx
shll $20, %eax
movl 12(%ebx), %ecx
orl %eax, %edx
movl %ecx, %eax
shrl $12, %edi
shll $14, %eax
orl %eax, %edi
movl 16(%ebx), %eax
shll $26, %esi
shrl $18, %ecx
shll $8, %eax
orl (%ebx), %esi
orl %eax, %ecx
addl 284(%ebx), %esi
adcl 292(%ebx), %edx
adcl 300(%ebx), %edi
adcl 308(%ebx), %ecx
vpxor %ymm0, %ymm0, %ymm0
movl 128(%esp), %eax
movl %ecx, 12(%eax)
movl %edi, 8(%eax)
movl %edx, 4(%eax)
movl %esi, (%eax)
vmovdqu %ymm0, (%ebx)
vmovdqu %ymm0, 32(%ebx)
vmovdqu %ymm0, 64(%ebx)
vmovdqu %ymm0, 96(%ebx)
vmovdqu %ymm0, 128(%ebx)
vmovdqu %ymm0, 160(%ebx)
vmovdqu %ymm0, 192(%ebx)
vmovdqu %ymm0, 224(%ebx)
vzeroupper
addl $180, %esp
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
subl $324, %esp
andl $~63, %esp
movl %esp, %edi
movl %edi, %eax
movl 24(%ebp), %edx
movl 20(%ebp), %ecx
subl $16, %esp
movl $0, 12(%esp)
call poly1305_init_ext_avx2_local
movl 20(%ebp), %ecx
andl $~63, %ecx
jz poly1305_auth_avx2_no_data
movl %edi, %eax
movl 16(%ebp), %edx
addl %ecx, 16(%ebp)
call poly1305_blocks_avx2_local
poly1305_auth_avx2_no_data:
pushl 12(%ebp)
movl 20(%ebp), %ecx
andl $63, %ecx
pushl %ecx
pushl 16(%ebp)
pushl %edi
call poly1305_finish_ext_avx2_local
movl %ebp, %esp
popl %edi
popl %ebp
ret
ENDFN(poly1305_auth)

ENDFILE()
