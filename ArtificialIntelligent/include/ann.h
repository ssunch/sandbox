#ifndef _ANN_H_
#define _ANN_H_

#include "mnist_hwn.h"

typedef enum
{
    ACTIVATION_SIGMOID,
    ACTIVATION_TANH,
    ACTIVATION_RELU,
    ACTIVATION_ELU,
    ACTIVATION_LEAKYRELU,
    ACTIVATION_MAX
} ACTIVATION_FUNC;

typedef enum
{
    DEACTIVATION_SIGMOID,
    DEACTIVATION_TANH,
    DEACTIVATION_RELU,
    DEACTIVATION_ELU,
    DEACTIVATION_LEAKYRELU,
    DEACTIVATION_MAX
} DEACTIVATION_FUNC;

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
    int Layer;                                              // layer count with output layer
    int *pLayerCount;                                       // neuron count for each layer
    Weight *layerWeight;                                    // weight for each layer
    Weight *layerDeltaWeight;                               // delta layer buffer for each layer
    Node *delta;                                            // delta buffer for output buffer
    Node *hidden;                                           // hidden layer neuron
    Node *data;                                             // processable data for each layer
    int expect;                                             // matched real value
    double error;                                           // error rate
    double learningRate;                                    // learning rate
    double momentum;                                        // Momentum (heuristics to optimize back-propagation algorithm)
    double epsilon;                                         // Epsilon, no more iterations if the learning error is smaller than epsilon

    double (*actFunc)(double);                              // Activation function callback
    double (*deactFucn)(double);
    void (*processUpdateInput)(struct _PROCESS *ann, unsigned char *image, int expect);
    void (*trainning)(struct _PROCESS *ann, unsigned char *image, int expect);
    void (*predict)(struct _PROCESS *ann, unsigned char *image, int expect);
    void (*load)(struct _PROCESS *ann, char *_addedStr);
    void (*save)(struct _PROCESS *ann, char *_addedStr);
    void (*perception)(struct _PROCESS *ann);
    void (*updateWeight)(struct _PROCESS *ann);
    void (*squareError)(struct _PROCESS *ann);
    void (*CrossentropyError)(struct _PROCESS *ann);
    
}Process;


void processInit(Process *ann, int layerCount, ...);
void processDeinit(Process *ann);

// thread process
void *annProcess(void *data);
// initialize input data set and Process
void setInitInputDataSet(Process *ann, pImageSetFile image, pLabelSetFile label);
int updateWeightThread(Process *ann);

#endif