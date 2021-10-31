#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <json-c/json.h>

#include "Network.h"
#include "ImportExport.h"

#define ABS(x) (((x) > 0) ? (x) : (-(x)))

#define NN 8

static void Network_Init(Network *);
static int destructor(Network *This);

static int generatesPreceptron(Network *This);
static int train(Network *This, unsigned long iter, double errMin);
static int initValueRandom(Network *This);
static int inputData(Network *This);
static int testNetwork(Network *This);

/******************************************************************************/
Network *New_Network(char *descFileName, char *structFileName, char *initValue)
{
    Network *This = malloc(sizeof(Network));
    if (!This)
        return NULL;
    Network_Init(This);

    fillInNetwork(This, descFileName, structFileName);
    generatesPreceptron(This);
    if (!strcmp(initValue, "RANDOM"))
    {
        initValueRandom(This);
    }
    else
    {
        if (!strcmp(initValue, "JSON"))
        {
            initValueJson(This, "result.json");
        }
    }

    return This;
}

/******************************************************************************/
static void Network_Init(Network *This)
{
    This->destructor = destructor;

    This->generatesPreceptron = generatesPreceptron;
    This->fillInNetwork = fillInNetwork;
    This->fillInNetworkJSON = fillInNetworkJSON;
    This->train = train;
    This->initValueRandom = initValueRandom;
    This->inputData = inputData;
    This->inputDataCsv = inputDataCsv;
    This->testNetwork = testNetwork;
    This->exportNetwork = exportNetwork;
    This->initValueJson = initValueJson;
}

/******************************************************************************/
static int generatesPreceptron(Network *This)
{

    This->perceptron = malloc(sizeof(Perceptron) * This->nperceptron);

    int n = 0;
    for (; n < This->ninput; n++)
    { //create input
        This->perceptron[n] = New_Perceptron(n,NN, NULL, 0, NULL, This->nperceptronExit[n], SIGMOID);
    }

    for (; n < This->nperceptron; n++)
    { //create perceptron
        This->perceptron[n] = New_Perceptron(n,NN, NULL, 0, NULL, This->nperceptronExit[n], SIGMOID);   
    }
    
    for (n = 0; n < This->nperceptron; n++)
    { //set link perceptron

        Perceptron **tempPrev = malloc(sizeof(Perceptron *) * This->nperceptron - This->ninput - This->noutput - This->nsolve);
        unsigned int np = 0;
        for (int i = 0; i < This->nperceptron; i++)
        {
            //printf("nExit: %d\t", This->nperceptronExit[i]);
            for (int j = 0; j < This->nperceptronExit[i]; j++)
            {
                if (This->perceptronExit[i][j] == n)
                {
                    tempPrev[np++] = This->perceptron[i];
                    //printf("%d:%d -> %d\t", i, j, This->perceptronExit[i][j]);
                    //printf("%d->%p\n", This->perceptronExit[i][j], (void *)This->perceptron[This->perceptronExit[i][j]]);
                }
            }
        }

        if(np != 0){
            This->perceptron[n]->set_prev(This->perceptron[n], tempPrev, np);
            printf("N: %d\t%p\tNperv: %d\tNnext: %d\tPrev0 :%p\n", n,
               (void *)This->perceptron[n],
               np,
               This->nperceptronExit[n],
               (void *)This->perceptron[n]->prev[0]);

        }
        else{
            printf("N: %d\t%p\tNperv: %d\tNnext: %d\n", n,
               (void *)This->perceptron[n],
               This->perceptron[n]->nprev,
               This->nperceptronExit[n]);

        }

        
    }

    printf("\n");
    //for (n = 0; n < This->noutput; n++)
    //{ //link perceptron to solve
    //    for (int i = 0; i < This->nsolve; i++)
    //    {
    //        int j = This->output[n];
    //        //printf("%p\n", (void *)This->perceptron[n]);
    //        This->perceptron[j]->solve = This->perceptron[This->solve[i]];
    //        printf("N: %d\t%p\tSolve: %p\n", j,
    //               (void *)This->perceptron[j],
    //               (void *)This->perceptron[j]->solve);
    //    }
    //}
    for (n = 0; n < This->nperceptron; n++)
    { //link perceptron to solve
        for (int i = 0; i < This->nsolve; i++)
        {
            //printf("%p\n", (void *)This->perceptron[n]);
            This->perceptron[n]->solve = This->perceptron[This->solve[i]];
            printf("N: %d\t%p\tSolve: %p\n", n,
                   (void *)This->perceptron[n],
                   (void *)This->perceptron[n]->solve);
        }
    }

    printf("\n");
    for (n = 0; n < This->nperceptron; n++)
    {
        Perceptron **tempNext = malloc(sizeof(Perceptron *) * This->perceptron[n]->nnext);
        for (unsigned int i = 0; i < This->perceptron[n]->nnext; i++)
        {
            //printf("%d _ %d __%d -- %d", n, i, This->perceptron[n]->nnext, This->perceptronExit[n][i]);
            //printf("\n");
            //printf("--------------------------------\n\n--------------------------------\n\n--------------------------------\n\n");

            tempNext[i] = This->perceptron[This->perceptronExit[n][i]];
            //printf("\n");
            //printf("###########################\n\n###########################\n\n###########################\n\n");
        }

        This->perceptron[n]->set_next(This->perceptron[n], tempNext, This->perceptron[n]->nnext);

        if(This->perceptron[n]->nprev != 0 && This->perceptron[n]->nnext !=0)
        {
            printf("N: %d\t%p\tNperv: %d\tNnext: %d\tPrev0 :%p\tNext0: %p\n", n,
               (void *)This->perceptron[n],
               This->perceptron[n]->nprev,
               This->perceptron[n]->nnext,
               (void *)This->perceptron[n]->prev[0],
               (void *)This->perceptron[n]->next[0]);
        }
        else{
            printf("N: %d\t%p\tNperv: %d\tNnext: %d\n", n,
               (void *)This->perceptron[n],
               This->perceptron[n]->nprev,
               This->perceptron[n]->nnext);

        }
    }

    return 0;
}

