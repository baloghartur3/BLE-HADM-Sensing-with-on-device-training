/*
 * evaluate.h
 *
 *  Created on: 2023. szept. 29.
 *      Author: balog
 */

#ifndef EVALUATE_H_
#define EVALUATE_H_

// Function to update true positives, false positives, true negatives, and false negatives
void truevsfalse(int actual, int predicted);

// Function to calculate support
unsigned int support();

// Function to calculate accuracy
float accuracy();

// Function to reset evaluation counters
void eval_reset();

#endif /* EVALUATE_H_ */
