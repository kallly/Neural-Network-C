#include <stdlib.h>
#include <stdio.h>

#include "activation.h"
#include "Perceptron.h"

#define ABS(x) (    ((x)>0)? (x) : (-x)     )

#define NN 4

typedef struct perceptronJSON
{
    int exit[10][10];
}perceptronJSON;

typedef struct networkJSON
{
    int *input;
    int *output;
    int *solve;
    perceptronJSON *perceptronJS;
}networkJSON;

int main()
{   
    size_t ninput= 3;
    size_t noutput= 1;
    size_t nperceptron = 5;
    size_t nsolve = 1;

    networkJSON network;
    network.input = malloc(sizeof(int) * ninput);
    network.input[0] = 0;network.input[1] = 1;network.input[2] = 2;
    network.output = malloc(sizeof(int) * noutput);
    network.output[0]=5;
    network.solve = malloc(sizeof(int) * nsolve);
    network.solve[0]=6;
    network.perceptronJS = malloc(sizeof(perceptronJSON) * nperceptron);
    network.perceptronJS->exit[0][0] = 3;
    network.perceptronJS->exit[0][1] = 4;

    network.perceptronJS->exit[1][0] = 3;
    network.perceptronJS->exit[1][1] = 4;

    network.perceptronJS->exit[2][0] = 3;
    network.perceptronJS->exit[2][1] = 4;

    network.perceptronJS->exit[3][0] = 5;
    network.perceptronJS->exit[4][0] = 5;

    network.perceptronJS->exit[5][0] = 6;


    srand(time(NULL));
    


    Perceptron **entree = malloc(sizeof(Perceptron) * ninput);

    for (size_t i = 0; i < ninput; i++)
    {
        entree[i] = New_Perceptron(NN,NULL,0,NULL,0);
    }

    
    Perceptron **perceptron = malloc(sizeof(Perceptron) * nperceptron);
    
    for(int n=0;n<(int)nperceptron;n++){    
        perceptron[n] = New_Perceptron(nperceptron,entree,ninput,NULL,0);
        perceptron[n]->set_nformed(perceptron[n],perceptron[n]->get_nprev(perceptron[n]));
    }

    Perceptron *end = New_Perceptron((unsigned int)4,perceptron,(unsigned int)2,NULL,0);

    Perceptron *solve = New_Perceptron((unsigned int)4,&end,(unsigned int)1,NULL,0);

    end->solve=solve;

    perceptron[0]->solve=solve;
    perceptron[1]->solve=solve;

    double temp1[3] = {(sqrt(-2.0*log((double)rand()/RAND_MAX)))*(cos(6.28318530718*(double)rand()/RAND_MAX))
                    ,(sqrt(-2.0*log((double)rand()/RAND_MAX)))*(cos(6.28318530718*(double)rand()/RAND_MAX))
                    ,(sqrt(-2.0*log((double)rand()/RAND_MAX)))*(cos(6.28318530718*(double)rand()/RAND_MAX))};

    double temp2[3] = {(sqrt(-2.0*log((double)rand()/RAND_MAX)))*(cos(6.28318530718*(double)rand()/RAND_MAX))
                    ,(sqrt(-2.0*log((double)rand()/RAND_MAX)))*(cos(6.28318530718*(double)rand()/RAND_MAX))
                    ,(sqrt(-2.0*log((double)rand()/RAND_MAX)))*(cos(6.28318530718*(double)rand()/RAND_MAX))};

    double temp3[3] = {(sqrt(-2.0*log((double)rand()/RAND_MAX)))*(cos(6.28318530718*(double)rand()/RAND_MAX))
                    ,(sqrt(-2.0*log((double)rand()/RAND_MAX)))*(cos(6.28318530718*(double)rand()/RAND_MAX))
                    ,(sqrt(-2.0*log((double)rand()/RAND_MAX)))*(cos(6.28318530718*(double)rand()/RAND_MAX))};

    perceptron[0]->set_next(perceptron[0],&end,1);
    perceptron[0]->set_weights(perceptron[0],temp1);
    perceptron[1]->set_next(perceptron[1],&end,1);
    perceptron[1]->set_weights(perceptron[1],temp2);

    end->set_weights(end,temp3);
    

    entree[0]->set_next(entree[0],perceptron,2);
    entree[1]->set_next(entree[1],perceptron,2);
    entree[2]->set_next(entree[2],perceptron,2);

    end->set_next(end,&solve,0);
      

    double tempSolve[4];
    double temp[3][4];
    for(int n=0;n<4;n++){
        for(int i=0;i<3;i++){  
            printf("%dx%d: ",n,i);
            scanf("%lf",&temp[i][n]);
        }
        printf("y%d: ",n);
        scanf("%lf",&tempSolve[n]);
    }

    solve->set_exit(solve,tempSolve);

    entree[0]->set_exit(entree[0],temp[0]);
    entree[1]->set_exit(entree[1],temp[1]);
    entree[2]->set_exit(entree[2],temp[2]);

    
    for(int i=0;i<30000;i++){
        double err=0;
        for(int n=0;n<2;n++)    perceptron[n]->set_nformed(perceptron[n],perceptron[n]->get_nprev(perceptron[n]));
        perceptron[0]->forming(perceptron[0]);
        printf("---------------------------------------------------------------------------------------------FORMING1\tOK\n");
        perceptron[1]->forming(perceptron[1]);
        printf("---------------------------------------------------------------------------------------------FORMING2\tOK\n");

        //end->forming(end);
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

    while (1)
    {
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
        for(int n=0;n<2;n++)    perceptronTEST[n]->set_nformed(perceptronTEST[n],perceptronTEST[n]->get_nprev(perceptronTEST[n]));

            perceptronTEST[0]->set_weights(perceptronTEST[0],perceptron[0]->get_weights(perceptron[0]));
            perceptronTEST[1]->set_weights(perceptronTEST[1],perceptron[1]->get_weights(perceptron[1]));

            Perceptron *endTest = New_Perceptron(1,perceptronTEST,2,NULL,0);

            endTest->set_weights(endTest,end->get_weights(end));

            perceptronTEST[0]->set_next(perceptronTEST[0],&endTest,1);
            perceptronTEST[1]->set_next(perceptronTEST[1],&endTest,1);

            perceptronTEST[0]->forming(perceptronTEST[0]);
            perceptronTEST[1]->forming(perceptronTEST[1]);               

           // endTest->forming(endTest);

        printf("Y: %lf\n",endTest->get_exit(endTest)[0]);

    }
    
    

#ifdef __WIN32__
        system("PAUSE");
#endif
        return 0;
}

