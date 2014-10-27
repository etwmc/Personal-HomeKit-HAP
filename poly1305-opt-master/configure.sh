#!/bin/sh

cc=gcc

while [ "$1" != "" ]; do
    case $1 in
        --compiler )           shift
                               cc=$1
                               ;;
    esac
    shift
done

> poly1305_config.inc

echo ".text;vpaddq %ymm0, %ymm0, %ymm0" | $cc -x assembler -c -o test.o - 2>/dev/null
if [ "$?" -eq "0" ]; then
	echo "#define POLY1305_EXT_AVX2" >> poly1305_config.inc
fi

echo ".text;vpaddq %xmm0, %xmm0, %xmm0" | $cc -x assembler -c -o test.o - 2>/dev/null
if [ "$?" -eq "0" ]; then
	echo "#define POLY1305_EXT_AVX" >> poly1305_config.inc
fi

echo ".text;pmuludq %xmm0, %xmm0" | $cc -x assembler -c -o test.o - 2>/dev/null
if [ "$?" -eq "0" ]; then
	echo "#define POLY1305_EXT_SSE2" >> poly1305_config.inc
fi

echo ".text;xor %eax, %eax" | $cc -x assembler -c -o test.o - 2>/dev/null
if [ "$?" -eq "0" ]; then
	echo "#define POLY1305_EXT_X86" >> poly1305_config.inc
fi

echo ".text;xor %rax, %rax" | $cc -x assembler -c -o test.o - 2>/dev/null
if [ "$?" -eq "0" ]; then
	echo "#define POLY1305_EXT_X86_64" >> poly1305_config.inc
fi

echo "int main(void) { return (((unsigned long long)0x9edcba98 * (unsigned long)0xd7f7f7f7) >> 60); }" | $cc -x c -o test - 2>/dev/null && ./test 2>/dev/null
if [ "$?" -eq "8" ]; then
	echo "#define POLY1305_EXT_REF_32" >> poly1305_config.inc
else
	echo "int main(void) { return (((unsigned short)0x98 * (unsigned char)0xf7) >> 12); }" | $cc -x c -o test - 2>/dev/null && ./test 2>/dev/null
	if [ "$?" -eq "9" ]; then
		echo "#define POLY1305_EXT_REF_8" >> poly1305_config.inc
	else
		echo "Unable to find a working reference implementation!"
	fi
fi

rm test.o -f
rm test -f
