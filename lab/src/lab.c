#include "lab.h"
#include <assert.h>


// Environment Setup Sanity Check.
void test_func(void){
    printf("Hi there, you shit!\n");
}


//____________ Pointer____________
// Demo
void pointer_basic_demo(void)
{
    int x = 5;

    int *p = &x;

    printf("x value = %d\n", x);
    printf("x addr  = %p\n", (void*)&x);

    printf("p value = %p\n", (void*)p);
    printf("*p value = %d\n", *p);
}

// Modify Value
void mod_val(int *x){
    printf("arrived val = %d\n", *x);
    *x = (((*x * 5)+4)*3) >> 2;
    printf("BEHOLD! a magic was done - check your value.\n");
}

// Swap
void swap_by_ref(int* x, int* y){
    printf("arrived (x,y) = (%d,%d)\n", *x, *y);
    int temp = *x;
    *x = *y;
    *y = temp;
}

// pointer arithmetic demo
void pointer_arith_demo(int arr[], size_t arr_size){
    printf("lets iterate over the array with no index!\n");
    for(size_t i = 0; i < arr_size; i++){
        printf("arr[%ld] = %d\n", i, *(arr+i));
    }
    printf("nice ha?\n");
}

// const & pointer
void pointer_and_const(void){
    // printf("lets test some const location and pointers..");
    // const int *x = 5; // same as int const *x
    // assert(*x == 5);
    // x = x+4;

    // int const *K = 5;
    // K = K+4;
}

// pointer 2 pointer
void pointer2pointer_demo(void){
    int *p;
    int **p_p;
    int var = 5;
    p = &var;
    p_p = &p;

    printf("var value = %d\n", var);
    printf("p value = %p\n", (void*)p);
    printf("*p value = %d\n", *p);
    printf("p_p value = %p\n", (void*)p_p);
    printf("*p_p value = %p\n", (void*)*p_p);
    printf("**p_p value = %d\n", **p_p);
}


//____________ Dynamic Memory + Strings ____________

// Stack vs Heap
/**
Requirements:
create a stack variable int a = 10
allocate an int on heap
print both addresses
free the heap variable

Observe:
stack addresses near each other
heap addresses far away */
void stack_vs_heap_demo(void){
    int a_stack = 10; // stack var
    int* a_heap = malloc(sizeof(int));
    *a_heap = 10;
    printf("stack var a_stack = %d, address = %p\n", a_stack, (void*)&a_stack);
    printf("heap var a_heap = %d, address = %p\n", *a_heap, (void*)a_heap);

    free(a_heap);
}

// malloc array
/**
returns NULL if n == 0
initialize values to 0
caller must free

Hint:

calloc exists
 */
int* create_int_array(size_t n){
    if(n == 0){
        return NULL;
    }
    int *arr = malloc(n*sizeof(int));
    if(arr == NULL){
        printf("malloc failed!\n");
        return NULL;
    }
    memset(arr, 0, n*sizeof(int));
    printf("dont forget to free the array after use!\n");
    return arr;
}

// realloc growth
/**
new elements initialized to 0
return NULL if failure
 */
int* grow_array(int *arr, size_t old_size, size_t new_size){
    if(arr ==NULL){
        printf("arr is NULL, cannot grow a non existing array\n");
        return NULL;
    }
    if (new_size == 0){
        free(arr);
        return NULL;
    }

    int* ans = NULL;
    ans = realloc(arr, new_size*sizeof(int));
    if(ans == NULL){
        printf("realloc failed!\n");
        return NULL;
    }

    memset(ans+old_size, 0, (new_size-old_size)*sizeof(int));
    return ans;
}


// string length
/**
NULL safe
do not use strlen
*/
size_t my_strlen(const char *s){
    if (s==NULL){
        printf("NULL string has length 0\n");
        return 0;
    }
    size_t len = 0;
    while(*(s+len)!='\0'){
        len++;
    }
    return len;
}


// safe string copy
/**
always null terminate
returns length of src
*/
size_t safe_str_copy(char *dst, size_t dst_size, const char *src){
    if(src == NULL){
        printf("NULL source string, nothing to copy\n");
        if(dst_size>0){
            dst[0] = '\0';
        }
        return 0;
    }
    if (dst == NULL){
        printf("NULL destination string, cannot copy\n");
        return 0;
    }

    size_t src_size = my_strlen(src);
    if(src_size > dst_size){
        printf("warning: source string is larger than destination buffer, truncating copy\n");
    }
    size_t idx = 0;
    while(idx < src_size && idx < (dst_size-1)){
        *(dst + idx) = *(src + idx);
        idx++;
    }
    *(dst+idx) = '\0';
    return idx;
}


// count char in string
/**
Example: count_char("banana",'a') → 3
*/
int count_char(const char *s, char c){
    if(s == NULL){
        printf("NULL string, cannot count chars\n");
        return 0;
    }
    size_t idx = 0;
    int count = 0;
    while(*(s + idx) != '\0'){
        if (*(s + idx) == c){
            count++;
        }
        idx++;
    }
    return count;
}


//____________ Pointer____________
//____________ Pointer____________
//____________ Pointer____________
//____________ Pointer____________
//____________ Pointer____________
//____________ Pointer____________