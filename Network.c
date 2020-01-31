#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <json-c/json.h>

#include "Network.h"

#define ABS(x) (    ((x)>0)? (x) : (-(x)))

#define NN 8

static void Network_Init(Network*);
static int destructor(Network *This);

static int generatesPreceptron(Network *This);
static int fillInNetworkJSON(Network *This,char *fileName);
static int train(Network *This,unsigned long iter,double errMin);
static int initValueRandom(Network *This);
static int inputData(Network *This);
static int testNetwork(Network *This);
static int exportNetwork(Network *This,char *networkName,char *fileName);
static int initValueJson(Network *This,char *fileName);
static int inputDataCsv(Network *This,char *fileName);

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
    else
    {
        if(!strcmp(initValue,"JSON")){
            initValueJson(This,"result.json");
        }
    }
    

    return This;
}

/******************************************************************************/
static void Network_Init(Network *This)
{
    This->destructor=destructor;

    This->generatesPreceptron=generatesPreceptron;
    This->fillInNetworkJSON=fillInNetworkJSON;
    This->train=train;
    This->initValueRandom=initValueRandom;
    This->inputData=inputData;
    This->inputDataCsv=inputDataCsv;
    This->testNetwork=testNetwork;
    This->exportNetwork=exportNetwork;
    This->initValueJson=initValueJson;
}

