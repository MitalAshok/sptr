/* Note that this uses the same header file, but probably has a larger padding. */

#include <string.h>

#include "sptr.h"

#define _sptr_min_pad (2 * sizeof(size_t) + 1)

/* utility functions */
static void* _get_sptr_meta(void* sptr);
static sptr_boolean_type _sptr_multiply_overflow_check(size_t a, size_t b, size_t* result);
static sptr_boolean_type _sptr_addition_overflow_check(size_t a, size_t b, size_t* result);
static sptr_boolean_type _sptr_subtraction_underflow_check(size_t a, size_t b, size_t* result);
static size_t _sptr_get_pad(size_t size);

#define _SPTR_LENGTH_INDEX (0)
#define _SPTR_SIZE_INDEX (1)

/**
 * Make a sptr from a regular pointer. Invalidates the pointer.
 *
 * @param ptr A pointer to a block of memory with `size` bytes allocated
 * @param nitems The number of items that need to be allocated
 * @param size The size (in bytes) of each item.
 * @return NULL on error (With original pointer untouched), otherwise a new sptr.
 */
void* make_sptr(void* ptr, size_t nitems, size_t size) {
    size_t allocate;
    size_t pad = _sptr_get_pad(size);
    char* sptr;
    size_t i;
    if (_sptr_multiply_overflow_check(nitems, size, &allocate)) {
        return NULL;
    }
    if (_sptr_addition_overflow_check(allocate, pad, &allocate)) {
        return NULL;
    }
    sptr = (char*) realloc(ptr, allocate);
    if (sptr == NULL) {
        return NULL;
    }
    memmove(sptr + pad, sptr, size);
    ((size_t*) sptr)[_SPTR_LENGTH_INDEX] = nitems;
    ((size_t*) sptr)[_SPTR_SIZE_INDEX] = size;
    sptr[_sptr_min_pad - 1] = 1;
    for (i = _sptr_min_pad; i < pad; i++) {
        sptr[i] = 0;  /* Set other bytes to 0 */
    }
    return sptr + pad;
}

/**
 * Make a regular pointer from a sptr that can be used by `free()`, `realloc()` and friends. Invalidates `sptr`.
 */
void* make_ptr(void* sptr) {
    if (sptr == NULL) {
        return NULL;
    } else {
        size_t size = sptr_size(sptr);
        void* meta = _get_sptr_meta(sptr);
        void* ptr;
        memmove(sptr, meta, size);
        ptr = realloc(meta, size);
        if (ptr == NULL) {
            return meta;
        }
        return ptr;
    }
}

/**
 * Frees the memory allocated to a sptr. Like `free(ptr)`, but for sptrs.
 */
void free_sptr(void* sptr) {
    free(_get_sptr_meta(sptr));
}

/**
 * Allocates a block of memory, and returns the pointer to the beginning of the memory.
 * Like `malloc(size)`, but for sptrs.
 *
 * @param size The number of bytes to allocated.
 * @return NULL if no memory is available, otherwise a pointer with `size` bytes of memory allocated.
 */
void* malloc_sptr(size_t nitems, size_t size) {
    size_t allocate;
    size_t pad = _sptr_get_pad(size);
    char* sptr;
    size_t i;
    if (_sptr_multiply_overflow_check(nitems, size, &allocate)) {
        return NULL;
    }
    if (_sptr_addition_overflow_check(allocate, pad, &allocate)) {
        return NULL;
    }
    sptr = (char*) malloc(allocate);
    if (sptr == NULL) {
        return NULL;
    }
    ((size_t*) sptr)[_SPTR_LENGTH_INDEX] = nitems;
    ((size_t*) sptr)[_SPTR_SIZE_INDEX] = size;
    sptr[_sptr_min_pad - 1] = 1;
    for (i = _sptr_min_pad; i < pad; i++) {
        sptr[i] = 0;  /* Set other bytes to 0 */
    }
    return ((char*) sptr) + pad;
}

/**
 * Resize the amount of memory allocated to a pointer. Like `realloc(ptr, size)`, but for sptrs.
 * Invalidates the original sptr on success.
 *
 * @param sptr The sptr to resize
 * @param size The new number of bytes to allocate
 * @return NULL if the reallocation was unsuccessful (With the input sptr untouched), a new sptr otherwise.
 */
void* realloc_sptr(void* sptr, size_t nitems) {
    if (sptr == NULL) {
        return NULL;
    } else {
        size_t* meta = (size_t*) _get_sptr_meta(sptr);
        size_t pad = (size_t) ((char*) sptr - (char*) meta);
        size_t allocate;
        if (_sptr_multiply_overflow_check(nitems, meta[_SPTR_SIZE_INDEX], &allocate)) {
            return NULL;
        }
        if (_sptr_addition_overflow_check(allocate, pad, &allocate)) {
            return NULL;
        } else {
            size_t* new_sptr = (size_t*) realloc(meta, allocate);
            if (new_sptr == NULL) {
                return NULL;
            }
            new_sptr[_SPTR_LENGTH_INDEX] = nitems;
            return ((char*) new_sptr) + pad;
        }
    }
}

