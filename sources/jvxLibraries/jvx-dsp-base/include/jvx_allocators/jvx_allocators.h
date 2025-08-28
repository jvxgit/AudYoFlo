#ifndef __JVX_ALLOCATORS_H__
#define __JVX_ALLOCATORS_H__

#define JVX_GET_GLOBAL_ALLOCATOR_REFERENCE jvx_get_global_allocator()

struct jvx_allocator_references;

// struct jvx_allocator_references* jvx_get_global_allocator(void);

// The requirement of void is pretty old stuff,
// https://stackoverflow.com/questions/47926942/c-warning-function-was-used-with-no-prototype-before-its-definition

int jvx_set_global_allocator(struct jvx_allocator_references* ref);
void jvx_reset_global_allocator();

#ifndef JVX_DEFINE_ALLOCATOR_SYMBOL
extern struct jvx_allocator_references* jvx_allocator;
#endif

#endif