/******************************************************************************/
static int train(Network *This, unsigned long iter, double errMin)
{
    This->err = 900;
    for (unsigned long i = 0; i < iter && This->err >= errMin; i++)
    {
        This->err = 0;

        for (int n = 0; n < This->ninput; n++)
        {
        
            for (int j = 0; j < This->nperceptronExit[n]; j++)
            {
                //printf("---------------------------------------------------------------------------------------------FORMING%d:%d\t\n",n,j);
                This->perceptron[n]->next[j]->forming(This->perceptron[n]->next[j]);
                //printf("---------------------------------------------------------------------------------------------FORMING%d:%d\tOK\n",n,j);
            }
        }

        //for (int n = 0; n < This->nperceptron; n++)
        //{
        //    if(This->perceptron[n]->nprev == 0){
        //        for (int j = 0; j < This->nperceptronExit[n]; j++)
        //        {
        //            //printf("---------------------------------------------------------------------------------------------FORMING%d:%d\t\n",n,j);
        //            This->perceptron[n]->next[j]->forming(This->perceptron[n]->next[j]);
        //            //printf("---------------------------------------------------------------------------------------------FORMING%d:%d\tOK\n",n,j);
        //        }
        //    }
        //}
        //TODO : do more clean en perform

        for (int n = 0; n < NN; n++)
        {
            //printf("%lf - %lf\n",This->perceptron[This->output[0]]->exit[n], This->perceptron[This->solve[0]]->exit[n]);
            This->err += ABS(This->perceptron[This->output[0]]->exit[n] - This->perceptron[This->solve[0]]->exit[n]);
        }

        // printf("---------------------------------------------------------------------------------------------ERR\tCALCULED\n");
        for (int n = This->output[This->noutput - 1]; n >= This->output[0]; n--)
        {

            This->perceptron[n]->nformedDelta = This->perceptron[n]->nnext - 1;

            This->perceptron[n]->formingDelta(This->perceptron[n]);
            //   printf("---------------------------------------------------------------------------------------------END_DELTA%d\tOK\n",n);
        }

        for (int n = This->ninput; n < This->nperceptron - 1; n++)
        {
            This->perceptron[n]->update(This->perceptron[n]);
        }

        //printf("ERR-------------------------------------------->>>>>>>>>>>>>>>>%lf\n",This->err);
    }
    return 1;
}