/**
 * Like `malloc_sptr(nitems * size)`, but sets all the allocated bytes to 0. Like `calloc(nitems, size)`, but for sptrs.
 *
 * @param nitems The number of items to set.
 * @param size The size of each item (e.g. `sizeof(int)`).
 * @return NULL if no memory is available, the pointer to the begining of a block of memory otherwise.
 */
void* calloc_sptr(size_t nitems, size_t size) {
    size_t allocate;
    size_t pad = _sptr_get_pad(size);
    if (_sptr_multiply_overflow_check(nitems, size, &allocate)) {
        return NULL;
    }
    if (_sptr_addition_overflow_check(allocate, pad, &allocate)) {
        return NULL;
    } else {
        size_t* sptr = (size_t*) calloc(allocate, 1);
        if (sptr == NULL) {
            return NULL;
        }
        sptr[_SPTR_LENGTH_INDEX] = nitems;
        sptr[_SPTR_SIZE_INDEX] = size;
        ((char*) sptr)[_sptr_min_pad - 1] = 1;
        /* No need to set to 0; calloc does this. */
        return ((char*) sptr) + pad;
    }
}

/**
 * Returns the size of the `sptr`.
 */
size_t sptr_size(void* sptr) {
    if (sptr == NULL) {
        return 0;
    } else {
        size_t* meta = (size_t*) _get_sptr_meta(sptr);
        size_t size;
        if (_sptr_multiply_overflow_check(meta[_SPTR_LENGTH_INDEX], meta[_SPTR_SIZE_INDEX], &size)) {
            return 0;
        }
        return size;
    }
}

/**
 * Returns the size of each item in `sptr`.
 */
size_t sptr_item_size(void* sptr) {
    if (sptr == NULL) {
        return 0;
    }
    return ((size_t*) _get_sptr_meta(sptr))[_SPTR_SIZE_INDEX];
}

/**
 * Returns the number of items in `sptr`.
 */
size_t sptr_length(void* sptr) {
    if (sptr == NULL) {
        return 0;
    }
    return ((size_t*) _get_sptr_meta(sptr))[_SPTR_LENGTH_INDEX];
}

/**
 * Reallocates memory, adding allocated bytes relative to the current size.
 *
 * If the new size would overflow size_t, it will be the maximum size_t. Invalidates the original sptr on success.
 *
 * @param sptr The sptr to resize
 * @param additional_items The number of items to add.
 * @return NULL if the reallocation was unsuccessful (With the input sptr untouched), a new sptr otherwise.
 */
void* add_realloc_sptr(void* sptr, size_t additional_items) {
    if (sptr == NULL) {
        return NULL;
    } else {
        size_t new_length;
        size_t allocate;
        size_t* meta = (size_t*) _get_sptr_meta(sptr);
        size_t pad = (size_t) ((char*) sptr - (char*) meta);
        size_t* new_sptr;
        if (_sptr_addition_overflow_check(meta[_SPTR_LENGTH_INDEX], additional_items, &new_length)) {
            new_length = (size_t) - 1;
        }
        if (_sptr_multiply_overflow_check(new_length, meta[_SPTR_SIZE_INDEX], &allocate)) {
            return NULL;
        }
        if (_sptr_addition_overflow_check(allocate, pad, &allocate)) {
            return NULL;
        }
        new_sptr = (size_t*) realloc(meta, allocate);
        if (new_sptr == NULL) {
            return NULL;
        }
        new_sptr[_SPTR_LENGTH_INDEX] = new_length;
        return ((char*) new_sptr) + pad;
    }
}

/**
 * Reallocates memory, adding allocated bytes relative to the current size.
 *
 * If the new size would be negative, zero bytes are allocated. Invalidates the original sptr on success.
 *
 * @param sptr The sptr to resize
 * @param removed_items The number of items to remove.
 * @return NULL if the reallocation was unsuccessful (With the input sptr untouched), a new sptr otherwise.
 */
void* subtract_realloc_sptr(void* sptr, size_t removed_items) {
    if (sptr == NULL) {
        return NULL;
    } else {
        size_t new_length;
        size_t allocate;
        size_t* meta = (size_t*) _get_sptr_meta(sptr);
        size_t pad = (size_t) ((char*) sptr - (char*) meta);
        size_t* new_sptr;
        if (_sptr_subtraction_underflow_check(meta[_SPTR_LENGTH_INDEX], removed_items, &new_length)) {
            new_length = 0;
        }
        if (_sptr_multiply_overflow_check(new_length, meta[_SPTR_SIZE_INDEX], &allocate)) {
            return NULL;
        }
        if (_sptr_addition_overflow_check(allocate, pad, &allocate)) {
            return NULL;
        }
        new_sptr = (size_t*) realloc(meta, allocate);
        if (new_sptr == NULL) {
            return NULL;
        }
        new_sptr[_SPTR_LENGTH_INDEX] = new_length;
        new_sptr[_SPTR_LENGTH_INDEX] = new_length;
        return ((char*) new_sptr) + pad;
    }
}

