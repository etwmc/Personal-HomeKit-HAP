#pragma once
//simple pthread emulation by rti

#include <windows.h>
#include <process.h>

typedef CRITICAL_SECTION pthread_mutex_t;

static void pthread_mutex_init(pthread_mutex_t* mutex, void* nazo)
{
	InitializeCriticalSection(mutex);
}

static void pthread_mutex_destroy(pthread_mutex_t* mutex)
{
	DeleteCriticalSection(mutex);
}

static void pthread_mutex_lock(pthread_mutex_t* mutex)
{
	EnterCriticalSection(mutex);
}

static void pthread_mutex_unlock(pthread_mutex_t* mutex)
{
	LeaveCriticalSection(mutex);
}

struct _threadcallback{
	static unsigned int __stdcall call(void* arg){
		_threadcallback* _this = ((_threadcallback*)arg);
		_this->start_routine(_this->arg);

		delete _this;
		return 0;
	}
		
	void *(*start_routine) (void *);
	void *arg;
};

typedef HANDLE pthread_t;
typedef void* pthread_attr_t;
static void pthread_create(pthread_t* thread, const pthread_attr_t *attr_null,void *(*start_routine) (void *), void *arg)
{
	_threadcallback* c = new _threadcallback;
	c->start_routine = start_routine;
	c->arg = arg;
	
	*thread = (HANDLE)_beginthreadex(NULL , 0 , _threadcallback::call , (void*)c ,  0 ,NULL );
}

static void pthread_join(pthread_t* thread,void * nazo)
{
	::WaitForSingleObject( thread , INFINITE);
}
