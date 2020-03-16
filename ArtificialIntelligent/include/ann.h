#ifndef _ANN_H_
#define _ANN_H_

#include "mnist_hwn.h"

typedef struct _WEIGHT
{
    double **weight;        // weight for neuron
    int frontCount;           // row neuron count
    int endCount;           // column neuron count
}Weight;

typedef struct _NODE
{
    double *val;            // double data array
}Node;


typedef struct _PROCESS
{
    int Layer;                  // layer count with output layer
    int *pLayerCount;           // neuron count for each layer
    Weight *layerWeight;        // weight for each layer
    Weight *layerDeltaWeight;   // delta layer buffer for each layer
    Node *delta;                // delta buffer for output buffer
    Node *hidden;               // hidden layer neuron
    Node *data;                 // processable data for each layer
    int expect;                 // matched real value
    double error;               // error rate
    double learningRate;        // learning rate
    double momentum;            // Momentum (heuristics to optimize back-propagation algorithm)
    double epsilon;             // Epsilon, no more iterations if the learning error is smaller than epsilon
}Process;


void loadWeight(Process *ann);
void saveWeight(Process *ann);

void perception(Process *ann);
void updateWeight(Process *ann);

void squareError(Process *ann);

void processInit(Process *ann, int layerCount, ...);
void processDeinit(Process *ann);
// input data update for Human writing number recognition
void processUpdateInput(Process *ann, unsigned char *image, int expect);

#endif