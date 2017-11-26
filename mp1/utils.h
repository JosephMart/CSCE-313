#ifndef MP1_CLION_UTILS_H
#define MP1_CLION_UTILS_H

#include "free_list.h"

double _log2(double x);

double determine_array_size(unsigned int _basic_block_size, unsigned int _length);

long convert_to_binary(unsigned int integer);

long get_digit_count(long x);

int get_index(size_t _len, FL_HEADER **fl_array);

int get_fl_array_size(FL_HEADER **fl_array);

#endif //MP1_CLION_UTILS_H
