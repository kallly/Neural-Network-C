#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "Network.h"

#define DEFAULTNETWORK ("netword_template.json")
#define DEFAULTRESULT ("result.json")
#define NPARAMETER 5

char** parameter(int argc,char *argv[]);

int main(int argc, char *argv[])
{   
    char **allParametre = parameter(argc,argv);
    printf("%s\n",allParametre[0]);
    Network *network = New_Network(allParametre[0],allParametre[1]);

    srand(time(NULL));

    //network->inputData(network);
    network->inputDataCsv(network,"data/(aETb)OUnonc.csv");
    network->train(network,strtoul(allParametre[3],NULL,10),strtod(allParametre[4],NULL));

    network->exportNetwork(network,allParametre[0],allParametre[2]);
            
    for (;;)
    {
        printf("\n1 train\n2 export\n3 test\n4 exit\n");
        int choice=0;
        scanf("%d",&choice);
        switch (choice)
        {
        case 1:
            network->train(network,strtoul(allParametre[3],NULL,10),strtod(allParametre[4],NULL));
            break;
        case 2:
            network->exportNetwork(network,allParametre[0],allParametre[2]);
            break;
        case 3:
            network->testNetwork(network);
            break;
        case 4:
            network->destructor(network);
            free(network);
            return 0;
            break;
        default:
            break;
        } 
    }
  
    network->destructor(network);
    free(network);

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


