/* cannibalized from the public domain x86 implementation in supercop by djb */

#include "poly1305_defines.inc"
#define POLY1305_SUFFIX x86

.text

GLOBAL(poly1305_block_size)
GLOBAL(poly1305_init_ext)
GLOBAL(poly1305_blocks)
GLOBAL(poly1305_finish_ext)
GLOBAL(poly1305_auth)

FN(poly1305_block_size)
movl $16, %eax
ret
ENDFN(poly1305_block_size)

FN(poly1305_init_ext)
poly1305_init_ext_x86_local:
pushl %ebx
pushl %esi
pushl %edi
pushl %eax
pushl %eax
movl 24(%esp), %eax
movl 28(%esp), %edx
movl $0x137f, %ecx
movl %ecx, 0(%esp)
fstcw 4(%esp)
fldcw 0(%esp)
movl 16(%edx), %ecx
movl 20(%edx), %ebx
movl 24(%edx), %esi
movl 28(%edx), %edi
movl %ecx, 104(%eax)
movl %ebx, 108(%eax)
movl %esi, 112(%eax)
movl %edi, 116(%eax)
movl 0(%edx), %ecx
movl 4(%edx), %ebx
movl 8(%edx), %esi
movl 12(%edx), %edi
andl $0x0fffffff, %ecx
andl $0x0ffffffc, %ebx
andl $0x0ffffffc, %esi
andl $0x0ffffffc, %edi
movl %ecx, 0(%eax)
movl $0x43300000, 4(%eax)
movl %ebx, 8(%eax)
movl $0x45300000, 12(%eax)
movl %esi, 16(%eax)
movl $0x47300000, 20(%eax)
movl %edi, 24(%eax)
movl $0x49300000, 28(%eax)
LOAD_GOTREG(%edx)
fldl 0(%eax)
fsubl REF_GOTVAR(poly1305_x86_doffset0, %edx)
fldl 8(%eax)
fsubl REF_GOTVAR(poly1305_x86_doffset1, %edx)
fldl 16(%eax)
fsubl REF_GOTVAR(poly1305_x86_doffset2, %edx)
fldl 24(%eax)
fsubl REF_GOTVAR(poly1305_x86_doffset3, %edx)
fxch %st(3)
fstpl 0(%eax)
fxch %st(1)
fstl 8(%eax)
fmull REF_GOTVAR(poly1305_x86_scale, %edx)
fstpl 16(%eax)
fstl 24(%eax)
fmull REF_GOTVAR(poly1305_x86_scale, %edx)
fstpl 32(%eax)
fstl 40(%eax)
fmull REF_GOTVAR(poly1305_x86_scale, %edx)
fstpl 48(%eax)
fldz
fstl 56(%eax)
fstl 64(%eax)
fstl 72(%eax)
fstl 80(%eax)
fstl 88(%eax)
fstpl 96(%eax)
fldcw 4(%esp)
popl %eax
popl %eax
popl %edi
popl %esi
popl %ebx
ret
ENDFN(poly1305_init_ext)


