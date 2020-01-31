#ifndef CGI_NETWORK_H
#define CGI_NETWORK_H

#include "Perceptron.h"

#ifdef __cplusplus
    extern "C" {
#endif

/*  Structure représantant l'objet Network. */
typedef struct Network
    {
        int             (*destructor)(struct Network*);

        int             (*train)(struct Network*,unsigned long,double);
        int             (*fillInNetworkJSON)(struct Network*,char*);
        int             (*generatesPreceptron)(struct Network*);
        int             (*initValueRandom)(struct Network*);
        int             (*inputData)(struct Network*);
        int             (*inputDataCsv)(struct Network*,char*);
        int             (*testNetwork)(struct Network*);
        int             (*exportNetwork)(struct Network*,char*,char*);
        int             (*initValueJson)(struct Network*,char*);

        int             ninput;
        int             noutput;
        int             nsolve;
        int             nperceptron;
        int             *nperceptronExit;

        int             *input;
        int             *output;
        int             *solve;
        int             **perceptronExit;

        Perceptron      **perceptron;
    } Network ;


Network* New_Network(char *fileName,char *initValue);

#ifdef __cplusplus
}
#endif

#endif
            