/******************************************************************************/
static int initValueRandom(Network *This)
{
    srand(time(NULL));
    for (int n = 0; n < This->nperceptron; n++)
    {
        double temp;
        for (unsigned int i = 0; i < This->perceptron[n]->nprev;i++)
        {
            temp = (sqrt(-2.0 * log((double)rand() / RAND_MAX))) * (cos(6.28318530718 * (double)rand() / RAND_MAX));
            
            This->perceptron[n]->set_synapse(This->perceptron[n],This->perceptron[n]->prev[i]->id, temp);
        }
    }
    return 1;
}

/******************************************************************************/
static int inputData(Network *This)
{
    double tempSolve[NN];
    double temp[3][NN];
    for (int n = 0; n < NN; n++)
    {
        for (int i = 0; i < 3; i++)
        {
            printf("%dx%d: ", n, i);
            scanf("%lf", &temp[i][n]);
        }
        printf("y%d: ", n);
        scanf("%lf", &tempSolve[n]);
    }

    This->perceptron[This->solve[0]]->set_exit(This->perceptron[This->solve[0]], tempSolve);

    This->perceptron[0]->set_exit(This->perceptron[0], temp[0]);
    This->perceptron[1]->set_exit(This->perceptron[1], temp[1]);
    This->perceptron[2]->set_exit(This->perceptron[2], temp[2]);

    return 0;
}

/******************************************************************************/
static int testNetwork(Network *This)
{

    Perceptron **perceptronTEST;
    perceptronTEST = malloc(sizeof(Perceptron) * This->nperceptron);
    memcpy(perceptronTEST, This->perceptron, sizeof(Perceptron) * This->nperceptron);

    double **tempExit = malloc(sizeof(double *) * This->nperceptron);
    for (int n = 0; n < This->nperceptron; n++)
    {
        tempExit[n] = malloc(sizeof(double) * NN);
        memcpy(tempExit[n], perceptronTEST[n]->exit, sizeof(double *) * NN);
    }

    for (int i = 0; i < This->ninput; i++)
    {
        double temp[2];
        printf("X%d: ", i);
        scanf("%lf", &temp[0]);
        perceptronTEST[i]->set_exit(perceptronTEST[i], temp);
    }

    for (int n = 0; n < This->ninput; n++)
    {
        for (int i = 0; i < This->nperceptronExit[n]; i++)
        {
            //printf("---------------------------------------------------------------------------------------------FORMING%d:%d\t\n",n,i);
            This->perceptron[n]->next[i]->forming(This->perceptron[n]->next[i]);
            //printf("---------------------------------------------------------------------------------------------FORMING%d:%d\tOK\n",n,i);
        }
    }

    printf("%lf\n", This->perceptron[This->output[0]]->exit[0]);

    free(perceptronTEST);

    for (int n = 0; n < This->nperceptron; n++)
    {
        memcpy(This->perceptron[n]->exit, tempExit[n], sizeof(double *) * NN);
    }
    free(tempExit);

    return 0;
}

/******************************************************************************/
static int destructor(Network *This)
{
    free(This->nperceptronExit);
    free(This->input);
    free(This->output);
    free(This->solve);

    for (int n = 0; n < This->nperceptron; n++)
    {
        free(This->perceptronExit[n]);
    }

    This->perceptronExit = realloc(This->perceptronExit, 1);
    free(This->perceptronExit);

    for (int n = 0; n < This->nperceptron; n++)
    {
        This->perceptron[n]->destructor(This->perceptron[n]);
        free(This->perceptron[n]);
    }

    free(This->perceptron);

    return 1;
}