FN(poly1305_blocks)
poly1305_blocks_x86_local:
movl %esp,%eax
andl $63,%eax
addl $192,%eax
subl %eax,%esp
movl %eax,0(%esp)
movl %ebx,4(%esp)
movl %esi,8(%esp)
movl %edi,12(%esp)
movl %ebp,16(%esp)
movl $0x137f, %ecx
movl %ecx, 188(%esp)
fstcw 184(%esp)
fldcw 188(%esp)
movl $0x43300000,100(%esp)
movl $0x45300000,108(%esp)
movl $0x47300000,116(%esp)
movl $0x49300000,124(%esp)
movl 4(%esp,%eax),%ebp
movl 8(%esp,%eax),%esi
movl 12(%esp,%eax),%ecx
LOAD_GOTREG(%edx)
cmp $16,%ecx
jb poly1305_blocks_x86_nomorebytes
fldt 92(%ebp)
fldt 80(%ebp)
fldt 68(%ebp)
fldt 56(%ebp)
add $16,%esi
sub $16,%ecx
movl %ecx, 20(%esp)
movl -4(%esi),%eax
movl -8(%esi),%ecx
movl -12(%esi),%ebx
movl -16(%esi),%edi
movl %eax,120(%esp)
movl %ecx,112(%esp)
movl %ebx,104(%esp)
movl %edi,96(%esp)
fxch %st(3)
faddl 120(%esp)
fsubl REF_GOTVAR(poly1305_x86_doffset3minustwo128, %edx)
fxch %st(1)
faddl 104(%esp)
fsubl REF_GOTVAR(poly1305_x86_doffset1, %edx)
fxch %st(2)
faddl 112(%esp)
fsubl REF_GOTVAR(poly1305_x86_doffset2, %edx)
fxch %st(3)
faddl 96(%esp)
fsubl REF_GOTVAR(poly1305_x86_doffset0, %edx)
movl 20(%esp), %ecx
cmp $16, %ecx
jb poly1305_blocks_x86_lastmultiply
poly1305_blocks_x86_multiplyaddatleast16bytes:
add $16,%esi
sub $16,%ecx
movl %ecx, 20(%esp)
movl -4(%esi),%eax
movl -8(%esi),%ecx
movl -12(%esi),%ebx
movl -16(%esi),%edi
movl %eax,120(%esp)
movl %ecx,112(%esp)
movl %ebx,104(%esp)
movl %edi,96(%esp)
fldl REF_GOTVAR(poly1305_x86_alpha130, %edx)
fadd %st(2),%st(0)
fsubl REF_GOTVAR(poly1305_x86_alpha130, %edx)
fsubr %st(0),%st(2)
fmull REF_GOTVAR(poly1305_x86_scale, %edx)
fldl REF_GOTVAR(poly1305_x86_alpha32, %edx)
fadd %st(2),%st(0)
fsubl REF_GOTVAR(poly1305_x86_alpha32, %edx)
fsubr %st(0),%st(2)
fxch %st(2)
faddp %st(0),%st(1)
fldl REF_GOTVAR(poly1305_x86_alpha64, %edx)
fadd %st(4),%st(0)
fsubl REF_GOTVAR(poly1305_x86_alpha64, %edx)
fsubr %st(0),%st(4)
fldl REF_GOTVAR(poly1305_x86_alpha96, %edx)
fadd %st(6),%st(0)
fsubl REF_GOTVAR(poly1305_x86_alpha96, %edx)
fsubr %st(0),%st(6)
fxch %st(6)
faddp %st(0),%st(1)
fxch %st(3)
faddp %st(0),%st(5)
fxch %st(3)
faddp %st(0),%st(1)
fldl 40(%ebp)
fmul %st(3),%st(0)
fldl 24(%ebp)
fmul %st(4),%st(0)
fldl 8(%ebp)
fmul %st(5),%st(0)
fldl 0(%ebp)
fmulp %st(0),%st(6)
fldl 24(%ebp)
fmul %st(4),%st(0)
faddp %st(0),%st(3)
fldl 8(%ebp)
fmul %st(4),%st(0)
faddp %st(0),%st(2)
fldl 0(%ebp)
fmul %st(4),%st(0)
faddp %st(0),%st(1)
fldl 48(%ebp)
fmulp %st(0),%st(4)
fxch %st(3)
faddp %st(0),%st(5)
fldl 8(%ebp)
fmul %st(4),%st(0)
faddp %st(0),%st(2)
fldl 0(%ebp)
fmul %st(4),%st(0)
faddp %st(0),%st(1)
fldl 48(%ebp)
fmul %st(4),%st(0)
faddp %st(0),%st(3)
fldl 32(%ebp)
fmulp %st(0),%st(4)
fxch %st(3)
faddp %st(0),%st(4)
fldl 0(%ebp)
fmul %st(5),%st(0)
faddp %st(0),%st(1)
fxch %st(3)
fldl 48(%ebp)
fmul %st(5),%st(0)
faddp %st(0),%st(3)
fxch %st(1)
fldl 32(%ebp)
fmul %st(5),%st(0)
faddp %st(0),%st(1)
fldl 16(%ebp)
fmulp %st(0),%st(5)
fxch %st(4)
faddp %st(0),%st(1)
movl 20(%esp), %ecx
fxch %st(2)
fldl 120(%esp)
fsubl REF_GOTVAR(poly1305_x86_doffset3minustwo128, %edx)
faddp %st(0),%st(1)
fxch %st(1)
fldl 112(%esp)
fsubl REF_GOTVAR(poly1305_x86_doffset2, %edx)
cmp $16,%ecx
faddp %st(0),%st(1)
fxch %st(3)
fldl 104(%esp)
fsubl REF_GOTVAR(poly1305_x86_doffset1, %edx)
faddp %st(0),%st(1)
fxch %st(2)
fldl 96(%esp)
fsubl REF_GOTVAR(poly1305_x86_doffset0, %edx)
faddp %st(0),%st(1)
jae poly1305_blocks_x86_multiplyaddatleast16bytes
poly1305_blocks_x86_lastmultiply:
fldl REF_GOTVAR(poly1305_x86_alpha130, %edx)
fadd %st(2),%st(0)
fsubl REF_GOTVAR(poly1305_x86_alpha130, %edx)
fsubr %st(0),%st(2)
fmull REF_GOTVAR(poly1305_x86_scale, %edx)
fldl REF_GOTVAR(poly1305_x86_alpha32, %edx)
fadd %st(2),%st(0)
fsubl REF_GOTVAR(poly1305_x86_alpha32, %edx)
fsubr %st(0),%st(2)
fldl REF_GOTVAR(poly1305_x86_alpha64, %edx)
fadd %st(5),%st(0)
fsubl REF_GOTVAR(poly1305_x86_alpha64, %edx)
fsubr %st(0),%st(5)
fldl REF_GOTVAR(poly1305_x86_alpha96, %edx)
fadd %st(7),%st(0)
fsubl REF_GOTVAR(poly1305_x86_alpha96, %edx)
fsubr %st(0),%st(7)
fxch %st(7)
faddp %st(0),%st(1)
fxch %st(5)
faddp %st(0),%st(1)
fxch %st(3)
faddp %st(0),%st(5)
faddp %st(0),%st(1)
fldl 40(%ebp)
fmul %st(1),%st(0)
fldl 24(%ebp)
fmul %st(2),%st(0)
fldl 8(%ebp)
fmul %st(3),%st(0)
fldl 0(%ebp)
fmulp %st(0),%st(4)
fldl 24(%ebp)
fmul %st(5),%st(0)
faddp %st(0),%st(3)
fldl 8(%ebp)
fmul %st(5),%st(0)
faddp %st(0),%st(2)
fldl 0(%ebp)
fmul %st(5),%st(0)
faddp %st(0),%st(1)
fldl 48(%ebp)
fmulp %st(0),%st(5)
fxch %st(4)
faddp %st(0),%st(3)
fldl 8(%ebp)
fmul %st(5),%st(0)
faddp %st(0),%st(2)
fldl 0(%ebp)
fmul %st(5),%st(0)
faddp %st(0),%st(1)
fldl 48(%ebp)
fmul %st(5),%st(0)
faddp %st(0),%st(4)
fldl 32(%ebp)
fmulp %st(0),%st(5)
fxch %st(4)
faddp %st(0),%st(2)
fldl 0(%ebp)
fmul %st(5),%st(0)
faddp %st(0),%st(1)
fldl 48(%ebp)
fmul %st(5),%st(0)
faddp %st(0),%st(4)
fldl 32(%ebp)
fmul %st(5),%st(0)
faddp %st(0),%st(3)
fldl 16(%ebp)
fmulp %st(0),%st(5)
fxch %st(4)
faddp %st(0),%st(1)
fstpt 56(%ebp)
fstpt 68(%ebp)
fstpt 80(%ebp)
fstpt 92(%ebp)
poly1305_blocks_x86_nomorebytes:
fldcw 184(%esp)
movl 0(%esp), %eax
movl 4(%esp), %ebx
movl 8(%esp), %esi
movl 12(%esp), %edi
movl 16(%esp), %ebp
addl %eax, %esp
ret
ENDFN(poly1305_blocks)


