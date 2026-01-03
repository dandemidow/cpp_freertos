#ifndef _GLIBCXX_HAS_GTHREADS
#error "glibcxx thread have to be done"
#endif
#include <thread>

#include "FreeRTOS.h"
#include "task.h"

#include "gthr-user.h"

namespace std {


static void execute_native_thread_routine(void* __p) {
    thread::_State_ptr __t{ static_cast<thread::_State*>(__p) };
    __t->_M_run();
    vTaskDelete(NULL);
}

void thread::_M_start_thread(thread::_State_ptr state, void (*depend)()) {
    // Make sure it's not optimized out, not even with LTO.
    //asm ("" : : "rm" (depend));

    auto &thread = this->_M_id._M_thread;
    thread.stacksize = 256;
    thread.priority = tskIDLE_PRIORITY + 1;
    BaseType_t result = xTaskCreate(static_cast<TaskFunction_t>(&execute_native_thread_routine),
        		                    (const portCHAR *)thread.name,
                                    thread.stacksize,
    				                state.get(),
    				                thread.priority,
                                    &thread.taskID);
    if (result != pdPASS)  {
    	__throw_system_error(static_cast<int32_t>(result));
    }
    state.release();
}

void thread::join() {
	if (_M_id != id{}) {
        eTaskState state {};
        while((state = eTaskGetState(_M_id._M_thread.taskID)) != eDeleted) {
        	switch(state) {
        	case eInvalid:
        		__throw_system_error(eInvalid);
        		break;
        	default:
        		//__wfi();
        		//__asm volatile( "wfi" );
        		taskYIELD();
        	}
        }
        this->_M_id = id{};
	}
}

void thread::detach() {

}

thread::_State::~_State() = default;

}  // namespace std
