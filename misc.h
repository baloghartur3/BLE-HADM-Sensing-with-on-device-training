/*
 * misc.h
 *
 *  Created on: 2023. okt. 25.
 *      Author: arbalogh
 */

#ifndef MISC_H_
#define MISC_H_

#pragma once
#include "math.h"
#include <stdlib.h>

typedef struct {
  float i;
  float q;
} hadm_struct;

typedef struct {
  float magnitude;
  float phase;
} transfer_function;

float calc_second_norm(float x, float y);
hadm_struct compl_multiplication(float i1, float q1,float i2,float q2);
void shuffle(int *array, size_t n);
float dot(float *x, float *w,int size);


#endif /* MISC_H_ */
