#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <json-c/json.h>

#include "Network.h"

#define ABS(x) (    ((x)>0)? (x) : (-(x)))

#define NN 4

static void Network_Init(Network*);

static int generatesPreceptron(Network *This);
static int fillInNetworkJSON(Network *This,char *fileName);
static int train(Network *This);
static int initValueRandom(Network *This);
static int inputData(Network *This);
static int testNetwork(Network *This);

/******************************************************************************/
Network* New_Network(char *fileName,char *initValue)
{
    Network *This = malloc(sizeof(Network));
    if(!This) return NULL;
    Network_Init(This);
    
    fillInNetworkJSON(This,fileName);
    generatesPreceptron(This);
    if(!strcmp(initValue,"RANDOM")){
        initValueRandom(This);
    }

    return This;
}

/******************************************************************************/
static void Network_Init(Network *This)
{
    This->generatesPreceptron=generatesPreceptron;
    This->fillInNetworkJSON=fillInNetworkJSON;
    This->train=train;
    This->initValueRandom=initValueRandom;
    This->inputData=inputData;
    This->testNetwork=testNetwork;
}

/******************************************************************************/
static int generatesPreceptron(Network *This){

    This->perceptron=malloc(sizeof(Perceptron) * This->nperceptron);
    
    int nshadow = (This->nperceptron - This->ninput - This->noutput - This->nsolve);

    int temp,n=0;
    for (; n < This->ninput; n++){//create input
        This->perceptron[n] = New_Perceptron(NN,NULL,0,NULL,This->nperceptronExit[n]);
    }
    temp = n;
    //pas sur
    for(;n<nshadow + temp;n++){//create perceptron    
        This->perceptron[n] = New_Perceptron(NN,This->perceptron,This->ninput,NULL,This->nperceptronExit[n]);
    }
    temp = n;
    for(;n<This->noutput + temp;n++){//create output    
        This->perceptron[n] = New_Perceptron((unsigned int)4,&(This->perceptron[This->ninput]),(unsigned int)nshadow,NULL,0);
    }
    temp = n;
    for(;n<This->nsolve + temp;n++){//create solve
        This->perceptron[n] = New_Perceptron((unsigned int)4,&(This->perceptron[This->ninput + nshadow]),(unsigned int)This->noutput,NULL,0);
    }
 

    for(n=0;n<This->nperceptron;n++){//link perceptron to solve
        for(int i=0;i<This->nsolve;i++){
            This->perceptron[n]->solve =  This->perceptron[This->solve[i]];
        }
    }

    for(n=0;n<This->nperceptron;n++){//link perceptron to next
        if(n != This->solve[0]){
            Perceptron **temp = malloc(sizeof(Perceptron*) * This->perceptron[n]->get_nnext(This->perceptron[n]));
            for(unsigned int i=0;i<This->perceptron[n]->get_nnext(This->perceptron[n]);i++){
                temp[i] = This->perceptron[This->perceptronExit[n][i]];
            }
            This->perceptron[n]->set_next(This->perceptron[n],temp,This->perceptron[n]->get_nnext(This->perceptron[n]));
        }
    }

    return 0;
}

