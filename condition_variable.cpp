#include <condition_variable>

#include "FreeRTOS.h"

namespace std {

condition_variable::condition_variable() noexcept = default;

condition_variable::~condition_variable() noexcept = default;

void condition_variable::wait(unique_lock<mutex>& __lock) {
    _M_cond.wait(*__lock.mutex());
}

void condition_variable::notify_one() noexcept {
    _M_cond.notify_one();
}

void condition_variable::notify_all() noexcept {
    _M_cond.notify_all();
}

}  // namespace std
