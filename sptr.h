#ifdef __cplusplus
extern "C" {
#endif

#ifndef SPTR_H
#define SPTR_H

#include <stdlib.h>

#if defined(__cplusplus) || defined(bool)
  #define sptr_boolean_type bool
#else
  #define sptr_boolean_type int
#endif

void* make_sptr(void* ptr, size_t nitems, size_t size);
void* make_ptr(void* sptr);
void free_sptr(void* sptr);
void* malloc_sptr(size_t nitems, size_t size);
void* realloc_sptr(void* sptr, size_t nitems);
void* calloc_sptr(size_t nitems, size_t size);
size_t sptr_size(void* sptr);
size_t sptr_item_size(void* sptr);
size_t sptr_length(void* sptr);
void* add_realloc_sptr(void* sptr, size_t additional_items);
void* subtract_realloc_sptr(void* sptr, size_t removed_items);
void* resize_sptr(void* sptr, size_t new_size);
void* sptr_multiply_allocation(void* sptr, double multiplier);
void* sptr_end(void* sptr);
sptr_boolean_type in_sptr(void* sptr, void* other);

#endif

#ifdef __cplusplus
}
#endif