/******************************************************************************/
static int fillInNetworkJSON(Network *This,char *fileName){

    FILE *fp;
	char buffer[1024];
	struct json_object *parsed_json;
    struct json_object *input;
    struct json_object *output;
    struct json_object *solve;
    struct json_object *nPerceptron;
    struct json_object *perceptron;
    struct json_object **perceptronExit;
    

	fp = fopen(fileName,"r");
	fread(buffer, 1024, 1, fp);
	fclose(fp);

	parsed_json = json_tokener_parse(buffer);

	json_object_object_get_ex(parsed_json, "input", &input);
    json_object_object_get_ex(parsed_json, "output", &output);
    json_object_object_get_ex(parsed_json, "solve", &solve);
    json_object_object_get_ex(parsed_json, "nPerceptron", &nPerceptron);
    json_object_object_get_ex(parsed_json, "perceptron", &perceptron);

    This->ninput = json_object_array_length(input);
    This->noutput = json_object_array_length(output);
    This->nsolve = json_object_array_length(solve);
    This->nperceptron = json_object_get_int(nPerceptron);

    This->nperceptronExit = malloc( sizeof(int) * This->nperceptron);

    This->input = malloc(sizeof(int) * This->ninput);
    This->output = malloc(sizeof(int) * This->noutput);
    This->solve = malloc(sizeof(int) * This->nsolve);
    This->perceptronExit = malloc( sizeof(int*) * This->nperceptron);

    perceptronExit = malloc(sizeof(perceptron) * 10);

    for(int n=0;n<This->ninput;n++){
        This->input[n] = json_object_get_int(json_object_array_get_idx(input,n));
        printf("input %d -> %d\n",n,This->input[n]);
    }

    for(int n=0;n<This->noutput;n++){
        This->output[n] = json_object_get_int(json_object_array_get_idx(output,n));
        printf("output %d -> %d\n",n,This->output[n]);
    }

    for(int n=0;n<This->nsolve;n++){
        This->solve[n] = json_object_get_int(json_object_array_get_idx(solve,n));
        printf("solve %d -> %d\n",n,This->solve[n]);
    }

    for(int n=0;n<This->nperceptron ;n++){
        
        char buffer[128];
        snprintf(buffer,sizeof(char) * 128,"%d",n);

        This->perceptronExit[n] = malloc(sizeof(int) * 10);

        json_object_object_get_ex(perceptron, buffer, &perceptronExit[n]);

        This->nperceptronExit[n] = (int)json_object_array_length(perceptronExit[n]);

        for(int i=0;i<This->nperceptronExit[n];i++){
            This->perceptronExit[n][i] = json_object_get_int(json_object_array_get_idx(perceptronExit[n],i));
            printf("Neural %d / Exit %d -> %d\n",n,i,This->perceptronExit[n][i]);
        }
    }
    
        return 0;
}

/******************************************************************************/
static int train(Network *This)
{
    double err=900;
    for(int i=0;i<100 && err !=0;i++){
        err=0;
        for(int n=This->ninput;n<This->nperceptron - This->noutput - This->nsolve ;n++){
            This->perceptron[n]->set_nformed(This->perceptron[n],This->perceptron[n]->get_nprev(This->perceptron[n])-1);
        
            This->perceptron[n]->forming(This->perceptron[n]);
            printf("---------------------------------------------------------------------------------------------FORMING1\tOK\n");
        }
        //TODO : do more clean en perform

        printf("%s%lf\t%lf\t%lf\t%lf\n\n",This->perceptron[This->output[0]]->toString(This->perceptron[This->output[0]]),
            This->perceptron[This->solve[0]]->get_exit(This->perceptron[This->solve[0]])[0],
            This->perceptron[This->solve[0]]->get_exit(This->perceptron[This->solve[0]])[1],
            This->perceptron[This->solve[0]]->get_exit(This->perceptron[This->solve[0]])[2],
            This->perceptron[This->solve[0]]->get_exit(This->perceptron[This->solve[0]])[3]);
       
        for(int n=0;n<4;n++){
            err+= ABS(This->perceptron[This->output[0]]->get_exit(This->perceptron[This->output[0]])[n] - This->perceptron[This->solve[0]]->get_exit(This->perceptron[This->solve[0]])[n] );
        }   
        printf("---------------------------------------------------------------------------------------------ERR\tCALCULED\n");

        
        for(int n=This->output[This->noutput-1];n>=This->output[0];n--){
            This->perceptron[n]->set_nformedDelta(This->perceptron[n],This->perceptron[n]->get_nnext(This->perceptron[n])-1);
        
            This->perceptron[n]->formingDelta(This->perceptron[n]);
            printf("---------------------------------------------------------------------------------------------END_DELTA1\tOK\n");
        }

   /* printf("-----------------------------FFFFFFFFFFFLLLLLLLAAAAAAAAAAAGGGGGG---------------\n\n");
printf("-----------------------------EEEEEEEEENNNNNNNNNNNNNNDDDDDDDDDDDDDDDD---------------\n\n");
return 0;*/
        for(int n=This->ninput;n<This->nperceptron - 1;n++){
            This->perceptron[n]->update(This->perceptron[n]);
            printf("\n");
        }
       

        printf("ERR-------------------------------------------->>>>>>>>>>>>>>>>%lf\n",err);
    
    }

    return 0;
}

