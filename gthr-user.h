#ifndef GTHR_USER_H
#define GTHR_USER_H

#ifndef _GLIBCXX_HAS_GTHREADS
#error "add the _GLIBCXX_HAS_GTHREADS define"
#endif

#include <compare>

#include <ctime>
#include <sys/_pthreadtypes.h>
#include <errno.h>

extern "C" {
#include <FreeRTOS.h>
#include <semphr.h>
#include <task.h>
}

using __gthread_time_t = struct timespec;
using __gthread_cond_t = pthread_cond_t;
using __gthread_once_t = int;

#define __GTHREAD_ONCE_INIT 0
#define __GTHREAD_COND_INIT_FUNCTION

// #define __GTHREAD_MUTEX_INIT 0
#define __GTHREAD_MUTEX_INIT_FUNCTION __gthread_mutex_init_func
#define __GTHREAD_RECURSIVE_MUTEX_INIT_FUNCTION __gthread_recursive_mutex_init_func

typedef struct gthread {
	TaskHandle_t taskID;
	char *name;
	int priority;
	uint32_t instances;
	uint32_t stacksize;

	friend bool operator==(const gthread& a, const gthread& b) {
		return a.taskID == b.taskID;
	}
	constexpr std::strong_ordering operator<=>(const gthread& a) const {
		return std::strong_ordering::less;
	};
} __gthread_t;

typedef struct gthreadMutexType {
	StaticSemaphore_t mutexBuffer;
	SemaphoreHandle_t mutex;
} __gthread_mutex_t;

using __gthread_recursive_mutex_t = __gthread_mutex_t;

inline void __gthread_mutex_init_func(__gthread_mutex_t *__mutex) {
    __mutex->mutex = xSemaphoreCreateMutexStatic(&__mutex->mutexBuffer);
}

inline int __gthread_mutex_lock(__gthread_mutex_t *__mutex) {
  BaseType_t const result {xSemaphoreTake(__mutex->mutex, portMAX_DELAY)};
  return result == pdTRUE ? 0 : EINVAL;
}

inline int __gthread_mutex_trylock(__gthread_mutex_t *__mutex) {
  BaseType_t const result {xSemaphoreTake(__mutex->mutex, 0)};
  return static_cast<int>(result == pdTRUE);
}

inline int __gthread_mutex_unlock(__gthread_mutex_t *__mutex) {
  BaseType_t const result {xSemaphoreGive(__mutex->mutex)};
  return result == pdTRUE ? 0 : EINVAL;
}

inline int __gthread_mutex_destroy(__gthread_mutex_t *__mutex) {
  vSemaphoreDelete(__mutex->mutex);
  return 0;
}

//  recursive mutex
inline void __gthread_recursive_mutex_init_func(__gthread_recursive_mutex_t *__mutex) {
  __mutex->mutex = xSemaphoreCreateRecursiveMutexStatic(&__mutex->mutexBuffer);
}

inline int __gthread_recursive_mutex_lock(__gthread_recursive_mutex_t *__mutex) {
  BaseType_t const result {xSemaphoreTakeRecursive(__mutex->mutex, portMAX_DELAY)};
  return result == pdTRUE ? 0 : EINVAL;
}

inline int __gthread_recursive_mutex_trylock (__gthread_recursive_mutex_t *__mutex) {
  BaseType_t const result {xSemaphoreTakeRecursive(__mutex->mutex, 0)};
  return static_cast<int>(result == pdTRUE);
}

inline int __gthread_recursive_mutex_unlock (__gthread_recursive_mutex_t *__mutex) {
  BaseType_t const result {xSemaphoreGiveRecursive(__mutex->mutex)};
  return result == pdTRUE ? 0 : EINVAL;
}

inline int __gthread_recursive_mutex_destroy(__gthread_recursive_mutex_t *__mutex) {
	vSemaphoreDelete(__mutex->mutex);
	return 0;
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

#ifndef _GTHREAD_USE_MUTEX_TIMEDLOCK
#warning "GTHREAD_USE_MUTEX_TIMEDLOCK is off"
#endif
inline int __gthread_mutex_timedlock(__gthread_mutex_t *__mutex,
			                         const __gthread_time_t *__abs_timeout) {
  TickType_t const ms {
    static_cast<TickType_t>(__abs_timeout->tv_sec * 1000U) +
    static_cast<TickType_t>(__abs_timeout->tv_nsec / 1000000U)};
  BaseType_t result {xSemaphoreTake(__mutex->mutex, ms)};
  return result == pdTRUE ? 0 : EINVAL;
}

inline int __gthread_recursive_mutex_timedlock (__gthread_recursive_mutex_t *__mutex,
				     const __gthread_time_t *__abs_timeout) {
  TickType_t const ms {
    static_cast<TickType_t>(__abs_timeout->tv_sec * 1000U) +
    static_cast<TickType_t>(__abs_timeout->tv_nsec / 1000000U)};
  BaseType_t result {xSemaphoreTakeRecursive(__mutex->mutex, ms)};
  return result == pdTRUE ? 0 : EINVAL;
}

#endif  // GTHR_USER_H
