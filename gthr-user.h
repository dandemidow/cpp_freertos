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
#include "simple_ring_buffer.hpp"

#define __GTHREAD_ONCE_INIT 0
#define __GTHREAD_COND_INIT_FUNCTION __gthread_cond_init_func

// #define __GTHREAD_MUTEX_INIT 0
#define __GTHREAD_MUTEX_INIT_FUNCTION __gthread_mutex_init_func
#define __GTHREAD_RECURSIVE_MUTEX_INIT_FUNCTION __gthread_recursive_mutex_init_func

#if defined(_GLIBCXX_HAVE_PLATFORM_WAIT)
namespace std::__detail {
inline void __platform_notify(const void *, uint32_t) {}
inline void __platform_wait(const void *, uint32_t) {}
}
#endif

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

struct freertos_cond_t {
	__gthread_mutex_t ring_mutex_;
	SimpleRingBuffer<TaskHandle_t> waiting_ring;
};

using __gthread_time_t = struct timespec;
using __gthread_cond_t = freertos_cond_t;
using __gthread_once_t = int;
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

static inline void __gthread_cond_init_func (__gthread_cond_t *__cond) {
	__gthread_mutex_init_func(&__cond->ring_mutex_);
}

inline int __gthread_cond_wait (__gthread_cond_t *__cond, __gthread_mutex_t *__mutex) {
  TaskHandle_t current {xTaskGetCurrentTaskHandle()};
  __gthread_mutex_lock(&__cond->ring_mutex_);
  __cond->waiting_ring.Push(current);
  __gthread_mutex_unlock(&__cond->ring_mutex_);
  __gthread_mutex_unlock(__mutex);
  vTaskSuspend(current);
  __gthread_mutex_lock(__mutex);
  return 0;
}

inline int __gthread_cond_destroy (__gthread_cond_t *__cond) {
  return __gthread_mutex_destroy(&__cond->ring_mutex_);
}

inline int __gthread_cond_timedwait (__gthread_cond_t *__cond,
		                             __gthread_mutex_t *__mutex,
			                         const __gthread_time_t *__abs_timeout) {
  return 0; //_Condition_Wait_timed (__cond, __mutex, __abs_timeout);
}

inline int __gthread_cond_signal (__gthread_cond_t *__cond) {
	__gthread_mutex_lock(&__cond->ring_mutex_);
	TaskHandle_t current{__cond->waiting_ring.Pop()};
	__gthread_mutex_unlock(&__cond->ring_mutex_);
	if (current) {
	    vTaskResume(current);
	}
  return 0;
}

inline int __gthread_cond_broadcast (__gthread_cond_t *__cond) {
	__gthread_mutex_lock(&__cond->ring_mutex_);
	TaskHandle_t current{__cond->waiting_ring.Pop()};
	__gthread_mutex_unlock(&__cond->ring_mutex_);
	while(current != nullptr) {
		vTaskResume(current);
		__gthread_mutex_lock(&__cond->ring_mutex_);
		current = __cond->waiting_ring.Pop();
		__gthread_mutex_unlock(&__cond->ring_mutex_);
    }
  return 0;
}

inline __gthread_t __gthread_self (void) {
  return {}; //pthread_self ();
}

#ifndef _GTHREAD_USE_MUTEX_TIMEDLOCK
#warning "GTHREAD_USE_MUTEX_TIMEDLOCK is off"
#endif
extern "C" int _gettimeofday(struct timeval *tv, void *tzvp);
inline int __gthread_mutex_timedlock(__gthread_mutex_t *__mutex,
			                         const __gthread_time_t *__abs_timeout) {
  struct timeval tv {};
  _gettimeofday(&tv, NULL);
  TickType_t const ms {
    static_cast<TickType_t>((__abs_timeout->tv_sec - tv.tv_sec) * 1000U) +
    static_cast<TickType_t>((__abs_timeout->tv_nsec / 1000000U) - tv.tv_usec / 1000U)};
  BaseType_t result {xSemaphoreTake(__mutex->mutex, ms / portTICK_PERIOD_MS)};
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
