//<unistd.h> in windows
//orignal soruce code: http://d.hatena.ne.jp/deraw/20070517/1179334643
#ifndef _UNISTD_H_
#define _UNISTD_H_
#ifdef _WIN32
#pragma once

#include <io.h>
/*
access
chmod
chsize
close
creat
dup
dup2
eof
filelength
isatty
locking
lseek
mktemp
open
read
setmode
sopen
tell
umask
unlink
write
*/
#include <direct.h>
/*
chdir
getcwd
mkdir
rmdir
*/
#include <process.h>
/*
cwait
execl
execle
execlp
execlpe
execv
execve
execvp
execvpe
spawnl
spawnle
spawnlp
spawnlpe
spawnv
spawnve
spawnvp
spawnvpe

*/
#pragma comment( lib, "ws2_32" )
#include <winsock2.h>
/*
gethostname
*/

#include <process.h>
/*
_getpid
*/
#include <time.h>

#ifdef	__cplusplus
extern "C" {
#endif

typedef int	pid_t;			/* process id type	*/

#ifndef _SSIZE_T_DEFINED
typedef int ssize_t;
#define _SSIZE_T_DEFINED
#endif

#ifndef vsnprintf
#define vsnprintf(s, n, format, ...) _vsnprintf_s(s, n, _TRUNCATE, format, __VA_ARGS__)
#endif

#ifndef snprintf
#define snprintf(s, n, format, ...) _snprintf_s(s, n, _TRUNCATE, format, __VA_ARGS__)
#endif


#define nice(incr) (SetPriorityClass(GetCurrentProcess(),incr))//TODO
#define sleep(seconds) (Sleep(seconds*1000))
#define usleep(useconds) (Sleep(useconds))

#define stime(tp) UNISTD_stime(tp)
__forceinline int UNISTD_stime(const time_t *tp ){
	FILETIME ft;
	SYSTEMTIME st;
    LONGLONG ll = Int32x32To64(*tp, 10000000) + 116444736000000000;
    ft.dwLowDateTime = (DWORD) ll;
    ft.dwHighDateTime = (DWORD)(ll >>32);
    FileTimeToSystemTime(&ft,&st);
	return SetSystemTime(&st);
}

//<sys/stat.h>
#define fstat64(fildes, stat) (_fstati64(fildes, stat))
#define stat64(path, buffer) (_stati64(path,buffer))

#ifdef	__cplusplus
}
#endif
#endif /* _WIN32 */
#endif /* _UNISTD_H_ */