/**
 * Resizes the length of each item in a sptr. The new length is rounded down to the nearest whole item.
 *
 * This will attempt to allocate more bytes if the memory alignment is off,
 * but if it fails, returns the misaligned sptr.
 *
 * The old sptr should be treated as invalid afterwards.
 *
 * @param sptr The sptr to resize the length of the items of.
 * @param new_size The new size of each item.
 * @return The pointer to the beginning of the new sptr. returns NULL iff new_size == 0.
 */
void* resize_sptr(void* sptr, size_t new_size) {
    if (new_size == 0) {
        free_sptr(sptr);
        return NULL;
    } else if (sptr == NULL) {
        return malloc_sptr(0, new_size);
    } else {
        size_t* meta = (size_t*) _get_sptr_meta(sptr);
        size_t overall_size;
        if (_sptr_multiply_overflow_check(meta[_SPTR_LENGTH_INDEX], meta[_SPTR_SIZE_INDEX], &overall_size)) {
            return NULL;
        } else {
            size_t new_length = overall_size / new_size;
            size_t old_pad = ((size_t) ((char*) sptr - (char*) meta));
            size_t new_pad = _sptr_get_pad(new_size);
            if (old_pad < new_pad) {
                size_t allocate;
                overall_size = new_length * new_size;
                if (!_sptr_addition_overflow_check(overall_size, new_pad, &allocate)) {
                    char*new_sptr = (char*) realloc(meta, allocate);
                    if (new_sptr != NULL) {
                        memmove(new_sptr + new_size, new_sptr + new_pad, overall_size);
                        return new_sptr + new_pad;
                    }
                }
            } else if (old_pad > new_pad) {
                size_t allocate;
                overall_size = new_length * new_size;
                memmove(((char*) meta) + new_pad, sptr, overall_size);
                if (!_sptr_addition_overflow_check(overall_size, new_pad, &allocate)) {
                    char*new_sptr = (char*) realloc(meta, allocate);
                    if (new_sptr == NULL) {
                        return ((char*) sptr) + new_pad;
                    } else {
                        return new_sptr + new_size;
                    }
                }
            }
        }
    }
    return sptr;
}

/**
 * Multiply the amount of bytes allocated to a sptr by an amount.
 *
 * Invalidates the input sptr on success, returns NULL on failure.
 */
void* sptr_multiply_allocation(void* sptr, double multiplier) {
    if (sptr == NULL || multiplier < 0) {
        return NULL;
    } else if (multiplier == 1) {
        return sptr;
    } else {
        size_t* meta = (size_t*) _get_sptr_meta(sptr);
        char* new_sptr;
        size_t length;
        size_t allocate;
        size_t pad = (size_t) ((char*) sptr - (char*) meta);
        if (multiplier) {
            length = meta[_SPTR_LENGTH_INDEX];
            if (((size_t) (length * multiplier)) / multiplier != length) {
                /* Overflow */
                return NULL;
            }
            length *= multiplier;
        } else {
            length = 0;
        }
        if (_sptr_multiply_overflow_check(meta[_SPTR_SIZE_INDEX], length, &allocate)) {
            return NULL;
        } else if (_sptr_addition_overflow_check(allocate, pad, &allocate)) {
            return NULL;
        }
        new_sptr = (char*) realloc(meta, allocate);
        if (new_sptr == NULL) {
            return NULL;
        }
        ((size_t*) new_sptr)[_SPTR_LENGTH_INDEX] = length;
        return new_sptr + pad;
    }
}

/**
 * Returns a pointer to the past-the-end item of a sptr (Cannot be dereferenced)
 */
void* sptr_end(void* sptr) {
    if (sptr == NULL) {
        return NULL;
    } else {
        return ((char*) sptr) + sptr_size(sptr);
    }
}

/**
 * Return whether `other` points inside the block pointed by `sptr`.
 */
sptr_boolean_type in_sptr(void* sptr, void* other) {
    if (sptr == NULL) {
        return other == NULL;
    }
    return sptr <= other && other < sptr_end(sptr);
}

static void* _get_sptr_meta(void* sptr) {
    if (sptr == NULL) {
        return NULL;
    } else {
        char* as_char = (char*) sptr;
        while (!*--as_char);
        return as_char + 1 - _sptr_min_pad;
    }
}

static sptr_boolean_type _sptr_multiply_overflow_check(size_t a, size_t b, size_t* result) {
    if (a == 0) {
        return (*result = 0);
    }
    return ((*result = a * b) / a != b);
}

static sptr_boolean_type _sptr_addition_overflow_check(size_t a, size_t b, size_t* result) {
    return (*result = a + b) < ((a < b) ? a : b);
}

static sptr_boolean_type _sptr_subtraction_underflow_check(size_t a, size_t b, size_t* result) {
    *result = a - b;
    return a < b;
}

static size_t _sptr_get_pad(size_t size) {
    return _sptr_min_pad + (_sptr_min_pad % (size) ? (size) - (_sptr_min_pad % (size)) : 0);
}
