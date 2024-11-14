#ifndef GTHR_USER_H
#define GTHR_USER_H

#include <ctime>
#include <sys/_pthreadtypes.h>
#include <errno.h>

extern "C" {
#include <FreeRTOS.h>
#include <semphr.h>
}

using __gthread_time_t = struct timespec;
using __gthread_cond_t = pthread_cond_t;
//using __gthread_mutex_t = int;
using __gthread_t = pthread_t;
using __gthread_recursive_mutex_t = int;
using __gthread_once_t = int;

#define __GTHREAD_ONCE_INIT 0
#define __GTHREAD_COND_INIT_FUNCTION

// #define __GTHREAD_MUTEX_INIT 0
#define __GTHREAD_MUTEX_INIT_FUNCTION(mx) do {} while (0)
#define __GTHREAD_RECURSIVE_MUTEX_INIT 0

typedef struct gthreadMutexType {
	StaticSemaphore_t mutexBuffer;
	SemaphoreHandle_t mutex;
} __gthread_mutex_t;

inline void __gthread_mutex_init_func (__gthread_mutex_t *__mutex) {
    __mutex->mutex = xSemaphoreCreateMutexStatic(&__mutex->mutexBuffer);
}

inline int __gthread_mutex_lock (__gthread_mutex_t *__mutex) {
  BaseType_t result {xSemaphoreTake(__mutex->mutex, portMAX_DELAY)};
  return result == pdTRUE ? 0 : EINVAL;
}

inline int __gthread_mutex_trylock (__gthread_mutex_t *__mutex) {
  BaseType_t result {xSemaphoreTake(__mutex->mutex, 0)};
  return static_cast<int>(result == pdTRUE);
}

inline int __gthread_mutex_unlock (__gthread_mutex_t *__mutex) {
  BaseType_t result {xSemaphoreGive(__mutex->mutex)};
  return result == pdTRUE ? 0 : EINVAL;
}

inline int __gthread_mutex_destroy (__gthread_mutex_t *__mutex) {
  vSemaphoreDelete(__mutex->mutex);
  return 0;
}


inline int __gthread_recursive_mutex_lock (__gthread_recursive_mutex_t *__mutex) {
  return 0;//__gthread_mutex_lock (__mutex);
}

inline int __gthread_recursive_mutex_trylock (__gthread_recursive_mutex_t *__mutex) {
  return 0;//__gthread_mutex_trylock (__mutex);
}

inline int __gthread_recursive_mutex_unlock (__gthread_recursive_mutex_t *__mutex) {
  return 0;//__gthread_mutex_unlock (__mutex);
}


inline int __gthread_once (__gthread_once_t *__once, void (*__func) (void)) {
  return 0;
}


inline int __gthread_cond_wait (__gthread_cond_t *__cond, __gthread_mutex_t *__mutex) {
  // _Condition_Wait (__cond, __mutex);
  return 0;
}

inline int __gthread_cond_destroy (__gthread_cond_t *__cond) {
  return 0;
}

inline int __gthread_cond_timedwait (__gthread_cond_t *__cond,
		                             __gthread_mutex_t *__mutex,
			                         const __gthread_time_t *__abs_timeout) {
  return 0; //_Condition_Wait_timed (__cond, __mutex, __abs_timeout);
}

inline int __gthread_cond_signal (__gthread_cond_t *__cond) {
  // _Condition_Signal (__cond);
  return 0;
}

inline int __gthread_cond_broadcast (__gthread_cond_t *__cond) {
  // _Condition_Broadcast (__cond);
  return 0;
}

inline __gthread_t __gthread_self (void) {
  return {}; //pthread_self ();
}

inline int __gthread_mutex_timedlock (__gthread_mutex_t *__mutex,
			   const __gthread_time_t *__abs_timeout) {
  return 0; //_Mutex_Acquire_timed (__mutex, __abs_timeout);
}

inline int __gthread_recursive_mutex_timedlock (__gthread_recursive_mutex_t *__mutex,
				     const __gthread_time_t *__abs_timeout) {
  return 0; // _Mutex_recursive_Acquire_timed (__mutex, __abs_timeout);
}

#endif  // GTHR_USER_H
