#include <stdlib.h>
#include <stdio.h>

#include "activation.h"
#include "Perceptron.h"

#define ABS(x) (    ((x)>0)? (x) : (-x)     )

int main()
{
    srand(time(NULL));
    Perceptron *entree[3];
    entree[0] = New_Perceptron(4,NULL,0,NULL,0);
    entree[1] = New_Perceptron(4,NULL,0,NULL,0);
    entree[2] = New_Perceptron(4,NULL,0,NULL,0);

    printf("\n\n");
    Perceptron *perceptron = New_Perceptron((unsigned int)4,entree,(unsigned int)3,NULL,0);


    double temp1 = (sqrt(-2.0*log((double)rand()/RAND_MAX)))*(cos(6.28318530718*(double)rand()/RAND_MAX));
    double temp2 = (sqrt(-2.0*log((double)rand()/RAND_MAX)))*(cos(6.28318530718*(double)rand()/RAND_MAX));
    double temp3 = (sqrt(-2.0*log((double)rand()/RAND_MAX)))*(cos(6.28318530718*(double)rand()/RAND_MAX));

    
    perceptron->set_weights(perceptron,temp1,temp2,temp3);
    

    entree[0]->set_next(entree[0],(Perceptron**){&perceptron},1);
    entree[1]->set_next(entree[1],&perceptron,1);
    entree[2]->set_next(entree[2],&perceptron,1);
      

    double solve[4];
    double temp[3][4];
    for(int n=0;n<4;n++){
        for(int i=0;i<3;i++){  
            printf("%dx%d: ",n,i);
            scanf("%lf",&temp[i][n]);
        }
        printf("y%d: ",n);
        scanf("%lf",&solve[n]);
    }
    entree[0]->set_exit(entree[0],temp[0]);
    entree[1]->set_exit(entree[1],temp[1]);
    entree[2]->set_exit(entree[2],temp[2]);

    
    for(int i=0;i<300;i++){
        double err=0,L[4],s[3];

        perceptron->forming(perceptron);
        printf("%s%lf\t%lf\t%lf\t%lf\n\n",perceptron->toString(perceptron),solve[0],solve[1],solve[2],solve[3]);
        
        for(int n=0;n<4;n++){
               err+= ABS(perceptron->get_exit(perceptron)[n] - solve[n]);
                L[n]=( (perceptron->get_exit(perceptron)[n] - solve[n]) * sigmoid_d(perceptron->get_exit(perceptron)[n]) );
        }

        for(int i=0;i<3;i++){
            s[i]=0.f;
            for(int n=0;n<4;n++){
                   s[i]+= perceptron->get_prev(perceptron)[i]->get_exit(perceptron->get_prev(perceptron)[i])[n] * L[n];
            }
        }

        double alpha = 1;

        perceptron->set_weights(perceptron
                , (perceptron->get_weights(perceptron)[0] - (s[0] * alpha) )
                , (perceptron->get_weights(perceptron)[1] - (s[1] * alpha) )
                , (perceptron->get_weights(perceptron)[2] - (s[2] * alpha) )
                );

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

        
        

        Perceptron *perceptronTEST = New_Perceptron(1,test,3,NULL,0);
            perceptronTEST->set_weights(perceptronTEST
                    ,perceptron->get_weights(perceptron)[0]
                    ,perceptron->get_weights(perceptron)[1]
                    ,perceptron->get_weights(perceptron)[2]
                    );

            perceptronTEST->forming(perceptronTEST);
    
        printf("SOM: %lf\n",perceptronTEST->get_exit(perceptronTEST)[0]);

    }
    
    

#ifdef __WIN32__
        system("PAUSE");
#endif
        return 0;
}

