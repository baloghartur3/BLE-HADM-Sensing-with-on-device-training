/*
 * ml_model.c
 *
 *  Created on: 2023. szept. 29.
 *      Author: balog
 */

#include "ml_model.h"
#include "evaluate.h"
#include "misc.h"
#include <stdlib.h>
#include <stdio.h>
#include "psa/crypto.h"


//Variables for data shuffle
uint8_t rand_num[2];
psa_status_t ret;

#define min(a, b) (a) < (b) ? (a) : (b)
#define max(a, b) (a) > (b) ? (a) : (b)
#define abs(x) ((x) > 0 ? (x) : -(x))

//Data collection variables
int idx=0;
int num_of_sampling=0;


// HADM samples
float initiator_i[IQ_SAMPLE_LEN];
float initiator_q[IQ_SAMPLE_LEN];
float reflector_i[IQ_SAMPLE_LEN];
float reflector_q[IQ_SAMPLE_LEN];
static hadm_struct hadm_data[IQ_SAMPLE_LEN];
static float X[DATASET_SIZE][2*IQ_SAMPLE_LEN];
static float X_inference[2*IQ_SAMPLE_LEN];
int y[DATASET_SIZE];
int indexes[DATASET_SIZE];


//Model parameters
float model_alpha=0.001;
float model_weights[NUM_CLASSES][NUM_FEATURES + 1];
float weights_updates[NUM_CLASSES][NUM_FEATURES + 1];
float predictions[NUM_CLASSES];
float highest_num;
int predicted;
int actual;


struct SGD_params{
  bool normalizeAlpha;
  float momentum;
};

struct SGD_params parameters;

void softmax(float* outputs, int size){
  float softmaxsum=0;
  for(int i=0; i<size;i++){
      outputs[i]=exp(outputs[i]);
      softmaxsum+=outputs[i];
  }
  for(int i=0;i<size;i++){
      outputs[i]/=softmaxsum;
  }
}

float dot(float *x, float *w,int size) {
            float sum = 0;
            for (int i = 0; i < size; i++)
                sum += x[i] * w[i];
            return sum;
}

void init_SGD(bool norm_alpha, float momentum, float alpha)
{
  model_alpha=alpha;
  parameters.normalizeAlpha = norm_alpha;
  parameters.momentum = momentum;
  ret = psa_crypto_init();
  ret = psa_generate_random(rand_num,  sizeof(rand_num));
  srand(rand_num[0]);
  for(int j=0;j<NUM_CLASSES;j++){
      for (unsigned int i = 0; i < NUM_FEATURES + 1; i++){
          model_weights[j][i] =((float)rand() / RAND_MAX);
          weights_updates[j][i] = 0;
      }
  }
}

//Training prediction for the collected dataset

float * train_predict(float *x)
{
  float xx[NUM_FEATURES + 1] = {1};
  for (unsigned int i = 0; i < NUM_FEATURES; i++)
    xx[i + 1] = x[i];

  for(int i=0; i<NUM_CLASSES;i++){
      predictions[i]=dot(xx, model_weights[i],NUM_FEATURES+1);
  }
  softmax(predictions,NUM_CLASSES);
  return predictions;
}

//Training function

void fitModel(float *x, int y) {
  float* predicted = train_predict(x);
  float error[NUM_CLASSES]={0};
  float alpha=model_alpha;
  bool iserror=false;
  for(int i=0; i<NUM_CLASSES;i++){
      if(y==i){
          error[i] = predicted[i]-1;
      }
      else{
          error[i] = predicted[i];
      }
  }

  for(int i=0; i<NUM_CLASSES;i++){
      if (error[i] != 0)
        iserror=true;
  }

  if (iserror==false)
    return;

  if (parameters.normalizeAlpha)
      alpha /= 1 + alpha * dot(x, x, NUM_FEATURES);
  for(int i=0; i<NUM_CLASSES;i++){
        weights_updates[i][0] = (parameters.momentum * weights_updates[i][0]) - (alpha * error[i]);
        model_weights[i][0] += weights_updates[i][0];
  }

  float momentum;
  float correction;
  for(int j=0;j<NUM_CLASSES;j++){
      for (unsigned int i = 0; i < NUM_FEATURES; i++)
      {
        momentum = parameters.momentum * weights_updates[j][i + 1];
        correction = alpha * x[i] * error[j];
        weights_updates[j][i + 1] = momentum - correction;
        model_weights[j][i + 1] += weights_updates[j][i + 1];
      }
  }
}



int inference(float *x){
  float xx[NUM_FEATURES + 1] = {1};
  for (unsigned int i = 0; i < NUM_FEATURES; i++)
    xx[i + 1] = x[i];

  for(int i=0; i<NUM_CLASSES;i++){
      predictions[i]=dot(xx, model_weights[i],NUM_FEATURES+1);
  }
  softmax(predictions,NUM_CLASSES);
  highest_num=predictions[0];
  int idx=0;
  for(int i=1;i<NUM_CLASSES;i++){
      if (predictions[i]>highest_num){
          idx=i;
          highest_num=predictions[i];
      }
  }
  return idx;
}

int train(int num_of_epoch)
{
  eval_reset();
  for (int cycle = 0; cycle < num_of_epoch; cycle++){
      for (int i = 0; i < TRAINING_SIZE; i++){
          fitModel(X[indexes[i]], y[indexes[i]]);
      }
      printf("Epoch: %d | Accuracy: %d %%\n\r",cycle+1,get_accuracy());
      eval_reset();
  }
  return 0;
}

