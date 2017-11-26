//
// Created by joseph on 9/26/17.
//

#include <math.h>
#include <stddef.h>
#include<stdio.h>

#include "utils.h"
#include "free_list.h"

#define NELEMS(x)  (sizeof(x) / sizeof((x)[0]))


double _log2(double x) {
    return log(x) / log(2);
}

double determine_array_size(unsigned int _basic_block_size, unsigned int _length) {
    return floor(_log2(_length) - _log2(_basic_block_size)) + 1;
}

long convert_to_binary(unsigned int n) {
    int remainder;
    long binary = 0, i = 1;

    while(n != 0) {
        remainder = n%2;
        n = n/2;
        binary= binary + (remainder*i);
        i = i*10;
    }
    return binary;
}

long get_digit_count(long x) {
    long n = 0;
    while(x) {
        x /= 10;
        n++;
    }
    return n;
}

int get_index(size_t _len, FL_HEADER **fl_array) {
    int arr_size = get_fl_array_size(fl_array);
    int wanted_len = (int) ceil(_log2((double) _len));
    for (int i = 0; i < arr_size; i++) {
        if (fl_array[i]->length == wanted_len) {
            return i;
        }
    }
    return -1;
}

int get_fl_array_size(FL_HEADER **fl_array) {
    int count = 0;

    while(fl_array[count]->head == NULL) {
        count++;
    }
//    for(int i = 0; i < count; i++){
//        printf("header_test_len: %zu\n", fl_array[i]->length);
//    }
    return count - 2;
}
