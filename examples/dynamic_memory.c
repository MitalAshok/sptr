/* Compile with "gcc -std=c89 dynamic_memory.c ../sptr.c -o dynamic_memory.out" */

#include <stdio.h>
#include "../sptr.h"

typedef sptr_boolean_type (*filter_function)(int);

static sptr_boolean_type is_even(int x) {
    return !(x & 1);
}

static sptr_boolean_type is_odd(int x) {
    return x & 1;
}

static void print_and_free(int* sptr) {
    size_t length = sptr_length(sptr);
    if (length) {
        size_t i;
        printf("%d", sptr[0]);
        for (i = 1; i < length; i++) {
            printf(", %d", sptr[i]);
        }
    }
    free_sptr(sptr);
    printf("\n");
}

static int* filter_range(int start, int end, int step, filter_function f) {
    int* sptr;
    int* current;
    if (step > 0) {
        if (start > end) {
            return NULL;
        }
    } else if (step == 0) {
        return NULL;
    } else {
        if (end > start) {
            return NULL;
        }
    }
    sptr = (int*) malloc_sptr(8, sizeof(int));
    current = sptr;
    if (sptr == NULL) {
        return NULL;
    }
    for (; (step > 0) ? (start < end) : (start > end); start += step) {
        if ((*f)(start)) {
            if (!in_sptr(sptr, current)) {
                int* new_sptr = sptr_multiply_allocation(sptr, 2);
                if (new_sptr == NULL) {
                    free_sptr(sptr);
                    return NULL;
                }
                sptr = new_sptr;
            }
            *(current++) = start;
        }
    }
    current = realloc_sptr(sptr, (size_t) (current - sptr));
    if (current == NULL) {
        free_sptr(sptr);
    }
    return current;
}

int main(void) {
    int* filtered;
    int* it;

    it = filtered = filter_range(0, 11, 1, &is_even);

    if (it != NULL) {
        printf("%d", *(it++));
        for (; (void*) it < sptr_end(filtered); it++) printf(", %d", *it);
    } else {
        printf("ERROR");
    }
    printf("\n");
    free_sptr(filtered);

    it = filtered = filter_range(0, 32, 3, &is_odd);

    if (it != NULL) {
        printf("%d", *(it++));
        for (; (void*) it < sptr_end(filtered); it++) printf(", %d", *it);
    } else {
        printf("ERROR");
    }
    printf("\n");
    free_sptr(filtered);

    it = filtered = filter_range(100, -1, -1, &is_odd);

    if (it != NULL) {
        printf("%d", *(it++));
        for (; (void*) it < sptr_end(filtered); it++) printf(", %d", *it);
    } else {
        printf("ERROR");
    }
    printf("\n");
    free_sptr(filtered);

    return 0;
}