int get_accuracy()
{
  for (int i = TRAINING_SIZE; i < DATASET_SIZE; i++)
  {
    predicted = inference(X[indexes[i]]);
    actual = y[indexes[i]];
    //printf("Detected label: %d | Real label: %d\r\n",predicted,actual);
    truevsfalse(actual, predicted);
  }
  return (int)(accuracy()*100);
}


// Custom function to keep track of classes
int get_class(){
  if (num_of_sampling<DATASET_SIZE/NUM_CLASSES){
      return 0;
  }
  else if (num_of_sampling>=DATASET_SIZE/NUM_CLASSES+20 && num_of_sampling<2*(DATASET_SIZE/NUM_CLASSES)+20)
  {
      return 1;
  }
  else if (num_of_sampling>=2*(DATASET_SIZE/NUM_CLASSES)+40 && num_of_sampling<3*(DATASET_SIZE/NUM_CLASSES)+40)
  {
      return 2;
  }
  else if (num_of_sampling>=3*(DATASET_SIZE/NUM_CLASSES)+60 && num_of_sampling<4*(DATASET_SIZE/NUM_CLASSES)+60)
  {
      return 3;
  }
  return -1;
}

int get_idx()
{
  return idx;
}


void init_model(){
  init_SGD(false,0.8,0.001);
  eval_reset();
  for(int i=0;i<DATASET_SIZE;i++){
      indexes[i]=i;
  }
  shuffle(indexes,DATASET_SIZE);
  printf("\r\nMODEL INITIALIZED\r\n");
}

//Get samples and add them to the dataset

void add_sample_to_data()
{
  printf("Class %d | Sample %d out of %d \n\r",get_class(),1+idx-(get_class()*DATASET_SIZE/NUM_CLASSES),DATASET_SIZE/NUM_CLASSES);
  //Turn the incoming IQ samples into a transfer function and normalize it
  if (idx<DATASET_SIZE){
      float minpha=0;
      float maxpha=0;
      float minmag=0;
      float maxmag=0;
      for (int i=0; i< IQ_SAMPLE_LEN;i++)
      {
          hadm_data[i]=compl_multiplication(initiator_i[i],initiator_q[i],reflector_i[i],reflector_q[i]);
          X[idx][i] = calc_second_norm(hadm_data[i].i,hadm_data[i].q);
          X[idx][i+IQ_SAMPLE_LEN]=atan2(hadm_data[i].q,hadm_data[i].i);
          if (i==0)
          {
              minpha=X[idx][i+IQ_SAMPLE_LEN];
              maxpha=X[idx][i+IQ_SAMPLE_LEN];
          }
          minpha=min(X[idx][i+IQ_SAMPLE_LEN],minpha);
          maxpha=max(X[idx][i+IQ_SAMPLE_LEN],maxpha);
          if (i==0){
              minmag=X[idx][i];
              maxmag=X[idx][i];
          }
          minmag=min(X[idx][i],minmag);
          maxmag=max(X[idx][i],maxmag);

      }
      for (int i=0; i< IQ_SAMPLE_LEN;i++)
      {
          X[idx][i] = (X[idx][i]-minmag)/(maxmag-minmag);
          X[idx][i+IQ_SAMPLE_LEN]=(X[idx][i+IQ_SAMPLE_LEN]-minpha)/(maxpha-minpha);
      }
      if (num_of_sampling<DATASET_SIZE/NUM_CLASSES){
          y[idx]=0;
          idx++;
      }
      else if (num_of_sampling>=DATASET_SIZE/NUM_CLASSES+20 && num_of_sampling<2*(DATASET_SIZE/NUM_CLASSES)+20)
      {
          y[idx]=1;
          idx++;
      }
      else if (num_of_sampling>=2*(DATASET_SIZE/NUM_CLASSES)+40 && num_of_sampling<3*(DATASET_SIZE/NUM_CLASSES)+40)
      {
          y[idx]=2;
          idx++;
      }
      else if (num_of_sampling>=3*(DATASET_SIZE/NUM_CLASSES)+60 && num_of_sampling<4*(DATASET_SIZE/NUM_CLASSES)+60)
      {
          y[idx]=3;
          idx++;
      }
      num_of_sampling=num_of_sampling+1;
  }
}

//Print the transfer function for debugging purposes

void print_X(int index)
{
  printf("TRANSFER FUNC: ");
  for(int i=0;i<NUM_FEATURES;i++){
        printf("%f, ",X[index][i]);
    }
    printf("\r\n");
}

//Get the latest sample, preprocess it and make a prediction

int predict()
{
  float minpha=0,maxpha=0;
  float minmag=0,maxmag=0;
  for (int i=0; i< IQ_SAMPLE_LEN;i++)
  {
      hadm_data[i]=compl_multiplication(initiator_i[i],initiator_q[i],reflector_i[i],reflector_q[i]);
      X_inference[i] = calc_second_norm(hadm_data[i].i,hadm_data[i].q);
      X_inference[i+IQ_SAMPLE_LEN]=atan2(hadm_data[i].q,hadm_data[i].i);
      minpha=min(X_inference[i+IQ_SAMPLE_LEN],minpha);
      maxpha=max(X_inference[i+IQ_SAMPLE_LEN],maxpha);
      minmag=min(X_inference[i],minmag);
      maxmag=max(X_inference[i],maxmag);
  }
  for (int i=0; i< IQ_SAMPLE_LEN;i++)
  {
      X_inference[i] =  (X_inference[i]-minmag)/(maxmag-minmag);
      X_inference[i+IQ_SAMPLE_LEN]=(X_inference[i+IQ_SAMPLE_LEN]-minpha)/(maxpha-minpha);
  }
  return  inference(X_inference);
}
