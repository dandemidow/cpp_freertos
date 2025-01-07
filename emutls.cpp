#include <cstdint>
#include <cstdlib>

#ifndef _GLIBCXX_HAVE_TLS
#warning "GLIBCXX_HAVE_TLS is off"
#endif

typedef unsigned int word __attribute__((mode(word)));
typedef unsigned int pointer __attribute__((mode(pointer)));
struct __emutls_object
{
  word size;
  word align;
  union {
    pointer offset;
    void *ptr;
  } loc;
  void *templ;
};

static_assert(sizeof(struct __emutls_object) == 16);

extern "C"
void *emutls_alloc(struct __emutls_object *obj) {
  void *ptr;
  ptr = malloc(obj->size);
  return ptr;
}

static int __emutls_index {};

extern "C"
void *__emutls_get_address (void *o) {
	struct __emutls_object * obj = static_cast<struct __emutls_object *>(o);
	if (obj->loc.offset == 0) {
		obj->loc.offset = ++__emutls_index;
	}
	int const index {static_cast<int32_t>(obj->loc.offset) - 1};
	void *ptr = pvTaskGetThreadLocalStoragePointer(NULL, index);
	if (ptr == nullptr) {
		ptr = emutls_alloc(obj);
		vTaskSetThreadLocalStoragePointer(NULL, index, ptr);
	}
	return ptr;
}

