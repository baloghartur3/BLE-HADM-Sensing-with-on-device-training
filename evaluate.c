/*
 * evaluate.c
 *
 *  Created on: 2023. szept. 29.
 *      Author: balog
 */


unsigned int t_p=0, t_n=0,f_p=0, f_n=0;

void truevsfalse(int actual, int predicted)
{
  if (predicted > 0 && predicted == actual)
    t_p++;
  else if (predicted > 0 && predicted != actual)
    f_p++;
  else if (predicted <= 0 && predicted == actual)
    t_n++;
  else
    f_n++;
}

unsigned int support()
{
  return t_p + t_n + f_p + f_n;
}

float accuracy()
{
  return (1.0f * t_p + t_n) / support();
}



void eval_reset()
{
  t_p=0; t_n=0; f_p=0; f_n=0;
}
