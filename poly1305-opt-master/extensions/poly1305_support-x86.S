/* handles including available x86 extensions and providing a cpuid implementation */

#define IS_X86_64 (defined(__amd64__) || defined(__amd64) || defined(__x86_64__) || defined(_M_X64))

/* extensions */
#if (IS_X86_64)
	#if defined(POLY1305_EXT_AVX2)
		#include "poly1305_avx2-64.S"
	#endif
	#if defined(POLY1305_EXT_AVX)
		#include "poly1305_avx-64.S"
	#endif
	#include "poly1305_x86-64.S"
#else
	#if defined(POLY1305_EXT_AVX2)
		#include "poly1305_avx2-32.S"
	#endif
	#if defined(POLY1305_EXT_AVX)
		#include "poly1305_avx-32.S"
	#endif
	#if defined(POLY1305_EXT_SSE2)
		#include "poly1305_sse2-32.S"
	#endif
	#include "poly1305_x86-32.S"
#endif

.text

/* cpuid */
#define CPUID_MMX    (1 << 0)
#define CPUID_SSE    (1 << 1)
#define CPUID_SSE2   (1 << 2)
#define CPUID_SSE3   (1 << 3)
#define CPUID_SSSE3  (1 << 4)
#define CPUID_SSE4_1 (1 << 5)
#define CPUID_SSE4_2 (1 << 6)
#define CPUID_AVX    (1 << 7)
#define CPUID_XOP    (1 << 8)
#define CPUID_AVX2   (1 << 9)

SYSGLOBAL(poly1305_cpuid)
SYSFN(poly1305_cpuid)
	#if (IS_X86_64)
		pushq %rbx
		pushq %rsi
		pushq %rdi
	#else
		pushl %ebx
		pushl %esi
		pushl %edi
	#endif

	xorl %esi, %esi

	#if !(IS_X86_64)
		pushfl
		popl %eax
		movl %eax, %ecx
		xorl $0x200000, %eax
		pushl %eax
		popfl
		pushfl
		popl %eax
		xorl %ecx, %eax
		shrl $21, %eax
		andl $1, %eax
		pushl %ecx
		popfl
		andl %eax, %eax
		jz poly1305_cpuid_done
	#endif

	/* cpuid 0 */
	xorl %eax, %eax
	xorl %ecx, %ecx
	cpuid

	movl %eax, %edi /* max level */
	testl $0x00000500, %edi
	jz poly1305_continue1

	/* Intel P5 pre-B0 */
	orl $CPUID_MMX, %esi
	jmp poly1305_cpuid_done

poly1305_continue1:
	/* cpuid 1 */
	movl $1, %eax
	xorl %ecx, %ecx
	cpuid

	#define TEST_FOR(reg, value, flag) \
		testl $value, reg;         \
		jz 1f;                     \
		orl $flag, %esi;           \
		1:;

	TEST_FOR(%ecx, (1 << 20), CPUID_SSE4_2)
	TEST_FOR(%ecx, (1 << 19), CPUID_SSE4_1)
	TEST_FOR(%ecx, (1 <<  9), CPUID_SSSE3)
	TEST_FOR(%ecx, (1      ), CPUID_SSE3)
	TEST_FOR(%edx, (1 << 26), CPUID_SSE2)
	TEST_FOR(%edx, (1 << 23), CPUID_SSE)
	TEST_FOR(%ecx, (1 << 23), CPUID_MMX)

#if defined(POLY1305_EXT_AVX)
		testl $(1 << 27), %ecx
		jz poly1305_cpuid_continue2
		testl $(1 << 28), %ecx
		jz poly1305_cpuid_continue2

		/* xgetbv 0 */
		xorl %ecx, %ecx
		xgetbv

		testl $(6), %eax
		jz poly1305_cpuid_continue2
		orl $CPUID_AVX, %esi

		/* max level >= 7 */
		cmpl $7, %edi
		jb poly1305_cpuid_noavx2

		/* cpuid 7 */
		movl $7, %eax
		xorl %ecx, %ecx
		cpuid

		TEST_FOR(%ebx, (1 << 5), CPUID_AVX2)

		/* test for xop */
	poly1305_cpuid_noavx2:
		/* cpuid 0x80000000 */
		movl $0x80000000, %eax
		xorl %ecx, %ecx
		cpuid

		/* max extended level >= 0x80000001 */
		cmpl $0x80000001, %eax
		jb poly1305_cpuid_continue2

		/* cpuid 0x80000001 */
		movl $0x80000001, %eax
		xorl %ecx, %ecx
		cpuid

		TEST_FOR(%ecx, (1 << 11), CPUID_XOP)

	poly1305_cpuid_continue2:
#endif /* POLY1305_EXT_AVX */

poly1305_cpuid_done:
	movl %esi, %eax

	#if (IS_X86_64)
		popq %rdi
		popq %rsi
		popq %rbx
	#else
		popl %edi
		popl %esi
		popl %ebx
	#endif

	ret
SYSENDFN(poly1305_cpuid)
