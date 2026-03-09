#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "lab_proto.h"


// Environment Setup Sanity Check.
void test_func(void);

//____________ Pointer____________
// Demo
void pointer_basic_demo(void);

// Modify Value
void mod_val(int *x);

// Swap
void swap_by_ref(int* x, int* y);

// pointer arithmetic demo
void pointer_arith_demo(int arr[], size_t arr_size);

// const & pointer
void pointer_and_const(void);

// pointer 2 pointer
void pointer2pointer_demo(void);


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
void stack_vs_heap_demo(void);

// malloc array
/**
returns NULL if n == 0
initialize values to 0
caller must free

Hint:

calloc exists
 */
int* create_int_array(size_t n);

// realloc growth
/**
new elements initialized to 0
return NULL if failure
 */
int* grow_array(int *arr, size_t old_size, size_t new_size);

// string length
/**
NULL safe
do not use strlen
*/
size_t my_strlen(const char *s);


// safe string copy
/**
always null terminate
returns length of src
*/
size_t safe_str_copy(char *dst, size_t dst_size, const char *src);


// count char in string
/**
Example: count_char("banana",'a') → 3
*/
int count_char(const char *s, char c);

//____________ Pointer ____________
//____________ Pointer ____________
//____________ Pointer ____________
//____________ Pointer ____________
//____________ Pointer ____________
//____________ Pointer ____________
//____________ Pointer ____________