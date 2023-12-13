/*
 * ml_model.h
 *
 *  Created on: 2023. szept. 29.
 *      Author: balog
 */

#ifndef ML_MODEL_H_
#define ML_MODEL_H_

#include <stdbool.h>

#define IQ_SAMPLE_LEN 37 // The number of IQ samples to get from each device
#define NUM_CLASSES 3
#define NUM_FEATURES 74
#define DATASET_SIZE 300
#define TRAINING_SIZE 0.8*DATASET_SIZE

int get_idx();

void init_model();

void add_sample_to_data();

int train(int num_of_epoch);

int predict();

void print_X(int index);

int get_accuracy();

#endif /* ML_MODEL_H_ */
