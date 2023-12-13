/*
 * misc.c
 *
 *  Created on: 2023. okt. 25.
 *      Author: arbalogh
 */

#include "misc.h"

float calc_second_norm(float x, float y)
{
  return sqrt(x*x+y*y);
}

hadm_struct compl_multiplication(float i1, float q1,float i2,float q2)
{
  hadm_struct temp;
  temp.i= i1*i2-q1*q2;
  temp.q= i1*q2+i2*q1;
  return temp;
}


void shuffle(int *array, size_t n)
{
    if (n > 1)
    {
        size_t i;
        for (i = 0; i < n - 1; i++)
        {
          size_t j = i + rand() / (RAND_MAX / (n - i) + 1);
          int t = array[j];
          array[j] = array[i];
          array[i] = t;
        }
    }
}
