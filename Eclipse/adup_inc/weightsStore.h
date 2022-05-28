#ifndef WEIGHTS_STORE_H
#define WEIGHTS_STORE_H

/***** Includes *****/
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include "mxc_device.h"
#include "led.h"
#include "board.h"
#include "mxc_delay.h"
#include "uart.h"
#include "dma.h"
#include "nvic_table.h"

#include "c_adup.h"

weightsTracker *tracker=NULL;

bool makeTracker(void); // call once to create the top level tracker object

/** API for SJ to interact with the weights data sets */

// memory management
int makeWeights(int numKernels, int kernelSz[], int numBiases, int biasSz[], int nameSz, char name[]); // returns -1 on fail, else ID; else mallocs memory for a weights object of given size params
bool freeWeights(int id); // return false on fail; else free all memory assoicated with a given wieghts ID

// data interactions
bool getName(int id, char **name); // fetch the name of a given weights object
bool getCounts(int id, int *kernelCnt, int *biasCnt); // return false false for invalid weights object; requests number of kernel and bias vectors for given ID
bool getKernel(int id, int kernelId, uint32_t **vector, uint32_t *size); // fetch a kernel vector from a given ID; returns false if ID isn't valid/extant
bool getBias(int id, int biasId, uint32_t **vector, uint32_t *size); // fetch a bias vector from a given ID; returns false if ID isn't valid/extant)

void adup_weights_handler(msg_t *msg);

typedef struct weightsObject weightsObject;
struct weightsObject {
  uint32_t id;
  uint32_t numKernels; // programmatic number of kernel vectors
  uint32_t *kernelSz; // one sz per kernel vector
  uint32_t *kernels; // flattened array of all kernel data

  uint32_t numBiases;
  uint32_t *biasSz;
  uint32_t *biases;

  uint32_t nameSz; // size of name field
  char *name;
};

typedef struct weightsTracker weightsTracker;
struct weightsTracker {
	uint32_t maxEntries;
	weightsObject **weights;
	bool *valid;
	bool *exists;
};

void make_handler(msg_t *msg); // request a new weights object to be created; returns id (-1=fail)
void status_handler(msg_t *msg); // check status flags for a given weights object id (valid, exists)
void free_handler(msg_t *msg); // frees memory for a given weights object id
void get_handler(msg_t *msg); // fetch metadata for a given id (num vectors and size of each vector)
void write_handler(msg_t *msg); // populate a given ID with kernel and bias vectors
void risk_handler(msg_t *msg); // return most recent risk data (top level decision, are you a human?)
void test_handler(msg_t *msg); // request the test to be run (same as pressing the push button)
void cnn_handler(msg_t *msg); // provide 2 weights IDs to the main app, indicating which weights objects to use in tests

#endif