/******************************************************************************/
static int initValueRandom(Network *This){
    srand(time(NULL));
    for(int n=0;n<This->nperceptron;n++){
        double *temp = malloc(sizeof(double) * This->perceptron[n]->get_nenter(This->perceptron[n]));
        for(unsigned int i=0;i<This->perceptron[n]->get_nenter(This->perceptron[n]);i++){
            temp[i] = (sqrt(-2.0*log((double)rand()/RAND_MAX)))*(cos(6.28318530718*(double)rand()/RAND_MAX));
        }
        This->perceptron[n]->set_weights(This->perceptron[n],temp);
    }
    return 0;
}

/******************************************************************************/
static int inputData(Network *This){
    double tempSolve[NN];
    double temp[3][NN];
    for(int n=0;n<NN;n++){
        for(int i=0;i<3;i++){  
            printf("%dx%d: ",n,i);
            scanf("%lf",&temp[i][n]);
        }
        printf("y%d: ",n);
        scanf("%lf",&tempSolve[n]);
    }

    This->perceptron[This->solve[0]]->set_exit(This->perceptron[This->solve[0]],tempSolve);

    This->perceptron[0]->set_exit(This->perceptron[0],temp[0]);
    This->perceptron[1]->set_exit(This->perceptron[1],temp[1]);
    This->perceptron[2]->set_exit(This->perceptron[2],temp[2]);

    return 0;
}

/******************************************************************************/
static int testNetwork(Network *This){
    Perceptron *test[3];
        for(int n=0;n<4;n++){
            test[0] = New_Perceptron(1,NULL,0,NULL,0);
            test[1] = New_Perceptron(1,NULL,0,NULL,0);
            test[2] = New_Perceptron(1,NULL,0,NULL,0);
        }

        printf("\n\nTEST\n");

        for(int i=0;i<3;i++){  
            double temp[2];
            printf("X%d: ",i);
            scanf("%lf",&temp[0]);
            test[i]->set_exit(test[i],temp);
        }

        Perceptron *perceptronTEST[2];

        for(int n=0;n<2;n++)    perceptronTEST[n] = New_Perceptron(1,test,3,NULL,0);
        for(int n=0;n<2;n++)    perceptronTEST[n]->set_nformed(perceptronTEST[n],perceptronTEST[n]->get_nprev(perceptronTEST[n])-1);

            perceptronTEST[0]->set_weights(perceptronTEST[0],This->perceptron[3]->get_weights(This->perceptron[3]));
            perceptronTEST[1]->set_weights(perceptronTEST[1],This->perceptron[4]->get_weights(This->perceptron[4]));

            Perceptron *endTest = New_Perceptron(1,perceptronTEST,2,NULL,0);

            endTest->set_weights(endTest,This->perceptron[5]->get_weights(This->perceptron[5]));

            perceptronTEST[0]->set_next(perceptronTEST[0],&endTest,1);
            perceptronTEST[1]->set_next(perceptronTEST[1],&endTest,1);

            perceptronTEST[0]->forming(perceptronTEST[0]);
            perceptronTEST[1]->forming(perceptronTEST[1]);

        printf("Y: %lf\n",endTest->get_exit(endTest)[0]);

    return 0;
}