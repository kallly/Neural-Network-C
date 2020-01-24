#include<stdlib.h>
#include<stdio.h>

#include "Network.h"

static void Network_Init(Network*);

static int train(Network *This);

/******************************************************************************/
Network* New_Network()
{
    Network *This = malloc(sizeof(Network));
    if(!This) return NULL;
    Network_Init(This);
    
    return This;
}

/******************************************************************************/
static void Network_Init(Network *This)
{
    This->train=train;
}

static int train(Network *This)
{
    for(int i=0;i<300;i++){
        double err=0;

        perceptron[0]->forming(perceptron[0]);
        printf("---------------------------------------------------------------------------------------------FORMING1\tOK\n");
        perceptron[1]->forming(perceptron[1]);
        printf("---------------------------------------------------------------------------------------------FORMING2\tOK\n");

        end->forming(end);
        printf("---------------------------------------------------------------------------------------------END\tOK\n");

        printf("%s%lf\t%lf\t%lf\t%lf\n\n",end->toString(end),solve->get_exit(solve)[0],solve->get_exit(solve)[1],solve->get_exit(solve)[2],solve->get_exit(solve)[3]);
        
        for(int n=0;n<4;n++) err+= ABS(end->get_exit(end)[n] - solve->get_exit(solve)[n] );
            
        printf("---------------------------------------------------------------------------------------------ERR\tCALCULED\n");

        end->formingDelta(end);
        printf("---------------------------------------------------------------------------------------------END_DELTA1\tOK\n");

        perceptron[0]->formingDelta(perceptron[0]);
        printf("---------------------------------------------------------------------------------------------FORMING_DELTA1\tOK\n");
        perceptron[1]->formingDelta(perceptron[1]);
        printf("---------------------------------------------------------------------------------------------FORMING_DELTA2\tOK\n");
        

        perceptron[0]->update(perceptron[0]);
        printf("\n");
        perceptron[1]->update(perceptron[1]);
        printf("\n");

        end->update(end);

        printf("-------------------------------------------->>>>>>>>>>>>>>>>%lf\n",err/4);
        
    }
}