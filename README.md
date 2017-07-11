# sptr
A pointer that carries its size

C89-C11 and C++98-C++17 compatible.

Note: If you do not have to be compatible with C89/C90 and are using C, please include `<stdbool.h>` before including `sptr.h`.

Functions defined:

## make_sptr

Make a sptr from a regular pointer, given the number of items and the size of each item.

### Prototype

```C
void* make_sptr(void* ptr, size_t nitems, size_t size);
```

### Example usage

```C
int* x = get_int_pointer_of_size(5);
int* sptr_x = (int*) make_sptr(x, 5 /* x has 5 items */, sizeof(int));
```

## make_ptr

Retrieve a regular pointer from a sptr.

### Prototype

```C
void* make_ptr(void* sptr);
```

### Example usage

```C
int* sptr_x = sptr_function();
int* x = (int*) make_ptr(x);
free(x);
```

## free_sptr

Free the memory allocated to a sptr.

### Prototype

```C
void free_sptr(void* sptr);
```

### Example usage

```C
int* sptr = sptr_function();
free_sptr(sptr);
```

## malloc_sptr

Allocate memory for `nitems` items of size `size`.

### Prototype

```C
void* malloc_sptr(size_t nitems, size_t size);
```

### Example usage

```C
int* sptr = (int*) malloc_sptr(3, sizeof(int));
assert(sptr != NULL);
sptr[0] = 3;
sptr[1] = 2;
sptr[2] = 1;
```

## realloc_sptr

Allocate memory for more or less items in a sptr.

### Prototype

```C
void* realloc_sptr(void* sptr, size_t nitems);
```

### Example usage

```C
int* sptr = (int*) malloc_sptr(3, sizeof(int));
assert(sptr != NULL);
sptr[0] = 3;
sptr[1] = 2;
sptr[2] = 1;
/* sizeof(int) isn't needed anymore, as the size of each item is already known */
int* new_sptr = (int*) realloc_sptr(sptr, 5);
assert(new_sptr != NULL);
sptr = new_sptr;
sptr[3] = 0;
sptr[4] = -1;
```

## calloc_sptr

Allocate memory for `nitems` items of size `size`, assigning zero to each element.

### Prototype

```C
void* calloc_sptr(size_t nitems, size_t size);
```

### Example usage

```C
int* sptr = (int*) calloc_sptr(3, sizeof(int));
assert(sptr != NULL);
/* sptr[0] is already 0. */
sptr[1] += 1;
sptr[2] += 2;
```

## sptr_size

Get the total size (in bytes) of a sptr. Mostly equivalent to `sptr_item_size(sptr) * sptr_length(sptr)`.

### Prototype

```C
size_t sptr_size(void* sptr);
```

### Example usage

```C
if (sptr_size(sptr)) {
    /* Not empty */
} else {
    /* Empty */
}
```

## sptr_item_size

Get the size of each item in a sptr.

### Prototype

```C
size_t sptr_item_size(void* sptr);
```

### Example usage

```C
if (sptr_item_size(sptr) == sizeof(int)) {
    /* Could have intended to be an int* sptr */
} else {
    /* Was not an int* sptr */
}
```

## sptr_length

Get the number of elements in a sptr.

### Prototype

```C
size_t sptr_length(void* sptr);
```

### Example usage

```C
size_t length = sptr_length(sptr);
size_t i;
for (i = 0; i < length; i++) {
    /* Iterate over a sptr */
}
```

## sptr_item_size

Get the size of each item in a sptr.

### Prototype

```C
size_t sptr_item_size(void* sptr);
```

### Example usage

```C
if (sptr_item_size(sptr) == sizeof(int)) {
    /* Could have intended to be an int* sptr */
} else {
    /* Was not an int* sptr */
}
```

## add_realloc_sptr

Like `realloc_sptr`, but relative to the current number of items.

### Prototype

```C
void* add_realloc_sptr(void* sptr, size_t additional_items);
```

### Example usage

```C
int* sptr = get_int_sptr();
size_t additional_required = get_extra_needed(sptr);
if (additional_required > 0) {
    int* new_sptr = (int*) add_realloc_sptr(sptr, additional_required);
    assert(new_sptr != NULL);
    sprt = new_sptr;
}
```

## add_realloc_sptr

Like `realloc_sptr`, but relative to the current number of items.

### Prototype

```C
void* subtract_realloc_sptr(void* sptr, size_t removed_items);
```

### Example usage

```C
int* sptr = get_int_sptr_with_extra_value();
int* new_sptr = subtract_realloc_sptr(sptr, 1);
assert(new_sptr != NULL);
sptr = new_sptr;
calculate_on(sptr);
```

## resize_sptr

Resize the size of each item, increasing or decreasing the length as necessary.

### Prototype

```C
void* resize_sptr(void* sptr, size_t new_size);
```

### Example usage

```C
long int* sptr_long = get_long_sptr();
/* Get shorts in native byte order */
short int* sptr_short = resize_sptr(sptr, sizeof(short));
assert(sptr_short != NULL);
```

## sptr_multiply_allocation

`realloc` by a multiplier.

### Prototype

```C
void* sptr_multiply_allocation(void* sptr, double multiplier);
```

### Example usage

```C
for (j = 0; j < length; j++) {
    if (f(j)) {
        if (i > sptr_length(sptr)) {
            int* new_sptr = sptr_multiply_allocation(sptr, 2);
            assert(new_sptr != NULL);
            sptr = new_sptr;
        }
        sptr[i++] = j;
    }
}
```

## sptr_end

Get the past-the-end item of a sptr. All elements of the sptr will be less than this.

### Prototype

```C
void* sptr_end(void* sptr);
```

### Example usage

```C
if (it < sptr_end(sptr)) {
    *it = value;
} else {
    /* Ran out of values */
}
```

## in_sptr

See if a pointer points to an item in a sptr.

### Prototype

```C
sptr_boolean_type in_sptr(void* sptr, void* other);
```

Note: `sptr_boolean_type` is defined like this:

```C
#if defined(__cplusplus) || defined(bool)
  #define sptr_boolean_type bool
#else
  #define sptr_boolean_type int
#endif
```

In C++, `sptr_boolean_type` will be the `bool` type.  
In C, if `<stdbool.h>` has been included, `sptr_boolean_type` will be the `bool` macro, which is the `_Bool` type. Otherwise, it will be the `int` type.

### Example usage

```C
if (in_sptr(sptr, it)) {
    *it = value;
} else {
    /* Ran out of values */
}
```
