/* Compile with "gcc -std=c89 print_function.c ../sptr.c -o print_function.out" */

#include <stdio.h>
#include "../sptr.h"


static void print_int_sptr(int* sptr) {  /* No need to pass the length. */
    size_t length = sptr_length(sptr);
    if (length) {
        size_t i;
        printf("%d", sptr[0]);
        for (i = 1; i < length; i++) {
            printf(", %d", sptr[i]);
        }
    }
    printf("\n");
}

int main(void) {
    int* sptr = (int*) malloc_sptr(2, sizeof(int));
    int* temp;
    if (sptr == NULL) {
        return 1;
    }
    sptr[0] = 123;
    sptr[1] = 231;
    print_int_sptr(sptr);  /* Expect: 123, 231 */
    /* Allocate 1 more int */
    temp = (int*) add_realloc_sptr(sptr, 1);
    if (temp == NULL) {
        free_sptr(sptr);
        return 1;
    }
    sptr = temp;
    sptr[2] = 321;
    print_int_sptr(sptr);  /* Expect: 123, 231, 321 */
    free_sptr(sptr);
    sptr = (int*) calloc_sptr(5, sizeof(int));
    if (sptr == NULL) {
        return 1;
    }
    sptr[4] = 5;
    print_int_sptr(sptr);  /* Expect: 0, 0, 0, 0, 5 */
    free_sptr(sptr);

    return 0;
}
