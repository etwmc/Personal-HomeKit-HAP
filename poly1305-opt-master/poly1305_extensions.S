#define POLY1305_ONEFILE

#include "extensions/poly1305_defines.inc"
#include "poly1305_config.inc"

/* everything x86 related */
#if defined(POLY1305_EXT_X86)
#include "extensions/poly1305_support-x86.S"
#endif

SYSENDFILE()
