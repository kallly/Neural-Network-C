#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <pthread.h>
#include "Network.h"

pthread_cond_t condition = PTHREAD_COND_INITIALIZER;
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

#define DEFAULTNETWORK ("network_template.json")
#define DEFAULTRESULT ("result.json")
#define NPARAMETER 5

typedef struct ThreadPara
{
    Network *network;
    char **allParametre;
    int activ;
    int training;
}ThreadPara;


char** parameter(int argc,char *argv[]);

void *thread_1(void *args)
{
    ThreadPara *threadPara = args;
    threadPara->activ = 1;
    for (;;)
    {

        printf("\n1 erreur\n2 export\n3 test\n4 %s\n5 exit\n",threadPara->training?"STOP train":"START train");
        int choice=0;
        scanf("%d",&choice);
        pthread_mutex_lock(&mutex);
		pthread_cond_wait(&condition, &mutex);
        switch (choice)
        {
        case 1:
            printf("ERR: %lf",threadPara->network->err);
            break;
        case 2:
            threadPara->network->exportNetwork(threadPara->network,threadPara->allParametre[0],threadPara->allParametre[2]);
            break;
        case 3:
            threadPara->network->testNetwork(threadPara->network);
            break;
        case 4:
            threadPara->training = threadPara->training?0:1;
            break;
        case 5:
            threadPara->activ = 0;
            pthread_mutex_unlock(&mutex);
            pthread_exit(NULL);
            break;
        default:
            break;
        }

        pthread_mutex_unlock(&mutex);
    }
    
    pthread_exit(NULL);
}

int main(int argc, char *argv[])
{   
    ThreadPara *threadPara = malloc(sizeof(ThreadPara*));

    threadPara->allParametre = parameter(argc,argv);
    printf("%s\n",threadPara->allParametre[0]);
    threadPara->network = New_Network(threadPara->allParametre[0],threadPara->allParametre[1]);

    srand(time(NULL));

    //network->inputData(network);
    threadPara->network->inputDataCsv(threadPara->network,"data/chiffre_3x5.csv");
    threadPara->network->train(threadPara->network,strtoul(threadPara->allParametre[3],NULL,10),strtod(threadPara->allParametre[4],NULL));

    pthread_t thread1;
            
    if(pthread_create(&thread1, NULL, thread_1, threadPara) == -1) {
	    perror("pthread_create");
	    return EXIT_FAILURE;
    }

    threadPara->network->exportNetwork(threadPara->network,threadPara->allParametre[0],threadPara->allParametre[2]);
    threadPara->activ = 1;      
    threadPara->training = 1;
    for(;threadPara->activ == 1;){
        pthread_mutex_unlock (&mutex);
        if(threadPara->training)
            threadPara->network->train(threadPara->network,strtoul(threadPara->allParametre[3],NULL,10),strtod(threadPara->allParametre[4],NULL));
        pthread_mutex_lock (&mutex);
        pthread_cond_signal (&condition);
        
    }
    threadPara->network->destructor(threadPara->network);

    free(threadPara->network);

#ifdef __WIN32__
        system("PAUSE");
#endif
        return 0;
}

char** parameter(int argc,char *argv[]){
    char **buffer = malloc(sizeof(char*) * NPARAMETER);
    for(int n=0;n<NPARAMETER;n++){
            buffer[n]=malloc(sizeof(char)*128);        
    }
    strcpy(buffer[0],DEFAULTNETWORK);
    strcpy(buffer[1],"RANDOM");
    strcpy(buffer[2],DEFAULTRESULT);
    strcpy(buffer[3],"10000");
    strcpy(buffer[4],"0");

    for(int n=1;n<argc;n++){
        if(!strcmp(argv[n],"-src:")){
            strcpy(buffer[0],argv[++n]);
        }
        else
        {
            if(!strcmp(argv[n],"-weight:")){
                strcpy(buffer[1],argv[++n]);
            }
            else
            {
                if(!strcmp(argv[n],"-dest:")){
                    strcpy(buffer[2],argv[++n]);
                }
                else
                {
                    if(!strcmp(argv[n],"-iter:")){
                        strcpy(buffer[3],argv[++n]);
                    }
                    else
                    {
                        if(!strcmp(argv[n],"-err:")){
                            strcpy(buffer[4],argv[++n]);
                        }
                    }
                }
            }
            
            
        }
        
        
    }
    return buffer;
}


