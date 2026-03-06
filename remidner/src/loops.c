#include "loops.h"


int loop_sum(int n, int diff){
    if (n < 0) return -1;
    if (diff <= 0) return -1;

    int sum = 0;
    for(int i = 0 ; i<n ; i++){
        sum += diff;
    }
    return sum;

}

int loop_product(int n, int k){
    int sum = 0;

    if(k == 0){
        return sum;
    }

    if(k>0){
        for(int i = 0 ; i<k ; i++){
            sum += n;
        }
    }
    else{
        for(int i = 0 ; i>k ; i--){
            sum -= n;
        }
    }
    return sum;

}

int loop_countdown_for(int n){
    if(n<0){
        printf("Input must be non-negative.\n");
        return -1;
    }
    printf("\n");
    for(int i = n; i >= 0; i--){
        printf("%d", i);
        // Force the output to show immediately
        fflush(stdout);
        sleep(1);
        printf(" \r");
    }
    printf("done :) \n");
    return 0;
}


int loop_countdown_while(int n){
    if(n<0){
        printf("Input must be non-negative.\n");
        return -1;
    }
    printf("\n");
    while((n)>0){
        printf("%d", n--);
        // Force the output to show immediately
        fflush(stdout);
        sleep(1);
        printf(" \r");
    }
    printf("done :) \n");
    return 0;
}


int loop_countdown_while_no_new_line(int n){
    if(n<0){
        printf("Input must be non-negative.\n");
        return -1;
    }
    printf("\n");
    while((n)>0){
        printf("%d", n--);
        // Force the output to show immediately
        fflush(stdout);
        sleep(1);
        printf(" \r");
    }
    return 0;
}