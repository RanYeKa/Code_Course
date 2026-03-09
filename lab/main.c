#include "lab.h"

int main(void){
    test_func();

// Pointers:
    // Demo
    pointer_basic_demo();

// Modify Value
    int y = 60;
    mod_val(&y);
    printf("after mod_val, y = %d\n", y);

// Swap
    int k =10;
    int l = 20;
    printf("before swap, (k,l) = (%d,%d)\n", k, l);
    swap_by_ref(&l,&k);
    printf("after swap, (k,l) = (%d,%d)\n", k, l);

// pointer arithmetic demo
    int arr[] = {10,20,30,40,50};
// size_t n = sizeof(arr)/sizeof(arr[0]);
    size_t n = sizeof(arr)/sizeof(*(arr));
    pointer_arith_demo(arr, n);

// const & pointer
// pointer_and_const();

// pointer 2 pointer
    pointer2pointer_demo();

// new section
printf("\n================ DYNAMIC MEMORY + STRINGS TESTS ================\n");

/* 1) stack vs heap */
printf("\n[1] stack_vs_heap_demo()\n");
stack_vs_heap_demo();

/* 2) create_int_array() */
printf("\n[2] create_int_array()\n");
int *arr0 = create_int_array(0);
printf("create_int_array(0) -> %p (expected NULL)\n", (void *)arr0);

size_t n1 = 10;
int *arr1 = create_int_array(n1);
printf("create_int_array(%zu) -> %p\n", n1, (void *)arr1);

if (arr1 != NULL) {
    printf("arr1 contents after init:\n");
    for (size_t i = 0; i < n1; i++) {
        printf("arr1[%zu] = %d\n", i, arr1[i]);
    }

    /* give first few elements non-zero values before growth */
    arr1[0] = 11;
    arr1[1] = 22;
    arr1[2] = 33;
}

/* 3) grow_array() */
printf("\n[3] grow_array()\n");
size_t n2 = 20;
int *arr2 = grow_array(arr1, n1, n2);
printf("grow_array(arr1, %zu, %zu) -> %p\n", n1, n2, (void *)arr2);

if (arr2 != NULL) {
    printf("arr2 contents after growth:\n");
    for (size_t i = 0; i < n2; i++) {
        printf("arr2[%zu] = %d\n", i, arr2[i]);
    }
}

/* 4) my_strlen() */
printf("\n[4] my_strlen()\n");
const char *str1 = "this is something this is!";
const char *str2 = "";
const char *str3 = NULL;

printf("my_strlen(\"%s\") = %zu\n", str1, my_strlen(str1));
printf("my_strlen(\"\") = %zu\n", my_strlen(str2));
printf("my_strlen(NULL) = %zu\n", my_strlen(str3));

/* 5) safe_str_copy() */
printf("\n[5] safe_str_copy()\n");
char dst1[64];
char dst2[8];
char dst3[1];

size_t copied1 = safe_str_copy(dst1, sizeof(dst1), str1);
printf("copy to dst1: src_len=%zu, dst=\"%s\"\n", copied1, dst1);

size_t copied2 = safe_str_copy(dst2, sizeof(dst2), str1);
printf("copy to dst2: src_len=%zu, dst=\"%s\"\n", copied2, dst2);

size_t copied3 = safe_str_copy(dst3, sizeof(dst3), "A");
printf("copy to dst3: src_len=%zu, dst=\"%s\"\n", copied3, dst3);

size_t copied4 = safe_str_copy(dst1, sizeof(dst1), NULL);
printf("copy NULL src: returned=%zu, dst=\"%s\"\n", copied4, dst1);

/* 6) count_char() */
printf("\n[6] count_char()\n");
printf("count_char(\"banana\", 'a') = %d\n", count_char("banana", 'a'));
printf("count_char(\"banana\", 'n') = %d\n", count_char("banana", 'n'));
printf("count_char(\"banana\", 'z') = %d\n", count_char("banana", 'z'));
printf("count_char(NULL, 'a') = %d\n", count_char(NULL, 'a'));

/* cleanup */
free(arr2);

printf("\n================ END OF TESTS ================\n");



    return 0;
}