/******************************************************************************/
static int generatesPreceptron(Network *This){

    This->perceptron=malloc(sizeof(Perceptron) * This->nperceptron);
    
    int nshadow = (This->nperceptron - This->ninput - This->noutput - This->nsolve);

    int temp,n=0;
    for (; n < This->ninput; n++){//create input
        This->perceptron[This->input[n]] = New_Perceptron(NN,NULL,0,NULL,This->nperceptronExit[This->input[n]],SIGMOID);
        printf("N: %d\t%p\tNperv: %d\tNnext: %d\n",n,
        (void*)This->perceptron[This->input[n]],
        0,
        This->nperceptronExit[This->input[n]]);
    }
   
    temp = n;
    for(;n<nshadow + temp;n++){//create perceptron    
        
        Perceptron **tempPerceptron = malloc(sizeof(Perceptron*) * This->nperceptron - This->ninput - This->noutput - This->nsolve);
        unsigned int np=0;
        for(int i=0;i<This->nperceptron;i++){
            for(int j=0;j<This->nperceptronExit[i];j++) {
                if(This->perceptronExit[i][j] == n){
                    tempPerceptron[np] = This->perceptron[This->perceptronExit[i][j]];
                    np++;
                    //printf("%d:%d\t",i,n);
                    //printf("%d->%p\n",This->perceptronExit[i][j],(void*)This->perceptron[This->perceptronExit[i][j]]);
                }
            }
        }

        This->perceptron[n] = New_Perceptron(NN,tempPerceptron,np,NULL,This->nperceptronExit[n],SIGMOID);
        printf("N: %d\t%p\tNperv: %d\tNnext: %d\tPrev0 :%p\n",n,
        (void*)This->perceptron[n],
        np,
        This->nperceptronExit[n],
        (void*)This->perceptron[n]->prev[0]);
   
    }
    
    temp = n;
    for(;n<This->noutput + temp;n++){//create output    

        unsigned int np=0;
        for(int i=0;i<This->nperceptron;i++){
            for(int j=0;j<This->nperceptronExit[i];j++) {
                if(This->perceptronExit[i][j] == n){
                    np++;
                }
            }
        }
            
        This->perceptron[n] = New_Perceptron(NN,&(This->perceptron[This->ninput]),(unsigned int)np,NULL,0,SIGMOID);
        printf("N: %d\t%p\tNperv: %d\tNnext: %d\tPrev0 :%p\n",n,
        (void*)This->perceptron[n],
        np,
        0,
        (void*)This->perceptron[n]->prev[0]);
    }
    
    temp = n;
    for(;n<This->nsolve + temp;n++){//create solve
        This->perceptron[n] = New_Perceptron(NN,&(This->perceptron[This->ninput + nshadow]),(unsigned int)This->noutput,NULL,0,SIGMOID);
        printf("N: %d\t%p\tNperv: %d\tNnext: %d\tPrev0 :%p\n",n,
        (void*)This->perceptron[n],
        This->noutput,
        0,
        (void*)This->perceptron[n]->prev[0]);
    }
    printf("\n");
    for(n=0;n<This->nperceptron;n++){//link perceptron to solve
        for(int i=0;i<This->nsolve;i++){
            This->perceptron[n]->solve =  This->perceptron[This->solve[i]];
            printf("N: %d\t%p\tSolve: %p\n",n,
            (void*)This->perceptron[n],
            (void*)This->perceptron[n]->solve);
        }
    }

    printf("\n");
    for(n=0;n<This->nperceptron;n++){
        Perceptron **tempPrev = malloc(sizeof(Perceptron*) * (This->perceptron[n]->get_nprev(This->perceptron[n])+2));
        unsigned int np=0;
        for(int i=0;i<This->nperceptron;i++){
            for(int j=0;j<This->nperceptronExit[i];j++) {
                if(This->perceptronExit[i][j] == n){
                    tempPrev[np] = This->perceptron[i];
                    np++;
                    //printf("%d:%d\t",n,i);
                    //printf("%d->%p\n",This->perceptronExit[i][j],(void*)This->perceptron[i]);
                }
            }
        }
        This->perceptron[n]->set_prev(This->perceptron[n],tempPrev,np);
    
        Perceptron **tempNext = malloc(sizeof(Perceptron*) * This->perceptron[n]->get_nnext(This->perceptron[n]));
        for(unsigned int i=0;i<This->perceptron[n]->get_nnext(This->perceptron[n]);i++){
            tempNext[i] = This->perceptron[This->perceptronExit[n][i]];
        }
        This->perceptron[n]->set_next(This->perceptron[n],tempNext,This->perceptron[n]->get_nnext(This->perceptron[n]));
        
        printf("N: %d\t%p\tNperv: %d\tNnext: %d\tPrev0 :%p\tNext0: %p\n",n,
        (void*)This->perceptron[n],
        np,
        This->perceptron[n]->get_nnext(This->perceptron[n]),
        (void*)This->perceptron[n]->prev[0],
        (void*)This->perceptron[n]->next[0]);
    }
    //exit(0);

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
static int train(Network *This,unsigned long iter,double errMin)
{
    double err=900;
    for(unsigned long i=0;i<iter && err >= errMin;i++){
        err=0;

        for(int n=0;n<This->ninput ;n++){

            for(int i=0;i<This->nperceptronExit[n];i++){
                //printf("---------------------------------------------------------------------------------------------FORMING%d:%d\t\n",n,i);
                This->perceptron[n]->get_next(This->perceptron[n])[i]->forming(This->perceptron[n]->get_next(This->perceptron[n])[i]);
                //printf("---------------------------------------------------------------------------------------------FORMING%d:%d\tOK\n",n,i);
            }
        }
        //TODO : do more clean en perform
    
        for(int n=0;n<NN;n++){
            //printf("%lf\t",This->perceptron[This->solve[0]]->get_exit(This->perceptron[This->solve[0]])[n]);
            //printf("%lf\n",This->perceptron[This->output[0]]->get_exit(This->perceptron[This->output[0]])[n]);
            
            err+= ABS(This->perceptron[This->output[0]]->get_exit(This->perceptron[This->output[0]])[n] - This->perceptron[This->solve[0]]->get_exit(This->perceptron[This->solve[0]])[n] );
        }   
       // printf("---------------------------------------------------------------------------------------------ERR\tCALCULED\n");
        for(int n=This->output[This->noutput-1];n>=This->output[0];n--){
            
            This->perceptron[n]->set_nformedDelta(This->perceptron[n],This->perceptron[n]->get_nnext(This->perceptron[n])-1);
        
            This->perceptron[n]->formingDelta(This->perceptron[n]);
         //   printf("---------------------------------------------------------------------------------------------END_DELTA%d\tOK\n",n);
        }

        for(int n=This->ninput;n<This->nperceptron - 1;n++){
             This->perceptron[n]->update(This->perceptron[n]);
        }
       
        printf("ERR-------------------------------------------->>>>>>>>>>>>>>>>%lf\n",err);
    
    }

    return 1;
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
    return 1;
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
static int inputDataCsv(Network *This,char *fileName){
    
    FILE *file = NULL;

    file = fopen(fileName,"r");
    
    double tempSolve[NN];
    double temp[3][NN];
    
    if (file == NULL)
    {
        return 0; 
    }
    else
    {
        char chaine = 'a';
        while(chaine != '\n'){
            chaine = fgetc(file);
            //printf("%c", chaine);
        }
        for(int n=0;n<NN;n++){
            for(int i=0;i<3;i++){  
                fscanf(file,"%lf",&temp[i][n]);
                //printf("%lf\t",temp[i][n]);
                fgetc(file);
            }
            fscanf(file,"%lf",&tempSolve[n]);
            //printf("%lf\n",tempSolve[n]);
            fgetc(file);
        }
            
    }
    

    This->perceptron[This->solve[0]]->set_exit(This->perceptron[This->solve[0]],tempSolve);

    for(int n=0;n<This->ninput;n++){
    This->perceptron[n]->set_exit(This->perceptron[n],temp[n]);
    }

    return 1;
}


/******************************************************************************/
static int initValueJson(Network *This,char *fileName){
    FILE *fp;
	char buffer[1024];
	struct json_object *parsed_json;

    struct json_object *neural;
    struct json_object **perceptronWeight;
    

	fp = fopen(fileName,"r");
	fread(buffer, 1024, 1, fp);
	fclose(fp);

	parsed_json = json_tokener_parse(buffer);

    json_object_object_get_ex(parsed_json, "neural", &neural);

    perceptronWeight = malloc(sizeof(neural) * 10);

    for(int n=0;n<This->nperceptron ;n++){
        
        char buffer[128];
        snprintf(buffer,sizeof(char) * 128,"%d",n);

        json_object_object_get_ex(neural, buffer, &perceptronWeight[n]);

        double *tempPV = malloc(sizeof(double) * This->perceptron[n]->get_nenter(This->perceptron[n]));
        for(unsigned int i=0;i<This->perceptron[n]->get_nprev(This->perceptron[n]);i++){
           printf("N: %d\tWeight: %d\t%lf\n",n,i,json_object_get_double(json_object_array_get_idx(perceptronWeight[n],i)));
            tempPV[i] = json_object_get_double(json_object_array_get_idx(perceptronWeight[n],i));
        }
        This->perceptron[n]->set_weights(This->perceptron[n],tempPV);
        
    }
    return 0;
}

/******************************************************************************/
static int testNetwork(Network *This){

    Perceptron **perceptronTEST;
    perceptronTEST = malloc(sizeof(Perceptron) * This->nperceptron);
    memcpy(perceptronTEST,This->perceptron,sizeof(Perceptron) * This->nperceptron);
    
    double **tempExit = malloc(sizeof(double*) * This->nperceptron);
    for(int n=0;n<This->nperceptron;n++){
        tempExit[n] = malloc(sizeof(double) * NN);
        memcpy(tempExit[n],perceptronTEST[n]->exit,sizeof(double*)*NN);
    }
    
    for(int i=0;i<3;i++){  
        double temp[2];
        printf("X%d: ",i);
        scanf("%lf",&temp[0]);
        perceptronTEST[i]->set_exit(perceptronTEST[i],temp);
    }

    for(int n=0;n<This->ninput ;n++){
            for(int i=0;i<This->nperceptronExit[n];i++){
                //printf("---------------------------------------------------------------------------------------------FORMING%d:%d\t\n",n,i);
                This->perceptron[n]->get_next(This->perceptron[n])[i]->forming(This->perceptron[n]->get_next(This->perceptron[n])[i]);
                //printf("---------------------------------------------------------------------------------------------FORMING%d:%d\tOK\n",n,i);
            }
        }

    printf("%lf\n",This->perceptron[This->output[0]]->get_exit(This->perceptron[This->output[0]])[0]);

    free(perceptronTEST);

    for(int n=0;n<This->nperceptron;n++){
        memcpy(This->perceptron[n]->exit,tempExit[n],sizeof(double*)*NN);
    }
    free(tempExit);

    return 0;
}

/******************************************************************************/
static int exportNetwork(Network *This,char *networkName,char *fileName){

    /*if(!strcmp(networkName,fileName)){
        FILE* source = NULL;
        FILE* temp = NULL;
        char ch;
        source = fopen(networkName, "r");
        temp = fopen("temp", "w+");

        for(int n=0; (ch = getc(source)) != EOF;n++){
                putc(ch, temp);
        }
        fclose(source);
        fclose(temp);
    }*/
    
    FILE* file = NULL;
    size_t size= (strlen(networkName) + strlen(fileName) + 2) * sizeof(char);
    char *buffer=malloc( size);
    snprintf(buffer,size,"%s_%s",networkName,fileName);

    file = fopen(fileName, "w+");
    
    if (file != NULL)
    {
        FILE* source;
        char ch;
  
        source = fopen(networkName, "r");

       int compte;
        for(int n=0; (ch = getc(source)) != EOF;n++){
            if(ch=='}'){
                compte=n;
            }
        }
        rewind(source);
        for(int n=0;(ch = getc(source)) != EOF;n++){
            if(n!=compte){
                putc(ch, file);
            }
        }
        fclose(source);

        char *neural;
        neural = malloc(sizeof(char) * 772 * This->nperceptron);

        for(int n=0;n<This->nperceptron;n++){
            char *temp;
            temp = malloc(sizeof(char) * 516);
            
            char *weights;
            weights = malloc(sizeof(char) * 256);
            unsigned int i=0;
            
            if(This->perceptron[n]->get_nprev(This->perceptron[n])>0){ 
                for(;i<(This->perceptron[n]->get_nprev(This->perceptron[n]) - 1);i++){
                    char *weight;
                    weight = malloc(sizeof(char) * 32);
                    snprintf(weight,sizeof(char) * 30,"%lf",This->perceptron[n]->get_weights(This->perceptron[n])[i]);
                    strcat(weights,weight);
                    strcat(weights,",");
                    free(weight);
                }
            
                char *weight;
                weight = malloc(sizeof(char) * 32);
                snprintf(weight,sizeof(char) * 30,"%lf",This->perceptron[n]->get_weights(This->perceptron[n])[i]);
                strcat(weights,weight);
                free(weight);
            }
                
            snprintf(temp,sizeof(char) * 516,"\t\t\"%d\":[%s]",n,weights);
            free(weights);
            strcat(neural,temp);
            if(n<This->nperceptron-1){
                strcat(neural,",\n");
            }
            free(temp);
        }

        char *buffer;
        buffer = malloc(sizeof(char) * 1032 * This->nperceptron);
        snprintf(buffer,sizeof(char) * 1032 * This->nperceptron,",\n\t\"neural\":{\n%s\n\t}\n}",neural);

        fputs(buffer, file);
        fclose(file);
    }
    else
    {
        return 0;
    }
    
    return 1;
}

/******************************************************************************/
static int destructor(Network *This)
{
    free(This->nperceptronExit);
    free(This->input);
    free(This->output);
    free(This->solve);
    
    for(int n=0;n<This->nperceptron;n++){
        free(This->perceptronExit[n]);
    }
    free(This->perceptronExit);
 
    for(int n=0;n<This->nperceptron;n++){
        This->perceptron[n]->destructor(This->perceptron[n]);
        free(This->perceptron[n]);
    }
    free(This->perceptron);

    return 1;
}