FN(poly1305_finish_ext)
poly1305_finish_ext_x86_local:
pushl %ebx
pushl %esi
pushl %edi
pushl %ebp
mov %esp, %ebp
andl $~63, %esp
subl $256, %esp
movl $0x137f, %ecx
movl %ecx, 0(%esp)
fstcw 4(%esp)
fldcw 0(%esp)
mov 20(%ebp), %ebx
mov 24(%ebp), %esi
mov 28(%ebp), %ecx
mov 32(%ebp), %eax
movl %ebp, 0(%esp)
movl %ebx, 4(%esp)
movl %eax, 8(%esp)
fldt 92(%ebx)
fldt 80(%ebx)
fldt 68(%ebx)
fldt 56(%ebx)
andl %ecx, %ecx
jz poly1305_finish_x86_nomorebytes
movl $0x43300000,100(%esp)
movl $0x45300000,108(%esp)
movl $0x47300000,116(%esp)
movl $0x49300000,124(%esp)
movl $0,64(%esp)
movl $0,4+64(%esp)
movl $0,8+64(%esp)
movl $0,12+64(%esp)
leal 64(%esp),%edi
rep movsb
movb $1,0(%edi)
movl 12+64(%esp),%eax
movl 8+64(%esp),%ecx
movl 4+64(%esp),%edx
movl 64(%esp),%esi
movl %eax,120(%esp)
movl %ecx,112(%esp)
movl %edx,104(%esp)
movl %esi,96(%esp)
LOAD_GOTREG(%edx)
fxch %st(3)
faddl 120(%esp)
fsubl REF_GOTVAR(poly1305_x86_doffset3, %edx)
fxch %st(2)
faddl 112(%esp)
fsubl REF_GOTVAR(poly1305_x86_doffset2, %edx)
fxch %st(1)
faddl 104(%esp)
fsubl REF_GOTVAR(poly1305_x86_doffset1, %edx)
fxch %st(3)
faddl 96(%esp)
fsubl REF_GOTVAR(poly1305_x86_doffset0, %edx)
fldl REF_GOTVAR(poly1305_x86_alpha130, %edx)
fadd %st(3),%st(0)
fsubl REF_GOTVAR(poly1305_x86_alpha130, %edx)
fsubr %st(0),%st(3)
fmull REF_GOTVAR(poly1305_x86_scale, %edx)
fldl REF_GOTVAR(poly1305_x86_alpha32, %edx)
fadd %st(2),%st(0)
fsubl REF_GOTVAR(poly1305_x86_alpha32, %edx)
fsubr %st(0),%st(2)
fldl REF_GOTVAR(poly1305_x86_alpha64, %edx)
fadd %st(6),%st(0)
fsubl REF_GOTVAR(poly1305_x86_alpha64, %edx)
fsubr %st(0),%st(6)
fldl REF_GOTVAR(poly1305_x86_alpha96, %edx)
fadd %st(5),%st(0)
fsubl REF_GOTVAR(poly1305_x86_alpha96, %edx)
fsubr %st(0),%st(5)
fxch %st(4)
faddp %st(0),%st(3)
fxch %st(6)
faddp %st(0),%st(1)
fxch %st(3)
faddp %st(0),%st(5)
fxch %st(3)
faddp %st(0),%st(1)
fldl 40(%ebx)
fmul %st(3),%st(0)
fldl 24(%ebx)
fmul %st(4),%st(0)
fldl 8(%ebx)
fmul %st(5),%st(0)
fldl 0(%ebx)
fmulp %st(0),%st(6)
fldl 24(%ebx)
fmul %st(5),%st(0)
faddp %st(0),%st(3)
fldl 8(%ebx)
fmul %st(5),%st(0)
faddp %st(0),%st(2)
fldl 0(%ebx)
fmul %st(5),%st(0)
faddp %st(0),%st(1)
fldl 48(%ebx)
fmulp %st(0),%st(5)
fxch %st(4)
faddp %st(0),%st(5)
fldl 8(%ebx)
fmul %st(6),%st(0)
faddp %st(0),%st(2)
fldl 0(%ebx)
fmul %st(6),%st(0)
faddp %st(0),%st(1)
fldl 48(%ebx)
fmul %st(6),%st(0)
faddp %st(0),%st(4)
fldl 32(%ebx)
fmulp %st(0),%st(6)
fxch %st(5)
faddp %st(0),%st(4)
fldl 0(%ebx)
fmul %st(2),%st(0)
faddp %st(0),%st(1)
fldl 48(%ebx)
fmul %st(2),%st(0)
faddp %st(0),%st(5)
fldl 32(%ebx)
fmul %st(2),%st(0)
faddp %st(0),%st(3)
fldl 16(%ebx)
fmulp %st(0),%st(2)
fxch %st(1)
faddp %st(0),%st(3)
fxch %st(3)
fxch %st(2)
poly1305_finish_x86_nomorebytes:
fldl REF_GOTVAR(poly1305_x86_alpha130, %edx)
fadd %st(4),%st(0)
fsubl REF_GOTVAR(poly1305_x86_alpha130, %edx)
fsubr %st(0),%st(4)
fmull REF_GOTVAR(poly1305_x86_scale, %edx)
fldl REF_GOTVAR(poly1305_x86_alpha32, %edx)
fadd %st(2),%st(0)
fsubl REF_GOTVAR(poly1305_x86_alpha32, %edx)
fsubr %st(0),%st(2)
fldl REF_GOTVAR(poly1305_x86_alpha64, %edx)
fadd %st(4),%st(0)
fsubl REF_GOTVAR(poly1305_x86_alpha64, %edx)
fsubr %st(0),%st(4)
fldl REF_GOTVAR(poly1305_x86_alpha96, %edx)
fadd %st(6),%st(0)
fsubl REF_GOTVAR(poly1305_x86_alpha96, %edx)
fxch %st(6)
fsub %st(6),%st(0)
fxch %st(4)
faddp %st(0),%st(3)
fxch %st(4)
faddp %st(0),%st(1)
fxch %st(2)
faddp %st(0),%st(3)
fxch %st(4)
faddp %st(0),%st(3)
fxch %st(3)
faddl REF_GOTVAR(poly1305_x86_hoffset0, %edx)
fxch %st(3)
faddl REF_GOTVAR(poly1305_x86_hoffset1, %edx)
fxch %st(1)
faddl REF_GOTVAR(poly1305_x86_hoffset2, %edx)
fxch %st(2)
faddl REF_GOTVAR(poly1305_x86_hoffset3, %edx)
fxch %st(3)
fstpl 96(%esp)
fstpl 104(%esp)
fstpl 112(%esp)
fstpl 120(%esp)
movl 100(%esp),%eax
and $63,%eax
movl 108(%esp),%ecx
and $63,%ecx
movl 116(%esp),%edx
and $63,%edx
movl 124(%esp),%ebx
and $63,%ebx
movl 104(%esp),%esi
addl %eax,%esi
movl %esi,28(%esp)
movl 112(%esp),%eax
adcl %ecx,%eax
movl %eax,32(%esp)
movl 120(%esp),%eax
adcl %edx,%eax
movl %eax,36(%esp)
mov $0,%eax
adcl %ebx,%eax
movl %eax,40(%esp)
mov $5,%eax
movl 96(%esp),%ecx
addl %ecx,%eax
movl %eax,44(%esp)
mov $0,%eax
movl 28(%esp),%edx
adcl %edx,%eax
movl %eax,28(%esp)
mov $0,%eax
movl 32(%esp),%ebx
adcl %ebx,%eax
movl %eax,32(%esp)
mov $0,%eax
movl 36(%esp),%esi
adcl %esi,%eax
movl %eax,36(%esp)
mov $0xfffffffc,%eax
movl 40(%esp),%edi
adcl %edi,%eax
sar $16,%eax
mov %eax,%edi
xor $0xffffffff,%edi
andl %eax,%ecx
movl 44(%esp),%ebp
andl %edi,%ebp
orl %ebp,%ecx
andl %eax,%edx
movl 28(%esp),%ebp
andl %edi,%ebp
orl %ebp,%edx
andl %eax,%ebx
movl 32(%esp),%ebp
andl %edi,%ebp
orl %ebp,%ebx
andl %eax,%esi
movl 36(%esp),%eax
andl %edi,%eax
orl %eax,%esi
movl 4(%esp),%eax
addl 104(%eax),%ecx
adcl 108(%eax),%edx
adcl 112(%eax),%ebx
adcl 116(%eax),%esi
movl 8(%esp),%eax
movl %ecx,0(%eax)
movl %edx,4(%eax)
movl %ebx,8(%eax)
movl %esi,12(%eax)
xorl %eax, %eax
movl 4(%esp),%edi
fldz
fstl 0(%edi)
fstl 8(%edi)
fstl 16(%edi)
fstl 24(%edi)
fstl 32(%edi)
fstl 40(%edi)
fstl 48(%edi)
fstl 56(%edi)
fstl 64(%edi)
fstl 72(%edi)
fstl 80(%edi)
fstl 88(%edi)
fstl 96(%edi)
fstl 104(%edi)
fstpl 112(%edi)
movl 0(%esp), %esp
popl %ebp
popl %edi
popl %esi
popl %ebx
ret
ENDFN(poly1305_finish_ext)

FN(poly1305_auth)
pushl %ebp
pushl %edi
movl %esp, %ebp
subl $128, %esp
andl $~63, %esp
movl %esp, %edi
pushl 24(%ebp)
pushl %edi
calll poly1305_init_ext_x86_local
movl 20(%ebp), %ecx
andl $~15, %ecx
jz poly1305_auth_x86_no_data
pushl %ecx
pushl 16(%ebp)
addl %ecx, 16(%ebp)
pushl %edi
calll poly1305_blocks_x86_local
poly1305_auth_x86_no_data:
pushl 12(%ebp)
movl 20(%ebp), %ecx
andl $15, %ecx
pushl %ecx
pushl 16(%ebp)
pushl %edi
calll poly1305_finish_ext_x86_local
movl %ebp, %esp
popl %edi
popl %ebp
ret
ENDFN(poly1305_auth)


.section rodata, "a"

.p2align 4,,15
poly1305_x86_scale: .long 0x0,0x37f40000
poly1305_x86_two32: .long 0x0,0x41f00000
poly1305_x86_two64: .long 0x0,0x43f00000
poly1305_x86_two96: .long 0x0,0x45f00000
poly1305_x86_alpha32: .long 0x0,0x45e80000
poly1305_x86_alpha64: .long 0x0,0x47e80000
poly1305_x86_alpha96: .long 0x0,0x49e80000
poly1305_x86_alpha130: .long 0x0,0x4c080000
poly1305_x86_doffset0: .long 0x0,0x43300000
poly1305_x86_doffset1: .long 0x0,0x45300000
poly1305_x86_doffset2: .long 0x0,0x47300000
poly1305_x86_doffset3: .long 0x0,0x49300000
poly1305_x86_doffset3minustwo128: .long 0x0,0x492ffffe
poly1305_x86_hoffset0: .long 0xfffffffb,0x43300001
poly1305_x86_hoffset1: .long 0xfffffffe,0x45300001
poly1305_x86_hoffset2: .long 0xfffffffe,0x47300001
poly1305_x86_hoffset3: .long 0xfffffffe,0x49300003

ENDFILE()
